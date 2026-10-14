#include <ROOT/RConfig.hxx>
#include <TFile.h>
#include <TLeaf.h>
#include <TROOT.h>
#include <TTree.h>
#include <nlohmann/json.hpp>

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
    std::vector<std::string> input, oldInput, newInput;
    std::string output, dataset, jec, tree = "rootTupleTree/tree", preset = "dijet";
    std::string match = "ordered";
    unsigned threads = std::max(1u, std::thread::hardware_concurrency());
    unsigned shard = 0, shards = 1;
    double minMass = 1181, minPt = 30, maxEta = 2.5, maxDEta = 1.3;
    double balanceAlpha = 0.3, jetDR = 0.02;
    long long cacheMB = 32, chunk = 1000000, maxEntries = -1;
    bool force = false, paired = false, requirePassJSON = false;
    Certification certification;
};

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
            std::cout << "Single: reduce_jec_comparison --input PATH --jec old|new --dataset ERA --output FILE.json\n"
                      << "Paired: reduce_jec_comparison --old PATH --new PATH --dataset ERA --output FILE.json\n"
                      << "PATH: ROOT file, directory, quoted glob, @filelist.txt, or explicit root:// URL. Repeat input flags.\n"
                      << "--threads N (0=all), --cache-mb N (per reader), --chunk-entries N\n"
                      << "--preset dijet|loose|all|all_stored, --mjj-min GeV, --pt-min GeV, --eta-max X, --deta-max X\n"
                      << "Default dijet preset requires PassJSON > 0.5 and the existing analysis cuts.\n"
                      << "all_stored applies no event cuts; PassJSON and external JSON filtering are disabled by default.\n"
                      << "--require-pass-json explicitly requires saved PassJSON > 0.5 for all_stored only.\n"
                      << "Jet observables require only their own usable stored quantities in all_stored.\n"
                      << "--cert-json FILE applies a run/LS mask before all histogram filling, including all_stored.\n"
                      << "--match ordered|keyed, --jet-match-dr X, --balance-alpha-max X\n"
                      << "--tree PATH, --shard-index I --shards N, --max-entries N (per file), --force\n"
                      << "ordered verifies event identities at every entry; keyed joins within corresponding file pairs.\n";
            std::exit(0);
        } else if (arg == "--input") expandInput(value(), o.input);
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
        else throw std::runtime_error("Unknown option: " + arg);
    }
    o.paired = !o.oldInput.empty() || !o.newInput.empty();
    if (o.output.empty() || o.dataset.empty()) throw std::runtime_error("--output and --dataset are required");
    if (o.preset != "dijet" && o.preset != "loose" && o.preset != "all" && o.preset != "all_stored") throw std::runtime_error("Unknown preset");
    if (o.requirePassJSON && o.preset != "all_stored") throw std::runtime_error("--require-pass-json is for all_stored; dijet and loose already require PassJSON");
    if (o.preset != "dijet" && !massSet) o.minMass = 0;
    if (o.preset == "all_stored" && o.minMass != 0) throw std::runtime_error("all_stored has no mass selection; use --preset all with --mjj-min instead");
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
    } else {
        uniqueInputs(o.input);
        if (o.jec != "old" && o.jec != "new") throw std::runtime_error("--jec must be old or new");
    }
    if (fs::exists(o.output) && !o.force) throw std::runtime_error("Output exists; use --force to replace it");
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

int bin(const std::vector<double>& e, double x) {
    if (!std::isfinite(x)) return -1;
    if (x < e.front()) return 0;
    if (x >= e.back()) return static_cast<int>(e.size());
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
    double uniformWidth = 0;
    Hist(std::string n, std::string l, std::vector<double> e, double width=0): name(std::move(n)), label(std::move(l)),
        edges(std::move(e)), counts(edges.size()+1, 0), uniformWidth(width) {}
    int fill(double x) {
        if(uniformWidth>0) return fillUniform(x,uniformWidth);
        const int b = bin(edges,x);
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
    Profile(std::string n, std::string l, std::string yl, std::vector<double> x, std::vector<double> y = {}):
        name(std::move(n)), label(std::move(l)), ylabel(std::move(yl)), xedges(std::move(x)),
        yedges(std::move(y)), values((xedges.size()+1)*(yedges.empty()?1:yedges.size()+1)) {}
    void fill(double x, double z, double y = 0) {
        const int bx=bin(xedges,x), by=yedges.empty()?0:bin(yedges,y);
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
    c[0]=o.preset=="all" || (e[Json]>0.5 && e[Trigger]>0.5);
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
    Bins bins;
    bool base,selected,seen=false;
    Record(const Event& e,const Options& o,Bins b):bins(b) {
        for(std::size_t i=0;i<v.size();++i) v[i]=e[pairFields[i]];
        const auto c=cutflow(e,o);base=c[5];selected=c[6];
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

struct Reader {
    Event e{};
    std::unique_ptr<TFile> file;
    TTree* tree=nullptr;
    Reader(const std::string& path,const Options& o,long long begin=0,long long end=-1) {
        file.reset(TFile::Open(path.c_str(),"READ"));
        if(!file || file->IsZombie())throw std::runtime_error("Cannot open ROOT file: "+path);
        tree=file->Get<TTree>(o.tree.c_str());
        if(!tree)throw std::runtime_error("Missing tree "+o.tree+" in "+path);
        tree->SetBranchStatus("*",0);
        tree->SetCacheSize(o.cacheMB*1024*1024);
        tree->SetCacheEntryRange(begin,end<0?tree->GetEntries():end);
        for(int i=0;i<NFields;++i) {
            auto* leaf=tree->GetLeaf(branches[i]);
            if(!leaf || std::string(leaf->GetTypeName())!="Double_t" || leaf->GetLenStatic()!=1 || leaf->GetLeafCount())
                throw std::runtime_error(path+": expected scalar Double_t branch "+branches[i]);
            tree->SetBranchStatus(branches[i],1);
            if(tree->SetBranchAddress(branches[i],&e[i])<0)throw std::runtime_error("Cannot bind branch "+std::string(branches[i]));
            if(o.cacheMB)tree->AddBranchToCache(branches[i],false);
        }
        tree->StopCacheLearningPhase();
    }
    const Event& read(long long entry) {
        if(tree->GetEntry(entry)<=0)throw std::runtime_error("ROOT read failed at entry "+std::to_string(entry)+" in "+file->GetName());
        return e;
    }
};
struct FileInfo { std::string oldPath,newPath; long long oldEntries=0,newEntries=0,oldLimit=0,newLimit=0;std::vector<long long> starts; };
struct Task {std::size_t file;long long begin,end;};

class Progress {
    std::string phase,unit;
    Count total;
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
        const double rate=seconds>0?completed/seconds:0;
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
    Progress(std::string stage,Count count,std::string units):phase(std::move(stage)),unit(std::move(units)),total(count),
        interactive(isatty(STDERR_FILENO) && (!std::getenv("TERM") || std::string(std::getenv("TERM"))!="dumb")) {draw(0,false,true);}
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

int main(int argc,char** argv) {
    try {
        const Options o=parse(argc,argv);
        ROOT::EnableThreadSafety();
        const auto started=Clock::now();
        std::cout<<"Preset: "<<o.preset<<"; PassJSON cut: "
                 <<(o.requirePassJSON || o.preset=="dijet" || o.preset=="loose"?"enabled (PassJSON > 0.5)":"disabled")
                 <<"; external run/LS mask: "<<(o.certification.active()?"enabled":"disabled")<<std::endl;
        if(o.certification.active()) std::cout<<"Run/LS certification filter: "<<o.certification.source<<" ("<<o.certification.ranges.size()<<" runs)"<<std::endl;
        std::vector<FileInfo> files;
        const auto& paths=o.paired?o.oldInput:o.input;
        for(std::size_t i=0;i<paths.size();++i)if(i%o.shards==o.shard) {
            FileInfo f;f.oldPath=paths[i];if(o.paired)f.newPath=o.newInput[i];files.push_back(std::move(f));
        }
        if(files.empty())throw std::runtime_error("This shard has no files");
        std::cout<<"Validating "<<files.size()<<" file"<<(o.paired?" pairs":"s")<<" using "<<o.threads<<" threads"<<std::endl;
        std::atomic<Count> validated{0};Progress validation("Validate",files.size(),o.paired?"pairs":"files");
        withProgress(validation,validated,[&]{parallel(files.size(),o.threads,[&](std::size_t i,unsigned){
            auto& f=files[i];Reader a(f.oldPath,o);f.oldEntries=a.tree->GetEntries();
            f.oldLimit=o.maxEntries<0?f.oldEntries:std::min(f.oldEntries,o.maxEntries);
            if(o.paired){Reader b(f.newPath,o);f.newEntries=b.tree->GetEntries();f.newLimit=o.maxEntries<0?f.newEntries:std::min(f.newEntries,o.maxEntries);
                if(o.match=="ordered" && f.oldEntries!=f.newEntries)throw std::runtime_error("Entry counts differ in "+f.oldPath+" and "+f.newPath+"; use --match keyed or independent summaries");}
            auto it=a.tree->GetClusterIterator(0);long long last=0,s;
            f.starts.push_back(0);
            while((s=it())<f.oldLimit)if(s-last>=o.chunk){f.starts.push_back(s);last=s;}
            if(f.oldLimit>0)f.starts.push_back(f.oldLimit);
            validated.fetch_add(1,std::memory_order_relaxed);
        });});
        std::vector<Task> tasks;
        for(std::size_t i=0;i<files.size();++i){const auto& f=files[i];
            if(o.paired && o.match=="keyed")tasks.push_back({i,0,f.oldLimit});
            else for(std::size_t j=1;j<f.starts.size();++j)tasks.push_back({i,f.starts[j-1],f.starts[j]});}
        const unsigned workers=std::max(1u,std::min<unsigned>(o.threads,std::max<std::size_t>(1,tasks.size())));
        std::vector<Summary> olds(workers),news(o.paired?workers:0);std::vector<Paired> pairs(o.paired?workers:0);
        std::atomic<Count> processed{0};
        Count total=0;for(const auto& f:files)total+=f.oldLimit+(o.paired?f.newLimit:0);
        std::cout<<"Scanning "<<total<<" entries in "<<tasks.size()<<" tasks; only "<<NFields<<" scalar branches are read"<<std::endl;
        if(o.paired && o.match=="keyed")std::cout<<"Keyed matching caches old events per active file pair; budget about 3 GB per 7.25 million events per worker"<<std::endl;
        Progress scanning("Scan",total,"entries");
        withProgress(scanning,processed,[&]{parallel(tasks.size(),workers,[&](std::size_t ti,unsigned slot){
            const auto& task=tasks[ti];const auto& f=files[task.file];
            Reader a(f.oldPath,o,task.begin,task.end);
            auto& old=olds[slot];Count pending=0;
            auto tick=[&]{if(++pending>=50000){processed+=pending;pending=0;}};
            if(!o.paired){for(long long i=task.begin;i<task.end;++i){const auto& e=a.read(i);old.fill(e,o,key(e));tick();}}
            else {
                Reader b(f.newPath,o,o.match=="keyed"?0:task.begin,o.match=="keyed"?f.newLimit:task.end);
                auto& next=news[slot];auto& pair=pairs[slot];
                if(o.match=="ordered")for(long long i=task.begin;i<task.end;++i){
                    const auto& ea=a.read(i);const auto& eb=b.read(i);const auto ka=key(ea),kb=key(eb);
                    if(!(ka==kb))throw std::runtime_error("Event identity mismatch at entry "+std::to_string(i)+" in "+f.oldPath+"; use --match keyed or independent summaries");
                    const auto ia=old.fill(ea,o,ka),ib=next.fill(eb,o,kb);const auto ca=cutflow(ea,o),cb=cutflow(eb,o);
                    if(old.retained && next.retained) pair.fill(ea,eb,ca[5],cb[5],ca[6],cb[6],ia,ib,ka,o);
                    else if(old.retained) ++pair.onlyOld;else if(next.retained) ++pair.onlyNew;tick();tick();
                } else {
                    const Count matchedBefore=pair.matched;
                    const Count oldBefore=old.cuts[0],newBefore=next.cuts[0];
                    std::unordered_map<Key,Record,KeyHash> index;index.max_load_factor(0.85);index.reserve(f.oldLimit);
                    for(long long i=0;i<f.oldLimit;++i){const auto& e=a.read(i);const auto k=key(e);auto bins=old.fill(e,o,k);
                        if(old.retained && !index.emplace(k,Record(e,o,std::move(bins))).second)throw std::runtime_error("Duplicate old event key in "+f.oldPath);tick();}
                    for(long long i=0;i<f.newLimit;++i){const auto& e=b.read(i);const auto k=key(e);const auto ib=next.fill(e,o,k);
                        if(!next.retained){tick();continue;}
                        const auto found=index.find(k);
                        if(found==index.end())++pair.onlyNew;
                        else {auto& r=found->second;if(r.seen)throw std::runtime_error("Duplicate matched new event key in "+f.newPath);r.seen=true;
                            const auto c=cutflow(e,o);pair.fill(r.event(),e,r.base,c[5],r.selected,c[6],r.bins,ib,k,o);}tick();}
                    for(const auto& item:index)pair.onlyOld+=!item.second.seen;
                    if(old.cuts[0]>oldBefore && next.cuts[0]>newBefore && pair.matched==matchedBefore)
                        throw std::runtime_error("No common events in file pair "+f.oldPath+" and "+f.newPath+"; check file pairing or use independent summaries");
                }
            }
            processed+=pending;
        });});
        Summary old,next;Paired pair;
        for(unsigned i=0;i<workers;++i){old.merge(olds[i]);if(o.paired){next.merge(news[i]);pair.merge(pairs[i]);}}
        if(o.certification.active() || o.requirePassJSON) {
            auto accounting=[&](const std::string& label,const Summary& s) {
                if(s.inputEntries!=s.cuts[0]+s.certificationRejected+s.passJSONRejected) throw std::runtime_error("Inconsistent event-filter accounting");
                std::cout<<label<<": read="<<s.inputEntries<<", retained="<<s.cuts[0]<<", rejected by run/LS mask="<<s.certificationRejected
                         <<", rejected by PassJSON="<<s.passJSONRejected<<std::endl;
            };
            accounting(o.paired?"old":o.jec,old);if(o.paired) accounting("new",next);
        }
        if(o.preset=="all" || o.preset=="all_stored") {
            if(o.preset=="all_stored") std::cout<<"All-stored preset: "
                <<(o.certification.active() || o.requirePassJSON?"stored events passing explicitly requested JSON filters":"every stored event")
                <<" retained; no trigger, jet-ID or kinematic cuts; jet observables use their own available quantities"<<std::endl;
            else std::cout<<"All preset: histograms require valid stored dijets; mass threshold="<<o.minMass<<" GeV"<<std::endl;
            auto validity=[&](const std::string& label,const Summary& s) {
                const Count valid=o.preset=="all_stored"?s.validDijets:s.cuts[2];
                std::cout<<label<<": stored="<<s.cuts[0]<<", valid dijets="<<valid
                         <<", without valid dijet="<<s.cuts[0]-valid<<", selected="<<s.cuts.back()<<std::endl;
            };
            validity(o.paired?"old":o.jec,old);if(o.paired)validity("new",next);
        }
        const double seconds=std::chrono::duration<double>(Clock::now()-started).count();
        const fs::path output=fs::absolute(o.output);if(output.has_parent_path())fs::create_directories(output.parent_path());
        const fs::path temp=output.string()+".tmp."+std::to_string(Clock::now().time_since_epoch().count());
        std::ofstream out(temp);if(!out)throw std::runtime_error("Cannot create output: "+temp.string());
        out<<std::setprecision(17)<<"{\"schema_version\":1,\"dataset\":"<<quote(o.dataset)<<",\"tree\":"<<quote(o.tree)
           <<",\"mode\":"<<quote(o.paired?o.match:"single")<<",\"partial\":"<<(o.maxEntries>=0?"true":"false")
           <<",\"config\":{\"preset\":"<<quote(o.preset)<<",\"mjj_min\":"<<o.minMass<<",\"pt_min\":"<<o.minPt
           <<",\"eta_max\":"<<o.maxEta<<",\"deta_max\":"<<o.maxDEta<<",\"balance_alpha_max\":"<<o.balanceAlpha
           <<",\"jet_match_dr\":"<<o.jetDR;
        if(o.certification.active()) out<<",\"certification_mask\":"<<o.certification.canonical;
        if(o.requirePassJSON) out<<",\"require_pass_json\":true";
        out<<"},\"thresholds\":";arrayJSON(out,thresholds);
        if(o.certification.active()) out<<",\"certification_source\":"<<quote(o.certification.source);
        if(o.preset=="all_stored") out<<",\"cutflow_labels\":[\"Stored\",\"Retained\",\"Retained\",\"Retained\",\"Retained\",\"Retained\",\"Retained\",\"Retained\"]";
        else out<<",\"cutflow_labels\":[\"Stored\",\"JSON and HLT\",\"Valid dijet\",\"Tight IDs\",\"pT\",\"eta\",\"delta eta\",\"Mass\"]";
        out<<",\"files\":{";
        auto writeFiles=[&](const std::string& label,bool isNew){out<<quote(label)<<":[";for(std::size_t i=0;i<files.size();++i){if(i)out<<',';const auto& f=files[i];
            out<<"{\"path\":"<<quote(isNew?f.newPath:f.oldPath)<<",\"entries\":"<<(isNew?f.newEntries:f.oldEntries)
               <<",\"processed\":"<<(isNew?f.newLimit:f.oldLimit)<<'}';}out<<']';};
        writeFiles(o.paired?"old":o.jec,false);if(o.paired){out<<',';writeFiles("new",true);}
        out<<"},\"samples\":{"<<quote(o.paired?"old":o.jec)<<':';summaryJSON(out,old,o.preset=="all_stored");
        if(o.paired){out<<",\"new\":";summaryJSON(out,next,o.preset=="all_stored");}out<<'}';
        if(o.paired){out<<",\"paired\":";pairedJSON(out,pair);}
        out<<",\"elapsed_seconds\":"<<seconds<<",\"threads\":"<<workers<<",\"processed_entries\":"<<processed<<"}\n";
        out.close();if(!out){fs::remove(temp);throw std::runtime_error("Failed to write summary");}
        if(fs::exists(output) && !o.force){fs::remove(temp);throw std::runtime_error("Output appeared during processing; refusing to replace it");}
        fs::rename(temp,output);
        std::cout<<"Saved "<<output<<"; "<<processed<<" entries in "<<std::fixed<<std::setprecision(2)<<seconds
                 <<" s ("<<std::setprecision(0)<<processed/seconds<<" entries/s)"<<std::endl;
        return 0;
    } catch(const std::exception& e){std::cerr<<"Error: "<<e.what()<<'\n';return 1;}
}
