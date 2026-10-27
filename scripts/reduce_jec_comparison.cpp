#include <ROOT/RConfig.hxx>
#include <TBranch.h>
#include <TFile.h>
#include <TLeaf.h>
#include <TH1.h>
#include <TParameter.h>
#include <TROOT.h>
#include <TTree.h>
#include <TUUID.h>
#include <nlohmann/json.hpp>
#include "../include/EOSUtils.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <glob.h>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

namespace fs = std::filesystem;
using Count = std::uint64_t;
using Clock = std::chrono::steady_clock;

struct Certification {
    using Range = std::pair<std::uint32_t,std::uint32_t>;
    std::map<std::uint32_t,std::vector<Range>> ranges;
    std::string source,canonical;
    bool active() const {return !source.empty();}
    void load(const std::string& path) {
        if(active()) throw std::runtime_error("Supply --cert-json once");
        std::ifstream stream(path);
        if(!stream) throw std::runtime_error("Cannot read certification JSON: "+path);
        using JSON = nlohmann::json;
        std::set<std::string> keys;
        const auto data=JSON::parse(stream,[&](int depth,JSON::parse_event_t event,JSON& value) {
            if(depth==1 && event==JSON::parse_event_t::key && !keys.insert(value.get<std::string>()).second)
                throw std::runtime_error("Duplicate run in certification JSON: "+path);
            return true;
        });
        if(!data.is_object() || data.empty()) throw std::runtime_error("Certification JSON must be a nonempty run-to-LS map: "+path);
        const auto number=[&](const JSON& value) {
            if(!value.is_number_integer()) throw std::runtime_error("Certification LS values must be positive integers: "+path);
            const Count n=value.get<Count>();
            if(!n || n>std::numeric_limits<std::uint32_t>::max()) throw std::runtime_error("Certification LS outside uint32 range: "+path);
            return static_cast<std::uint32_t>(n);
        };
        Count intervals=0;
        JSON normalized=JSON::object();
        for(auto it=data.begin();it!=data.end();++it) {
            const auto text=it.key();
            if(text.empty() || text.find_first_not_of("0123456789")!=std::string::npos)
                throw std::runtime_error("Invalid certification run: "+text);
            const Count n=std::stoull(text);
            if(!n || n>std::numeric_limits<std::uint32_t>::max()) throw std::runtime_error("Certification run outside uint32 range: "+text);
            const auto run=static_cast<std::uint32_t>(n);
            if(!it.value().is_array()) throw std::runtime_error("Certification run requires an array of LS ranges: "+text);
            std::vector<Range> input,merged;
            for(const auto& row:it.value()) {
                if(!row.is_array() || row.size()!=2) throw std::runtime_error("Malformed certification LS range for run "+text);
                const auto low=number(row[0]),high=number(row[1]);
                if(low>high) throw std::runtime_error("Reversed certification LS range for run "+text);
                input.emplace_back(low,high);
            }
            std::sort(input.begin(),input.end());
            for(const auto& row:input) {
                if(!merged.empty() && row.first<=Count(merged.back().second)+1)
                    merged.back().second=std::max(merged.back().second,row.second);
                else merged.push_back(row);
            }
            if(!ranges.emplace(run,merged).second) throw std::runtime_error("Duplicate numeric certification run: "+text);
            normalized[std::to_string(run)]=JSON::array();
            for(const auto& row:merged) normalized[std::to_string(run)].push_back({row.first,row.second});
            intervals+=merged.size();
        }
        if(!intervals) throw std::runtime_error("Certification JSON contains no allowed LS ranges: "+path);
        canonical=normalized.dump();source=fs::absolute(path).string();
    }
    bool accepts(std::uint32_t run,std::uint32_t ls) const {
        const auto found=ranges.find(run);
        if(found==ranges.end()) return false;
        const auto& rows=found->second;
        auto it=std::upper_bound(rows.begin(),rows.end(),ls,[](std::uint32_t value,const Range& row){return value<row.first;});
        return it!=rows.begin() && ls<=(--it)->second;
    }
};

enum Field { Run, Lumi, EventId, Json, Trigger, NJet, Id1, Id2, Mjj, AKMjj, Pt1, Pt2,
             Eta1, Eta2, Phi1, Phi2, AKPt1, AKPt2, AKEta1, AKEta2, AKPhi1, AKPhi2,
             Jec1, Jec2, Unc1, Unc2, HT, NVtx, Met, MetHT, NAK, NFields };
const std::array<const char*, NFields> branches = {
    "run", "lumi", "event", "PassJSON", "passHLT_PFScoutingHT", "nJet", "IdTight_j1",
    "IdTight_j2", "mjj", "Dijet_MassAK4PF", "pTWJ_j1", "pTWJ_j2", "etaWJ_j1",
    "etaWJ_j2", "phiWJ_j1", "phiWJ_j2", "pTAK4PF_j1", "pTAK4PF_j2", "etaAK4PF_j1",
    "etaAK4PF_j2", "phiAK4PF_j1", "phiAK4PF_j2", "jetJecAK4PF_j1", "jetJecAK4PF_j2",
    "jetJecUncAK4PF_j1", "jetJecUncAK4PF_j2", "HTAK4PF", "nVtx", "met",
    "METoverHTAK4PF", "NAK4PF"};
using Event = std::array<double, NFields>;
using Bins = std::array<std::int16_t, 36>;

struct Options {
    std::vector<std::string> input, oldInput, newInput, mcLists;
    std::string output, dataset, jec, tree = "rootTupleTree/tree", preset = "dijet";
    std::string match = "ordered";
    unsigned threads = std::max(1u, std::thread::hardware_concurrency());
    unsigned shard = 0, shards = 1;
    double minMass = 500, minPt = 30, maxEta = 2.5, maxDEta = 1.3;
    double balanceAlpha = 0.3, jetDR = 0.02;
    long long cacheMB = 32, chunk = 1000000, maxEntries = -1;
    bool force = false, resume = false, paired = false, requirePassJSON = false, mc = false;
    Certification certification;
};

std::array<fs::path,2> summaryPaths(const Options& o) {
    const fs::path path=fs::absolute(o.output);
    fs::path directory;
    std::string name;
    if(path.extension()==".json" && !fs::is_directory(path)) {
        directory=path.parent_path();name=path.stem().string();
        for(const std::string suffix:{"_selected","_all_stored"})
            if(name.size()>=suffix.size() && name.compare(name.size()-suffix.size(),suffix.size(),suffix)==0) {
                name.resize(name.size()-suffix.size());break;
            }
        if(name.empty()) throw std::runtime_error("--output requires a nonempty summary base name");
    } else {
        if(fs::exists(path) && !fs::is_directory(path)) throw std::runtime_error("Output is not a directory: "+path.string());
        if(o.dataset=="." || o.dataset==".." || o.dataset.find_first_of("/\\")!=std::string::npos)
            throw std::runtime_error("--dataset must be a filename label when --output is a directory");
        directory=path;name=o.dataset;
        if(!o.jec.empty()) name+="_"+o.jec;
    }
    return {directory/(name+"_selected.json"),directory/(name+"_all_stored.json")};
}

void checkSummaryOutputs(const Options& o) {
    for(const auto& path:summaryPaths(o)) {
        if(fs::exists(path) && !fs::is_regular_file(path)) throw std::runtime_error("Output is not a regular file: "+path.string());
        if(fs::exists(path) && !o.force && !o.resume) throw std::runtime_error("Output exists: "+path.string()+"; use --resume with its checkpoint or --force to start over");
    }
}

std::string trim(std::string s) {
    const auto a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return {};
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}

void expandInput(const std::string& value, std::vector<std::string>& out, unsigned depth = 0) {
    if (depth > 8) throw std::runtime_error("Input list recursion exceeds eight levels");
    if (value.empty()) throw std::runtime_error("Empty input path");
    if (value[0] == '@' || (fs::is_regular_file(value) && fs::path(value).extension() == ".txt")) {
        const fs::path path = value[0] == '@' ? value.substr(1) : value;
        std::ifstream stream(path);
        if (!stream) throw std::runtime_error("Cannot open input list: " + path.string());
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            if (line.find("://") == std::string::npos) {
                const bool nested = line[0] == '@';
                fs::path relative = nested ? line.substr(1) : line;
                if (relative.is_relative()) line = (nested ? "@" : "") + (path.parent_path() / relative).string();
            }
            expandInput(line, out, depth + 1);
        }
    } else if (value.find("://") != std::string::npos) {
        out.push_back(value);
    } else if (fs::is_directory(value)) {
        std::vector<std::string> local;
        for (const auto& item : fs::recursive_directory_iterator(value))
            if (item.is_regular_file() && item.path().extension() == ".root")
                local.push_back(fs::canonical(item.path()).string());
        std::sort(local.begin(), local.end());
        out.insert(out.end(), local.begin(), local.end());
    } else if (value.find_first_of("*?[") != std::string::npos) {
        glob_t result{};
        const int code = glob(value.c_str(), 0, nullptr, &result);
        if (code != 0) { globfree(&result); throw std::runtime_error("No files match: " + value); }
        for (std::size_t i = 0; i < result.gl_pathc; ++i)
            out.push_back(fs::canonical(result.gl_pathv[i]).string());
        globfree(&result);
    } else {
        if (!fs::is_regular_file(value)) throw std::runtime_error("Input does not exist: " + value);
        out.push_back(fs::canonical(value).string());
    }
}

void uniqueInputs(std::vector<std::string>& paths) {
    std::set<std::string> seen;
    for (const auto& p : paths)
        if (!seen.insert(p).second) throw std::runtime_error("Repeated input file: " + p);
    if (paths.empty()) throw std::runtime_error("No input files");
}

Options parse(int argc, char** argv) {
    Options o;
    bool massSet = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto value = [&]() { if (++i >= argc) throw std::runtime_error("Missing value for " + arg); return std::string(argv[i]); };
        if (arg == "--help" || arg == "-h") {
            std::cout << "Single: reduce_jec_comparison --input PATH --jec old|new --dataset ERA --output DIR\n"
                      << "Paired: reduce_jec_comparison --old PATH --new PATH --dataset ERA --output DIR\n"
                      << "QCD: reduce_jec_comparison --mc-list FILE --dataset QCD_2024 --output DIR\n"
                      << "One input scan writes DATASET_selected.json and DATASET_all_stored.json inside DIR.\n"
                      << "Single data samples include the JEC label: ERA_old_selected.json, ERA_old_all_stored.json (or new).\n"
                      << "The output directory is created automatically. Explicit --output FILE.json remains supported;\n"
                      << "an existing _selected or _all_stored suffix in that filename is replaced automatically.\n"
                      << "--mc-list FILE may also accompany data inputs and may be repeated.\n"
                      << "Each MC list row: path_or_glob xsec_pb; one complete QCD component per row.\n"
                      << "MC paths may be EOS paths, directories, globs, ROOT URLs or @filelists, relative to the MC list.\n"
                      << "QCD uses unit generator weights, skips data JSON and HLT cuts, and keeps the other preset cuts.\n"
                      << "Supply the sample cross section in pb, including any generator-filter efficiency already in that cross section.\n"
                      << "MC normalization is xsec_pb / generated_events, using counters before analysis skim cuts.\n"
                      << "Generator-weight branches, MC sharding and MC --max-entries are unsupported. Reduce QCD once and reuse its JSON.\n"
                      << "PATH: ROOT file, directory, quoted glob, @filelist.txt, or explicit root:// URL. Repeat input flags.\n"
                      << "--threads N (0=all), --cache-mb N (per reader), --chunk-entries N\n"
                      << "--preset dijet|loose|all configures the selected summary; default: dijet.\n"
                      << "--mjj-min GeV, --pt-min GeV, --eta-max X, --deta-max X\n"
                      << "Default dijet preset requires mjj > 500 GeV, PassJSON > 0.5 and the other analysis cuts.\n"
                      << "The all_stored summary is always produced, with no event cuts by default.\n"
                      << "Legacy --preset all_stored is accepted as an alias for the default dual output.\n"
                      << "--require-pass-json explicitly requires saved PassJSON > 0.5 for all_stored only.\n"
                      << "Jet observables require only their own usable stored quantities in all_stored.\n"
                      << "--cert-json FILE applies a run/LS mask before all histogram filling, including all_stored.\n"
                      << "--match ordered|keyed, --jet-match-dr X, --balance-alpha-max X\n"
                      << "--tree PATH, --shard-index I --shards N, --max-entries N (per file), --force\n"
                      << "Completed chunks are checkpointed automatically beside the summaries.\n"
                      << "--resume continues a matching checkpoint; unfinished chunks are reread.\n"
                      << "Keep the same inputs, cuts, thread count and chunk size; --force discards saved progress.\n"
                      << "ordered verifies event identities at every entry; keyed joins within corresponding file pairs.\n";
            std::exit(0);
        } else if (arg == "--input") expandInput(value(), o.input);
        else if (arg == "--mc-list") o.mcLists.push_back(value());
        else if (arg == "--old") expandInput(value(), o.oldInput);
        else if (arg == "--new") expandInput(value(), o.newInput);
        else if (arg == "--output") o.output = value();
        else if (arg == "--dataset") o.dataset = value();
        else if (arg == "--jec") o.jec = value();
        else if (arg == "--tree") o.tree = value();
        else if (arg == "--preset") o.preset = value();
        else if (arg == "--cert-json") o.certification.load(value());
        else if (arg == "--require-pass-json") o.requirePassJSON = true;
        else if (arg == "--match") o.match = value();
        else if (arg == "--threads") { const int n = std::stoi(value()); if (n < 0) throw std::runtime_error("Negative threads"); o.threads = n ? n : std::max(1u, std::thread::hardware_concurrency()); }
        else if (arg == "--shards") { const int n = std::stoi(value()); if (n <= 0) throw std::runtime_error("Invalid shard count"); o.shards = n; }
        else if (arg == "--shard-index") { const int n = std::stoi(value()); if (n < 0) throw std::runtime_error("Negative shard index"); o.shard = n; }
        else if (arg == "--mjj-min") { o.minMass = std::stod(value()); massSet = true; }
        else if (arg == "--pt-min") o.minPt = std::stod(value());
        else if (arg == "--eta-max") o.maxEta = std::stod(value());
        else if (arg == "--deta-max") o.maxDEta = std::stod(value());
        else if (arg == "--balance-alpha-max") o.balanceAlpha = std::stod(value());
        else if (arg == "--jet-match-dr") o.jetDR = std::stod(value());
        else if (arg == "--cache-mb") o.cacheMB = std::stoll(value());
        else if (arg == "--chunk-entries") o.chunk = std::stoll(value());
        else if (arg == "--max-entries") o.maxEntries = std::stoll(value());
        else if (arg == "--force") o.force = true;
        else if (arg == "--resume") o.resume = true;
        else throw std::runtime_error("Unknown option: " + arg);
    }
    o.paired = !o.oldInput.empty() || !o.newInput.empty();
    if(o.force && o.resume) throw std::runtime_error("Use either --resume or --force, not both");
    if (o.output.empty() || o.dataset.empty()) throw std::runtime_error("--output and --dataset are required");
    if (o.preset != "dijet" && o.preset != "loose" && o.preset != "all" && o.preset != "all_stored") throw std::runtime_error("Unknown preset");
    if (o.preset == "all_stored") {
        std::cout<<"--preset all_stored is no longer needed; writing dijet selected and all_stored summaries"<<std::endl;
        o.preset="dijet";
    }
    if (o.preset != "dijet" && !massSet) o.minMass = 0;
    if (o.match != "ordered" && o.match != "keyed") throw std::runtime_error("Unknown matching mode");
    for (double x : {o.minMass, o.minPt, o.maxEta, o.maxDEta, o.balanceAlpha, o.jetDR})
        if (!std::isfinite(x) || x < 0) throw std::runtime_error("Cuts must be finite and nonnegative");
    if (o.maxEta == 0 || o.maxDEta == 0 || o.jetDR == 0 || o.chunk <= 0 || o.cacheMB < 0 || o.maxEntries < -1)
        throw std::runtime_error("Invalid numeric option");
    if (o.shard >= o.shards) throw std::runtime_error("Shard index must be less than shard count");
    if (o.paired) {
        if (!o.input.empty() || !o.jec.empty()) throw std::runtime_error("Use either --input/--jec or --old/--new");
        uniqueInputs(o.oldInput); uniqueInputs(o.newInput);
        if (o.oldInput.size() != o.newInput.size()) throw std::runtime_error("Paired mode requires equal numbers of corresponding files");
    } else if (!o.input.empty() || o.mcLists.empty()) {
        uniqueInputs(o.input);
        if (o.jec != "old" && o.jec != "new") throw std::runtime_error("--jec must be old or new");
    } else if (!o.jec.empty()) {
        throw std::runtime_error("MC-only reduction does not use --jec");
    }
    if (!o.mcLists.empty() && (o.shards != 1 || o.maxEntries >= 0))
        throw std::runtime_error("--mc-list requires complete components: do not use --shards > 1 or --max-entries");
    checkSummaryOutputs(o);
    return o;
}

std::vector<double> uniform(int n, double a, double b) {
    std::vector<double> e(n + 1);
    for (int i = 0; i <= n; ++i) e[i] = a + (b - a) * i / n;
    return e;
}
const std::vector<double> massEdges = {1,3,6,10,16,23,31,40,50,61,74,88,103,119,137,156,176,197,220,
    244,270,296,325,354,386,419,453,489,526,565,606,649,693,740,788,838,890,944,1000,1058,1118,
    1181,1246,1313,1383,1455,1530,1607,1687,1770,1856,1945,2037,2132,2231,2332,2438,2546,
    2659,2775,2895,3019,3147,3279,3416,3558,3704,3854,4010,4171,4337,4509,4686,4869,5058,
    5253,5455,5663,5877,6099,6328,6564,6808,7060,7320,7589,7866,8152,8447,8752,9067,9391,
    9726,10072,10430,10798,11179,11571,11977,12395,12827,13272,13732,14000};
const std::vector<double> ptEdges = {0,20,30,40,50,60,80,100,120,150,200,250,300,400,500,600,800,
    1000,1200,1500,2000,2500,3000,4000,5000,7000};
const std::vector<double> uniformPtEdges = uniform(70,0,7000);
const std::vector<double> thresholds = {0,250,300,400,500,600,800,1000,1181,1500,2000,3000};

double inverseBinWidth(const std::vector<double>& e) {
    if(e.size()<2) return 0;
    const double width=(e.back()-e.front())/(e.size()-1);
    if(!std::isfinite(width) || width<=0) return 0;
    for(std::size_t i=1;i<e.size();++i)
        if(std::abs((e[i]-e[i-1])-width)>width*1e-10) return 0;
    return 1/width;
}
int bin(const std::vector<double>& e, double x, double inverseWidth=0) {
    if (!std::isfinite(x)) return -1;
    if (x < e.front()) return 0;
    if (x >= e.back()) return static_cast<int>(e.size());
    if(inverseWidth>0) {
        int b=std::min(static_cast<int>(e.size())-1,1+static_cast<int>((x-e.front())*inverseWidth));
        while(b>1 && x<e[b-1]) --b;
        while(b<static_cast<int>(e.size())-1 && x>=e[b]) ++b;
        return b;
    }
    return static_cast<int>(std::upper_bound(e.begin(), e.end(), x) - e.begin());
}
struct Moment {
    Count n = 0;
    double sum = 0, sum2 = 0;
    void fill(double x) { if (std::isfinite(x)) { ++n; sum += x; sum2 += x*x; } }
    void merge(const Moment& m) { n += m.n; sum += m.sum; sum2 += m.sum2; }
};
struct Hist {
    std::string name, label;
    std::vector<double> edges;
    std::vector<Count> counts;
    Count invalid = 0, unavailable = 0;
    double uniformWidth = 0, inverseWidth = 0;
    Hist(std::string n, std::string l, std::vector<double> e, double width=0): name(std::move(n)), label(std::move(l)),
        edges(std::move(e)), counts(edges.size()+1, 0), uniformWidth(width), inverseWidth(inverseBinWidth(edges)) {}
    int fill(double x) {
        if(uniformWidth>0) return fillUniform(x,uniformWidth);
        const int b = bin(edges,x,inverseWidth);
        if(b < 0) ++invalid; else ++counts[b];
        return b;
    }
    int fillUniform(double x, double width) {
        const int b = !std::isfinite(x) ? -1 : x < edges.front() ? 0 : x >= edges.back() ? static_cast<int>(edges.size()) :
            std::min(static_cast<int>(edges.size())-1, 1+static_cast<int>((x-edges.front())/width));
        if(b < 0) ++invalid; else ++counts[b];
        return b;
    }
    void merge(const Hist& h) { for(std::size_t i=0;i<counts.size();++i) counts[i]+=h.counts[i]; invalid+=h.invalid; unavailable+=h.unavailable; }
};
struct Profile {
    std::string name, label, ylabel;
    std::vector<double> xedges, yedges;
    std::vector<Moment> values;
    double inverseXWidth=0,inverseYWidth=0;
    Profile(std::string n, std::string l, std::string yl, std::vector<double> x, std::vector<double> y = {}):
        name(std::move(n)), label(std::move(l)), ylabel(std::move(yl)), xedges(std::move(x)),
        yedges(std::move(y)), values((xedges.size()+1)*(yedges.empty()?1:yedges.size()+1)),
        inverseXWidth(inverseBinWidth(xedges)),inverseYWidth(inverseBinWidth(yedges)) {}
    void fill(double x, double z, double y = 0) {
        const int bx=bin(xedges,x,inverseXWidth), by=yedges.empty()?0:bin(yedges,y,inverseYWidth);
        if(bx>=0 && by>=0) values[bx*(yedges.empty()?1:yedges.size()+1)+by].fill(z);
    }
    void merge(const Profile& p) { for(std::size_t i=0;i<values.size();++i) values[i].merge(p.values[i]); }
};

std::vector<Hist> makeHists() {
    std::vector<Hist> h;
    auto add=[&](const std::string& n,const std::string& l,const std::vector<double>& e,double width=0){h.emplace_back(n,l,e,width);};
    add("mjj_base","Wide-dijet mass [GeV]",massEdges);
    add("mjj","Wide-dijet mass [GeV]",massEdges);
    add("ak4_mjj","AK4 dijet mass [GeV]",massEdges);
    for(const std::string family:{"wj","ak4"}) for(int j=1;j<=2;++j) {
        const std::string suffix=family+"_j"+std::to_string(j);
        add("pt_"+suffix,(j==1?"Leading ":"Subleading ")+family+" jet pT [GeV]",family=="wj"?uniformPtEdges:ptEdges,family=="wj"?100:0);
        add("eta_"+suffix,(j==1?"Leading ":"Subleading ")+family+" jet eta",uniform(60,-3,3));
        add("phi_"+suffix,(j==1?"Leading ":"Subleading ")+family+" jet phi",uniform(64,-std::acos(-1),std::acos(-1)));
    }
    for(int j=1;j<=2;++j) {
        const std::string s="_j"+std::to_string(j);
        add("jec"+s,"AK4 JEC factor",uniform(100,0,3));
        add("jec_unc"+s,"AK4 relative JEC uncertainty",uniform(100,0,0.2));
        add("raw_pt"+s,"AK4 pT / saved JEC [GeV]",uniformPtEdges,100);
        add("pt_positive"+s,"Wide-jet pT, eta > 0 [GeV]",uniformPtEdges,100);
        add("pt_negative"+s,"Wide-jet pT, eta < 0 [GeV]",uniformPtEdges,100);
    }
    add("ht","AK4 HT [GeV]",massEdges);
    add("nvtx","Vertex multiplicity",uniform(100,-0.5,99.5));
    add("met","Stored MET [GeV]",ptEdges);
    add("met_over_ht","Stored MET / AK4 HT",uniform(100,0,1));
    add("nak4","AK4 jet multiplicity",uniform(25,-0.5,24.5));
    add("delta_eta","Absolute wide-dijet delta eta",uniform(60,0,6));
    add("delta_phi","Absolute wide-dijet delta phi",uniform(64,0,std::acos(-1)));
    add("pt_asymmetry","Wide-dijet (pT1-pT2)/(pT1+pT2)",uniform(100,-1,1));
    add("cos_theta_star","Absolute tanh(delta eta / 2)",uniform(50,0,1));
    add("wide_over_ak4_mass","Wide / AK4 dijet mass",uniform(100,0,3));
    add("ht_50gev","AK4 HT [GeV]",uniform(280,0,14000));
    return h;
}
std::vector<Profile> makeProfiles() {
    std::vector<Profile> p;
    for(int j=1;j<=2;++j) {
        const std::string s="_j"+std::to_string(j);
        p.emplace_back("jec_eta"+s,"AK4 jet eta","Mean JEC factor",uniform(50,-2.5,2.5));
        p.emplace_back("jec_rawpt"+s,"AK4 pT / JEC [GeV]","Mean JEC factor",ptEdges);
        p.emplace_back("jec_nvtx"+s,"Vertex multiplicity","Mean JEC factor",uniform(20,-0.5,99.5));
        p.emplace_back("pt_eta"+s,"Wide-jet eta","Mean wide-jet pT [GeV]",uniform(50,-2.5,2.5));
        p.emplace_back("occupancy"+s,"Wide-jet eta","Wide-jet phi",uniform(50,-2.5,2.5),uniform(48,-std::acos(-1),std::acos(-1)));
        p.emplace_back("jec_eta_rawpt"+s,"AK4 jet eta","AK4 pT / JEC [GeV]",uniform(50,-2.5,2.5),ptEdges);
    }
    p.emplace_back("balance_eta","Probe AK4 eta","Mean probe / central-tag pT",uniform(50,-2.5,2.5));
    p.emplace_back("balance_rawpt","Central-tag pT / JEC [GeV]","Mean probe / central-tag pT",ptEdges);
    return p;
}

std::uint64_t mix(std::uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
struct Key {
    std::uint64_t event;
    std::uint32_t run,lumi;
    bool operator==(const Key& k) const {return event==k.event && run==k.run && lumi==k.lumi;}
};
struct KeyHash { std::size_t operator()(const Key& k) const {return mix(k.event)^mix((Count(k.run)<<32)|k.lumi);} };
Key key(const Event& e) {
    for(int i:{Run,Lumi,EventId}) if(!std::isfinite(e[i]) || e[i]<0 || e[i]!=std::floor(e[i]) || e[i]>=9007199254740992.0)
        throw std::runtime_error("Event identifier is not an exact nonnegative integer below 2^53");
    if(e[Run]>std::numeric_limits<std::uint32_t>::max() || e[Lumi]>std::numeric_limits<std::uint32_t>::max())
        throw std::runtime_error("Run or lumisection outside uint32 range");
    return {static_cast<Count>(e[EventId]),static_cast<std::uint32_t>(e[Run]),static_cast<std::uint32_t>(e[Lumi])};
}
double dphi(double a,double b) { return std::abs(std::remainder(a-b,2*std::acos(-1))); }
bool validJets(const Event& e) {
    for(int i:{Mjj,AKMjj,Pt1,Pt2,Eta1,Eta2,Phi1,Phi2,AKPt1,AKPt2,AKEta1,AKEta2,AKPhi1,AKPhi2,Jec1,Jec2})
        if(!std::isfinite(e[i])) return false;
    return e[NJet]>1 && e[Pt1]>0 && e[Pt2]>0 && e[AKPt1]>0 && e[AKPt2]>0 && e[Jec1]>0 && e[Jec2]>0 && e[Mjj]>0;
}
std::array<bool,7> cutflow(const Event& e,const Options& o) {
    std::array<bool,7> c{};
    if(o.preset=="all_stored") {c.fill(true);return c;}
    c[0]=o.mc || o.preset=="all" || (e[Json]>0.5 && e[Trigger]>0.5);
    c[1]=c[0] && validJets(e);
    c[2]=c[1] && (o.preset=="all" || (e[Id1]>0.5 && e[Id2]>0.5));
    c[3]=c[2] && (o.preset=="all" || (e[Pt1]>o.minPt && e[Pt2]>o.minPt));
    c[4]=c[3] && (o.preset=="all" || (std::abs(e[Eta1])<o.maxEta && std::abs(e[Eta2])<o.maxEta));
    c[5]=c[4] && (o.preset=="all" || std::abs(e[Eta1]-e[Eta2])<o.maxDEta);
    c[6]=c[5] && e[Mjj]>o.minMass;
    return c;
}
struct RunStat {
    Count stored=0,base=0,selected=0;
    std::array<Count,4> signs{};
    std::array<Count,2> etaZero{};
    std::array<Moment,5> means;
    void merge(const RunStat& r) {
        stored+=r.stored; base+=r.base; selected+=r.selected;
        for(int i=0;i<4;++i) signs[i]+=r.signs[i];
        for(int i=0;i<2;++i) etaZero[i]+=r.etaZero[i];
        for(int i=0;i<5;++i) means[i].merge(r.means[i]);
    }
};
struct LSStat { Count stored=0,base=0,selected=0,hashSum=0,hashXor=0;
    void merge(const LSStat& r) {stored+=r.stored;base+=r.base;selected+=r.selected;hashSum+=r.hashSum;hashXor^=r.hashXor;}
};
struct Summary {
    std::vector<Hist> h=makeHists();
    std::vector<Profile> p=makeProfiles();
    std::array<Count,8> cuts{};
    std::vector<Count> thresholdCounts=std::vector<Count>(thresholds.size());
    std::map<std::uint32_t,RunStat> runs;
    std::map<Count,LSStat> lumis;
    Count validDijets=0;
    Count inputEntries=0,certificationRejected=0,passJSONRejected=0,lastSection=0;
    bool cachedSection=false,certificationPassed=true,retained=true;
    void merge(const Summary& s) {
        for(std::size_t i=0;i<h.size();++i) h[i].merge(s.h[i]);
        for(std::size_t i=0;i<p.size();++i) p[i].merge(s.p[i]);
        for(std::size_t i=0;i<cuts.size();++i) cuts[i]+=s.cuts[i];
        for(std::size_t i=0;i<thresholds.size();++i) thresholdCounts[i]+=s.thresholdCounts[i];
        for(const auto& v:s.runs) runs[v.first].merge(v.second);
        for(const auto& v:s.lumis) lumis[v.first].merge(v.second);
        validDijets+=s.validDijets;
        inputEntries+=s.inputEntries;certificationRejected+=s.certificationRejected;passJSONRejected+=s.passJSONRejected;
    }
    Bins fillStored(const Event& e,const Options& o,const Key& k,RunStat& r,LSStat& ls) {
        ++r.base;++ls.base;++r.selected;++ls.selected;
        validDijets+=validJets(e);
        const auto positive=[](double x){return std::isfinite(x) && x>0;};
        const std::array<bool,2> w={positive(e[Pt1]),positive(e[Pt2])};
        const std::array<bool,2> a={positive(e[AKPt1]),positive(e[AKPt2])};
        const std::array<bool,2> jec={a[0] && positive(e[Jec1]),a[1] && positive(e[Jec2])};
        const bool widePair=w[0] && w[1],akPair=a[0] && a[1];
        const bool mass=widePair && positive(e[Mjj]),akMass=akPair && positive(e[AKMjj]);
        const bool etaPair=widePair && std::isfinite(e[Eta1]) && std::isfinite(e[Eta2]);
        const bool phiPair=widePair && std::isfinite(e[Phi1]) && std::isfinite(e[Phi2]);
        Bins bins;bins.fill(-1);std::size_t hi=0;
        auto fill=[&](double value,bool available=true){
            if(available) bins[hi]=h[hi].fill(value);else ++h[hi].unavailable;
            ++hi;
        };
        for(std::size_t i=0;i<thresholds.size();++i) thresholdCounts[i]+=mass && e[Mjj]>thresholds[i];
        fill(e[Mjj],mass);fill(e[Mjj],mass);fill(e[AKMjj],akMass);
        if(mass) r.means[0].fill(e[Mjj]);
        for(int family=0;family<2;++family) for(int j=0;j<2;++j) {
            const bool available=family?a[j]:w[j];
            fill(e[(family?AKPt1:Pt1)+j],available);
            fill(e[(family?AKEta1:Eta1)+j],available);
            fill(e[(family?AKPhi1:Phi1)+j],available);
        }
        for(int j=0;j<2;++j) {
            const bool eta=w[j] && std::isfinite(e[Eta1+j]);
            if(eta) {r.signs[2*j]+=e[Eta1+j]>0;r.signs[2*j+1]+=e[Eta1+j]<0;r.etaZero[j]+=e[Eta1+j]==0;}
            if(w[j]) r.means[1+j].fill(e[Pt1+j]);
            if(jec[j]) r.means[3+j].fill(e[Jec1+j]);
            fill(e[Jec1+j],jec[j]);fill(e[Unc1+j],a[j]);
            fill(jec[j]?e[AKPt1+j]/e[Jec1+j]:0,jec[j]);
            fill(e[Pt1+j],eta && e[Eta1+j]>0);fill(e[Pt1+j],eta && e[Eta1+j]<0);
        }
        fill(e[HT]);fill(e[NVtx]);fill(e[Met]);fill(e[MetHT],positive(e[HT]));fill(e[NAK]);
        const double de=etaPair?std::abs(e[Eta1]-e[Eta2]):0;
        fill(de,etaPair);fill(phiPair?dphi(e[Phi1],e[Phi2]):0,phiPair);
        fill(widePair?(e[Pt1]-e[Pt2])/(e[Pt1]+e[Pt2]):0,widePair);
        fill(std::tanh(de/2),etaPair);fill(mass && akMass?e[Mjj]/e[AKMjj]:0,mass && akMass);
        bins[hi]=h[hi].fillUniform(e[HT],50);
        std::size_t pi=0;
        for(int j=0;j<2;++j) {
            const double raw=jec[j]?e[AKPt1+j]/e[Jec1+j]:0;
            if(jec[j]) p[pi].fill(e[AKEta1+j],e[Jec1+j]);++pi;
            if(jec[j]) p[pi].fill(raw,e[Jec1+j]);++pi;
            if(jec[j]) p[pi].fill(e[NVtx],e[Jec1+j]);++pi;
            if(w[j]) p[pi].fill(e[Eta1+j],e[Pt1+j]);++pi;
            if(w[j]) p[pi].fill(e[Eta1+j],1,e[Phi1+j]);++pi;
            if(jec[j]) p[pi].fill(e[AKEta1+j],e[Jec1+j],raw);++pi;
        }
        if(akPair && std::isfinite(e[AKPhi1]) && std::isfinite(e[AKPhi2]) &&
           std::isfinite(e[AKEta1]) && std::isfinite(e[AKEta2]) && std::isfinite(e[HT])) {
            const double alpha=(e[HT]-e[AKPt1]-e[AKPt2])/((e[AKPt1]+e[AKPt2])/2);
            if(dphi(e[AKPhi1],e[AKPhi2])>2.7 && alpha>=-1e-6 && alpha<o.balanceAlpha) {
                const bool central1=std::abs(e[AKEta1])<1.3,central2=std::abs(e[AKEta2])<1.3;
                if(central1 || central2) {
                    const int tag=central1 && central2?(mix(k.event)&1):(central1?0:1),probe=1-tag;
                    const double response=e[AKPt1+probe]/e[AKPt1+tag];
                    p[pi].fill(e[AKEta1+probe],response);
                    if(jec[tag]) p[pi+1].fill(e[AKPt1+tag]/e[Jec1+tag],response);
                }
            }
        }
        return bins;
    }
    Bins fill(const Event& e,const Options& o,const Key& k) {
        ++inputEntries;
        retained=false;
        if(o.certification.active()) {
            const Count section=(Count(k.run)<<32)|k.lumi;
            if(!cachedSection || section!=lastSection) {
                lastSection=section;cachedSection=true;
                certificationPassed=o.certification.accepts(k.run,k.lumi);
            }
            if(!certificationPassed) {++certificationRejected;Bins bins;bins.fill(-1);return bins;}
        }
        if(o.requirePassJSON && !(e[Json]>0.5)) {++passJSONRejected;Bins bins;bins.fill(-1);return bins;}
        retained=true;
        const auto c=cutflow(e,o);
        ++cuts[0]; for(std::size_t i=0;i<c.size();++i) cuts[i+1]+=c[i];
        auto& r=runs[k.run]; auto& ls=lumis[(Count(k.run)<<32)|k.lumi];
        ++r.stored; ++ls.stored;
        const Count hash=KeyHash{}(k); ls.hashSum+=hash; ls.hashXor^=hash;
        if(o.preset=="all_stored") return fillStored(e,o,k,r,ls);
        Bins bins; bins.fill(-1);
        if(!c[5]) return bins;
        ++r.base; ++ls.base;
        for(std::size_t i=0;i<thresholds.size();++i) thresholdCounts[i]+=e[Mjj]>thresholds[i];
        bins[0]=h[0].fill(e[Mjj]);
        if(!c[6]) return bins;
        ++r.selected; ++ls.selected;
        for(int j=0;j<2;++j) {r.signs[2*j]+=e[Eta1+j]>0;r.signs[2*j+1]+=e[Eta1+j]<0;}
        for(int i=0;i<5;++i) r.means[i].fill(e[std::array<int,5>{Mjj,Pt1,Pt2,Jec1,Jec2}[i]]);
        std::size_t hi=1;
        auto fill=[&](double v){bins[hi]=h[hi].fill(v);++hi;};
        fill(e[Mjj]);fill(e[AKMjj]);
        for(int family=0;family<2;++family) for(int j=0;j<2;++j) {
            fill(e[(family?AKPt1:Pt1)+j]);fill(e[(family?AKEta1:Eta1)+j]);fill(e[(family?AKPhi1:Phi1)+j]);
        }
        for(int j=0;j<2;++j) {
            fill(e[Jec1+j]);fill(e[Unc1+j]);fill(e[AKPt1+j]/e[Jec1+j]);
            if(e[Eta1+j]>0) fill(e[Pt1+j]); else ++hi;
            if(e[Eta1+j]<0) fill(e[Pt1+j]); else ++hi;
        }
        fill(e[HT]);fill(e[NVtx]);fill(e[Met]);fill(e[MetHT]);fill(e[NAK]);
        const double de=std::abs(e[Eta1]-e[Eta2]);
        fill(de);fill(dphi(e[Phi1],e[Phi2]));fill((e[Pt1]-e[Pt2])/(e[Pt1]+e[Pt2]));
        fill(std::tanh(de/2));fill(e[AKMjj]>0?e[Mjj]/e[AKMjj]:std::numeric_limits<double>::quiet_NaN());
        bins[hi]=h[hi].fillUniform(e[HT],50);
        std::size_t pi=0;
        for(int j=0;j<2;++j) {
            const double raw=e[AKPt1+j]/e[Jec1+j];
            p[pi++].fill(e[AKEta1+j],e[Jec1+j]);p[pi++].fill(raw,e[Jec1+j]);
            p[pi++].fill(e[NVtx],e[Jec1+j]);p[pi++].fill(e[Eta1+j],e[Pt1+j]);
            p[pi++].fill(e[Eta1+j],1,e[Phi1+j]);p[pi++].fill(e[AKEta1+j],e[Jec1+j],raw);
        }
        const double avg=(e[AKPt1]+e[AKPt2])/2;
        const double alpha=(e[HT]-e[AKPt1]-e[AKPt2])/avg;
        if(dphi(e[AKPhi1],e[AKPhi2])>2.7 && alpha>=-1e-6 && alpha<o.balanceAlpha) {
            const bool central1=std::abs(e[AKEta1])<1.3,central2=std::abs(e[AKEta2])<1.3;
            if(central1 || central2) {
                const int tag=central1 && central2?(mix(k.event)&1):(central1?0:1),probe=1-tag;
                const double response=e[AKPt1+probe]/e[AKPt1+tag];
                p[pi++].fill(e[AKEta1+probe],response);
                p[pi].fill(e[AKPt1+tag]/e[Jec1+tag],response);
            }
        }
        return bins;
    }
};

const std::array<int,16> pairFields={Mjj,AKMjj,Pt1,Pt2,Eta1,Eta2,AKPt1,AKPt2,AKEta1,AKEta2,AKPhi1,AKPhi2,Jec1,Jec2,NVtx,HT};
struct Record {
    std::array<double,16> v;
    std::array<Bins,2> bins;
    std::array<bool,2> retained,base,selected;
    bool seen=false;
    Record(const Event& e,const std::array<Options,2>& options,std::array<Bins,2> b,std::array<bool,2> keep):bins(b),retained(keep) {
        for(std::size_t i=0;i<v.size();++i) v[i]=e[pairFields[i]];
        for(std::size_t view=0;view<options.size();++view) {
            const auto c=cutflow(e,options[view]);base[view]=c[5];selected[view]=c[6];
        }
    }
    Event event() const {Event e{};for(std::size_t i=0;i<v.size();++i)e[pairFields[i]]=v[i];return e;}
};
struct Paired {
    Count matched=0,onlyOld=0,onlyNew=0,jetMatched=0,jetUnmatched=0,reordered=0;
    std::array<Count,4> migration{};
    std::vector<std::array<Count,4>> thresholdMigration=std::vector<std::array<Count,4>>(thresholds.size());
    std::vector<Hist> shifts;
    std::vector<Profile> profiles;
    std::vector<std::vector<Count>> overlap;
    std::map<std::uint32_t,std::array<Count,3>> runs;
    std::map<std::uint32_t,std::array<Moment,3>> runShifts;
    Paired() {
        for(const std::string name:{"mjj","ak4_mjj","pt_wj_j1","pt_wj_j2","pt_ak4_j1","jec_j1"})
            shifts.emplace_back(name,"100 (new - old) / old [%]",uniform(240,-30,30));
        profiles.emplace_back("mjj_oldmass","Old wide-dijet mass [GeV]","Mean mass shift [%]",massEdges);
        profiles.emplace_back("pt_ak4_eta","Old leading AK4 eta","Mean matched AK4 pT shift [%]",uniform(50,-2.5,2.5));
        profiles.emplace_back("pt_ak4_rawpt","Old leading AK4 pT / JEC [GeV]","Mean matched AK4 pT shift [%]",ptEdges);
        profiles.emplace_back("mjj_nvtx","Vertex multiplicity","Mean mass shift [%]",uniform(20,-0.5,99.5));
        profiles.emplace_back("pt_ak4_eta_rawpt","Old leading AK4 eta","Old pT / JEC [GeV]",uniform(50,-2.5,2.5),ptEdges);
        for(const auto& h:makeHists()) overlap.emplace_back(h.counts.size(),0);
    }
    void merge(const Paired& p) {
        matched+=p.matched;onlyOld+=p.onlyOld;onlyNew+=p.onlyNew;jetMatched+=p.jetMatched;jetUnmatched+=p.jetUnmatched;reordered+=p.reordered;
        for(int i=0;i<4;++i)migration[i]+=p.migration[i];
        for(std::size_t i=0;i<thresholds.size();++i)for(int j=0;j<4;++j)thresholdMigration[i][j]+=p.thresholdMigration[i][j];
        for(std::size_t i=0;i<shifts.size();++i)shifts[i].merge(p.shifts[i]);
        for(std::size_t i=0;i<profiles.size();++i)profiles[i].merge(p.profiles[i]);
        for(std::size_t i=0;i<overlap.size();++i)for(std::size_t j=0;j<overlap[i].size();++j)overlap[i][j]+=p.overlap[i][j];
        for(const auto& v:p.runs)for(int i=0;i<3;++i)runs[v.first][i]+=v.second[i];
        for(const auto& v:p.runShifts)for(int i=0;i<3;++i)runShifts[v.first][i].merge(v.second[i]);
    }
    void fill(const Event& a,const Event& b,bool ba,bool bb,bool sa,bool sb,
              const Bins& ia,const Bins& ib,const Key& k,const Options& o) {
        ++matched;++migration[2*sa+sb];
        auto& r=runs[k.run];r[0]+=sa;r[1]+=sb;r[2]+=sa&&sb;
        for(std::size_t i=0;i<thresholds.size();++i)++thresholdMigration[i][2*(ba&&ia[0]>=0&&a[Mjj]>thresholds[i])+(bb&&ib[0]>=0&&b[Mjj]>thresholds[i])];
        for(std::size_t i=0;i<ia.size();++i)if(ia[i]>=0 && ia[i]==ib[i])++overlap[i][ia[i]];
        if(!(sa&&sb))return;
        const auto shift=[](double x,double y){return x>0?100*(y-x)/x:std::numeric_limits<double>::quiet_NaN();};
        const bool inclusive=o.preset=="all_stored";
        const double dm=!inclusive || (ia[1]>=0 && ib[1]>=0)?shift(a[Mjj],b[Mjj]):std::numeric_limits<double>::quiet_NaN();
        shifts[0].fill(dm);
        shifts[1].fill(!inclusive || (ia[2]>=0 && ib[2]>=0)?shift(a[AKMjj],b[AKMjj]):std::numeric_limits<double>::quiet_NaN());
        const double dpWide=!inclusive || (ia[3]>=0 && ib[3]>=0)?shift(a[Pt1],b[Pt1]):std::numeric_limits<double>::quiet_NaN();
        shifts[2].fill(dpWide);
        shifts[3].fill(!inclusive || (ia[6]>=0 && ib[6]>=0)?shift(a[Pt2],b[Pt2]):std::numeric_limits<double>::quiet_NaN());
        profiles[0].fill(a[Mjj],dm);profiles[3].fill(a[NVtx],dm);
        runShifts[k.run][0].fill(dm);runShifts[k.run][1].fill(dpWide);
        if(inclusive) {
            for(int field:{AKPt1,AKPt2,AKEta1,AKEta2,AKPhi1,AKPhi2})
                if(!std::isfinite(a[field]) || !std::isfinite(b[field])) {++jetUnmatched;return;}
            if(a[AKPt1]<=0 || a[AKPt2]<=0 || b[AKPt1]<=0 || b[AKPt2]<=0) {++jetUnmatched;return;}
        }
        auto dr=[&](int i,int j){return std::hypot(a[AKEta1+i]-b[AKEta1+j],dphi(a[AKPhi1+i],b[AKPhi1+j]));};
        const bool swap=dr(0,1)+dr(1,0)<dr(0,0)+dr(1,1);
        const int j=swap?1:0;
        if(dr(0,j)>=o.jetDR || dr(1,1-j)>=o.jetDR){++jetUnmatched;return;}
        ++jetMatched;reordered+=swap;
        const double dp=shift(a[AKPt1],b[AKPt1+j]);
        const double raw=a[Jec1]>0?a[AKPt1]/a[Jec1]:std::numeric_limits<double>::quiet_NaN();
        shifts[4].fill(dp);
        shifts[5].fill(!inclusive || (std::isfinite(a[Jec1]) && std::isfinite(b[Jec1+j]) && a[Jec1]>0 && b[Jec1+j]>0)?shift(a[Jec1],b[Jec1+j]):std::numeric_limits<double>::quiet_NaN());
        profiles[1].fill(a[AKEta1],dp);profiles[2].fill(raw,dp);profiles[4].fill(a[AKEta1],dp,raw);
        runShifts[k.run][2].fill(dp);
    }
};

using State = nlohmann::json;

template<class T> void restoreValues(const State& state,T& values) {
    if(!state.is_array() || state.size()!=values.size()) throw std::runtime_error("Checkpoint array dimensions do not match this reducer");
    state.get_to(values);
}
template<class T> State momentState(const T& values) {
    State out=State::array();
    for(const auto& v:values) out.push_back({v.n,v.sum,v.sum2});
    return out;
}
template<class T> void restoreMoments(const State& state,T& values) {
    if(!state.is_array() || state.size()!=values.size()) throw std::runtime_error("Checkpoint moment dimensions do not match this reducer");
    for(std::size_t i=0;i<values.size();++i) {
        const auto& row=state.at(i);
        if(row.size()!=3) throw std::runtime_error("Invalid checkpoint moment");
        values[i]={row.at(0).get<Count>(),row.at(1).get<double>(),row.at(2).get<double>()};
    }
}
State histState(const std::vector<Hist>& h) {
    State out=State::array();for(const auto& v:h) out.push_back({v.counts,v.invalid,v.unavailable});return out;
}
void restoreHists(const State& state,std::vector<Hist>& h) {
    if(state.size()!=h.size()) throw std::runtime_error("Checkpoint histogram count does not match this reducer");
    for(std::size_t i=0;i<h.size();++i) {
        restoreValues(state.at(i).at(0),h[i].counts);
        h[i].invalid=state.at(i).at(1).get<Count>();h[i].unavailable=state.at(i).at(2).get<Count>();
    }
}
State profileState(const std::vector<Profile>& p) {
    State out=State::array();for(const auto& v:p) out.push_back(momentState(v.values));return out;
}
void restoreProfiles(const State& state,std::vector<Profile>& p) {
    if(state.size()!=p.size()) throw std::runtime_error("Checkpoint profile count does not match this reducer");
    for(std::size_t i=0;i<p.size();++i) restoreMoments(state.at(i),p[i].values);
}
State summaryState(const Summary& s) {
    State runs=State::array(),lumis=State::array();
    for(const auto& item:s.runs) {
        const auto& r=item.second;
        runs.push_back({item.first,r.stored,r.base,r.selected,r.signs,r.etaZero,momentState(r.means)});
    }
    for(const auto& item:s.lumis) {
        const auto& l=item.second;lumis.push_back({item.first,l.stored,l.base,l.selected,l.hashSum,l.hashXor});
    }
    return {{"h",histState(s.h)},{"p",profileState(s.p)},{"cuts",s.cuts},{"thresholds",s.thresholdCounts},
            {"runs",runs},{"lumis",lumis},{"valid",s.validDijets},{"input",s.inputEntries},
            {"cert_rejected",s.certificationRejected},{"json_rejected",s.passJSONRejected}};
}
void restoreSummary(const State& state,Summary& s) {
    restoreHists(state.at("h"),s.h);restoreProfiles(state.at("p"),s.p);
    restoreValues(state.at("cuts"),s.cuts);restoreValues(state.at("thresholds"),s.thresholdCounts);
    s.validDijets=state.at("valid").get<Count>();s.inputEntries=state.at("input").get<Count>();
    s.certificationRejected=state.at("cert_rejected").get<Count>();s.passJSONRejected=state.at("json_rejected").get<Count>();
    for(const auto& row:state.at("runs")) {
        RunStat r;r.stored=row.at(1).get<Count>();r.base=row.at(2).get<Count>();r.selected=row.at(3).get<Count>();
        restoreValues(row.at(4),r.signs);restoreValues(row.at(5),r.etaZero);restoreMoments(row.at(6),r.means);
        if(!s.runs.emplace(row.at(0).get<std::uint32_t>(),r).second) throw std::runtime_error("Duplicate run in checkpoint");
    }
    for(const auto& row:state.at("lumis")) {
        LSStat l{row.at(1).get<Count>(),row.at(2).get<Count>(),row.at(3).get<Count>(),row.at(4).get<Count>(),row.at(5).get<Count>()};
        if(!s.lumis.emplace(row.at(0).get<Count>(),l).second) throw std::runtime_error("Duplicate lumisection in checkpoint");
    }
    if(s.inputEntries!=s.cuts[0]+s.certificationRejected+s.passJSONRejected) throw std::runtime_error("Invalid checkpoint event accounting");
}
State pairedState(const Paired& p) {
    State runs=State::array(),shifts=State::array();
    for(const auto& item:p.runs) runs.push_back({item.first,item.second});
    for(const auto& item:p.runShifts) shifts.push_back({item.first,momentState(item.second)});
    return {{"counts",{p.matched,p.onlyOld,p.onlyNew,p.jetMatched,p.jetUnmatched,p.reordered}},
            {"migration",p.migration},{"thresholds",p.thresholdMigration},{"h",histState(p.shifts)},
            {"p",profileState(p.profiles)},{"overlap",p.overlap},{"runs",runs},{"shifts",shifts}};
}
void restorePaired(const State& state,Paired& p) {
    std::array<Count,6> counts;restoreValues(state.at("counts"),counts);
    p.matched=counts[0];p.onlyOld=counts[1];p.onlyNew=counts[2];p.jetMatched=counts[3];p.jetUnmatched=counts[4];p.reordered=counts[5];
    restoreValues(state.at("migration"),p.migration);restoreValues(state.at("thresholds"),p.thresholdMigration);
    restoreHists(state.at("h"),p.shifts);restoreProfiles(state.at("p"),p.profiles);
    if(state.at("overlap").size()!=p.overlap.size()) throw std::runtime_error("Invalid checkpoint overlap dimensions");
    for(std::size_t i=0;i<p.overlap.size();++i) restoreValues(state.at("overlap").at(i),p.overlap[i]);
    for(const auto& row:state.at("runs")) {
        std::array<Count,3> values;restoreValues(row.at(1),values);
        if(!p.runs.emplace(row.at(0).get<std::uint32_t>(),values).second) throw std::runtime_error("Duplicate paired run in checkpoint");
    }
    for(const auto& row:state.at("shifts")) {
        std::array<Moment,3> values;restoreMoments(row.at(1),values);
        if(!p.runShifts.emplace(row.at(0).get<std::uint32_t>(),values).second) throw std::runtime_error("Duplicate paired shift in checkpoint");
    }
}

class Checkpoints {
    fs::path directory;
    int lock=-1;
    static Count checksum(const std::vector<std::uint8_t>& bytes,std::size_t begin=0) {
        Count hash=14695981039346656037ULL;
        for(std::size_t i=begin;i<bytes.size();++i) {hash^=bytes[i];hash*=1099511628211ULL;}
        return hash;
    }
    State read(const fs::path& path) const {
        std::ifstream in(path,std::ios::binary);
        if(!in) throw std::runtime_error("Cannot read checkpoint: "+path.string());
        std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(in)),{});
        if(in.bad() || bytes.size()<9) throw std::runtime_error("Incomplete checkpoint: "+path.string());
        Count expected=0;for(int i=0;i<8;++i) expected|=Count(bytes[i])<<(8*i);
        if(checksum(bytes,8)!=expected) throw std::runtime_error("Checkpoint checksum mismatch: "+path.string());
        const auto* begin=reinterpret_cast<const char*>(bytes.data());
        return State::from_cbor(begin+8,begin+bytes.size());
    }
    void write(const fs::path& path,const State& state) const {
        const auto bytes=State::to_cbor(state);const auto hash=checksum(bytes);
        const fs::path temporary=path.string()+".tmp."+std::to_string(getpid());
        try {
            std::ofstream out(temporary,std::ios::binary|std::ios::trunc);
            if(!out) throw std::runtime_error("Cannot write checkpoint: "+temporary.string());
            for(int i=0;i<8;++i) out.put(static_cast<char>((hash>>(8*i))&255));
            out.write(reinterpret_cast<const char*>(bytes.data()),bytes.size());out.close();
            if(!out) throw std::runtime_error("Failed writing checkpoint: "+temporary.string());
            const int fd=open(temporary.c_str(),O_RDWR);
            if(fd<0) throw std::runtime_error("Cannot sync checkpoint: "+temporary.string());
            const int status=fsync(fd);close(fd);
            if(status!=0) throw std::runtime_error("Cannot sync checkpoint: "+temporary.string());
            fs::rename(temporary,path);
        } catch(...) {std::error_code error;fs::remove(temporary,error);throw;}
    }
public:
    Checkpoints(const Options& o,const State& identity) {
        const auto output=summaryPaths(o)[0];
        auto stem=output.stem().string();stem.resize(stem.size()-std::string("_selected").size());
        directory=output.parent_path()/("."+stem+".checkpoint");fs::create_directories(directory);
        lock=open((directory/"lock").c_str(),O_CREAT|O_RDWR|O_CLOEXEC,0600);
        if(lock<0) throw std::runtime_error("Cannot open checkpoint lock: "+directory.string());
        if(flock(lock,LOCK_EX|LOCK_NB)!=0) {close(lock);lock=-1;throw std::runtime_error("Checkpoint is locked by another reducer (or filesystem locking is unavailable): "+directory.string());}
        try {
            const auto config=directory/"config.cbor";
            if(o.force) {
                for(const auto& item:fs::directory_iterator(directory)) {
                    const auto name=item.path().filename().string();
                    if(item.is_regular_file() && (item.path().extension()==".cbor" || name.find(".cbor.tmp.")!=std::string::npos)) fs::remove(item.path());
                }
            }
            if(fs::exists(config)) {
                if(!o.resume) throw std::runtime_error("Saved progress exists: "+directory.string()+"; use --resume to continue or --force to start over");
                if(read(config)!=identity) throw std::runtime_error("Checkpoint inputs or options differ; use the original inputs/options (including --threads and --chunk-entries) or --force to start over");
            } else {
                for(const auto& item:fs::directory_iterator(directory))
                    if(item.path().extension()==".cbor") throw std::runtime_error("Checkpoint configuration is missing; use --force to start over: "+directory.string());
                if(o.resume) for(const auto& path:summaryPaths(o))
                    if(fs::exists(path)) throw std::runtime_error("Cannot resume existing summaries without their matching checkpoint; use --force to start over");
                write(config,identity);
            }
            std::cout<<"Checkpoint directory: "<<directory<<" (saved after each completed chunk)"<<std::endl;
        } catch(...) {flock(lock,LOCK_UN);close(lock);lock=-1;throw;}
    }
    ~Checkpoints(){if(lock>=0){flock(lock,LOCK_UN);close(lock);}}
    Checkpoints(const Checkpoints&)=delete;
    Checkpoints& operator=(const Checkpoints&)=delete;
    void prepare(const std::string& phase,const State& identity) const {
        const auto path=directory/(phase+".plan.cbor");
        if(fs::exists(path)) {
            if(read(path)!=identity) throw std::runtime_error("Checkpoint "+phase+" input metadata or task boundaries changed; use --force to start over");
        } else {
            for(const auto& item:fs::directory_iterator(directory))
                if(item.path().filename().string().find(phase+".worker.")==0 && item.path().extension()==".cbor")
                    throw std::runtime_error("Checkpoint task plan is missing; use --force to start over: "+path.string());
            write(path,identity);
        }
    }
    State load(const std::string& phase,unsigned slot) const {
        const auto path=directory/(phase+".worker."+std::to_string(slot)+".cbor");
        return fs::exists(path)?read(path):State();
    }
    void save(const std::string& phase,unsigned slot,const State& state) const {
        write(directory/(phase+".worker."+std::to_string(slot)+".cbor"),state);
    }
};

State fileIdentity(TFile& file,const std::string& path) {
    State state={{"path",path},{"uuid",file.GetUUID().AsString()},{"bytes",file.GetSize()},
                 {"modified",file.GetModificationDate().Get()}};
    if(path.find("://")==std::string::npos) state["mtime"]=fs::last_write_time(path).time_since_epoch().count();
    return state;
}

struct Reader {
    Event e{};
    std::unique_ptr<TFile> file;
    TTree* tree=nullptr;
    std::vector<TBranch*> activeBranches;
    Reader(const std::string& path,const Options& o,long long begin=0,long long end=-1) {
        file.reset(TFile::Open(path.c_str(),"READ"));
        if(!file || file->IsZombie())throw std::runtime_error("Cannot open ROOT file: "+path);
        tree=file->Get<TTree>(o.tree.c_str());
        if(!tree)throw std::runtime_error("Missing tree "+o.tree+" in "+path);
        if(o.mc) for(const char* name:{"genWeight","Generator_weight","LHEWeight_originalXWGTUP"})
            if(tree->GetBranch(name)) throw std::runtime_error(path+": generator-weight branch "+name+" requires weighted-generator normalization; this reducer supports unit-weight QCD only");
        tree->SetBranchStatus("*",0);
        tree->SetCacheSize(o.cacheMB*1024*1024);
        tree->SetCacheEntryRange(begin,end<0?tree->GetEntries():end);
        for(int i=0;i<NFields;++i) {
            if(o.mc && (i==Run || i==Lumi || i==EventId || i==Json || i==Trigger)) continue;
            auto* leaf=tree->GetLeaf(branches[i]);
            if(!leaf || std::string(leaf->GetTypeName())!="Double_t" || leaf->GetLenStatic()!=1 || leaf->GetLeafCount())
                throw std::runtime_error(path+": expected scalar Double_t branch "+branches[i]);
            tree->SetBranchStatus(branches[i],1);
            TBranch* branch=nullptr;
            if(tree->SetBranchAddress(branches[i],&e[i],&branch)<0 || !branch)throw std::runtime_error("Cannot bind branch "+std::string(branches[i]));
            activeBranches.push_back(branch);
            if(o.cacheMB)tree->AddBranchToCache(branches[i],false);
        }
        tree->StopCacheLearningPhase();
    }
    const Event& read(long long entry) {
        if(tree->LoadTree(entry)<0)throw std::runtime_error("ROOT read failed at entry "+std::to_string(entry)+" in "+file->GetName());
        for(auto* branch:activeBranches)
            if(branch->GetEntry(entry)<=0)throw std::runtime_error("ROOT read failed for branch "+std::string(branch->GetName())+" at entry "+std::to_string(entry)+" in "+file->GetName());
        return e;
    }
};
struct FileInfo { std::string oldPath,newPath; long long oldEntries=0,newEntries=0,oldLimit=0,newLimit=0;std::vector<long long> starts;State oldIdentity,newIdentity; };
struct Task {std::size_t file;long long begin,end;};

class Progress {
    std::string phase,unit;
    Count total,initial;
    bool interactive;
    Clock::time_point started=Clock::now(),last=started;
    std::size_t previousWidth=0;

    static std::string compact(double value) {
        const std::array<const char*,5> suffix={"","k","M","G","T"};
        std::size_t scale=0;
        while(value>=1000 && scale+1<suffix.size()){value/=1000;++scale;}
        std::ostringstream out;
        out<<std::fixed<<std::setprecision(scale?(value<10?2:1):0)<<value<<suffix[scale];
        return out.str();
    }
    static std::string duration(double value) {
        const Count seconds=static_cast<Count>(std::max(0.0,value));
        std::ostringstream out;
        if(seconds>=86400)out<<seconds/86400<<'d'<<' ';
        if(seconds>=3600)out<<std::setfill('0')<<std::setw(2)<<(seconds/3600)%24<<':';
        out<<std::setfill('0')<<std::setw(2)<<(seconds/60)%60<<':'<<std::setw(2)<<seconds%60;
        return out.str();
    }
    void draw(Count completed,bool finished,bool success) {
        const auto now=Clock::now();
        const double seconds=std::chrono::duration<double>(now-started).count();
        const double fraction=total?std::min(1.0,double(completed)/total):(finished&&success?1.0:0.0);
        const double rate=seconds>0?(completed-std::min(completed,initial))/seconds:0;
        const std::string eta=finished&&success?"00:00":(rate>0?duration(std::ceil((total-std::min(completed,total))/rate)):"--:--");
        std::ostringstream percent;percent<<std::fixed<<std::setprecision(1)<<100*fraction;
        const std::string metrics=" | "+compact(rate)+" "+unit+"/s | elapsed "+duration(seconds)+" | ETA "+eta
            +(finished?(success?" | done":" | stopped"):"");
        std::string tail=" "+percent.str()+"% "+compact(completed)+"/"+compact(total)+metrics;
        int width=140;
        if(interactive){winsize size{};width=ioctl(STDERR_FILENO,TIOCGWINSZ,&size)==0&&size.ws_col?size.ws_col:100;}
        if(width-static_cast<int>(phase.size()+tail.size())-4<10)tail=" "+percent.str()+"%"+metrics;
        const int barWidth=std::max(4,std::min(30,width-static_cast<int>(phase.size()+tail.size())-4));
        const int filled=static_cast<int>(fraction*barWidth);
        std::string bar(filled,'=');
        if(filled<barWidth){bar+='>';bar+=std::string(barWidth-filled-1,'.');}
        std::string line=phase+" ["+bar+"]"+tail;
        if(interactive){std::cerr<<'\r'<<line;
            if(previousWidth>line.size())std::cerr<<std::string(previousWidth-line.size(),' ');
            previousWidth=line.size();
            if(finished)std::cerr<<'\n';
        }else std::cerr<<line<<'\n';
        std::cerr.flush();last=now;
    }
public:
    Progress(std::string stage,Count count,std::string units,Count resumed=0):phase(std::move(stage)),unit(std::move(units)),total(count),initial(resumed),
        interactive(isatty(STDERR_FILENO) && (!std::getenv("TERM") || std::string(std::getenv("TERM"))!="dumb")) {draw(initial,false,true);}
    void update(Count completed) {
        if(std::chrono::duration<double>(Clock::now()-last).count()>=(interactive?0.25:10.0))draw(completed,false,true);
    }
    void finish(Count completed,bool success) {draw(completed,true,success);}
};

template<class F> void withProgress(Progress& progress,const std::atomic<Count>& completed,F work) {
    std::atomic<bool> done{false};std::exception_ptr error;
    std::thread job([&]{try{work();}catch(...){error=std::current_exception();}done=true;});
    while(!done){std::this_thread::sleep_for(std::chrono::milliseconds(100));progress.update(completed.load(std::memory_order_relaxed));}
    job.join();progress.finish(completed.load(std::memory_order_relaxed),!error);
    if(error)std::rethrow_exception(error);
}

template<class F> void parallel(std::size_t size,unsigned threads,F fn) {
    std::atomic<std::size_t> next{0};std::atomic<bool> failed{false};std::exception_ptr error;std::mutex mutex;
    std::vector<std::thread> workers;
    for(unsigned slot=0;slot<std::min<std::size_t>(threads,size);++slot) workers.emplace_back([&,slot] {
        try {while(!failed){const auto i=next.fetch_add(1);if(i>=size)break;fn(i,slot);}}
        catch(...){failed=true;std::lock_guard<std::mutex> lock(mutex);if(!error)error=std::current_exception();}
    });
    for(auto& t:workers)t.join();
    if(error)std::rethrow_exception(error);
}

struct MCFile {
    std::string path;
    Count generated=0;
    long long entries=0;
    std::vector<long long> starts;
    State identity;
};
struct MCComponent {
    std::string source;
    double xsec=0,weight=0;
    Count generated=0,stored=0;
    std::vector<MCFile> files;
    std::array<Summary,2> summaries;
};

void expandMCInput(const std::string& value,std::vector<std::string>& out,unsigned depth=0) {
    if(depth>8) throw std::runtime_error("MC input list recursion exceeds eight levels");
    if(value.empty()) throw std::runtime_error("Empty MC input path");
    if(value[0]=='@' || (fs::is_regular_file(value) && fs::path(value).extension()==".txt")) {
        const fs::path path=value[0]=='@'?value.substr(1):value;
        std::ifstream stream(path);
        if(!stream) throw std::runtime_error("Cannot open MC input list: "+path.string());
        std::string line;
        while(std::getline(stream,line)) {
            line=trim(line);
            if(line.empty() || line[0]=='#') continue;
            const bool nested=line[0]=='@';
            fs::path relative=nested?line.substr(1):line;
            if(line.find("://")==std::string::npos && relative.is_relative())
                line=(nested?"@":"")+(path.parent_path()/relative).string();
            expandMCInput(line,out,depth+1);
        }
    } else if(eos_utils::parse_xrootd_path(value)) {
        const auto expanded=eos_utils::expand_remote_input(value);
        out.insert(out.end(),expanded.begin(),expanded.end());
    } else expandInput(value,out,depth);
}

std::string inputIdentity(const std::string& path) {
    if(const auto remote=eos_utils::parse_xrootd_path(path)) return remote->path;
    if(path.find("://")!=std::string::npos) return path;
    return fs::weakly_canonical(path).string();
}

std::vector<MCComponent> loadMCComponents(const Options& o) {
    if(o.mcLists.empty()) return {};
    std::set<std::string> seen;
    for(const auto* paths:{&o.input,&o.oldInput,&o.newInput})
        for(const auto& path:*paths) seen.insert(inputIdentity(path));
    std::vector<MCComponent> components;
    for(const auto& list:o.mcLists) {
        const fs::path path=fs::absolute(list);
        std::ifstream stream(path);
        if(!stream) throw std::runtime_error("Cannot read MC list: "+path.string());
        std::string line;
        unsigned row=0;
        while(std::getline(stream,line)) {
            ++row;line=trim(line);
            if(line.empty() || line[0]=='#') continue;
            const std::string location=path.string()+":"+std::to_string(row);
            std::istringstream fields(line);
            MCComponent component;
            if(!(fields>>std::quoted(component.source)>>component.xsec))
                throw std::runtime_error(location+": expected path_or_glob xsec_pb");
            fields>>std::ws;
            if(fields.peek()!=std::char_traits<char>::eof() && fields.peek()!='#')
                throw std::runtime_error(location+": expected only path_or_glob xsec_pb; include any generator filter in xsec_pb");
            if(!std::isfinite(component.xsec) || component.xsec<=0)
                throw std::runtime_error(location+": cross section must be finite and positive");
            const bool nested=!component.source.empty() && component.source[0]=='@';
            fs::path relative=nested?component.source.substr(1):component.source;
            if(component.source.find("://")==std::string::npos && relative.is_relative())
                component.source=(nested?"@":"")+(path.parent_path()/relative).string();
            std::vector<std::string> expanded;
            expandMCInput(component.source,expanded);
            if(expanded.empty()) throw std::runtime_error(location+": no MC ROOT files");
            for(const auto& file:expanded) {
                if(!seen.insert(inputIdentity(file)).second)
                    throw std::runtime_error("MC file repeated across components or reused as data: "+file);
                MCFile info;info.path=file;component.files.push_back(std::move(info));
            }
            components.push_back(std::move(component));
        }
    }
    if(!o.mcLists.empty() && components.empty()) throw std::runtime_error("MC lists contain no components");
    return components;
}

bool countParameter(TFile& file,const char* name,Count& result) {
    TObject* object=file.Get(name);
    if(!object) return false;
    long double value=-1;
    if(const auto* p=dynamic_cast<TParameter<Long64_t>*>(object)) value=p->GetVal();
    else if(const auto* p=dynamic_cast<TParameter<Long_t>*>(object)) value=p->GetVal();
    else if(const auto* p=dynamic_cast<TParameter<int>*>(object)) value=p->GetVal();
    else if(const auto* p=dynamic_cast<TParameter<double>*>(object)) value=p->GetVal();
    if(!std::isfinite(value) || value<0 || std::floor(value)!=value || value>std::numeric_limits<Long64_t>::max())
        throw std::runtime_error(std::string(file.GetName())+": invalid integer counter "+name);
    result=static_cast<Count>(value);
    return true;
}

Count generatedEvents(TFile& file,long long entries) {
    const std::string path=file.GetName();
    if(entries<0 || file.TestBit(TFile::kRecovered)) throw std::runtime_error("Invalid or recovered MC ROOT file: "+path);
    Count expected=0,processed=0,actual=0;
    const bool hasExpected=countParameter(file,"metaData/expectedEvents",expected);
    const bool hasProcessed=countParameter(file,"metaData/processedEvents",processed);
    const bool hasActual=countParameter(file,"metaData/actualEvents",actual);
    if(hasExpected && hasProcessed && expected!=processed)
        throw std::runtime_error(path+": incomplete MC processing (processedEvents != expectedEvents)");
    if(hasActual && actual!=Count(entries)) throw std::runtime_error(path+": actualEvents does not match stored tree entries");
    const bool metadata=hasExpected || hasProcessed;
    Count generated=hasExpected?expected:processed;
    auto* object=file.Get("DijetFilter/EventCount/EventCounter");
    auto* counter=dynamic_cast<TH1*>(object);
    if(object && (!counter || counter->GetNbinsX()<3)) throw std::runtime_error(path+": invalid EventCounter histogram");
    const bool overflow=counter && metadata && counter->IsA()==TH1I::Class() &&
        generated>Count(std::numeric_limits<int>::max()) && counter->GetBinContent(2)!=static_cast<double>(generated);
    if(counter && !overflow) {
        std::array<Count,3> counts{};
        for(int i=0;i<3;++i) {
            const double n=counter->GetBinContent(i+1);
            if(!std::isfinite(n) || n<0 || std::floor(n)!=n || n>=9007199254740992.0)
                throw std::runtime_error(path+": invalid EventCounter value");
            counts[i]=static_cast<Count>(n);
        }
        if(counts[0]<counts[1] || counts[1]<Count(entries) || counts[2]!=Count(entries) ||
           (metadata && counts[1]!=generated)) throw std::runtime_error(path+": inconsistent MC event counters");
        generated=counts[0];
    } else if(!metadata) throw std::runtime_error(path+": missing generated-event normalization metadata");
    if(generated<Count(entries)) throw std::runtime_error(path+": generated count is smaller than stored entries");
    return generated;
}

Count reduceMC(std::vector<MCComponent>& components,std::array<Options,2> options,unsigned& workers,Checkpoints& checkpoints,Count& restoredEntries) {
    if(components.empty()) return 0;
    for(auto& option:options) {option.mc=true;option.requirePassJSON=false;option.certification=Certification{};}
    const auto& o=options[0];
    std::vector<std::pair<std::size_t,std::size_t>> files;
    for(std::size_t c=0;c<components.size();++c)
        for(std::size_t f=0;f<components[c].files.size();++f) files.emplace_back(c,f);
    std::cout<<"QCD: "<<components.size()<<" components, "<<files.size()<<" files; unit generator weights; JSON and HLT cuts disabled"<<std::endl;
    std::atomic<Count> validated{0};Progress validation("Validate QCD",files.size(),"files");
    withProgress(validation,validated,[&]{parallel(files.size(),o.threads,[&](std::size_t i,unsigned){
        auto& file=components[files[i].first].files[files[i].second];
        Reader reader(file.path,o);file.entries=reader.tree->GetEntries();file.generated=generatedEvents(*reader.file,file.entries);
        file.identity=fileIdentity(*reader.file,file.path);
        auto clusters=reader.tree->GetClusterIterator(0);long long last=0,next;
        file.starts.push_back(0);
        while((next=clusters())<file.entries) if(next-last>=o.chunk) {file.starts.push_back(next);last=next;}
        if(file.entries>0) file.starts.push_back(file.entries);
        ++validated;
    });});
    std::vector<Task> tasks;Count total=0;
    for(std::size_t i=0;i<files.size();++i) {
        auto& component=components[files[i].first];const auto& file=component.files[files[i].second];
        if(file.generated>std::numeric_limits<Count>::max()-component.generated) throw std::runtime_error("MC generated counter overflow");
        component.generated+=file.generated;component.stored+=file.entries;total+=file.entries;
        for(std::size_t j=1;j<file.starts.size();++j) tasks.push_back({i,file.starts[j-1],file.starts[j]});
    }
    for(auto& component:components) {
        if(!component.generated) throw std::runtime_error("MC component has zero generated events: "+component.source);
        component.weight=component.xsec/component.generated;
        if(!std::isfinite(component.weight) || component.weight<=0 ||
           !std::isfinite(component.weight*component.weight*component.stored))
            throw std::runtime_error("Invalid MC component weight or variance: "+component.source);
    }
    workers=static_cast<unsigned>(std::min<std::size_t>(o.threads,tasks.size()));
    State planFiles=State::array(),planTasks=State::array();
    for(const auto& id:files) {
        const auto& file=components[id.first].files[id.second];
        planFiles.push_back({file.identity,file.entries,file.generated});
    }
    for(const auto& task:tasks) planTasks.push_back({task.file,task.begin,task.end});
    checkpoints.prepare("mc",{{"files",planFiles},{"tasks",planTasks},{"workers",workers}});
    std::vector<std::map<std::size_t,std::array<Summary,2>>> summaries(workers);
    std::vector<std::vector<std::size_t>> finished(workers);
    std::vector<bool> completed(tasks.size(),false);
    Count restored=0,restoredTasks=0;
    for(unsigned slot=0;slot<workers;++slot) {
        const auto state=checkpoints.load("mc",slot);if(state.is_null()) continue;
        finished[slot]=state.at("done").get<std::vector<std::size_t>>();
        std::map<std::size_t,Count> expected;
        for(const auto i:finished[slot]) {
            if(i>=tasks.size() || completed[i]) throw std::runtime_error("Invalid or duplicate MC task in checkpoint");
            completed[i]=true;++restoredTasks;
            const auto& task=tasks[i];const Count count=task.end-task.begin;
            expected[files[task.file].first]+=count;restored+=count;
        }
        for(const auto& row:state.at("components")) {
            const auto id=row.at(0).get<std::size_t>();
            if(!expected.count(id) || !summaries[slot].try_emplace(id).second) throw std::runtime_error("Invalid MC component in checkpoint");
            auto& views=summaries[slot].at(id);
            for(std::size_t view=0;view<options.size();++view) {
                restoreSummary(row.at(1).at(view),views[view]);
                if(views[view].inputEntries!=expected.at(id)) throw std::runtime_error("MC checkpoint task/event counts disagree");
            }
        }
        if(summaries[slot].size()!=expected.size()) throw std::runtime_error("Missing MC component in checkpoint");
    }
    if(restoredTasks) std::cout<<"Resuming QCD: "<<restoredTasks<<"/"<<tasks.size()<<" chunks complete; skipping "<<restored<<" entries"<<std::endl;
    restoredEntries=restored;
    std::atomic<Count> processed{restored};Progress scan("Scan QCD",total,"entries",restored);
    withProgress(scan,processed,[&]{parallel(tasks.size(),workers,[&](std::size_t i,unsigned slot){
        if(completed[i]) return;
        const auto& task=tasks[i];const auto id=files[task.file];const auto& component=components[id.first];
        Reader reader(component.files[id.second].path,o,task.begin,task.end);std::array<Summary,2> views;Count pending=0;
        for(long long entry=task.begin;entry<task.end;++entry) {
            const auto& event=reader.read(entry);
            for(std::size_t view=0;view<options.size();++view)
                views[view].fill(event,options[view],{static_cast<Count>(entry),0,0});
            if(++pending>=50000) {processed+=pending;pending=0;}
        }
        for(std::size_t view=0;view<options.size();++view) summaries[slot][id.first][view].merge(views[view]);
        finished[slot].push_back(i);
        State saved=State::array();
        for(const auto& item:summaries[slot]) saved.push_back({item.first,{summaryState(item.second[0]),summaryState(item.second[1])}});
        checkpoints.save("mc",slot,{{"done",finished[slot]},{"components",saved}});
        processed+=pending;
    });});
    for(const auto& slot:summaries) for(const auto& item:slot)
        for(std::size_t view=0;view<options.size();++view) components[item.first].summaries[view].merge(item.second[view]);
    return processed.load();
}

std::string quote(const std::string& s) {
    std::ostringstream out;out << '"';
    for(unsigned char c:s){if(c=='"' || c=='\\')out << '\\' << c;else if(c=='\n')out << "\\n";
        else if(c<32)out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << int(c) << std::dec;else out << c;}
    out << '"';return out.str();
}
std::string hex(Count n){std::ostringstream o;o<<std::hex<<std::setw(16)<<std::setfill('0')<<n;return o.str();}
template<class T> void arrayJSON(std::ostream& out,const T& v) {
    out << '[';bool first=true;for(const auto& x:v){if(!first)out<<',';first=false;out<<x;}out<<']';
}
void momentsJSON(std::ostream& out,const std::vector<Moment>& v) {
    out << "{\"count\":";std::vector<Count> n;std::vector<double> s,s2;
    for(const auto& m:v){n.push_back(m.n);s.push_back(m.sum);s2.push_back(m.sum2);}
    arrayJSON(out,n);out<<",\"sum\":";arrayJSON(out,s);out<<",\"sum2\":";arrayJSON(out,s2);out<<'}';
}
void histJSON(std::ostream& out,const std::vector<Hist>& h) {
    out<<'{';bool first=true;for(const auto& v:h){if(!first)out<<',';first=false;
        out<<quote(v.name)<<":{\"label\":"<<quote(v.label)<<",\"edges\":";arrayJSON(out,v.edges);
        out<<",\"counts\":";arrayJSON(out,v.counts);out<<",\"invalid\":"<<v.invalid<<",\"unavailable\":"<<v.unavailable<<'}';}out<<'}';
}
void configJSON(std::ostream& out,const Options& o,bool data=true) {
    out<<"{\"preset\":"<<quote(o.preset)<<",\"mjj_min\":"<<o.minMass<<",\"pt_min\":"<<o.minPt
       <<",\"eta_max\":"<<o.maxEta<<",\"deta_max\":"<<o.maxDEta<<",\"balance_alpha_max\":"<<o.balanceAlpha
       <<",\"jet_match_dr\":"<<o.jetDR;
    if(data && o.certification.active()) out<<",\"certification_mask\":"<<o.certification.canonical;
    if(data && o.requirePassJSON) out<<",\"require_pass_json\":true";
    out<<'}';
}
void mcJSON(std::ostream& out,const std::vector<MCComponent>& components,const Options& o,std::size_t view) {
    out<<"{\"config\":";configJSON(out,o,false);
    out<<",\"tree\":"<<quote(o.tree)<<",\"normalization\":\"cross_section_pb\",\"weight_convention\":\"unit\""
       <<",\"selection\":\"preset kinematics without data certification, PassJSON or HLT; generated counters before skim cuts\""
       <<",\"partial\":false,\"components\":[";
    double selected=0,variance=0;
    for(std::size_t i=0;i<components.size();++i) {
        if(i) out<<',';
        const auto& c=components[i];
        const auto& summary=c.summaries[view];
        selected+=summary.cuts.back()*c.weight;variance+=summary.cuts.back()*c.weight*c.weight;
        out<<"{\"source\":"<<quote(c.source)<<",\"xsec_pb\":"<<c.xsec
           <<",\"generated_events\":"<<c.generated<<",\"stored_entries\":"<<c.stored
           <<",\"selected_entries\":"<<summary.cuts.back()<<",\"weight_pb\":"<<c.weight<<",\"files\":[";
        for(std::size_t j=0;j<c.files.size();++j) {
            if(j) out<<',';
            const auto& f=c.files[j];out<<"{\"path\":"<<quote(f.path)<<",\"entries\":"<<f.entries<<",\"generated_events\":"<<f.generated<<'}';
        }
        out<<"],\"histograms\":";histJSON(out,summary.h);out<<'}';
    }
    out<<"],\"selected_sumw\":"<<selected<<",\"selected_sumw2\":"<<variance<<",\"histograms\":{";
    const auto h=makeHists();
    for(std::size_t i=0;i<h.size();++i) {
        if(i) out<<',';
        std::vector<double> sumw(h[i].counts.size()),sumw2(sumw.size());
        for(const auto& c:components) for(std::size_t j=0;j<sumw.size();++j) {
            sumw[j]+=c.summaries[view].h[i].counts[j]*c.weight;
            sumw2[j]+=c.summaries[view].h[i].counts[j]*c.weight*c.weight;
        }
        out<<quote(h[i].name)<<":{\"label\":"<<quote(h[i].label)<<",\"edges\":";arrayJSON(out,h[i].edges);
        out<<",\"sumw\":";arrayJSON(out,sumw);out<<",\"sumw2\":";arrayJSON(out,sumw2);out<<'}';
    }
    out<<"}}";
}
void profileJSON(std::ostream& out,const std::vector<Profile>& p) {
    out<<'{';bool first=true;for(const auto& v:p){if(!first)out<<',';first=false;
        out<<quote(v.name)<<":{\"label\":"<<quote(v.label)<<",\"ylabel\":"<<quote(v.ylabel)<<",\"xedges\":";
        arrayJSON(out,v.xedges);out<<",\"yedges\":";arrayJSON(out,v.yedges);out<<",\"moments\":";
        momentsJSON(out,v.values);out<<'}';}out<<'}';
}
void summaryJSON(std::ostream& out,const Summary& s,bool inclusive=false) {
    out<<"{\"cutflow\":";arrayJSON(out,s.cuts);out<<",\"threshold_counts\":";arrayJSON(out,s.thresholdCounts);
    out<<",\"input_entries\":"<<s.inputEntries<<",\"certification_rejected\":"<<s.certificationRejected
       <<",\"pass_json_rejected\":"<<s.passJSONRejected;
    if(inclusive) out<<",\"valid_dijets\":"<<s.validDijets;
    out<<",\"histograms\":";histJSON(out,s.h);out<<",\"profiles\":";profileJSON(out,s.p);
    out<<",\"runs\": [";bool first=true;
    for(const auto& r:s.runs){if(!first)out<<',';first=false;
        out<<"{\"run\":"<<r.first<<",\"stored\":"<<r.second.stored<<",\"base\":"<<r.second.base
           <<",\"selected\":"<<r.second.selected<<",\"signs\":";arrayJSON(out,r.second.signs);
        if(inclusive) {out<<",\"eta_zero\":";arrayJSON(out,r.second.etaZero);}
        out<<",\"moments\":";momentsJSON(out,std::vector<Moment>(r.second.means.begin(),r.second.means.end()));out<<'}';}
    out<<"],\"lumisections\":[";first=true;
    for(const auto& l:s.lumis){if(!first)out<<',';first=false;out<<'['<<(l.first>>32)<<','<<(l.first&0xffffffffULL)
        <<','<<l.second.stored<<','<<l.second.base<<','<<l.second.selected<<','<<quote(hex(l.second.hashSum))
        <<','<<quote(hex(l.second.hashXor))<<']';}out<<"]}";
}
void pairedJSON(std::ostream& out,const Paired& p) {
    out<<"{\"matched\":"<<p.matched<<",\"only_old\":"<<p.onlyOld<<",\"only_new\":"<<p.onlyNew
       <<",\"jet_matched\":"<<p.jetMatched<<",\"jet_unmatched\":"<<p.jetUnmatched<<",\"reordered\":"<<p.reordered
       <<",\"migration\":";arrayJSON(out,p.migration);out<<",\"threshold_migration\":[";
    for(std::size_t i=0;i<p.thresholdMigration.size();++i){if(i)out<<',';arrayJSON(out,p.thresholdMigration[i]);}
    out<<"],\"shifts\":";histJSON(out,p.shifts);out<<",\"profiles\":";profileJSON(out,p.profiles);
    out<<",\"overlap\":{";const auto h=makeHists();for(std::size_t i=0;i<h.size();++i){if(i)out<<',';out<<quote(h[i].name)<<':';arrayJSON(out,p.overlap[i]);}
    out<<"},\"runs\":[";bool first=true;for(const auto& r:p.runs){if(!first)out<<',';first=false;out<<'['<<r.first;for(auto n:r.second)out<<','<<n;out<<']';}
    out<<"],\"run_shifts\":[";first=true;for(const auto& r:p.runShifts){if(!first)out<<',';first=false;
        out<<"{\"run\":"<<r.first<<",\"moments\":";momentsJSON(out,std::vector<Moment>(r.second.begin(),r.second.end()));out<<'}';}out<<"]}";
}

void writeSummary(std::ostream& out,const Options& o,const std::vector<FileInfo>& files,
                  const Summary& old,const Summary& next,const Paired& pair,
                  const std::vector<MCComponent>& components,std::size_t view,double seconds,unsigned workers,Count processed) {
    const bool mcOnly=!o.paired && o.input.empty();
    out<<std::setprecision(17)<<"{\"schema_version\":1,\"dataset\":"<<quote(o.dataset)<<",\"tree\":"<<quote(o.tree)
       <<",\"mode\":"<<quote(mcOnly?"mc":o.paired?o.match:"single")<<",\"partial\":"<<(o.maxEntries>=0?"true":"false")
       <<",\"config\":";configJSON(out,o,!mcOnly);
    out<<",\"thresholds\":";arrayJSON(out,thresholds);
    if(!mcOnly && o.certification.active()) out<<",\"certification_source\":"<<quote(o.certification.source);
    if(o.preset=="all_stored") out<<",\"cutflow_labels\":[\"Stored\",\"Retained\",\"Retained\",\"Retained\",\"Retained\",\"Retained\",\"Retained\",\"Retained\"]";
    else out<<",\"cutflow_labels\":[\"Stored\",\"JSON and HLT\",\"Valid dijet\",\"Tight IDs\",\"pT\",\"eta\",\"delta eta\",\"Mass\"]";
    out<<",\"files\":{";
    auto writeFiles=[&](const std::string& label,bool isNew){out<<quote(label)<<":[";for(std::size_t i=0;i<files.size();++i){if(i)out<<',';const auto& f=files[i];
        out<<"{\"path\":"<<quote(isNew?f.newPath:f.oldPath)<<",\"entries\":"<<(isNew?f.newEntries:f.oldEntries)
           <<",\"processed\":"<<(isNew?f.newLimit:f.oldLimit)<<'}';}out<<']';};
    if(!mcOnly) writeFiles(o.paired?"old":o.jec,false);if(o.paired){out<<',';writeFiles("new",true);}
    out<<"},\"samples\":{";
    if(!mcOnly) {out<<quote(o.paired?"old":o.jec)<<':';summaryJSON(out,old,o.preset=="all_stored");}
    if(o.paired){out<<",\"new\":";summaryJSON(out,next,o.preset=="all_stored");}out<<'}';
    if(o.paired){out<<",\"paired\":";pairedJSON(out,pair);}
    if(!components.empty()) {out<<",\"qcd\":";mcJSON(out,components,o,view);}
    out<<",\"elapsed_seconds\":"<<seconds<<",\"threads\":"<<workers<<",\"processed_entries\":"<<processed<<"}\n";
}

int main(int argc,char** argv) {
    try {
        const Options requested=parse(argc,argv);
        std::array<Options,2> options={requested,requested};
        options[0].requirePassJSON=false;
        options[1].preset="all_stored";options[1].minMass=0;
        const auto& o=options[0];
        const auto outputs=summaryPaths(o);
        ROOT::EnableThreadSafety();
        const auto started=Clock::now();
        const bool mcOnly=!o.paired && o.input.empty();
        auto components=loadMCComponents(o);
        State mcIdentity=State::array();
        for(const auto& component:components) {
            State paths=State::array();for(const auto& file:component.files) paths.push_back(file.path);
            mcIdentity.push_back({component.source,component.xsec,paths});
        }
        Checkpoints checkpoints(requested,{{"version",1},{"dataset",o.dataset},{"jec",o.jec},{"tree",o.tree},
            {"input",o.input},{"old",o.oldInput},{"new",o.newInput},{"mc",mcIdentity},{"preset",o.preset},
            {"cuts",{o.minMass,o.minPt,o.maxEta,o.maxDEta,o.balanceAlpha,o.jetDR}},
            {"certification",o.certification.canonical},{"require_json",requested.requirePassJSON},
            {"match",o.match},{"threads",o.threads},{"chunk",o.chunk},{"shard",o.shard},{"shards",o.shards},
            {"max_entries",o.maxEntries},{"outputs",{outputs[0].string(),outputs[1].string()}}});
        std::cout<<"One scan, two summaries: "<<outputs[0]<<" and "<<outputs[1]<<std::endl;
        if(!mcOnly) {
            std::cout<<"Selected preset: "<<o.preset<<"; PassJSON cut: "
                     <<(o.preset=="dijet" || o.preset=="loose"?"enabled (PassJSON > 0.5)":"disabled")
                     <<"; external run/LS mask: "<<(o.certification.active()?"enabled":"disabled")<<std::endl;
            std::cout<<"All-stored summary: no trigger, jet-ID or kinematic cuts; PassJSON filter: "
                     <<(options[1].requirePassJSON?"enabled":"disabled")<<std::endl;
        }
        if(!mcOnly && o.certification.active()) std::cout<<"Run/LS certification filter for both summaries: "<<o.certification.source<<" ("<<o.certification.ranges.size()<<" runs)"<<std::endl;
        std::vector<FileInfo> files;
        const auto& paths=o.paired?o.oldInput:o.input;
        for(std::size_t i=0;i<paths.size();++i)if(i%o.shards==o.shard) {
            FileInfo f;f.oldPath=paths[i];if(o.paired)f.newPath=o.newInput[i];files.push_back(std::move(f));
        }
        if(files.empty() && !mcOnly)throw std::runtime_error("This shard has no files");
        if(!mcOnly) {
            std::cout<<"Validating "<<files.size()<<" file"<<(o.paired?" pairs":"s")<<" using "<<o.threads<<" threads"<<std::endl;
            std::atomic<Count> validated{0};Progress validation("Validate",files.size(),o.paired?"pairs":"files");
            withProgress(validation,validated,[&]{parallel(files.size(),o.threads,[&](std::size_t i,unsigned){
                auto& f=files[i];Reader a(f.oldPath,o);f.oldEntries=a.tree->GetEntries();
                f.oldIdentity=fileIdentity(*a.file,f.oldPath);
                f.oldLimit=o.maxEntries<0?f.oldEntries:std::min(f.oldEntries,o.maxEntries);
                if(o.paired){Reader b(f.newPath,o);f.newIdentity=fileIdentity(*b.file,f.newPath);f.newEntries=b.tree->GetEntries();f.newLimit=o.maxEntries<0?f.newEntries:std::min(f.newEntries,o.maxEntries);
                    if(o.match=="ordered" && f.oldEntries!=f.newEntries)throw std::runtime_error("Entry counts differ in "+f.oldPath+" and "+f.newPath+"; use --match keyed or independent summaries");}
                auto it=a.tree->GetClusterIterator(0);long long last=0,s;
                f.starts.push_back(0);
                while((s=it())<f.oldLimit)if(s-last>=o.chunk){f.starts.push_back(s);last=s;}
                if(f.oldLimit>0)f.starts.push_back(f.oldLimit);
                validated.fetch_add(1,std::memory_order_relaxed);
            });});
        }
        std::vector<Task> tasks;
        for(std::size_t i=0;i<files.size();++i){const auto& f=files[i];
            if(o.paired && o.match=="keyed")tasks.push_back({i,0,f.oldLimit});
            else for(std::size_t j=1;j<f.starts.size();++j)tasks.push_back({i,f.starts[j-1],f.starts[j]});}
        const unsigned workers=std::max(1u,std::min<unsigned>(o.threads,std::max<std::size_t>(1,tasks.size())));
        std::array<std::vector<Summary>,2> olds,news;
        std::array<std::vector<Paired>,2> pairs;
        for(std::size_t view=0;view<options.size();++view) {
            olds[view].resize(workers);
            if(o.paired){news[view].resize(workers);pairs[view].resize(workers);}
        }
        State planFiles=State::array(),planTasks=State::array();
        for(const auto& f:files) planFiles.push_back({f.oldIdentity,f.newIdentity,f.oldEntries,f.newEntries,f.oldLimit,f.newLimit});
        for(const auto& task:tasks) planTasks.push_back({task.file,task.begin,task.end});
        checkpoints.prepare("data",{{"files",planFiles},{"tasks",planTasks},{"workers",workers}});
        std::vector<std::vector<std::size_t>> finished(workers);
        std::vector<bool> completed(tasks.size(),false);
        Count restored=0,restoredTasks=0;
        for(unsigned slot=0;slot<workers;++slot) {
            const auto state=checkpoints.load("data",slot);if(state.is_null()) continue;
            finished[slot]=state.at("done").get<std::vector<std::size_t>>();Count oldCount=0,newCount=0;
            for(const auto ti:finished[slot]) {
                if(ti>=tasks.size() || completed[ti]) throw std::runtime_error("Invalid or duplicate data task in checkpoint");
                completed[ti]=true;++restoredTasks;const auto& task=tasks[ti];oldCount+=task.end-task.begin;
                if(o.paired) newCount+=o.match=="keyed"?files[task.file].newLimit:task.end-task.begin;
            }
            restored+=oldCount+newCount;
            for(std::size_t view=0;view<options.size();++view) {
                const auto& saved=state.at("views").at(view);
                restoreSummary(saved.at("old"),olds[view][slot]);
                if(olds[view][slot].inputEntries!=oldCount) throw std::runtime_error("Data checkpoint task/event counts disagree");
                if(o.paired) {
                    restoreSummary(saved.at("new"),news[view][slot]);restorePaired(saved.at("pair"),pairs[view][slot]);
                    if(news[view][slot].inputEntries!=newCount) throw std::runtime_error("Paired checkpoint task/event counts disagree");
                }
            }
        }
        if(restoredTasks) std::cout<<"Resuming data: "<<restoredTasks<<"/"<<tasks.size()<<" chunks complete; skipping "<<restored<<" entries"<<std::endl;
        std::atomic<Count> processed{restored};
        Count total=0;for(const auto& f:files)total+=f.oldLimit+(o.paired?f.newLimit:0);
        if(!mcOnly) std::cout<<"Scanning "<<total<<" entries once in "<<tasks.size()<<" tasks; only "<<NFields<<" scalar branches are read"<<std::endl;
        if(o.paired && o.match=="keyed")std::cout<<"Keyed matching caches one old-event record with both histogram assignments per active file pair"<<std::endl;
        if(!mcOnly) {
            Progress scanning("Scan",total,"entries",restored);
            withProgress(scanning,processed,[&]{parallel(tasks.size(),workers,[&](std::size_t ti,unsigned slot){
                if(completed[ti]) return;
                const auto& task=tasks[ti];const auto& f=files[task.file];
                Reader a(f.oldPath,o,task.begin,task.end);Count pending=0;
                auto tick=[&]{if(++pending>=50000){processed+=pending;pending=0;}};
                if(!o.paired) {
                    for(long long i=task.begin;i<task.end;++i) {
                        const auto& e=a.read(i);const auto k=key(e);
                        for(std::size_t view=0;view<options.size();++view) olds[view][slot].fill(e,options[view],k);
                        tick();
                    }
                } else {
                    Reader b(f.newPath,o,o.match=="keyed"?0:task.begin,o.match=="keyed"?f.newLimit:task.end);
                    if(o.match=="ordered") {
                        for(long long i=task.begin;i<task.end;++i) {
                            const auto& ea=a.read(i);const auto& eb=b.read(i);const auto ka=key(ea),kb=key(eb);
                            if(!(ka==kb))throw std::runtime_error("Event identity mismatch at entry "+std::to_string(i)+" in "+f.oldPath+"; use --match keyed or independent summaries");
                            for(std::size_t view=0;view<options.size();++view) {
                                auto& old=olds[view][slot];auto& next=news[view][slot];auto& pair=pairs[view][slot];const auto& opt=options[view];
                                const auto ia=old.fill(ea,opt,ka),ib=next.fill(eb,opt,kb);const auto ca=cutflow(ea,opt),cb=cutflow(eb,opt);
                                if(old.retained && next.retained) pair.fill(ea,eb,ca[5],cb[5],ca[6],cb[6],ia,ib,ka,opt);
                                else if(old.retained) ++pair.onlyOld;else if(next.retained) ++pair.onlyNew;
                            }
                            tick();tick();
                        }
                    } else {
                        std::array<Count,2> matchedBefore,oldBefore,newBefore;
                        for(std::size_t view=0;view<options.size();++view) {
                            matchedBefore[view]=pairs[view][slot].matched;
                            oldBefore[view]=olds[view][slot].cuts[0];newBefore[view]=news[view][slot].cuts[0];
                        }
                        std::unordered_map<Key,Record,KeyHash> index;index.max_load_factor(0.85);index.reserve(f.oldLimit);
                        for(long long i=0;i<f.oldLimit;++i) {
                            const auto& e=a.read(i);const auto k=key(e);std::array<Bins,2> bins;std::array<bool,2> retained;
                            for(std::size_t view=0;view<options.size();++view) {
                                bins[view]=olds[view][slot].fill(e,options[view],k);retained[view]=olds[view][slot].retained;
                            }
                            if((retained[0] || retained[1]) && !index.emplace(k,Record(e,options,std::move(bins),retained)).second)
                                throw std::runtime_error("Duplicate old event key in "+f.oldPath);
                            tick();
                        }
                        for(long long i=0;i<f.newLimit;++i) {
                            const auto& e=b.read(i);const auto k=key(e);std::array<Bins,2> bins;std::array<bool,2> retained;
                            for(std::size_t view=0;view<options.size();++view) {
                                bins[view]=news[view][slot].fill(e,options[view],k);retained[view]=news[view][slot].retained;
                            }
                            if(!retained[0] && !retained[1]){tick();continue;}
                            const auto found=index.find(k);
                            if(found==index.end()) {
                                for(std::size_t view=0;view<options.size();++view) pairs[view][slot].onlyNew+=retained[view];
                            } else {
                                auto& r=found->second;if(r.seen)throw std::runtime_error("Duplicate matched new event key in "+f.newPath);r.seen=true;
                                const auto oldEvent=r.event();
                                for(std::size_t view=0;view<options.size();++view) {
                                    auto& pair=pairs[view][slot];const auto& opt=options[view];
                                    if(r.retained[view] && retained[view]) {
                                        const auto c=cutflow(e,opt);
                                        pair.fill(oldEvent,e,r.base[view],c[5],r.selected[view],c[6],r.bins[view],bins[view],k,opt);
                                    } else if(r.retained[view]) ++pair.onlyOld;
                                    else if(retained[view]) ++pair.onlyNew;
                                }
                            }
                            tick();
                        }
                        for(const auto& item:index) if(!item.second.seen)
                            for(std::size_t view=0;view<options.size();++view) pairs[view][slot].onlyOld+=item.second.retained[view];
                        for(std::size_t view=0;view<options.size();++view)
                            if(olds[view][slot].cuts[0]>oldBefore[view] && news[view][slot].cuts[0]>newBefore[view] && pairs[view][slot].matched==matchedBefore[view])
                                throw std::runtime_error("No common events for "+options[view].preset+" in file pair "+f.oldPath+" and "+f.newPath+"; check file pairing or use independent summaries");
                    }
                }
                finished[slot].push_back(ti);State views=State::array();
                for(std::size_t view=0;view<options.size();++view) {
                    State saved={{"old",summaryState(olds[view][slot])}};
                    if(o.paired) {saved["new"]=summaryState(news[view][slot]);saved["pair"]=pairedState(pairs[view][slot]);}
                    views.push_back(std::move(saved));
                }
                checkpoints.save("data",slot,{{"done",finished[slot]},{"views",views}});
                processed+=pending;
            });});
        }
        std::array<Summary,2> old,next;std::array<Paired,2> pair;
        for(std::size_t view=0;view<options.size();++view) {
            for(unsigned i=0;i<workers;++i){old[view].merge(olds[view][i]);if(o.paired){next[view].merge(news[view][i]);pair[view].merge(pairs[view][i]);}}
            if(!mcOnly) {
                auto accounting=[&](const std::string& label,const Summary& s) {
                    if(s.inputEntries!=s.cuts[0]+s.certificationRejected+s.passJSONRejected) throw std::runtime_error("Inconsistent event-filter accounting");
                    std::cout<<(view?"all_stored":"selected")<<" "<<label<<": read="<<s.inputEntries<<", retained="<<s.cuts[0]
                             <<", selected="<<s.cuts.back()<<", rejected by run/LS mask="<<s.certificationRejected
                             <<", rejected by PassJSON="<<s.passJSONRejected<<std::endl;
                };
                accounting(o.paired?"old":o.jec,old[view]);if(o.paired) accounting("new",next[view]);
            }
        }
        unsigned mcWorkers=0;
        Count mcRestored=0;
        processed+=reduceMC(components,options,mcWorkers,checkpoints,mcRestored);
        const double seconds=std::chrono::duration<double>(Clock::now()-started).count();
        std::array<fs::path,2> temporary;
        try {
            for(std::size_t view=0;view<options.size();++view) {
                fs::create_directories(outputs[view].parent_path());
                temporary[view]=outputs[view].string()+".tmp."+std::to_string(Clock::now().time_since_epoch().count());
                std::ofstream out(temporary[view]);if(!out)throw std::runtime_error("Cannot create output: "+temporary[view].string());
                writeSummary(out,options[view],files,old[view],next[view],pair[view],components,view,seconds,std::max(workers,mcWorkers),processed.load());
                out.close();if(!out)throw std::runtime_error("Failed to write summary: "+temporary[view].string());
            }
            checkSummaryOutputs(o);
            for(std::size_t view=0;view<options.size();++view) fs::rename(temporary[view],outputs[view]);
        } catch(...) {
            for(const auto& path:temporary) if(!path.empty()) {std::error_code error;fs::remove(path,error);}
            throw;
        }
        for(const auto& output:outputs) std::cout<<"Saved "<<output<<std::endl;
        const Count reused=restored+mcRestored,read=processed.load()-reused;
        if(reused) std::cout<<reused<<" entries restored from checkpoints; ";
        std::cout<<read<<" entries read once for both summaries in "<<std::fixed<<std::setprecision(2)<<seconds
                 <<" s ("<<std::setprecision(0)<<(seconds>0?read/seconds:0)<<" entries/s)"<<std::endl;
        return 0;
    } catch(const std::exception& e){std::cerr<<"Error: "<<e.what()<<'\n';return 1;}
}
