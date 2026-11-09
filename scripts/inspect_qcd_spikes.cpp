#define KINEMATICS_JSON_LIBRARY
#include "reduce_kinematics.cpp"
#include <TLeaf.h>
#include <TMD5.h>
#include <numeric>
#include <regex>
#include <tuple>
#include <unordered_map>

namespace spikes {
using J = KJSON;
using Count = std::uint64_t;
using Entry = Long64_t;

const std::vector<std::string> required = {"nJet","IdTight_j1","IdTight_j2","pTWJ_j1","pTWJ_j2",
    "etaWJ_j1","etaWJ_j2","deltaETAjj","mjj"};
const std::array<std::string,2> ptNames = {"pTWJ_j1","pTWJ_j2"};

std::vector<std::string> fields() {
    auto names = required;
    for(const std::string name : {"run","lumi","event","nVtx","NAK4PF","HTAK4PF","met","metphi","metSig",
        "METoverHTAK4PF","unclusteredEnFracAK4PF","minDPhiMetAK4PF","mjj_shiftJEC","Dijet_MassAK4PF",
        "deltaPHIjj","deltaETAjjAK4PF","deltaPHIjjAK4PF","passGoodMuonBaseSelection","PassJSON",
        "passHLT_PFScoutingHT","passHLT_PFScouting_SingleMuon","passL1ForPFScoutingHT",
        "leadingJetHardScatterMatched","Generator_binvar","Generator_scalePDF","Generator_weight","genWeight",
        "Pileup_nTrueInt","Pileup_nPU","Pileup_pthatmax","Pileup_pudensity","rho","fixedGridRhoFastjetAll","pthat","pThat","qScale"}) names.push_back(name);
    for(int jet : {1,2}) for(const std::string name : {"pTAK4PF","etaAK4PF","phiAK4PF","jetJecAK4PF","jetJecUncAK4PF",
        "Jet_genJetIdx","GenJet_pt","phiWJ","massWJ","neutrHadEnFrac","chargedHadEnFrac","photonEnFrac",
        "muEnFract","neutrElectromFrac","chargedMult","neutrMult","photonMult"}) names.push_back(name+"_j"+std::to_string(jet));
    return names;
}

const J descriptions = {
    {"pileup_scale_exceeds_generator","Pileup_pthatmax exceeds Generator_binvar; this candidate fails the optional pileup veto."},
    {"pileup_comparison_unavailable","Pileup_pthatmax or Generator_binvar was not saved as a supported scalar branch."},
    {"invalid_pileup_or_generator_scale","Pileup_pthatmax must be finite and nonnegative, and Generator_binvar must be finite and positive; no veto decision is assigned."},
    {"large_stored_jec","Stored AK4 JEC exceeds the diagnostic threshold; inspect correction inputs."},
    {"invalid_stored_jec","Stored AK4 JEC is nonpositive or nonfinite."},
    {"ak4_high_pt_before_jec_estimate","AK4 pT divided by stored JEC is already above the target lower threshold."},
    {"large_reco_gen_response","Matched AK4 reco/gen pT is outside [1/threshold, threshold]."},
    {"high_pt_already_in_matched_gen_jet","A matched generator jet is above the target lower threshold with a compatible pT ratio."},
    {"unmatched_ak4","Stored generator-jet index is negative; this alone does not establish pileup."},
    {"invalid_gen_match","Stored generator index or matched pT is invalid or inconsistent."},
    {"shared_gen_match","Both leading AK4 jets refer to the same generator jet."},
    {"gen_match_flag_inconsistent","Saved matching flag disagrees with the two stored indices and pT values."},
    {"generator_matching_unavailable","Complete scalar generator matching information was not saved for both AK4 jets."},
    {"large_widejet_growth","A geometrically associated wide jet has substantially more pT than its AK4 seed."},
    {"widejet_seed_association_unavailable","The two stored AK4 seeds could not both be associated within the selected radius."},
    {"wide_mass_inconsistent","Stored wide-dijet mass differs from that reconstructed from saved four-vectors."},
    {"wide_mass_check_unavailable","Full wide-jet four-vectors were not saved."},
    {"nonfinite_observable","A saved candidate observable or derived quantity is nonfinite."},
    {"event_id_precision_limit","A floating-point event ID is at or above 2^53; file and entry are authoritative."}
};

struct Options {
    fs::path summary="kinematics/QCD_2024.json",output="kinematics/qcd_spike_diagnostics",workdir,existing,worker;
    fs::path nanoEvents,nanoConfigDir="../dijetCondor/inputFiles_QCD_NanoAOD";
    std::map<std::string,std::string> nanoInputs;
    std::string nanoRedirector="root://cmsxrootd.fnal.gov";
    bool outputSet=false,samplesSet=false;
    J nanoSettings={{"seed_match_dr",0.05},{"gen_match_dr",0.4},{"raw_pt_relative_tolerance",0.001},{"response_warning",2.0}};
    std::string action,proxy,shared,schedd,pool,flavour="tomorrow",cmssw,arch;
    std::vector<std::string> samples={"50to80","80to120","120to170","170to300"};
    unsigned threads=4,jobs=64,memory=4096,disk=2048;
    bool condor=false,noSubmit=false,force=false;
    J settings={{"pt_min",900.0},{"pt_max",1150.0},{"jec_warning",3.0},{"response_warning",2.0},
                {"growth_warning",2.0},{"seed_dr",1.1},{"mass_relative_tolerance",0.001}};
};

std::string env(const char* name) {const char* value=std::getenv(name);return value?value:"";}
void need(bool ok,const std::string& message) {if(!ok) throw std::runtime_error(message);}
std::string quote(const std::string& value) {return eos_utils::shell_quote(value);}
std::string md5(const fs::path& path) {
    std::unique_ptr<TMD5> value(TMD5::FileChecksum(path.c_str()));
    need(bool(value),"Cannot checksum "+path.string());return value->AsString();
}
void textfile(const fs::path& path,const std::string& content) {
    std::ofstream out(path);need(bool(out),"Cannot write "+path.string());out<<content;out.close();need(bool(out),"Failed writing "+path.string());
}
void command(const std::string& value) {need(std::system(value.c_str())==0,"Command failed: "+value);}
std::string sample(const J& c) {
    std::smatch match;const std::string source=c.at("source");
    need(std::regex_search(source,match,std::regex("QCD_Bin-PT-([0-9]+(to([0-9]+|Inf))?)_")),"Cannot identify QCD bin: "+source);
    return match[1]=="3000toInf"?"3000":match[1].str();
}

Options arguments(int argc,char** argv) {
    Options o;o.cmssw=env("CMSSW_VERSION");o.arch=env("SCRAM_ARCH");bool newOptions=false,nanoOptions=false,reducedOptions=false;
    for(int i=1;i<argc;++i) {
        const std::string arg=argv[i];
        nanoOptions=nanoOptions || arg.rfind("--nano-",0)==0 || arg=="--gen-match-dr";
        reducedOptions=reducedOptions || std::set<std::string>{"--summary","--pt-min","--pt-max","--jec-warning","--growth-warning","--seed-dr","--mass-relative-tolerance"}.count(arg);
        if(!std::set<std::string>{"--merge","--submit","--retry-failed","--proxy","--shared-dir","--schedd","--pool","--force","--help","-h","--worker"}.count(arg))newOptions=true;
        auto value=[&](){need(++i<argc,"Missing value for "+arg);return std::string(argv[i]);};
        auto number=[&](){const auto s=value();std::size_t used=0;const auto n=std::stoll(s,&used);need(used==s.size() && n>0 && n<=1000000,"Invalid "+arg);return unsigned(n);};
        if(arg=="--summary")o.summary=value();
        else if(arg=="--output") {o.output=value();o.outputSet=true;}
        else if(arg=="--nano-events")o.nanoEvents=value();
        else if(arg=="--nano-config-dir")o.nanoConfigDir=value();
        else if(arg=="--nano-redirector")o.nanoRedirector=value();
        else if(arg=="--nano-input") {
            const auto s=value();const auto at=s.find('=');need(at!=std::string::npos && at>0 && at+1<s.size(),"Expected --nano-input SAMPLE=ROOT_OR_LIST");
            const auto label=s.substr(0,at);need(o.nanoInputs.emplace(label=="3000toInf"?"3000":label,s.substr(at+1)).second,"Repeated --nano-input sample");
        }
        else if(arg=="--nano-match-dr" || arg=="--gen-match-dr") {
            const auto s=value();std::size_t used=0;const double n=std::stod(s,&used);need(used==s.size() && std::isfinite(n) && n>0,"Invalid matching radius");
            o.nanoSettings[arg=="--nano-match-dr"?"seed_match_dr":"gen_match_dr"]=n;
        }
        else if(arg=="--workdir")o.workdir=value();
        else if(arg=="--samples") {o.samplesSet=true;o.samples.clear();while(i+1<argc && std::string(argv[i+1]).rfind("--",0)!=0)o.samples.push_back(argv[++i]);need(!o.samples.empty(),"Empty --samples");}
        else if(arg=="--threads")o.threads=number();
        else if(arg=="--jobs")o.jobs=number();
        else if(arg=="--request-memory-mb")o.memory=number();
        else if(arg=="--request-disk-mb")o.disk=number();
        else if(arg=="--condor")o.condor=true;
        else if(arg=="--no-submit")o.noSubmit=true;
        else if(arg=="--force")o.force=true;
        else if(arg=="--proxy")o.proxy=value();
        else if(arg=="--shared-dir")o.shared=value();
        else if(arg=="--schedd")o.schedd=value();
        else if(arg=="--pool")o.pool=value();
        else if(arg=="--job-flavour")o.flavour=value();
        else if(arg=="--cmssw-version")o.cmssw=value();
        else if(arg=="--scram-arch")o.arch=value();
        else if(arg=="--merge" || arg=="--submit" || arg=="--retry-failed") {need(o.action.empty() && o.worker.empty(),"Choose one action");o.action=arg;o.existing=value();}
        else if(arg=="--worker") {need(o.action.empty() && o.worker.empty(),"Choose one action");o.worker=value();}
        else if(arg=="--pt-min" || arg=="--pt-max" || arg=="--jec-warning" || arg=="--response-warning" || arg=="--growth-warning" || arg=="--seed-dr" || arg=="--mass-relative-tolerance") {
            auto key=arg.substr(2);std::replace(key.begin(),key.end(),'-','_');const auto s=value();std::size_t used=0;double n=std::stod(s,&used);
            need(used==s.size() && std::isfinite(n),"Invalid "+arg);o.settings[key]=n;
        } else if(arg=="--help" || arg=="-h") {
            std::cout<<"Usage: inspect_qcd_spikes [--summary kinematics/QCD_2024.json] [--samples 50to80 80to120 120to170 170to300]\n"
                <<"  [--pt-min 900 --pt-max 1150] [--threads 4] [--output kinematics/qcd_spike_diagnostics] [--force]\n"
                <<"  --condor [--jobs 64] [--workdir DIR] [--no-submit] [--request-memory-mb 4096] [--request-disk-mb 2048]\n"
                <<"  --submit WORKDIR | --retry-failed WORKDIR | --merge WORKDIR [--force]\n"
                <<"  [--proxy PATH] [--shared-dir DIR] [--schedd NAME] [--pool NAME] [--job-flavour tomorrow]\n"
                <<"  [--cmssw-version CMSSW_VERSION] [--scram-arch SCRAM_ARCH]\n"
                <<"  [--jec-warning 3] [--response-warning 2] [--growth-warning 2] [--seed-dr 1.1] [--mass-relative-tolerance 0.001]\n"
                <<"Original NanoAOD inspection: --nano-events PATH/events.jsonl [--nano-config-dir ../dijetCondor/inputFiles_QCD_NanoAOD]\n"
                <<"  [--nano-input SAMPLE=ROOT_OR_LIST] (repeat per sample; otherwise DAS uses Dataset in the matching config)\n"
                <<"  [--nano-redirector root://cmsxrootd.fnal.gov] [--nano-match-dr 0.05] [--gen-match-dr 0.4]\n"
                <<"  Supports the same --condor/--submit/--retry-failed/--merge workflow; NanoAOD jobs split by file.\n"
                <<"The pT window is [min,max) for either wide jet. MC uses the original offline cuts without HLT/JSON cuts.\n"
                <<"Condor splits disjoint entry ranges, including inside large merged files. Merge is manual.\n";std::exit(0);
        } else throw std::runtime_error("Unknown argument: "+arg);
    }
    need(o.threads<=128,"At most 128 threads");
    need(o.settings["pt_min"].get<double>()>=0 && o.settings["pt_max"]>o.settings["pt_min"],"Invalid pT window");
    for(const auto& item:o.settings.items())need(item.value().get<double>()>0 || item.key()=="pt_min","Diagnostic thresholds must be positive");
    need(o.settings["response_warning"].get<double>()>=1,"Response threshold must be >=1");
    need(!o.noSubmit || o.condor,"--no-submit requires --condor");
    need(o.action.empty() || (!o.condor && o.worker.empty() && o.workdir.empty()),"Existing productions use their saved options");
    need(o.action.empty() || !newOptions,"Do not change production options with --submit, --retry-failed, or --merge");
    need(o.worker.empty() || argc==3,"--worker uses only its saved manifest");
    need(o.workdir.empty() || o.condor,"--workdir requires --condor");
    need(!o.condor || !o.force,"Use a new Condor workdir instead of --force");
    need(!nanoOptions || !o.nanoEvents.empty(),"NanoAOD options require --nano-events");
    need(o.nanoEvents.empty() || !reducedOptions,"--nano-events uses the saved candidates; do not supply reduced-scan input or cut options");
    if(!o.nanoEvents.empty() && !o.outputSet)o.output="kinematics/qcd_spike_nanoaod";
    o.nanoSettings["response_warning"]=o.settings["response_warning"];
    return o;
}

J make_plan(const Options& o) {
    const auto summary=k_read_json(o.summary);
    need(summary.at("format")=="kinematics_summary" && summary.at("schema_version")==1 && summary.at("kind")=="mc" && summary.at("complete")==true,"Expected a complete MC kinematics summary");
    need(summary.at("contract")==kinematics_contract(),"Summary analysis differs from the current kinematics selection/binning");
    std::set<std::string> wanted(o.samples.begin(),o.samples.end()),seen,ids;J components=J::array(),files=J::array();
    if(wanted.erase("3000toInf"))wanted.insert("3000");
    for(const auto& c:summary.at("components")) {
        const auto label=sample(c);if(!wanted.count(label))continue;
        need(ids.insert(c.at("id").get<std::string>()).second,"Repeated component ID");
        const double xsec=c.at("xsec_pb"),generated=c.at("generated_events");
        need(std::isfinite(xsec) && xsec>0 && std::isfinite(generated) && generated>0,"Invalid MC normalization");
        double total=0;const unsigned ci=components.size();
        need(!c.at("files").empty(),"Empty component");
        for(const auto& f:c.at("files")) {
            const std::string path=eos_utils::normalize_storage_path(f.at("path"));
            need(seen.insert(k_identity(path)).second,"Duplicate input file: "+path);
            const Entry entries=f.at("entries");const double n=f.at("generated_events");
            need(entries>=0 && f.at("processed")==entries && std::isfinite(n) && n>0,"Incomplete file metadata");
            total+=n;files.push_back({{"component",ci},{"path",path},{"metadata",f}});
        }
        need(total==generated,"Generated-event sum differs from unique files");
        Count selected=0;for(const std::string category:{"pass","fail","unavailable"}) {
            const auto& g=c.at("categories").at(category);const auto count=g.at("events").get<Count>();selected+=count;
            for(const auto& name:ptNames) {
                const auto bins=g.at("histograms").at(name).get<std::vector<Count>>();
                need(bins.size()==std::size_t(nbins_for_spec(kPlotSpecs.at(name))) && std::accumulate(bins.begin(),bins.end(),Count(0))==count,"Invalid pT histogram counts");
            }
        }
        Count stored=0;for(const auto& f:c.at("files"))stored+=f.at("entries").get<Count>();need(selected<=stored,"Selected events exceed stored events");
        components.push_back(c);
    }
    std::set<std::string> found;for(const auto& c:components)found.insert(sample(c));need(found==wanted,"One or more requested QCD samples were not found");
    need(!files.empty(),"No input files");
    J ranges=J::array();for(unsigned i=0;i<files.size();++i)ranges.push_back({{"file",i},{"begin",0},{"end",files[i]["metadata"]["entries"]}});
    return {{"format","qcd_spike_plan"},{"version",1},{"production",k_uuid(TUUID())},{"index",0},{"contract",summary["contract"]},
        {"settings",o.settings},{"components",components},{"files",files},{"ranges",ranges},{"threads",o.threads}};
}

double value(const J& values,const std::string& key) {return values.contains(key) && values[key].is_number()?values[key].get<double>():std::numeric_limits<double>::quiet_NaN();}

J diagnose(const J& values,const J& cfg,const J& nonfinite) {
    J derived=J::object();std::set<std::string> flags;auto get=[&](const std::string& name){return value(values,name);};
    const double threshold=cfg["pt_min"],responseLimit=cfg["response_warning"];
    if(!nonfinite.empty())flags.insert("nonfinite_observable");
    if(values.contains("event") && values["event"].is_number_float() && std::abs(get("event"))>=9007199254740992.0)flags.insert("event_id_precision_limit");
    derived["pileup_pthatmax_over_generator_binvar"]=nullptr;
    derived["passes_pileup_veto"]=nullptr;
    if(!values.contains("Pileup_pthatmax") || !values.contains("Generator_binvar"))flags.insert("pileup_comparison_unavailable");
    else {
        const double pileup=get("Pileup_pthatmax"),generator=get("Generator_binvar");
        if(!std::isfinite(pileup) || !std::isfinite(generator) || pileup<0 || generator<=0)flags.insert("invalid_pileup_or_generator_scale");
        else {
            derived["pileup_pthatmax_over_generator_binvar"]=pileup/generator;
            derived["passes_pileup_veto"]=pileup<=generator;
            if(pileup>generator)flags.insert("pileup_scale_exceeds_generator");
        }
    }
    bool complete=true;std::array<bool,2> valid{};
    for(int j:{1,2}) {
        const auto s="_j"+std::to_string(j);const double pt=get("pTAK4PF"+s),jec=get("jetJecAK4PF"+s),idx=get("Jet_genJetIdx"+s),gen=get("GenJet_pt"+s);
        if(values.contains("jetJecAK4PF"+s)) {
            if(!std::isfinite(jec) || jec<=0)flags.insert("invalid_stored_jec");
            else {
                if(jec>cfg["jec_warning"].get<double>())flags.insert("large_stored_jec");
                if(std::isfinite(pt) && pt>=0) {derived["ak4_pt_div_stored_jec"+s]=pt/jec;if(pt/jec>=threshold)flags.insert("ak4_high_pt_before_jec_estimate");}
            }
        }
        complete=complete && values.contains("Jet_genJetIdx"+s) && values.contains("GenJet_pt"+s);
        valid[j-1]=std::isfinite(idx) && idx>=0 && std::floor(idx)==idx && std::isfinite(gen) && gen>0;
        if(std::isfinite(idx) && idx<0) {flags.insert("unmatched_ak4");if(gen>0)flags.insert("invalid_gen_match");}
        else if(values.contains("Jet_genJetIdx"+s) && values.contains("GenJet_pt"+s) && !valid[j-1])flags.insert("invalid_gen_match");
        if(valid[j-1] && std::isfinite(pt) && pt>0) {
            const double response=pt/gen;derived["ak4_reco_over_gen"+s]=response;
            if(response>responseLimit || response<1/responseLimit)flags.insert("large_reco_gen_response");
            if(gen>=threshold && response<=responseLimit && response>=1/responseLimit)flags.insert("high_pt_already_in_matched_gen_jet");
        }
    }
    if(!complete)flags.insert("generator_matching_unavailable");
    const bool distinct=get("Jet_genJetIdx_j1")!=get("Jet_genJetIdx_j2");
    if(valid[0] && valid[1] && !distinct)flags.insert("shared_gen_match");
    if(complete && std::isfinite(get("leadingJetHardScatterMatched")) && ((get("leadingJetHardScatterMatched")>0.5)!=(valid[0] && valid[1] && distinct)))flags.insert("gen_match_flag_inconsistent");
    auto dr=[&](int wide,int seed){const auto a="_j"+std::to_string(wide),b="_j"+std::to_string(seed);return std::hypot(get("etaWJ"+a)-get("etaAK4PF"+b),std::remainder(get("phiWJ"+a)-get("phiAK4PF"+b),2*std::acos(-1.0)));};
    const double direct=dr(1,1)+dr(2,2),swapped=dr(1,2)+dr(2,1);const bool swap=swapped<direct;
    if(std::isfinite(direct) && std::isfinite(swapped) && dr(1,swap?2:1)<=cfg["seed_dr"].get<double>() && dr(2,swap?1:2)<=cfg["seed_dr"].get<double>()) {
        for(int j:{1,2}) {
            const int seed=swap?3-j:j;const auto s="_j"+std::to_string(j);const double ak4=get("pTAK4PF_j"+std::to_string(seed)),wide=get("pTWJ"+s);
            derived["wide_seed_index"+s]=seed;derived["wide_seed_dr"+s]=dr(j,seed);
            if(ak4>0 && std::isfinite(ak4) && std::isfinite(wide)) {derived["wide_over_seed_pt"+s]=wide/ak4;if(wide/ak4>cfg["growth_warning"].get<double>())flags.insert("large_widejet_growth");}
        }
    } else flags.insert("widejet_seed_association_unavailable");
    std::array<double,4> sum{};bool vectors=true;
    for(int j:{1,2}) {
        const auto s="_j"+std::to_string(j);const double pt=get("pTWJ"+s),eta=get("etaWJ"+s),phi=get("phiWJ"+s),mass=get("massWJ"+s);
        if(!std::isfinite(pt) || !std::isfinite(eta) || !std::isfinite(phi) || !std::isfinite(mass) || pt<0 || mass<0) {vectors=false;break;}
        const double px=pt*std::cos(phi),py=pt*std::sin(phi),pz=pt*std::sinh(eta);
        sum[0]+=std::hypot(std::hypot(px,py),std::hypot(pz,mass));sum[1]+=px;sum[2]+=py;sum[3]+=pz;
    }
    if(vectors && std::isfinite(get("mjj"))) {
        const double mass=std::sqrt(std::max(0.0,sum[0]*sum[0]-sum[1]*sum[1]-sum[2]*sum[2]-sum[3]*sum[3]));
        derived["wide_mjj_recomputed"]=mass;derived["wide_mjj_relative_difference"]=(get("mjj")-mass)/std::max(mass,1.0);
        if(std::abs(derived["wide_mjj_relative_difference"].get<double>())>cfg["mass_relative_tolerance"].get<double>())flags.insert("wide_mass_inconsistent");
    } else flags.insert("wide_mass_check_unavailable");
    for(auto& item:derived.items())if(item.value().is_number() && !std::isfinite(item.value().get<double>())) {item.value()=nullptr;flags.insert("nonfinite_observable");}
    return {{"derived",derived},{"flags",flags}};
}

bool scalar(TLeaf* leaf) {
    static const std::set<std::string> types={"Double_t","Float_t","Double32_t","Float16_t","Int_t","UInt_t","Long64_t","ULong64_t","Long_t","ULong_t","Short_t","UShort_t","Char_t","UChar_t","Bool_t"};
    return leaf && !leaf->GetLeafCount() && leaf->GetLenStatic()==1 && types.count(leaf->GetTypeName());
}

J scan_piece(const J& plan,const J& range) {
    const auto& f=plan.at("files").at(range.at("file").get<unsigned>());const auto& c=plan.at("components").at(f.at("component").get<unsigned>());
    const std::string path=f.at("path"),runtime=f.value("runtime",path);const auto& meta=f.at("metadata");
    const Entry begin=range.at("begin"),end=range.at("end");
    std::unique_ptr<TFile> file(TFile::Open(runtime.c_str()));need(file && !file->IsZombie() && !file->TestBit(TFile::kRecovered),"Cannot open complete ROOT file: "+runtime);
    auto* tree=dynamic_cast<TTree*>(file->Get(kTreePathDefault));need(tree,"Missing analysis tree: "+path);
    need(tree->GetEntries()==meta.at("entries") && begin>=0 && end>=begin && end<=tree->GetEntries(),"Input size or entry range mismatch: "+path);
    need(!meta.contains("uuid") || meta["uuid"]==k_uuid(file->GetUUID()),"Input UUID differs from summary: "+path);
    need(read_event_counter(*file,path)==meta.at("generated_events"),"Generated counter differs from summary: "+path);
    J available=J::array(),missing=J::array(),all=J::array();
    TIter branches(tree->GetListOfBranches());while(auto* branch=branches())all.push_back(branch->GetName());
    for(const auto& name:fields())(scalar(tree->GetLeaf(name.c_str()))?available:missing).push_back(name);
    for(const auto& name:required) {auto* leaf=tree->GetLeaf(name.c_str());need(scalar(leaf) && std::string(leaf->GetTypeName())=="Double_t","Expected reduced-ntuple Double_t branch: "+name);}
    configure_tree_for_reading(*tree,std::set<std::string>(required.begin(),required.end()));
    std::array<std::vector<Count>,2> bins;std::array<const PlotSpec*,2> specs;
    for(int j=0;j<2;++j) {specs[j]=&kPlotSpecs.at(ptNames[j]);bins[j].resize(nbins_for_spec(*specs[j]));}
    struct Candidate {Entry entry;std::array<double,9> values;};std::vector<Candidate> candidates;Count scanned=0,selected=0;
    if(begin<end) {
        TTreeReader reader(tree);reader.SetEntriesRange(begin,end);
        std::vector<std::unique_ptr<TTreeReaderValue<double>>> v;for(const auto& name:required)v.emplace_back(std::make_unique<TTreeReaderValue<double>>(reader,name.c_str()));
        while(reader.Next()) {
            if(scanned==0)for(const auto& x:v)need(x->GetSetupStatus()>=0,"Invalid branch reader in "+path);
            ++scanned;
            if(!passes_base_selection(1,false,**v[0],**v[1],**v[2],**v[3],**v[4],**v[5],**v[6],**v[7],**v[8]))continue;
            ++selected;const double low=plan["settings"]["pt_min"],high=plan["settings"]["pt_max"];
            for(int j=0;j<2;++j) {const double pt=**v[3+j];need(std::isfinite(pt),"Nonfinite selected pT");const int bin=find_bin_index(*specs[j],pt);need(bin>=0,"Unexpected pT underflow");++bins[j][bin];}
            if((**v[3]>=low && **v[3]<high) || (**v[4]>=low && **v[4]<high)) {Candidate hit;hit.entry=reader.GetCurrentEntry();for(unsigned i=0;i<v.size();++i)hit.values[i]=**v[i];candidates.push_back(hit);}
        }
        need(reader.GetEntryStatus()==TTreeReader::kEntryBeyondEnd && scanned==Count(end-begin),"Incomplete entry-range read: "+path);
    }
    tree->SetBranchStatus("*",false);tree->SetCacheSize(16LL*1024*1024);
    for(const auto& n:available) {const std::string name=n;tree->SetBranchStatus(name.c_str(),true);tree->AddBranchToCache(name.c_str(),true);}tree->StopCacheLearningPhase();
    J events=J::array();
    for(const auto& hit:candidates) {
        tree->SetCacheEntryRange(hit.entry,hit.entry+1);need(tree->GetEntry(hit.entry)>0,"Cannot read candidate entry");
        for(unsigned i=0;i<required.size();++i)need(tree->GetLeaf(required[i].c_str())->GetValue()==hit.values[i],"Candidate entry locator failed validation");
        J values=J::object(),nonfinite=J::array();
        for(const auto& n:available) {
            const std::string name=n;auto* leaf=tree->GetLeaf(name.c_str());const std::string type=leaf->GetTypeName();
            if(type=="Double_t" || type=="Float_t" || type=="Double32_t" || type=="Float16_t") {
                const double x=leaf->GetValue();if(std::isfinite(x))values[name]=x;else {values[name]=nullptr;nonfinite.push_back(name);}
            } else if(type[0]=='U')values[name]=static_cast<ULong64_t>(leaf->GetValueLong64());else values[name]=leaf->GetValueLong64();
        }
        auto event=diagnose(values,plan.at("settings"),nonfinite);event["file"]=path;event["entry"]=hit.entry;event["sample"]=sample(c);
        event["event_weight_pb"]=c.at("xsec_pb").get<double>()/c.at("generated_events").get<double>();event["observables"]=values;event["nonfinite_branches"]=nonfinite;event["target_jets"]=J::array();
        for(int j:{1,2}) {const double pt=value(values,"pTWJ_j"+std::to_string(j));if(pt>=plan["settings"]["pt_min"].get<double>() && pt<plan["settings"]["pt_max"].get<double>())event["target_jets"].push_back(j);}
        events.push_back(event);
    }
    return {{"range",range},{"scanned",scanned},{"selected",selected},{"histograms",{{ptNames[0],bins[0]},{ptNames[1],bins[1]}}},{"events",events},
        {"available_branches",available},{"missing_or_nonscalar_branches",missing},{"all_branches",all}};
}

J coverage(const J& ranges) {
    std::map<unsigned,std::vector<std::pair<Entry,Entry>>> byFile;
    for(const auto& r:ranges) {const Entry b=r.at("begin"),e=r.at("end");need(b>=0 && e>=b,"Invalid entry range");byFile[r.at("file").get<unsigned>()].emplace_back(b,e);}
    J result=J::array();
    for(auto& item:byFile) {
        auto& rows=item.second;std::sort(rows.begin(),rows.end());Entry begin=-1,end=-1;
        std::set<std::pair<Entry,Entry>> seen;
        for(const auto& r:rows) {
            need(seen.insert(r).second && (end<0 || r.first>=end),"Duplicate or overlapping entry ranges");
            if(begin>=0 && r.first!=end) {result.push_back({{"file",item.first},{"begin",begin},{"end",end}});begin=-1;}
            if(begin<0)begin=r.first;end=r.second;
        }
        result.push_back({{"file",item.first},{"begin",begin},{"end",end}});
    }
    return result;
}

bool nano_mode(const J& p) {return p.value("mode",std::string())=="nano";}

Count exact_id(const J& v) {
    if(v.is_number_unsigned())return v.get<Count>();
    if(v.is_number_integer()) {need(v.get<std::int64_t>()>=0,"Negative event identifier");return v.get<Count>();}
    need(v.is_number_float(),"Event identifiers must be numeric");const double n=v.get<double>();
    need(std::isfinite(n) && n>=0 && n<9007199254740992.0 && std::floor(n)==n,"Floating-point event identifier is not exact; recover its original integer value first");return static_cast<Count>(n);
}

J nano_id(const J& target) {
    const auto& v=target.at("observables");return {{"run",exact_id(v.at("run"))},{"lumi",exact_id(v.at("lumi"))},{"event",exact_id(v.at("event"))}};
}

std::string id_key(const J& id) {return id.at("run").dump()+":"+id.at("lumi").dump()+":"+id.at("event").dump();}

std::string nano_url(std::string path,const Options& o,const fs::path& base) {
    if(path.rfind("/store/",0)==0) {auto host=o.nanoRedirector;while(!host.empty() && host.back()=='/')host.pop_back();need(host.rfind("root://",0)==0,"Expected an XRootD redirector");return host+"/"+path;}
    return eos_utils::normalize_storage_path(normalize_path_token(path,base));
}

void nano_expand(const std::string& token,const Options& o,const fs::path& base,std::vector<std::string>& paths,unsigned depth=0) {
    need(depth<8,"Too many nested NanoAOD lists");const bool list=!token.empty() && token[0]=='@';
    const auto path=nano_url(list?token.substr(1):token,o,base);
    if(list || (path.find("://")==std::string::npos && fs::path(path).extension()==".txt")) {
        for(const auto& line:clean_lines(path)) {need(line.find_first_of("\t ")==std::string::npos,"NanoAOD lists require one file path per line, without cross sections");nano_expand(line,o,fs::path(path).parent_path(),paths,depth+1);}
    } else {need(path.size()>5 && path.substr(path.size()-5)==".root","--nano-input expects a ROOT file or a text list");paths.push_back(path);}
}

std::vector<std::string> das_files(const std::string& dataset,const J& id,const Options& o) {
    const std::string query="file dataset="+dataset+" run="+id.at("run").dump()+" lumi="+id.at("lumi").dump();
    std::cout<<"[DAS] "<<query<<std::endl;const std::string cmd="dasgoclient -query "+quote(query)+" -limit 0";
    FILE* pipe=popen(cmd.c_str(),"r");need(pipe,"Cannot start dasgoclient");std::string output;char buffer[4096];while(fgets(buffer,sizeof(buffer),pipe))output+=buffer;
    need(pclose(pipe)==0,"DAS lookup failed; check cmsenv and the VOMS proxy, or provide --nano-input SAMPLE=LIST");
    std::vector<std::string> paths;std::istringstream lines(output);std::string line;
    while(std::getline(lines,line)) {line=eos_utils::trim(line);if(line.empty())continue;need(line.rfind("/store/",0)==0 && line.size()>5 && line.substr(line.size()-5)==".root","Unexpected DAS response: "+line);paths.push_back(nano_url(line,o,fs::current_path()));}
    need(!paths.empty(),"DAS found no file for "+query+"; verify the original production Dataset or use --nano-input");return paths;
}

J make_nano_plan(const Options& o) {
    std::ifstream input(o.nanoEvents);need(bool(input),"Cannot read "+o.nanoEvents.string());J targets=J::array();std::string line;std::set<std::string> seen;
    std::set<std::string> selected(o.samples.begin(),o.samples.end());if(selected.erase("3000toInf"))selected.insert("3000");
    while(std::getline(input,line)) {
        if(eos_utils::trim(line).empty())continue;const auto event=J::parse(line);const std::string label=event.at("sample");
        need(std::regex_match(label,std::regex("[0-9]+(to([0-9]+|Inf))?")),"Invalid sample label in candidate events");if(o.samplesSet && !selected.count(label))continue;
        const auto id=nano_id(event);need(seen.insert(label+":"+id_key(id)).second,"Repeated sample/run/lumi/event in candidates; resolve duplicate reduced events first");
        for(int j:{1,2})for(const std::string field:{"pTAK4PF","etaAK4PF","phiAK4PF"})need(std::isfinite(value(event.at("observables"),field+"_j"+std::to_string(j))),"Candidate lacks finite reduced AK4 jet coordinates");
        targets.push_back({{"sample",label},{"id",id},{"reduced",event}});
    }
    need(input.eof() && !targets.empty(),"No valid candidate events selected");
    std::map<std::string,std::vector<unsigned>> samples;for(unsigned i=0;i<targets.size();++i)samples[targets[i]["sample"]].push_back(i);
    for(const auto& entry:o.nanoInputs)need(samples.count(entry.first),"--nano-input sample has no selected candidate: "+entry.first);
    J files=J::array(),components=J::array();std::map<std::string,unsigned> fileIndex;
    auto add=[&](const std::vector<std::string>& paths,unsigned ci,const std::vector<unsigned>& ids) {
        for(const auto& path:paths) {
            const auto identity=k_identity(path);unsigned fi;
            if(!fileIndex.count(identity)) {fi=files.size();fileIndex[identity]=fi;files.push_back({{"path",path},{"component",ci},{"targets",J::array()}});}else fi=fileIndex.at(identity);
            need(files[fi]["component"]==ci,"One NanoAOD file was assigned to different QCD samples");
            auto& dest=files[fi]["targets"];for(auto i:ids)if(std::find(dest.begin(),dest.end(),J(i))==dest.end())dest.push_back(i);
        }
    };
    for(const auto& entry:samples) {
        const auto& label=entry.first;const unsigned ci=components.size();
        if(o.nanoInputs.count(label)) {
            std::vector<std::string> paths;nano_expand(o.nanoInputs.at(label),o,fs::current_path(),paths);need(!paths.empty(),"Empty NanoAOD input list");
            components.push_back({{"sample",label},{"source",o.nanoInputs.at(label)},{"lookup","explicit files"}});add(paths,ci,entry.second);
        } else {
            const auto cfg=o.nanoConfigDir/("QCDMC_2024_PT"+(label=="3000"?"3000toInf":label)+"_cfg.txt");
            std::string dataset;for(const auto& row:clean_lines(cfg))if(row.rfind("Dataset=",0)==0)dataset=eos_utils::trim(row.substr(8));
            need(std::regex_match(dataset,std::regex("/[^/ ]+/[^/ ]+/NANOAODSIM")),"Missing valid Dataset in "+cfg.string());
            need(sample(J{{"source",dataset}})==label,"Dataset QCD bin differs from candidate sample");
            components.push_back({{"sample",label},{"source",dataset},{"lookup","DAS run/lumi"},{"config",fs::absolute(cfg).string()}});
            std::map<std::pair<Count,Count>,std::vector<unsigned>> lumis;
            for(auto i:entry.second)lumis[{targets[i]["id"]["run"].get<Count>(),targets[i]["id"]["lumi"].get<Count>()}].push_back(i);
            for(const auto& ls:lumis)add(das_files(dataset,targets[ls.second.front()]["id"],o),ci,ls.second);
        }
    }
    J ranges=J::array();for(unsigned i=0;i<files.size();++i)ranges.push_back({{"file",i},{"begin",0},{"end",1}});
    std::cout<<"[INFO] Finding "<<targets.size()<<" target events in "<<files.size()<<" original NanoAOD files; only ID branches are scanned"<<std::endl;
    return {{"format","qcd_spike_plan"},{"version",1},{"mode","nano"},{"range_unit","file"},{"production",k_uuid(TUUID())},{"index",0},
        {"contract",kinematics_contract()},{"settings",o.nanoSettings},{"components",components},{"files",files},{"targets",targets},{"ranges",ranges},{"threads",o.threads}};
}

J leaf_number(TLeaf* leaf,int i=0) {
    const std::string type=leaf->GetTypeName();
    if(type=="Double_t" || type=="Float_t" || type=="Double32_t" || type=="Float16_t") {const double x=leaf->GetValue(i);return std::isfinite(x)?J(x):J(nullptr);}
    return type[0]=='U'?J(static_cast<ULong64_t>(leaf->GetValueLong64(i))):J(leaf->GetValueLong64(i));
}

double jet_dr(const J& a,const J& b) {return std::hypot(value(a,"eta")-value(b,"eta"),std::remainder(value(a,"phi")-value(b,"phi"),2*std::acos(-1.0)));}

J nano_compare(J& jets,const J& gen,const J& target,const J& settings) {
    J comparisons=J::array();std::set<std::string> flags;const auto& saved=target.at("reduced").at("observables");
    for(auto& jet:jets) {
        const double rawFactor=value(jet,"rawFactor"),pt=value(jet,"pt");
        if(std::isfinite(rawFactor) && rawFactor<1 && std::isfinite(pt)) {jet["raw_pt"]=pt*(1-rawFactor);jet["input_jec"]=1/(1-rawFactor);}
        int closest=-1;double best=std::numeric_limits<double>::infinity();
        for(unsigned i=0;i<gen.size();++i) {const double dr=jet_dr(jet,gen[i]);if(dr<best) {best=dr;closest=i;}}
        jet["nearest_gen"]=nullptr;
        if(closest>=0) {jet["nearest_gen"]={{"index",closest},{"dr",best},{"within_radius",best<=settings.at("gen_match_dr").get<double>()},{"pt",gen[closest].at("pt")}};
            if(value(gen[closest],"pt")>0)jet["nearest_gen"]["reco_over_gen_pt"]=pt/value(gen[closest],"pt");}
        jet["stored_gen_match"]=nullptr;const double idx=value(jet,"genJetIdx");
        if(std::isfinite(idx) && std::floor(idx)==idx && idx>=0 && idx<double(gen.size())) {
            const auto& g=gen.at(static_cast<unsigned>(idx));const double dr=jet_dr(jet,g);
            jet["stored_gen_match"]={{"index",int(idx)},{"pt",g.at("pt")},{"dr",std::isfinite(dr)?J(dr):J(nullptr)}};
            if(value(g,"pt")>0)jet["stored_gen_match"]["reco_over_gen_pt"]=pt/value(g,"pt");
        } else if(idx!=-1)flags.insert("invalid_nanoaod_gen_index");
    }
    std::array<J,2> seeds;for(int j=0;j<2;++j) {const auto s="_j"+std::to_string(j+1);seeds[j]={{"eta",saved.at("etaAK4PF"+s)},{"phi",saved.at("phiAK4PF"+s)}};}
    std::array<int,2> assigned{{-1,-1}};double best=std::numeric_limits<double>::infinity();unsigned alternatives=0;
    for(unsigned a=0;a<jets.size();++a)for(unsigned b=0;b<jets.size();++b)if(a!=b) {
        const double d1=jet_dr(seeds[0],jets[a]),d2=jet_dr(seeds[1],jets[b]);
        if(d1<=settings.at("seed_match_dr").get<double>() && d2<=settings.at("seed_match_dr").get<double>()) {++alternatives;if(d1+d2<best) {best=d1+d2;assigned={{int(a),int(b)}};}}
    }
    if(alternatives>1)flags.insert("ambiguous_reduced_seed_assignment");
    for(int j=0;j<2;++j) {
        const auto s="_j"+std::to_string(j+1);J c={{"reduced_seed",j+1},{"nano_jet_index",assigned[j]}};
        if(assigned[j]<0)flags.insert("reduced_seed_not_matched");
        else {
            const auto& jet=jets[assigned[j]];c["dr"]=jet_dr(seeds[j],jet);c["reduced_pt"]=saved.at("pTAK4PF"+s);c["nano_pt"]=jet.at("pt");
            if(value(jet,"pt")>0)c["reduced_over_nano_pt"]=value(saved,"pTAK4PF"+s)/value(jet,"pt");
            c["nano_raw_pt"]=jet.value("raw_pt",J(nullptr));const double reducedRaw=value(saved,"pTAK4PF"+s)/value(saved,"jetJecAK4PF"+s);
            if(std::isfinite(reducedRaw))c["reduced_raw_pt_estimate"]=reducedRaw;
            if(std::isfinite(reducedRaw) && value(jet,"raw_pt")>0) {const double diff=reducedRaw/value(jet,"raw_pt")-1;c["raw_pt_relative_difference"]=diff;
                if(std::abs(diff)>settings.at("raw_pt_relative_tolerance").get<double>())flags.insert("reduced_raw_pt_differs");}
            c["nano_gen_index"]=jet.at("genJetIdx");c["reduced_gen_index"]=saved.value("Jet_genJetIdx"+s,J(nullptr));c["stored_gen_match"]=jet.at("stored_gen_match");c["nearest_gen"]=jet.at("nearest_gen");
            if(saved.contains("Jet_genJetIdx"+s) && value(saved,"Jet_genJetIdx"+s)!=value(jet,"genJetIdx"))flags.insert("reduced_gen_index_differs");
            if(value(jet,"genJetIdx")<0)flags.insert("original_unmatched_seed");
            if(!jet.at("stored_gen_match").is_null()) {
                const double gpt=value(jet.at("stored_gen_match"),"pt"),response=value(jet.at("stored_gen_match"),"reco_over_gen_pt");
                if(saved.contains("GenJet_pt"+s) && std::isfinite(gpt) && std::abs(value(saved,"GenJet_pt"+s)-gpt)>1e-4*std::max(gpt,1.0))flags.insert("reduced_gen_pt_differs");
                if(std::isfinite(response) && (response>settings.at("response_warning").get<double>() || response<1/settings.at("response_warning").get<double>()))flags.insert("original_large_reco_gen_response");
            }
        }comparisons.push_back(c);
    }
    return {{"seeds",comparisons},{"assignment_candidates",alternatives},{"flags",flags}};
}

J nano_scan_file(const J& plan,const J& range) {
    const unsigned fi=range.at("file");const auto& f=plan.at("files").at(fi);const std::string path=f.at("path"),runtime=f.value("runtime",path);
    std::unique_ptr<TFile> file(TFile::Open(runtime.c_str()));need(file && !file->IsZombie() && !file->TestBit(TFile::kRecovered),"Cannot open complete NanoAOD: "+path);
    auto* tree=dynamic_cast<TTree*>(file->Get("Events"));need(tree,"Missing Events tree: "+path);
    for(const std::string name:{"run","luminosityBlock","event"}) {auto* leaf=tree->GetLeaf(name.c_str());need(scalar(leaf) && std::string(leaf->GetTypeName())==(name=="event"?"ULong64_t":"UInt_t"),"Unexpected NanoAOD ID branch type: "+name);}
    using Key=std::tuple<Count,Count,Count>;std::map<Key,unsigned> wanted;
    for(const auto& t:f.at("targets")) {const unsigned i=t;const auto& id=plan.at("targets").at(i).at("id");wanted.emplace(Key{exact_id(id.at("run")),exact_id(id.at("lumi")),exact_id(id.at("event"))},i);}
    std::map<Count,std::vector<std::pair<Key,unsigned>>> byEvent;for(const auto& item:wanted)byEvent[std::get<2>(item.first)].push_back(item);
    std::unordered_map<Count,std::vector<std::pair<Key,unsigned>>> lookup(byEvent.begin(),byEvent.end());
    configure_tree_for_reading(*tree,{"run","luminosityBlock","event"});std::vector<std::pair<Entry,unsigned>> hits;Entry scanned=0;
    {
        TTreeReader reader(tree);TTreeReaderValue<UInt_t> run(reader,"run"),lumi(reader,"luminosityBlock");TTreeReaderValue<ULong64_t> event(reader,"event");
        while(reader.Next()) {++scanned;const auto found=lookup.find(*event);if(found==lookup.end())continue;const Key key{*run,*lumi,*event};for(const auto& item:found->second)if(item.first==key)hits.emplace_back(reader.GetCurrentEntry(),item.second);}
        need(reader.GetEntryStatus()==TTreeReader::kEntryBeyondEnd && scanned==tree->GetEntries(),"Incomplete NanoAOD ID scan: "+path);
    }
    const std::vector<std::string> jetFields={"pt","eta","phi","mass","rawFactor","genJetIdx","area","jetId","chHEF","neHEF","chEmEF","neEmEF","muEF","hfHEF","hfEmEF","chMultiplicity","neMultiplicity","nConstituents","puId","partonFlavour","hadronFlavour"};
    const std::vector<std::string> genFields={"pt","eta","phi","mass","partonFlavour","hadronFlavour"};
    const std::vector<std::string> scalars={"run","luminosityBlock","event","nJet","nGenJet","PV_npvs","PV_npvsGood","PV_z","PV_score","Rho_fixedGridRhoFastjetAll","fixedGridRhoFastjetAll","Generator_binvar","Generator_scalePDF","Generator_weight","genWeight","Pileup_nTrueInt","Pileup_nPU","Pileup_pthatmax","Pileup_pudensity","Pileup_gpudensity","Pileup_sumEOOT","Pileup_sumLOOT","MET_pt","MET_phi","PuppiMET_pt","PuppiMET_phi","GenMET_pt","GenMET_phi"};
    J available=J::array(),missing=J::array();std::set<std::string> enabled;std::map<std::string,std::vector<std::string>> collections;
    for(const auto& name:scalars) {if(scalar(tree->GetLeaf(name.c_str()))) {enabled.insert(name);available.push_back(name);}else missing.push_back(name);}
    for(const std::string prefix:{"Jet","GenJet"})for(const auto& suffix:(prefix=="Jet"?jetFields:genFields)) {
        const auto name=prefix+"_"+suffix;auto* leaf=tree->GetLeaf(name.c_str());
        if(leaf && leaf->GetLeafCount() && std::string(leaf->GetLeafCount()->GetName())=="n"+prefix) {collections[prefix].push_back(suffix);enabled.insert(name);available.push_back(name);}else missing.push_back(name);
    }
    if(!hits.empty()) {
        for(const std::string name:{"nJet","nGenJet","Jet_pt","Jet_eta","Jet_phi","Jet_mass","Jet_rawFactor","Jet_genJetIdx","GenJet_pt","GenJet_eta","GenJet_phi","GenJet_mass"})need(enabled.count(name),"Matched NanoAOD event lacks required jet branch: "+name);
    }
    configure_tree_for_reading(*tree,enabled);tree->SetCacheSize(16LL*1024*1024);tree->StopCacheLearningPhase();J events=J::array();
    for(const auto& hit:hits) {
        tree->SetCacheEntryRange(hit.first,hit.first+1);need(tree->GetEntry(hit.first)>0,"Cannot load matched NanoAOD event");const auto& target=plan.at("targets").at(hit.second);
        J values=J::object();for(const auto& name:scalars)if(enabled.count(name))values[name]=leaf_number(tree->GetLeaf(name.c_str()));
        const J id={{"run",exact_id(values.at("run"))},{"lumi",exact_id(values.at("luminosityBlock"))},{"event",exact_id(values.at("event"))}};need(id==target.at("id"),"NanoAOD event locator changed on reread");
        J arrays=J::object();
        for(const std::string prefix:{"Jet","GenJet"}) {
            const auto n=exact_id(values.at("n"+prefix));need(n<=100000,"Invalid jet multiplicity");J jets=J::array();
            for(const auto& suffix:collections[prefix])need(tree->GetLeaf((prefix+"_"+suffix).c_str())->GetNdata()==int(n),"Inconsistent NanoAOD jet array length");
            for(unsigned i=0;i<n;++i) {J jet={{"index",i}};for(const auto& suffix:collections[prefix])jet[suffix]=leaf_number(tree->GetLeaf((prefix+"_"+suffix).c_str()),i);jets.push_back(jet);}arrays[prefix]=jets;
        }
        const auto comparison=nano_compare(arrays["Jet"],arrays["GenJet"],target,plan.at("settings"));
        events.push_back({{"target",hit.second},{"sample",target.at("sample")},{"id",id},{"file",path},{"entry",hit.first},{"observables",values},
            {"jets",arrays["Jet"]},{"gen_jets",arrays["GenJet"]},{"comparison",comparison}});
    }
    return {{"range",range},{"file",path},{"uuid",k_uuid(file->GetUUID())},{"entries",tree->GetEntries()},{"scanned",scanned},{"events",events},
        {"available_branches",available},{"missing_branches",missing}};
}

void nano_validate_result(const J& plan,const J& result) {
    need(result.at("format")=="qcd_nano_split" && result.at("version")==1 && result.at("complete")==true && result.at("production")==plan.at("production") &&
        result.at("index")==plan.at("index") && result.at("settings")==plan.at("settings"),"Invalid NanoAOD split");J ranges=J::array();std::set<std::pair<unsigned,Entry>> seen;
    for(const auto& piece:result.at("pieces")) {
        const auto& range=piece.at("range");const unsigned fi=range.at("file");const auto& f=plan.at("files").at(fi);ranges.push_back(range);
        need(piece.at("file")==f.at("path") && piece.at("entries").get<Entry>()>=0 && piece.at("scanned")==piece.at("entries"),"Incomplete NanoAOD file scan");
        for(const auto& event:piece.at("events")) {
            const unsigned ti=event.at("target");const auto& target=plan.at("targets").at(ti);const Entry entry=event.at("entry");
            need(std::find(f.at("targets").begin(),f.at("targets").end(),J(ti))!=f.at("targets").end() && event.at("id")==target.at("id") && event.at("sample")==target.at("sample") &&
                event.at("file")==f.at("path") && entry>=0 && entry<piece.at("entries").get<Entry>() && seen.emplace(fi,entry).second,"Wrong or duplicate NanoAOD match");
            need(event.at("jets").is_array() && event.at("gen_jets").is_array() && event.at("comparison").at("seeds").size()==2,"Missing jet comparison");
        }
    }
    need(coverage(ranges)==coverage(plan.at("ranges")),"Incomplete NanoAOD split coverage");
}

J nano_run(const J& plan) {
    const unsigned threads=plan.at("threads");need(threads>0 && threads<=128,"Invalid thread count");ROOT::EnableThreadSafety();ROOT::TThreadExecutor pool(threads);
    std::vector<J> ranges=plan.at("ranges").get<std::vector<J>>();std::mutex printing;unsigned done=0;
    auto pieces=pool.Map([&](const J& range) {auto piece=nano_scan_file(plan,range);std::lock_guard<std::mutex> guard(printing);
        std::cout<<"[NANO] "<<++done<<'/'<<ranges.size()<<" files; "<<piece["events"].size()<<" matches in "<<piece["scanned"]<<" entries: "<<piece["file"].get<std::string>()<<std::endl;return piece;},ranges);
    J result={{"format","qcd_nano_split"},{"version",1},{"complete",true},{"production",plan.at("production")},{"index",plan.at("index")},{"settings",plan.at("settings")},{"pieces",pieces}};
    nano_validate_result(plan,result);return result;
}

void validate_plan(const J& p) {
    need(p.at("format")=="qcd_spike_plan" && p.at("version")==1 && p.at("contract")==kinematics_contract(),"Invalid or incompatible inspection plan");
    need(!p.at("ranges").empty() && !p.at("files").empty(),"Empty inspection plan");
    for(const auto& r:p.at("ranges")) {const auto& f=p.at("files").at(r.at("file").get<unsigned>());
        if(nano_mode(p)) {need(r.at("begin")==0 && r.at("end")==1 && !f.at("targets").empty(),"Invalid NanoAOD file task");for(const auto& ti:f.at("targets"))need(p.at("targets").at(ti.get<unsigned>()).at("sample")==p.at("components").at(f.at("component").get<unsigned>()).at("sample"),"NanoAOD target sample mismatch");}
        else need(r.at("end").get<Entry>()<=f.at("metadata").at("entries").get<Entry>(),"Range exceeds file size");p.at("components").at(f.at("component").get<unsigned>());}
    coverage(p.at("ranges"));
}

void validate_result(const J& plan,const J& result) {
    if(nano_mode(plan)) {nano_validate_result(plan,result);return;}
    need(result.at("format")=="qcd_spike_split" && result.at("version")==1 && result.at("complete")==true &&
         result.at("production")==plan.at("production") && result.at("index")==plan.at("index") &&
         result.at("settings")==plan.at("settings") && result.at("contract")==plan.at("contract"),"Wrong or incomplete split output");
    J ranges=J::array();std::set<std::pair<unsigned,Entry>> events;
    for(const auto& piece:result.at("pieces")) {
        const auto& r=piece.at("range");const Entry begin=r.at("begin"),end=r.at("end");const unsigned fi=r.at("file");
        const auto& f=plan.at("files").at(fi);const auto& c=plan.at("components").at(f.at("component").get<unsigned>());
        ranges.push_back(r);const Count scanned=piece.at("scanned"),selected=piece.at("selected");
        need(scanned==Count(end-begin) && selected<=scanned,"Incomplete scan in split");
        for(const auto& name:ptNames) {const auto bins=piece.at("histograms").at(name).get<std::vector<Count>>();need(bins.size()==std::size_t(nbins_for_spec(kPlotSpecs.at(name))) && std::accumulate(bins.begin(),bins.end(),Count(0))==selected,"Invalid split histogram");}
        need(piece.at("events").size()<=selected,"Too many candidate events");
        for(const auto& event:piece.at("events")) {
            const Entry entry=event.at("entry");need(entry>=begin && entry<end && events.emplace(fi,entry).second && event.at("file")==f.at("path") && event.at("sample")==sample(c),"Wrong or duplicate candidate locator");
            need(event.at("event_weight_pb").get<double>()==c.at("xsec_pb").get<double>()/c.at("generated_events").get<double>(),"Candidate normalization differs");
            for(const auto& flag:event.at("flags"))need(descriptions.contains(flag.get<std::string>()),"Unknown diagnostic flag");
            need(!event.at("target_jets").empty(),"Candidate has no target jet");
        }
    }
    need(coverage(ranges)==coverage(plan.at("ranges")),"Incomplete or wrong split coverage");
}

J run(const J& plan) {
    validate_plan(plan);if(nano_mode(plan))return nano_run(plan);const unsigned threads=plan.at("threads");need(threads>0 && threads<=128,"Invalid thread count");
    Entry total=0;for(const auto& r:plan.at("ranges"))total+=r.at("end").get<Entry>()-r.at("begin").get<Entry>();
    const Entry chunk=std::max<Entry>(1,std::min<Entry>(5000000,(total+threads*2-1)/(threads*2)));
    std::vector<J> tasks;for(const auto& r:plan.at("ranges")) {
        const Entry begin=r.at("begin"),end=r.at("end");if(begin==end)tasks.push_back(r);
        for(Entry b=begin;b<end;b+=chunk)tasks.push_back({{"file",r.at("file")},{"begin",b},{"end",std::min(end,b+chunk)}});
    }
    std::vector<unsigned> indices(tasks.size());std::iota(indices.begin(),indices.end(),0);
    ROOT::EnableThreadSafety();ROOT::TThreadExecutor pool(threads);std::mutex printing;Entry done=0;
    auto last=std::chrono::steady_clock::now();const auto start=last;
    std::cout<<"[SCAN] "<<total<<" entries in "<<tasks.size()<<" disjoint chunks, "<<threads<<" threads"<<std::endl;
    auto pieces=pool.Map([&](unsigned index) {
        auto result=scan_piece(plan,tasks[index]);std::lock_guard<std::mutex> guard(printing);done+=result.at("scanned").get<Entry>();
        const auto now=std::chrono::steady_clock::now();if(done==total || std::chrono::duration<double>(now-last).count()>=10) {
            std::cout<<"[SCAN] "<<done<<'/'<<total<<" entries, "<<std::fixed<<std::setprecision(1)<<std::chrono::duration<double>(now-start).count()<<"s"<<std::endl;last=now;
        }return result;
    },indices);
    J result={{"format","qcd_spike_split"},{"version",1},{"complete",true},{"production",plan.at("production")},
        {"index",plan.at("index")},{"contract",plan.at("contract")},{"settings",plan.at("settings")},{"pieces",pieces}};
    validate_result(plan,result);return result;
}

std::string csv(const std::string& s) {std::string out="\"";for(char c:s) {if(c=='\"')out+='\"';out+=c;}return out+'\"';}

void nano_produce(const J& plan,const std::vector<J>& results,const fs::path& output,bool force) {
    J ranges=J::array(),files=J::array(),events=J::array(),targets=J::array();std::vector<unsigned> matches(plan.at("targets").size(),0);
    for(const auto& result:results)for(const auto& piece:result.at("pieces")) {
        ranges.push_back(piece.at("range"));auto file=piece;file.erase("events");files.push_back(file);
        for(auto event:piece.at("events")) {const unsigned ti=event.at("target");++matches.at(ti);event["reduced_event"]=plan.at("targets").at(ti).at("reduced");events.push_back(event);}
    }
    need(coverage(ranges)==coverage(plan.at("ranges")),"NanoAOD merge does not cover every requested file exactly once");
    std::sort(events.begin(),events.end(),[](const J& a,const J& b){return std::make_tuple(a.at("target").get<unsigned>(),a.at("file").get<std::string>(),a.at("entry").get<Entry>())<std::make_tuple(b.at("target").get<unsigned>(),b.at("file").get<std::string>(),b.at("entry").get<Entry>());});
    unsigned found=0,missing=0,ambiguous=0;J flags=J::object();
    for(const auto& event:events)for(const auto& flag:event.at("comparison").at("flags")) {const std::string key=flag;flags[key]=flags.value(key,0U)+1;}
    for(unsigned ti=0;ti<matches.size();++ti) {
        auto t=plan.at("targets").at(ti);t.erase("reduced");t["matches"]=matches[ti];const auto status=matches[ti]==0?"NOT_FOUND":(matches[ti]==1?"FOUND":"AMBIGUOUS");t["status"]=status;targets.push_back(t);
        if(matches[ti]==0)++missing;else if(matches[ti]==1)++found;else ++ambiguous;
        std::cout<<(matches[ti]==1?"[MATCH] ":"[WARN] ")<<t["sample"].get<std::string>()<<' '<<id_key(t["id"])<<" | "<<status<<" | "<<matches[ti]<<" matches"<<std::endl;
    }
    fs::create_directories(output);const int fd=open((output/".inspection.lock").c_str(),O_CREAT|O_RDWR,0600);need(fd>=0,"Cannot open output lock");
    struct Close {int fd;~Close(){close(fd);}} closeFile{fd};need(flock(fd,LOCK_EX|LOCK_NB)==0,"Output is being written by another process");
    for(const std::string name:{"nano_events.jsonl","nano_events.csv","nano_report.json"})need(force || !fs::exists(output/name),"NanoAOD reports exist; use --force");
    std::ostringstream jsonl,table;
    table<<"sample,run,lumi,event,status,nano_file,nano_entry,reduced_seed,nano_jet_index,dr,reduced_pt,nano_pt,nano_raw_pt,reduced_raw_pt_estimate,raw_pt_relative_difference,reduced_gen_index,nano_gen_index,gen_pt,reco_over_gen_pt,nearest_gen_index,nearest_gen_dr,flags\n";
    for(const auto& event:events) {
        jsonl<<event.dump()<<'\n';const auto& id=event.at("id");
        for(const auto& seed:event.at("comparison").at("seeds")) {
            table<<csv(event.at("sample"))<<','<<id.at("run")<<','<<id.at("lumi")<<','<<id.at("event")<<','<<(matches[event.at("target").get<unsigned>()]==1?"FOUND":"AMBIGUOUS")<<','<<csv(event.at("file"))<<','<<event.at("entry");
            for(const std::string key:{"reduced_seed","nano_jet_index","dr","reduced_pt","nano_pt","nano_raw_pt","reduced_raw_pt_estimate","raw_pt_relative_difference","reduced_gen_index","nano_gen_index"}) {table<<',';if(seed.contains(key) && !seed[key].is_null())table<<seed[key];}
            for(const auto& item:std::vector<std::pair<std::string,std::string>>{{"stored_gen_match","pt"},{"stored_gen_match","reco_over_gen_pt"},{"nearest_gen","index"},{"nearest_gen","dr"}}) {
                table<<',';if(seed.contains(item.first) && seed[item.first].is_object() && seed[item.first].contains(item.second) && !seed[item.first][item.second].is_null())table<<seed[item.first][item.second];}
            table<<','<<csv(event.at("comparison").at("flags").dump())<<'\n';
        }
    }
    J report={{"format","qcd_nano_report"},{"version",1},{"complete",true},{"all_targets_unique",missing==0 && ambiguous==0},{"production",plan.at("production")},
        {"settings",plan.at("settings")},{"sources",plan.at("components")},{"targets",targets},{"files",files},{"found",found},{"not_found",missing},{"ambiguous",ambiguous},{"comparison_flags",flags},
        {"limitations",{"Complete means every planned file was searched; all_targets_unique separately records whether every target was found exactly once.",
            "Only ID branches are scanned; full jet arrays are read for matched events, without kinematic or trigger cuts.",
            "Nearest generator jets are geometric candidates, not replacements for NanoAOD associations or proof of hard-scatter origin.",
            "Reduced/NanoAOD corrected pT can differ when JECs were reapplied. Raw-pT comparison assumes saved JEC semantics and no muon cleaning.",
            "This comparison does not reconstruct wide jets or prove reuse of a particular pileup event; that may require MiniAOD or simulation provenance.",
            "Ambiguous event identities retain every match; none is chosen silently. Missing targets require checking the dataset or input-file list."}}};
    textfile(output/"nano_events.jsonl.tmp",jsonl.str());textfile(output/"nano_events.csv.tmp",table.str());fs::rename(output/"nano_events.jsonl.tmp",output/"nano_events.jsonl");fs::rename(output/"nano_events.csv.tmp",output/"nano_events.csv");k_write_json(output/"nano_report.json",report);
    std::cout<<"[INFO] NanoAOD targets: "<<found<<" unique, "<<missing<<" missing, "<<ambiguous<<" ambiguous\n[INFO] Saved "<<output<<"/{nano_events.jsonl,nano_events.csv,nano_report.json}"<<std::endl;
}

void produce(const J& plan,const std::vector<J>& results,const fs::path& output,bool force) {
    if(nano_mode(plan)) {nano_produce(plan,results,output,force);return;}
    J ranges=J::array(),events=J::array(),components=J::array(),files=J::array();
    for(const auto& c:plan.at("components")) {
        J h=J::object();for(const auto& name:ptNames)h[name]=std::vector<Count>(nbins_for_spec(kPlotSpecs.at(name)),0);
        components.push_back({{"sample",sample(c)},{"source",c.at("source")},{"xsec_pb",c.at("xsec_pb")},
            {"generated_events",c.at("generated_events")},{"selected",0},{"candidates",0},{"flags",J::object()},{"histograms",h}});
    }
    for(const auto& f:plan.at("files"))files.push_back({{"path",f.at("path")},{"scanned",0}});
    std::set<std::pair<std::string,Entry>> seenEvents;
    for(const auto& result:results)for(const auto& piece:result.at("pieces")) {
        const auto& r=piece.at("range");ranges.push_back(r);const unsigned fi=r.at("file"),ci=plan.at("files").at(fi).at("component");
        auto& c=components.at(ci);c["selected"]=c["selected"].get<Count>()+piece.at("selected").get<Count>();
        auto& f=files.at(fi);f["scanned"]=f["scanned"].get<Count>()+piece.at("scanned").get<Count>();
        for(const std::string field:{"available_branches","missing_or_nonscalar_branches","all_branches"}) {
            need(!f.contains(field) || f[field]==piece.at(field),"Branch layout differs between file chunks");f[field]=piece.at(field);
        }
        for(const auto& name:ptNames)for(unsigned b=0;b<c["histograms"][name].size();++b)c["histograms"][name][b]=c["histograms"][name][b].get<Count>()+piece.at("histograms").at(name).at(b).get<Count>();
        for(const auto& event:piece.at("events")) {
            need(seenEvents.emplace(event.at("file").get<std::string>(),event.at("entry").get<Entry>()).second,"Duplicate candidate while merging");events.push_back(event);
            c["candidates"]=c["candidates"].get<Count>()+1;for(const auto& flag:event.at("flags")) {const std::string key=flag;c["flags"][key]=c["flags"].value(key,Count(0))+1;}
        }
    }
    need(coverage(ranges)==coverage(plan.at("ranges")),"Merge does not cover all requested entries exactly once");
    for(unsigned ci=0;ci<components.size();++ci) {
        const auto& original=plan.at("components").at(ci);auto& c=components[ci];Count expected=0;
        for(const std::string cat:{"pass","fail","unavailable"})expected+=original.at("categories").at(cat).at("events").get<Count>();
        need(c["selected"]==expected,"Selected-event count differs from original JSON for "+sample(original));
        for(const auto& name:ptNames)for(unsigned b=0;b<c["histograms"][name].size();++b) {
            Count count=0;for(const std::string cat:{"pass","fail","unavailable"})count+=original.at("categories").at(cat).at("histograms").at(name).at(b).get<Count>();
            need(c["histograms"][name][b]==count,"pT histogram differs from original JSON for "+sample(original));
        }
        c["summary_counts_match"]=true;std::cout<<"[CHECK] "<<sample(original)<<": "<<c["candidates"]<<" candidate events; selected count and both pT histograms match JSON"<<std::endl;
        for(const auto& flag:c["flags"].items())std::cout<<"  "<<flag.key()<<": "<<flag.value()<<'\n';
    }
    std::sort(events.begin(),events.end(),[](const J& a,const J& b){return std::make_pair(a.at("file").get<std::string>(),a.at("entry").get<Entry>())<std::make_pair(b.at("file").get<std::string>(),b.at("entry").get<Entry>());});
    fs::create_directories(output);const int fd=open((output/".inspection.lock").c_str(),O_CREAT|O_RDWR,0600);
    need(fd>=0,"Cannot open output lock");struct Close {int fd;~Close(){close(fd);}} closeFile{fd};need(flock(fd,LOCK_EX|LOCK_NB)==0,"Output is being written by another process");
    for(const std::string name:{"events.jsonl","events.csv","report.json"})need(force || !fs::exists(output/name),"Reports exist; use --force");
    std::ostringstream jsonl,table;table<<"sample,file,entry,event_weight_pb,target_jets,flags";for(const auto& name:fields())table<<','<<name;table<<",derived_json\n";
    for(const auto& event:events) {
        jsonl<<event.dump()<<'\n';table<<csv(event["sample"])<<','<<csv(event["file"])<<','<<event["entry"]<<','<<event["event_weight_pb"]<<','<<csv(event["target_jets"].dump())<<','<<csv(event["flags"].dump());
        for(const auto& name:fields()) {table<<',';if(event["observables"].contains(name) && !event["observables"][name].is_null())table<<event["observables"][name].dump();}table<<','<<csv(event["derived"].dump())<<'\n';
    }
    J report={{"format","qcd_spike_report"},{"version",1},{"complete",true},{"production",plan.at("production")},
        {"settings",plan.at("settings")},{"contract",plan.at("contract")},{"trigger","all"},{"flag_descriptions",descriptions},
        {"components",components},{"files",files},{"candidate_events",events.size()},
        {"limitations",{"Flags are diagnostic clues, not proof of a cause or event-rejection criteria.",
            "Unmatched generator jets and high vertex multiplicity alone do not prove pileup.",
            "pT divided by stored JEC estimates raw pT only when that JEC is the applied multiplicative factor; raw-only production and muon cleaning can invalidate the estimate.",
            "Only two AK4 seeds are available; full constituent-level wide-jet reconstruction cannot be repeated.",
            "Hard-process and pileup details require the original NanoAOD if absent here.",
            "File and entry are authoritative; double event IDs at or above 2^53 can have lost precision.",
            "Weights are cross section / generated events in pb; no luminosity or fitted normalization is applied."}}};
    textfile(output/"events.jsonl.tmp",jsonl.str());textfile(output/"events.csv.tmp",table.str());
    fs::rename(output/"events.jsonl.tmp",output/"events.jsonl");fs::rename(output/"events.csv.tmp",output/"events.csv");k_write_json(output/"report.json",report);
    std::cout<<"[INFO] Saved "<<events.size()<<" events: "<<output<<"/{events.csv,events.jsonl,report.json}"<<std::endl;
}

void worker(const fs::path& input) {
    const auto plan=k_read_json(input);auto result=run(plan);result["plan_md5"]=md5(input);
    const std::string name=plan.at("result"),receipt=plan.at("receipt");need(fs::path(name).filename()==name && fs::path(receipt).filename()==receipt,"Invalid worker output name");
    k_write_json(name,result);k_write_json(receipt,{{"production",plan.at("production")},{"index",plan.at("index")},{"md5",md5(name)},{"plan_md5",md5(input)}});
}

void condor_path(const fs::path& p) {
    const auto s=p.string();need(s.find_first_of(" \t\n\r,;=$\"\\")==std::string::npos,"Unsupported Condor path characters: "+s);
}

J checked_job(const fs::path& dir,const J& production,const J& job) {
    const auto plan=k_read_json(dir/job.at("input").get<std::string>());
    need(plan.at("production")==production.at("plan").at("production") && plan.at("index")==job.at("index") && plan.at("ranges")==job.at("ranges"),"Prepared job identity mismatch");
    const auto path=dir/"parts"/job.at("result").get<std::string>();const auto receipt=k_read_json(dir/"parts"/job.at("receipt").get<std::string>());
    const std::string hash=md5(dir/job.at("input").get<std::string>());
    need(receipt.at("production")==plan.at("production") && receipt.at("index")==job.at("index") && receipt.at("md5")==md5(path) && receipt.at("plan_md5")==hash,"Invalid/incomplete output receipt");
    const auto result=k_read_json(path);need(result.at("plan_md5")==hash,"Wrong input manifest in output");validate_result(plan,result);return result;
}

J load_production(const fs::path& dir) {
    const auto p=k_read_json(dir/"production.json");need(p.at("format")=="qcd_spike_production" && p.at("version")==1 && !p.at("jobs").empty(),"Invalid production");
    validate_plan(p.at("plan"));J ranges=J::array();std::set<unsigned> indices;
    for(const auto& item:p.at("frozen_md5").items()) {need(fs::path(item.key()).filename()==item.key(),"Invalid frozen filename");need(md5(dir/item.key())==item.value(),"Prepared input changed: "+item.key());}
    for(const auto& job:p.at("jobs")) {
        need(indices.insert(job.at("index").get<unsigned>()).second,"Duplicate job index");
        for(const std::string key:{"input","script","jdl","result","receipt"})need(fs::path(job.at(key).get<std::string>()).filename()==job.at(key).get<std::string>(),"Invalid job filename");
        for(const auto& r:job.at("ranges"))ranges.push_back(r);
        const auto part=k_read_json(dir/job.at("input").get<std::string>());
        need(nano_mode(part)==nano_mode(p.at("plan")),"Split inspection mode differs");
        if(nano_mode(part))need(part.at("targets")==p.at("plan").at("targets") && part.at("range_unit")=="file","Split NanoAOD targets differ");
        for(const std::string key:{"production","settings","contract","components"})need(part.at(key)==p.at("plan").at(key),"Split definition differs from production");
        need(part.at("files").size()==p.at("plan").at("files").size(),"Split file map differs");
        for(unsigned i=0;i<part.at("files").size();++i) {auto file=part.at("files")[i];file.erase("runtime");need(file==p.at("plan").at("files")[i],"Split file identity differs");}
        need(part.at("ranges")==job.at("ranges") && part.at("index")==job.at("index") && part.at("result")==job.at("result") && part.at("receipt")==job.at("receipt"),"Split job metadata differs");
    }
    need(coverage(ranges)==coverage(p.at("plan").at("ranges")),"Prepared splits do not partition the production");return p;
}

void submit(const fs::path& dir,const J& production,const Options& o,bool fresh) {
    std::ostringstream pending;unsigned count=0;
    for(const auto& job:production.at("jobs")) {
        bool valid=false;if(!fresh)try {checked_job(dir,production,job);valid=true;}catch(const std::exception&) {}
        if(!valid) {pending<<job.at("jdl").get<std::string>()<<'\n';++count;}
    }
    if(!count) {std::cout<<"[INFO] All splits complete; run --merge "<<dir<<std::endl;return;}
    textfile(dir/"pending_jobs.txt",pending.str());
    std::string cmd="python3 "+quote((dir/"condor_submit_jobs.py").string())+" "+quote(dir.string())+" --jdl-list pending_jobs.txt";
    if(!fresh)cmd+=(o.action=="--retry-failed"?" --retry-failed":" --resume");
    for(const auto& item:std::vector<std::pair<std::string,std::string>>{{"--proxy",o.proxy},{"--shared-dir",o.shared.empty()?production.value("shared_dir",std::string()):o.shared},
        {"--schedd",o.schedd.empty()?production.value("schedd",std::string()):o.schedd},{"--pool",o.pool.empty()?production.value("pool",std::string()):o.pool}})if(!item.second.empty())cmd+=" "+item.first+" "+quote(item.second);
    command(cmd);
}

void prepare(const Options& o,const fs::path& source) {
    need(std::regex_match(o.cmssw,std::regex("CMSSW_[A-Za-z0-9_]+")) && std::regex_match(o.arch,std::regex("[A-Za-z0-9_]+")),"Run cmsenv first, or supply --cmssw-version and --scram-arch");
    need((env("CMSSW_VERSION").empty() || env("CMSSW_VERSION")==o.cmssw) && (env("SCRAM_ARCH").empty() || env("SCRAM_ARCH")==o.arch),"Worker CMSSW/architecture must match the build environment");
    const auto output=fs::absolute(o.output).lexically_normal();const auto dir=fs::absolute(o.workdir.empty()?output/"condor":o.workdir).lexically_normal();condor_path(dir);condor_path(output);
    need(output.string().find("://")==std::string::npos && output.string().rfind("/eos/",0)!=0,"Output must be a local/shared directory");
    need(!fs::exists(dir),"Condor workdir already exists; use --submit, --retry-failed, or a new directory");
    auto plan=o.nanoEvents.empty()?make_plan(o):make_nano_plan(o);
    fs::create_directories(dir/"parts");fs::create_directories(dir/"logs");fs::create_directories(dir/"inputs");
    for(const std::string name:{"condor_submit_jobs.py","eos_utils.py"})fs::copy_file(source.parent_path()/name,dir/name);
    std::cout<<"[INFO] Building portable C++ worker once"<<std::endl;
    command("c++ -O3 -DNDEBUG "+quote(source.string())+" -o "+quote((dir/"inspect_qcd_spikes").string())+" $(root-config --cflags --libs)");
    Entry total=0;for(const auto& f:plan["files"])total+=nano_mode(plan)?1:f["metadata"]["entries"].get<Entry>();
    const Entry target=std::max<Entry>(1,(total+o.jobs-1)/o.jobs);std::vector<J> ranges;
    for(unsigned fi=0;fi<plan["files"].size();++fi) {
        const Entry end=nano_mode(plan)?1:plan["files"][fi]["metadata"]["entries"].get<Entry>();
        if(!end)ranges.push_back({{"file",fi},{"begin",0},{"end",0}});
        for(Entry begin=0;begin<end;begin+=target)ranges.push_back({{"file",fi},{"begin",begin},{"end",std::min(end,begin+target)}});
    }
    const unsigned number=std::min<std::size_t>(o.jobs,ranges.size());std::vector<J> assigned(number,J::array());std::vector<Entry> loads(number,0);
    std::stable_sort(ranges.begin(),ranges.end(),[](const J& a,const J& b){return a.at("end").get<Entry>()-a.at("begin").get<Entry>()>b.at("end").get<Entry>()-b.at("begin").get<Entry>();});
    for(unsigned i=0;i<ranges.size();++i) {const unsigned job=i<number?i:std::min_element(loads.begin(),loads.end())-loads.begin();assigned[job].push_back(ranges[i]);loads[job]+=ranges[i]["end"].get<Entry>()-ranges[i]["begin"].get<Entry>();}
    std::map<unsigned,std::pair<std::string,std::uintmax_t>> local;
    for(unsigned fi=0;fi<plan["files"].size();++fi) {
        const std::string path=plan["files"][fi]["path"];if(path.find("://")!=std::string::npos)continue;
        const auto alias="input_"+std::to_string(fi)+".root";fs::create_symlink(fs::absolute(path),dir/"inputs"/alias);local[fi]={alias,fs::file_size(path)};
    }
    J p={{"format","qcd_spike_production"},{"version",1},{"plan",plan},{"output",output.string()},
        {"cmssw_version",o.cmssw},{"scram_arch",o.arch},{"shared_dir",o.shared},{"schedd",o.schedd},{"pool",o.pool},{"jobs",J::array()},{"frozen_md5",J::object()}};
    for(const std::string name:{"inspect_qcd_spikes","condor_submit_jobs.py","eos_utils.py"})p["frozen_md5"][name]=md5(dir/name);
    for(unsigned index=0;index<number;++index) {
        const std::string batch=nano_mode(plan)?"inspect_QCD_nano":"inspect_QCD_spikes";
        std::ostringstream label;label<<batch<<'_'<<std::setw(4)<<std::setfill('0')<<index;const auto stem=label.str();
        J job={{"index",index},{"ranges",assigned[index]},{"input",stem+".input.json"},{"script",stem+".sh"},
               {"jdl",stem+".jdl"},{"result",stem+".json"},{"receipt",stem+".done.json"}};
        auto part=plan;for(const std::string key:{"index","ranges","result","receipt"})part[key]=job[key];
        std::string inputs="inspect_qcd_spikes, "+job["input"].get<std::string>()+", "+job["script"].get<std::string>();std::set<unsigned> attached;std::uintmax_t bytes=0;
        for(const auto& range:assigned[index]) {
            const unsigned fi=range["file"];if(local.count(fi) && attached.insert(fi).second) {part["files"][fi]["runtime"]=local.at(fi).first;inputs+=", inputs/"+local.at(fi).first;bytes+=local.at(fi).second;}
        }
        k_write_json(dir/job["input"].get<std::string>(),part);
        const std::string script="#!/usr/bin/env bash\nset -eo pipefail\nexport SCRAM_ARCH="+quote(o.arch)+
            "\nsource /cvmfs/cms.cern.ch/cmsset_default.sh\njob_scratch=\"${_CONDOR_SCRATCH_DIR:-$PWD}\"\ncd \"$job_scratch\"\nscramv1 project CMSSW "+quote(o.cmssw)+
            "\ncd "+quote(o.cmssw)+"/src\neval \"$(scramv1 runtime -sh)\"\ncd \"$job_scratch\"\nexec ./inspect_qcd_spikes --worker "+quote(job["input"])+"\n";
        textfile(dir/job["script"].get<std::string>(),script);fs::permissions(dir/job["script"].get<std::string>(),fs::perms::owner_exec|fs::perms::group_exec|fs::perms::others_exec,fs::perm_options::add);
        std::ostringstream jdl;jdl<<"universe = vanilla\nbatch_name = "<<batch<<"\n+DijetDataset = \""<<batch<<"\"\nexecutable = "<<job["script"].get<std::string>()
            <<"\nshould_transfer_files = YES\nwhen_to_transfer_output = ON_SUCCESS\nsuccess_exit_code = 0\ntransfer_input_files = "<<inputs
            <<"\ntransfer_output_files = "<<job["result"].get<std::string>()<<", "<<job["receipt"].get<std::string>()
            <<"\ntransfer_output_remaps = \""<<job["result"].get<std::string>()<<"=parts/"<<job["result"].get<std::string>()<<';'<<job["receipt"].get<std::string>()<<"=parts/"<<job["receipt"].get<std::string>()
            <<"\"\nrequest_cpus = "<<o.threads<<"\nrequest_memory = "<<o.memory<<"MB\nrequest_disk = "<<o.disk+(bytes+1048575)/1048576<<"MB\nuse_x509userproxy = true\nx509userproxy = proxy_resolved_at_submission\n"
            <<"output = logs/"<<stem<<".$(Cluster).$(Process).out\nerror = logs/"<<stem<<".$(Cluster).$(Process).err\nlog = logs/"<<stem<<".$(Cluster).$(Process).log\n";
        if(!o.flavour.empty())jdl<<"+JobFlavour = "<<J(o.flavour).dump()<<'\n';jdl<<"queue 1\n";textfile(dir/job["jdl"].get<std::string>(),jdl.str());
        for(const std::string key:{"input","script","jdl"}) {const std::string name=job[key];p["frozen_md5"][name]=md5(dir/name);}p["jobs"].push_back(job);
    }
    k_write_json(dir/"production.json",p);load_production(dir);
    std::cout<<"[INFO] Prepared "<<number<<" jobs for "<<total<<(nano_mode(plan)?" file tasks from ":" entries from ")<<plan["files"].size()<<" files\n[INFO] Workdir: "<<dir<<"\n[INFO] Final diagnostics: "<<output<<std::endl;
    if(!o.noSubmit)submit(dir,p,o,true);
}

int main_impl(int argc,char** argv) {
    const auto o=arguments(argc,argv);
    if(!o.proxy.empty())need(setenv("X509_USER_PROXY",normalize_path_token(o.proxy,fs::current_path()).c_str(),1)==0,"Cannot set submission/read proxy");
    if(!o.worker.empty()) {worker(o.worker);return 0;}
    if(!o.action.empty()) {
        need(!o.force || o.action=="--merge","--force applies only to local output or merge");const auto dir=fs::absolute(o.existing).lexically_normal();const auto p=load_production(dir);
        if(o.action=="--merge") {std::vector<J> results;for(const auto& job:p["jobs"])results.push_back(checked_job(dir,p,job));produce(p.at("plan"),results,p.at("output").get<std::string>(),o.force);}
        else submit(dir,p,o,false);return 0;
    }
    if(o.condor) {
        fs::path source=__FILE__;if(!fs::exists(source))source=fs::absolute(argv[0]).parent_path()/"inspect_qcd_spikes.cpp";
        need(fs::exists(source),"Cannot locate inspect_qcd_spikes.cpp for the portable worker build");prepare(o,fs::canonical(source));
    } else {
        for(const std::string name:{"events.jsonl","events.csv","report.json"})need(o.force || !fs::exists(o.output/(o.nanoEvents.empty()?name:"nano_"+name)),"Reports exist; use --force");
        const auto plan=o.nanoEvents.empty()?make_plan(o):make_nano_plan(o);const auto result=run(plan);produce(plan,{result},fs::absolute(o.output).lexically_normal(),o.force);
    }
    return 0;
}
}

int main(int argc,char** argv) {
    try {return spikes::main_impl(argc,argv);}catch(const std::exception& error) {std::cerr<<"[ERROR] "<<error.what()<<std::endl;return 1;}
}
