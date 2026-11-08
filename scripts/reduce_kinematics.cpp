#define main kinematics_reference_main
#include "plot_kinematics.cpp"
#undef main
#include <nlohmann/json.hpp>
#include <TUUID.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

using KJSON = nlohmann::json;
using KCount = std::uint64_t;

KJSON kinematics_contract() {
    KJSON plots=KJSON::object();
    for(const auto& item:kPlotSpecs) {
        const auto& s=item.second;
        std::vector<double> edges=s.bin_edges;
        if(!s.variable_bins) for(int i=0;i<=s.bins;++i) edges.push_back(s.xmin+(s.xmax-s.xmin)*i/s.bins);
        plots[item.first]={{"edges",edges},{"variable_bins",s.variable_bins}};
    }
    return {{"version",1},{"tree",kTreePathDefault},{"mjj_min",kDefaultMinMjj},{"eta_max",kEtaCut},
            {"deta_max",kDeltaEtaCut},{"pt_min",30.0},{"njet_min",1.0},{"id_min",0.5},{"json_min",0.5},
            {"trigger","passHLT_PFScoutingHT"},{"trigger_min",0.5},{"weights","unit"},
            {"selection","passes_base_selection; stored deltaETAjj and deltaPHIjj; data PassJSON only"},
            {"flow","discard underflow; include overflow in final bin"},{"plots",plots}};
}

KJSON kinematics_pileup_veto() {
    return {{"version",1},{"selection","Pileup_pthatmax <= Generator_binvar"},
            {"normalization","unfiltered generated_events"}};
}

void k_write_json(const fs::path& path,const KJSON& value) {
    const fs::path temporary=path.string()+".tmp."+std::to_string(getpid());
    try {
        std::ofstream out(temporary);
        if(!out) throw std::runtime_error("Cannot write "+temporary.string());
        out<<value.dump()<<'\n';out.close();
        if(!out) throw std::runtime_error("Failed writing "+temporary.string());
        fs::rename(temporary,path);
    } catch(...) {fs::remove(temporary);throw;}
}

KJSON k_read_json(const fs::path& path) {
    std::ifstream in(path);
    if(!in) throw std::runtime_error("Cannot read "+path.string());
    KJSON value;in>>value;return value;
}

std::string k_identity(const std::string& path) {
    const auto at=path.find("/store/");
    return at==std::string::npos?path:path.substr(at);
}

std::string k_uuid(const TUUID& uuid) {
    UChar_t bytes[16];uuid.GetUUID(bytes);
    std::ostringstream out;out<<std::hex<<std::setfill('0');
    for(const auto byte:bytes)out<<std::setw(2)<<static_cast<unsigned>(byte);
    return out.str();
}

bool k_label(const std::string& label) {
    return !label.empty() && std::isalnum(static_cast<unsigned char>(label[0])) &&
        label.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-")==std::string::npos;
}

struct KOptions {
    std::vector<std::string> inputs;
    std::string mcList,manifest,dataset,jec,output="kinematics";
    unsigned threads=8;
    bool resume=false,force=false,plan=false,describe=false,pileupVeto=false;
};

KOptions k_parse(int argc,char** argv) {
    KOptions o;
    for(int i=1;i<argc;++i) {
        const std::string arg=argv[i];
        auto value=[&](){if(++i>=argc)throw std::runtime_error("Missing value for "+arg);return std::string(argv[i]);};
        if(arg=="--input")o.inputs.push_back(value());
        else if(arg=="--mc-list")o.mcList=value();
        else if(arg=="--manifest")o.manifest=value();
        else if(arg=="--dataset")o.dataset=value();
        else if(arg=="--jec")o.jec=value();
        else if(arg=="--output")o.output=value();
        else if(arg=="--threads") {const auto n=std::stoll(value());if(n<1 || n>1024)throw std::runtime_error("Invalid threads");o.threads=n;}
        else if(arg=="--resume")o.resume=true;
        else if(arg=="--force")o.force=true;
        else if(arg=="--pileup-veto")o.pileupVeto=true;
        else if(arg=="--plan")o.plan=true;
        else if(arg=="--describe")o.describe=true;
        else if(arg=="--help" || arg=="-h") {
            std::cout<<"Usage: reduce_kinematics (--input ROOT|DIR|@LIST | --mc-list LIST) --dataset NAME [--jec old|new]\n"
                     <<"       [--output DIRECTORY|FILE.json] [--threads N] [--resume | --force] [--pileup-veto]\n"
                     <<"       --pileup-veto stores both unfiltered and vetoed MC histograms in one scan.\n"
                     <<"       --plan prints a frozen input plan; --describe prints the analysis contract.\n"
                     <<"       Checkpoints are saved after each completed file.\n";std::exit(0);
        } else throw std::runtime_error("Unknown argument: "+arg);
    }
    if(o.resume && o.force)throw std::runtime_error("--resume and --force are mutually exclusive");
    return o;
}

void k_expand(const std::string& value,const fs::path& base,std::vector<std::string>& out,unsigned depth=0) {
    if(depth>8)throw std::runtime_error("Input list nesting exceeds eight levels");
    const bool list=!value.empty() && value[0]=='@';
    const std::string path=normalize_path_token(list?value.substr(1):value,base);
    if(list || (fs::path(path).extension()==".txt" && path.find("://")==std::string::npos)) {
        for(const auto& line:clean_lines(path))k_expand(line,fs::path(path).parent_path(),out,depth+1);
    } else {
        auto files=expand_source_token(value,base);
        out.insert(out.end(),files.begin(),files.end());
    }
}

KJSON k_plan(const KOptions& o) {
    if(!o.manifest.empty()) {
        if(!o.inputs.empty() || !o.mcList.empty() || !o.dataset.empty() || !o.jec.empty() || o.pileupVeto)
            throw std::runtime_error("--manifest supplies the production inputs and labels");
        auto plan=k_read_json(o.manifest);
        if(plan.at("contract")!=kinematics_contract())throw std::runtime_error("Manifest/reducer analysis mismatch");
        if(plan.contains("pileup_veto") && (plan.at("kind")!="mc" || plan.at("pileup_veto")!=kinematics_pileup_veto()))
            throw std::runtime_error("Invalid pileup-veto manifest");
        return plan;
    }
    if(!k_label(o.dataset) || (!o.jec.empty() && o.jec!="old" && o.jec!="new"))throw std::runtime_error("Invalid dataset or JEC label");
    if(o.inputs.empty()==o.mcList.empty())throw std::runtime_error("Provide --input or --mc-list, exclusively");
    if(o.pileupVeto && o.mcList.empty())throw std::runtime_error("--pileup-veto is only supported with --mc-list");
    KJSON plan={{"version",1},{"dataset",o.dataset},{"jec",o.jec},{"kind",o.mcList.empty()?"data":"mc"},
                {"contract",kinematics_contract()},{"components",KJSON::array()},{"tasks",KJSON::array()}};
    if(o.pileupVeto)plan["pileup_veto"]=kinematics_pileup_veto();
    if(o.mcList.empty()) {
        std::vector<std::string> files;
        for(const auto& input:o.inputs)k_expand(input,fs::current_path(),files);
        plan["components"].push_back({{"id","data"},{"source","data"},{"xsec_pb",0.0}});
        for(const auto& path:files)plan["tasks"].push_back({{"path",eos_utils::normalize_storage_path(path)},{"component","data"}});
    } else {
        if(!o.jec.empty())throw std::runtime_error("--jec is for data; use --dataset to label MC");
        const fs::path base=fs::absolute(o.mcList).parent_path();
        for(const auto& line:clean_lines(o.mcList)) {
            std::istringstream in(line);std::string source,extra;double xsec=0;
            if(!(in>>source>>xsec) || (in>>extra) || !std::isfinite(xsec) || xsec<=0)
                throw std::runtime_error("MC list requires exactly: path_or_list cross_section_pb");
            const bool list=source[0]=='@';
            const std::string id=eos_utils::normalize_storage_path(normalize_path_token(list?source.substr(1):source,base));
            plan["components"].push_back({{"id",id},{"source",id},{"xsec_pb",xsec}});
            std::vector<std::string> files;k_expand(source,base,files);
            if(files.empty())throw std::runtime_error("Empty MC component: "+source);
            for(const auto& path:files)plan["tasks"].push_back({{"path",eos_utils::normalize_storage_path(path)},{"component",id}});
        }
    }
    return plan;
}

KJSON k_empty_category(const std::vector<PlotRequest>& plots) {
    KJSON h=KJSON::object();
    for(const auto& p:plots)h[p.name]=std::vector<KCount>(nbins_for_spec(*p.spec),0);
    return {{"events",0},{"histograms",h}};
}

KJSON k_reduce_file(const KJSON& task,bool mc,const std::vector<PlotRequest>& plots,bool pileupVeto=false) {
    const std::string path=task.at("path"),runtime=task.value("runtime",path);
    std::unique_ptr<TFile> file(TFile::Open(runtime.c_str()));
    if(!file || file->IsZombie() || file->TestBit(TFile::kRecovered))throw std::runtime_error("Cannot read complete ROOT file: "+path);
    auto* tree=dynamic_cast<TTree*>(file->Get(kTreePathDefault));
    if(!tree)throw std::runtime_error("Missing analysis tree: "+path);
    auto required=required_branches_for_plots(plots);
    const auto available=get_branch_names(*tree);
    if(pileupVeto)for(const std::string name:{"Pileup_pthatmax","Generator_binvar"}) {
        if(!available.count(name))throw std::runtime_error("--pileup-veto requires "+name+" in "+path+
            "; regenerate the reduced MC nTuples with these fields saved");
        required.insert(name);
    }
    if(!mc)required.insert("PassJSON");
    const bool trigger=available.count("passHLT_PFScoutingHT");
    if(trigger || !mc)required.insert("passHLT_PFScoutingHT");
    for(const auto& name:required)if(!available.count(name))throw std::runtime_error("Missing "+name+" in "+path);
    configure_tree_for_reading(*tree,required);
    const auto entries=tree->GetEntries();
    if(entries<0)throw std::runtime_error("Cannot count input entries: "+path);
    const double generated=mc?read_event_counter(*file,path):0;
    if(mc && (!std::isfinite(generated) || generated<=0))throw std::runtime_error("Invalid MC generated-event count: "+path);
    KJSON result={{"file",{{"path",path},{"uuid",k_uuid(file->GetUUID())},{"entries",entries},{"processed",0},{"generated_events",generated}}},
                  {"component",task.at("component")},{"categories",KJSON::object()}};
    const std::vector<std::string> categories={"pass","fail","unavailable"};
    std::vector<std::vector<std::vector<KCount>>> counts(3);
    std::array<KCount,3> totals{};
    for(auto& group:counts)for(const auto& plot:plots)group.emplace_back(nbins_for_spec(*plot.spec),0);
    auto vetoCounts=pileupVeto?counts:decltype(counts){};
    std::array<KCount,3> vetoTotals{};
    TTreeReader reader(tree);
    std::map<std::string,std::unique_ptr<TTreeReaderValue<double>>> values;
    for(const auto& name:required)values[name]=std::make_unique<TTreeReaderValue<double>>(reader,name.c_str());
    auto& nj=*values.at("nJet");auto& id1=*values.at("IdTight_j1");auto& id2=*values.at("IdTight_j2");
    auto& pt1=*values.at("pTWJ_j1");auto& pt2=*values.at("pTWJ_j2");
    auto& eta1=*values.at("etaWJ_j1");auto& eta2=*values.at("etaWJ_j2");
    auto& deta=*values.at("deltaETAjj");auto& mass=*values.at("mjj");
    auto* json=mc?nullptr:values.at("PassJSON").get();
    auto* hlt=trigger?values.at("passHLT_PFScoutingHT").get():nullptr;
    auto* pileup=pileupVeto?values.at("Pileup_pthatmax").get():nullptr;
    auto* generator=pileupVeto?values.at("Generator_binvar").get():nullptr;
    std::vector<TTreeReaderValue<double>*> plotValues;
    for(const auto& plot:plots)plotValues.push_back(values.at(plot.name).get());
    KCount processed=0;
    while(reader.Next()) {
        if(processed==0)for(const auto& v:values)if(v.second->GetSetupStatus()<0)throw std::runtime_error("Invalid branch type: "+v.first+" in "+path);
        ++processed;
        bool keep=false;
        if(pileupVeto) {
            const double pu=**pileup,hard=**generator;
            if(!std::isfinite(pu) || !std::isfinite(hard) || pu<0 || hard<=0)
                throw std::runtime_error("Invalid pileup/generator scale in "+path+" at entry "+std::to_string(processed-1));
            keep=pu<=hard;
        }
        if(!passes_base_selection(json?**json:1,!mc,*nj,*id1,*id2,*pt1,*pt2,*eta1,*eta2,*deta,*mass))continue;
        const auto category=hlt?(**hlt<=0.5?1:0):2;
        ++totals[category];
        if(keep)++vetoTotals[category];
        for(std::size_t i=0;i<plots.size();++i) {
            const double value=**plotValues[i];
            if(!std::isfinite(value))throw std::runtime_error("Non-finite selected "+plots[i].name+" in "+path);
            const int b=find_bin_index(*plots[i].spec,value);
            if(b>=0) {++counts[category][i][b];if(keep)++vetoCounts[category][i][b];}
        }
    }
    if(processed!=static_cast<KCount>(entries) || reader.GetEntryStatus()!=TTreeReader::kEntryBeyondEnd)
        throw std::runtime_error("Incomplete event read in "+path);
    result["file"]["processed"]=processed;
    for(std::size_t c=0;c<categories.size();++c) {
        auto group=k_empty_category(plots);group["events"]=totals[c];
        for(std::size_t i=0;i<plots.size();++i)group["histograms"][plots[i].name]=counts[c][i];
        result["categories"][categories[c]]=group;
        if(pileupVeto) {
            group["events"]=vetoTotals[c];
            for(std::size_t i=0;i<plots.size();++i)group["histograms"][plots[i].name]=vetoCounts[c][i];
            result["pileup_veto_categories"][categories[c]]=group;
        }
    }
    return result;
}

void k_add_categories(KJSON& dest,const KJSON& source) {
    for(const auto& group:source.items()) {
        auto& out=dest.at(group.key());
        out["events"]=out["events"].get<KCount>()+group.value().at("events").get<KCount>();
        for(const auto& h:group.value().at("histograms").items()) {
            auto& bins=out["histograms"].at(h.key());
            if(bins.size()!=h.value().size())throw std::runtime_error("Checkpoint histogram mismatch");
            for(std::size_t i=0;i<bins.size();++i)bins[i]=bins[i].get<KCount>()+h.value()[i].get<KCount>();
        }
    }
}

#ifndef KINEMATICS_JSON_LIBRARY
int main(int argc,char** argv) {
    int lock=-1;
    try {
        const auto o=k_parse(argc,argv);
        if(o.describe) {std::cout<<kinematics_contract().dump()<<'\n';return 0;}
        const auto plan=k_plan(o);
        if(plan.at("version")!=1 || plan.at("tasks").empty() || !k_label(plan.at("dataset")))throw std::runtime_error("Invalid input plan");
        const bool mc=plan.at("kind")=="mc";
        const bool pileupVeto=plan.contains("pileup_veto");
        if(!mc && plan.at("kind")!="data")throw std::runtime_error("Invalid input kind");
        std::set<std::string> components,files;
        for(const auto& c:plan.at("components"))if(!components.insert(c.at("id").get<std::string>()).second)throw std::runtime_error("Repeated component");
        for(const auto& task:plan.at("tasks")) {
            if(!components.count(task.at("component")) || !files.insert(k_identity(task.at("path"))).second)
                throw std::runtime_error("Repeated file or unknown component");
        }
        if(o.plan) {std::cout<<plan.dump()<<'\n';return 0;}
        const std::string dataset=plan.at("dataset"),jec=plan.at("jec");
        if(!jec.empty() && jec!="old" && jec!="new")throw std::runtime_error("Invalid JEC label");
        const std::string label=dataset+(jec.empty()?"":"_"+jec);
        fs::path output=fs::absolute(o.output);
        if(output.extension()!=".json")output/=label+".json";
        fs::create_directories(output.parent_path());
        const fs::path checkpoint=output.parent_path()/("."+output.stem().string()+".checkpoint");
        fs::create_directories(checkpoint);
        lock=open((checkpoint/"lock").c_str(),O_CREAT|O_RDWR,0600);
        if(lock<0 || flock(lock,LOCK_EX|LOCK_NB)!=0)throw std::runtime_error("Another reducer is using this output");
        if(o.force)for(const auto& entry:fs::directory_iterator(checkpoint))if(entry.path().filename()!="lock")fs::remove_all(entry.path());
        if((fs::exists(output) || fs::exists(checkpoint/"plan.json")) && !o.resume && !o.force)
            throw std::runtime_error("Output/checkpoint exists; use --resume or --force");
        if(fs::exists(checkpoint/"plan.json")) {
            if(k_read_json(checkpoint/"plan.json")!=plan)throw std::runtime_error("Checkpoint inputs/configuration changed; use --force");
        } else k_write_json(checkpoint/"plan.json",plan);
        ROOT::EnableThreadSafety();
        const auto plots=parse_plot_requests("all");
        std::vector<unsigned> indices(plan["tasks"].size());
        for(unsigned i=0;i<indices.size();++i)indices[i]=i;
        std::atomic<unsigned> completed{0};std::mutex printing;
        std::cout<<"[INFO] "<<indices.size()<<" files, "<<o.threads<<" threads; checkpoint after each file: "<<checkpoint<<std::endl;
        if(pileupVeto)std::cout<<"[INFO] One scan: unfiltered and Pileup_pthatmax <= Generator_binvar; generated-event normalization unchanged"<<std::endl;
        ROOT::TThreadExecutor executor(o.threads);
        auto results=executor.Map([&](unsigned index) {
            const auto& task=plan["tasks"][index];const fs::path path=checkpoint/(std::to_string(index)+".json");
            KJSON result;
            if(o.resume && fs::exists(path)) {
                result=k_read_json(path);
                if(result.at("file").at("path")!=task.at("path") || result.at("component")!=task.at("component") ||
                   result.at("file").at("entries")!=result.at("file").at("processed") ||
                   result.contains("pileup_veto_categories")!=pileupVeto)throw std::runtime_error("Invalid checkpoint: "+path.string());
                const std::string runtime=task.value("runtime",task.at("path").get<std::string>());
                std::unique_ptr<TFile> file(TFile::Open(runtime.c_str()));
                auto* tree=file && !file->IsZombie()?dynamic_cast<TTree*>(file->Get(kTreePathDefault)):nullptr;
                if(!tree || result["file"]["uuid"]!=k_uuid(file->GetUUID()) || result["file"]["entries"]!=tree->GetEntries() ||
                   (mc && result["file"]["generated_events"]!=read_event_counter(*file,runtime)))
                    throw std::runtime_error("Completed input changed or is unreadable; restore it or use --force: "+runtime);
            } else {result=k_reduce_file(task,mc,plots,pileupVeto);k_write_json(path,result);}
            const auto done=++completed;
            if(done==indices.size() || done%10==0) {std::lock_guard<std::mutex> guard(printing);std::cout<<"[INFO] Files complete: "<<done<<'/'<<indices.size()<<std::endl;}
            return result;
        },indices);
        KJSON document={{"format","kinematics_summary"},{"schema_version",1},{"complete",true},{"dataset",dataset},
                        {"datasets",KJSON::array({dataset})},{"jec",jec},{"kind",plan["kind"]},{"contract",kinematics_contract()},
                        {"components",KJSON::array()}};
        if(pileupVeto)document["pileup_veto"]=kinematics_pileup_veto();
        for(const auto& c:plan["components"]) {
            KJSON aggregate=c;aggregate["files"]=KJSON::array();aggregate["generated_events"]=0.0;aggregate["categories"]=KJSON::object();
            for(const std::string category:{"pass","fail","unavailable"})aggregate["categories"][category]=k_empty_category(plots);
            if(pileupVeto)aggregate["pileup_veto_categories"]=aggregate["categories"];
            for(const auto& result:results)if(result.at("component")==c.at("id")) {
                aggregate["files"].push_back(result["file"]);
                aggregate["generated_events"]=aggregate["generated_events"].get<double>()+result["file"]["generated_events"].get<double>();
                k_add_categories(aggregate["categories"],result["categories"]);
                if(pileupVeto)k_add_categories(aggregate["pileup_veto_categories"],result["pileup_veto_categories"]);
            }
            if(!aggregate["files"].empty())document["components"].push_back(aggregate);
        }
        k_write_json(output,document);
        std::cout<<"[INFO] Saved "<<output<<std::endl;
        close(lock);return 0;
    } catch(const std::exception& error) {
        if(lock>=0)close(lock);
        std::cerr<<"[ERROR] "<<error.what()<<std::endl;return 1;
    }
}
#endif
