//Andrew Loeliger
//Input options: 
// -s disables shape uncertainties
// -e disables embedded
// -b disables bin-by-bin uncertainties
// -g uses inclusive ggH distributions
// -q uses inclusive qqH distributions
// -c run in control plot mode
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"
#include "CombineHarvester/Run2HTT_Combine/interface/InputParserUtility.h"
#include "CombineHarvester/Run2HTT_Combine/interface/UtilityFunctions.h"

using namespace std;

int main(int argc, char **argv)

{
  InputParserUtility Input(argc,argv);

  //! [part1]
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  cout<<"test"<<endl;
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/auxiliaries/shapes/";
  
  //keep a handle on the file, we need it to check if shapes are empty.
  TFile* TheFile;
  if (Input.OptionExists("-c")) TheFile = new TFile((aux_shapes+"em_controls_2016.root").c_str());
  else if (Input.OptionExists("-gf")) TheFile = new TFile((aux_shapes+"smh2016em_GOF.root").c_str());
  else if (Input.OptionExists("-dp") or Input.OptionExists("-dn") or Input.OptionExists("-dm")||Input.OptionExists("-dljpt")) TheFile = new TFile((aux_shapes+"smh2016em_Differential.root").c_str());
  else TheFile = new TFile((aux_shapes+"smh2016em.root").c_str());  
    
  //categories loaded from configurations
  std::vector<std::pair<int,std::string>> cats = {};
  std::vector<std::string> CategoryArgs = Input.GetAllArguments("--Categories");
  int CatNum=1;
  for (auto it = CategoryArgs.begin(); it != CategoryArgs.end(); ++it)
    {					       
      std::cout<<"Making category for: "<<CatNum<<" "<<*it<<std::endl;
      cats.push_back({CatNum,(std::string)*it});
      CatNum++;
    }  

  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  vector<string> masses = {""};;
  //! [part3]
  cb.AddObservations({"*"}, {"smh2016"}, {"13TeV"}, {"em"}, cats);

  vector<string> bkg_procs = {"ZL","QCD","VVL","STL","TTL"};
  if(Input.OptionExists("-e"))
    {
      bkg_procs.push_back("ZT");            
      bkg_procs.push_back("TTT");      
      bkg_procs.push_back("VVT");      
      bkg_procs.push_back("STT");      
    }
  else bkg_procs.push_back("embedded");
  if (Input.OptionExists("-dp") || Input.OptionExists("-dn") || Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
    {
      bkg_procs.push_back("ggH_htt_nonfid125");
      bkg_procs.push_back("qqH_htt_nonfid125");
      bkg_procs.push_back("WH_htt_nonfid125");
      bkg_procs.push_back("ZH_htt_nonfid125");
    }
  else
    {
      bkg_procs.push_back("ggH_hww125");
      bkg_procs.push_back("qqH_hww125");
      bkg_procs.push_back("WH_hww125");
      bkg_procs.push_back("ZH_hww125");
    }
  cb.AddProcesses({"*"}, {"smh2016"}, {"13TeV"}, {"em"}, bkg_procs, cats, false);

  vector<string> ggH_STXS;
  if (Input.OptionExists("-g")) ggH_STXS = {"ggH_htt125"};
  else if (Input.OptionExists("-dp")) ggH_STXS = {
      "ggH_PTH_0_45_htt125",
      "ggH_PTH_45_80_htt125",
      "ggH_PTH_80_120_htt125",
      "ggH_PTH_120_200_htt125",
      "ggH_PTH_200_350_htt125",
      "ggH_PTH_350_450_htt125",
      "ggH_PTH_GT450_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) ggH_STXS = {
      "ggH_NJETS_0_htt125",
      "ggH_NJETS_1_htt125",
      "ggH_NJETS_2_htt125",
      "ggH_NJETS_3_htt125",
      "ggH_NJETS_GE4_htt125",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) ggH_STXS = {
      "ggH_MJJ_0_150_htt125",
      "ggH_MJJ_150_300_htt125",
      "ggH_MJJ_300_450_htt125",
      "ggH_MJJ_450_600_htt125",
      "ggH_MJJ_600_1000_htt125",
      "ggH_MJJ_1000_1400_htt125",
      "ggH_MJJ_1400_1800_htt125",
      "ggH_MJJ_GE1800_htt125",
    };
  else if (Input.OptionExists("-dljpt")) ggH_STXS = {
      "ggH_LJPT_30_60_htt125",
      "ggH_LJPT_60_120_htt125",
      "ggH_LJPT_120_200_htt125",
      "ggH_LJPT_200_350_htt125",
      "ggH_LJPT_GT350_htt125",
    };
  else ggH_STXS = {"ggH_PTH_0_200_0J_PTH_10_200_htt125",
		   "ggH_PTH_0_200_0J_PTH_0_10_htt125",
		   "ggH_PTH_0_200_1J_PTH_0_60_htt125",
		   "ggH_PTH_0_200_1J_PTH_60_120_htt125",
		   "ggH_PTH_0_200_1J_PTH_120_200_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125",
		   "ggH_PTH_200_300_htt125",
		   "ggH_PTH_300_450_htt125",
		   "ggH_PTH_450_650_htt125",
		   "ggH_PTH_GE650_htt125"};
  
  vector<string> qqH_STXS; 
  if(Input.OptionExists("-q")) qqH_STXS = {"qqH_htt125"};
  else if (Input.OptionExists("-dp")) qqH_STXS = {
      "qqH_PTH_0_45_htt125",
      "qqH_PTH_45_80_htt125",
      "qqH_PTH_80_120_htt125",
      "qqH_PTH_120_200_htt125",
      "qqH_PTH_200_350_htt125",
      "qqH_PTH_350_450_htt125",
      "qqH_PTH_GT450_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) qqH_STXS = {
      "qqH_NJETS_0_htt125",
      "qqH_NJETS_1_htt125",
      "qqH_NJETS_2_htt125",
      "qqH_NJETS_3_htt125",
      "qqH_NJETS_GE4_htt125",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) qqH_STXS = {
      "qqH_MJJ_0_150_htt125",
      "qqH_MJJ_150_300_htt125",
      "qqH_MJJ_300_450_htt125",
      "qqH_MJJ_450_600_htt125",
      "qqH_MJJ_600_1000_htt125",
      "qqH_MJJ_1000_1400_htt125",
      "qqH_MJJ_1400_1800_htt125",
      "qqH_MJJ_GE1800_htt125",
    };
  else if (Input.OptionExists("-dljpt")) qqH_STXS = {
      "qqH_LJPT_30_60_htt125",
      "qqH_LJPT_60_120_htt125",
      "qqH_LJPT_120_200_htt125",
      "qqH_LJPT_200_350_htt125",
      "qqH_LJPT_GT350_htt125",
    };
  else qqH_STXS = {"qqH_0J_htt125",
		   "qqH_1J_htt125",
		   "qqH_GE2J_MJJ_0_60_htt125",
		   "qqH_GE2J_MJJ_60_120_htt125",
		   "qqH_GE2J_MJJ_120_350_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"};

  vector<string> WH_STXS;
  if (Input.OptionExists("-q")) WH_STXS = {"WH_lep_htt125","WH_had_htt125"};
  else if (Input.OptionExists("-dp")) WH_STXS = {
      "WH_PTH_0_45_htt125",
      "WH_PTH_45_80_htt125",
      "WH_PTH_80_120_htt125",
      "WH_PTH_120_200_htt125",
      "WH_PTH_200_350_htt125",
      "WH_PTH_350_450_htt125",
      "WH_PTH_GT450_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) WH_STXS = {
      "WH_NJETS_0_htt125",
      "WH_NJETS_1_htt125",
      "WH_NJETS_2_htt125",
      "WH_NJETS_3_htt125",
      "WH_NJETS_GE4_htt125",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) WH_STXS = {
      "WH_MJJ_0_150_htt125",
      "WH_MJJ_150_300_htt125",
      "WH_MJJ_300_450_htt125",
      "WH_MJJ_450_600_htt125",
      "WH_MJJ_600_1000_htt125",
      "WH_MJJ_1000_1400_htt125",
      "WH_MJJ_1400_1800_htt125",
      "WH_MJJ_GE1800_htt125",
    };
  else if (Input.OptionExists("-dljpt")) WH_STXS = {
      "WH_LJPT_30_60_htt125",
      "WH_LJPT_60_120_htt125",
      "WH_LJPT_120_200_htt125",
      "WH_LJPT_200_350_htt125",
      "WH_LJPT_GT350_htt125",
    };
  else WH_STXS = {
      "WH_lep_htt125",
      "WH_0J_htt125",
      "WH_1J_htt125",
      "WH_GE2J_MJJ_0_60_htt125",
      "WH_GE2J_MJJ_60_120_htt125",
      "WH_GE2J_MJJ_120_350_htt125",
      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
      "WH_GE2J_MJJ_GE350_PTH_GE200_htt125",
    };

  vector<string> ZH_STXS;
  if (Input.OptionExists("-q")) ZH_STXS = {"ZH_lep_htt125","ZH_had_htt125"};
  else if (Input.OptionExists("-dp")) ZH_STXS = {
      "ZH_PTH_0_45_htt125",
      "ZH_PTH_45_80_htt125",
      "ZH_PTH_80_120_htt125",
      "ZH_PTH_120_200_htt125",
      "ZH_PTH_200_350_htt125",
      "ZH_PTH_350_450_htt125",
      "ZH_PTH_GT450_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) ZH_STXS = {
      "ZH_NJETS_0_htt125",
      "ZH_NJETS_1_htt125",
      "ZH_NJETS_2_htt125",
      "ZH_NJETS_3_htt125",
      "ZH_NJETS_GE4_htt125",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) ZH_STXS = {
      "ZH_MJJ_0_150_htt125",
      "ZH_MJJ_150_300_htt125",
      "ZH_MJJ_300_450_htt125",
      "ZH_MJJ_450_600_htt125",
      "ZH_MJJ_600_1000_htt125",
      "ZH_MJJ_1000_1400_htt125",
      "ZH_MJJ_1400_1800_htt125",
      "ZH_MJJ_GE1800_htt125",
    };
  else if (Input.OptionExists("-dljpt")) ZH_STXS = {
      "ZH_LJPT_30_60_htt125",
      "ZH_LJPT_60_120_htt125",
      "ZH_LJPT_120_200_htt125",
      "ZH_LJPT_200_350_htt125",
      "ZH_LJPT_GT350_htt125",
    };
  else ZH_STXS = {
      "ZH_lep_htt125",
      "ZH_0J_htt125",
      "ZH_1J_htt125",
      "ZH_GE2J_MJJ_0_60_htt125",
      "ZH_GE2J_MJJ_60_120_htt125",
      "ZH_GE2J_MJJ_120_350_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_GE200_htt125",
    };
  vector<string> ggZH_STXS;
  if (Input.OptionExists("-g")) ggZH_STXS = {"ggZH_lep_htt125","ggZH_had_htt125"};
  else if (Input.OptionExists("-dm")||Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dljpt")) ggZH_STXS = {};
  else ggZH_STXS = {
      "ggZH_lep_htt125",
      "ggZH_PTH_0_200_0J_PTH_10_200_htt125",
      "ggZH_PTH_0_200_0J_PTH_0_10_htt125",
      "ggZH_PTH_0_200_1J_PTH_0_60_htt125",
      "ggZH_PTH_0_200_1J_PTH_60_120_htt125",
      "ggZH_PTH_0_200_1J_PTH_120_200_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125",
      "ggZH_PTH_200_300_htt125",
      "ggZH_PTH_300_450_htt125",
      "ggZH_PTH_450_650_htt125",
      "ggZH_PTH_GE650_htt125"
    };

  vector<string> sig_procs = ch::JoinStr({ggH_STXS,
	qqH_STXS,
	WH_STXS,
	ZH_STXS,
	ggZH_STXS});
  
  cb.AddProcesses(masses, {"smh2016"}, {"13TeV"}, {"em"}, sig_procs, cats, true);    

  //! [part4]

  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;
  using ch::JoinStr;

  //***********************************
  //start with lnN errors
  //***********************************
  
  //Theory uncerts
  if (not(Input.OptionExists("-x0")||Input.OptionExists("-x1")))
    {
      cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_PU_alphas", "lnN", SystMap<>::init(1.0062));
      cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_PU_mq", "lnN", SystMap<>::init(1.0099));
      cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_THU", "lnN", SystMap<>::init(1.017));
      cb.cp().process(JoinStr({WH_STXS,{"WH_hww125","WH_htt_nonfid125"}})).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.008));
      cb.cp().process(JoinStr({ZH_STXS,{"ZH_hww125","ZH_htt_nonfid125"}})).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.009));
      cb.cp().process(JoinStr({WH_STXS,{"WH_hww125","WH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.018));
      cb.cp().process(JoinStr({ZH_STXS,{"ZH_hww125","ZH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.013));
      cb.cp().process(JoinStr({ggH_STXS,{"ggH_hww125","ggH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_gg", "lnN", SystMap<>::init(1.032));
      cb.cp().process(JoinStr({qqH_STXS,{"qqH_hww125","qqH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_qq", "lnN", SystMap<>::init(1.021));
    }
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_PU_alphas", "lnN", ch::syst::SystMapAsymm<>::init(1.0066,1.0063));
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_PU_mq", "lnN", ch::syst::SystMapAsymm<>::init(1.0099,1.0098));
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_THU", "lnN", SystMap<>::init(1.0099));  
  //cb.cp().process(JoinStr({qqH_STXS,{"qqH_hww125"}})).AddSyst(cb, "QCDScale_qqH", "lnN", SystMap<>::init(1.005));  

  if(not (Input.OptionExists("-x1")))
    {
      cb.cp().process({"ggH_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.002,0.998));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.006,0.997));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.000,0.998));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.954,1.026));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.069,0.956));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"qqH_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.027,0.986));
      cb.cp().process({"qqH_0J_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.022,0.991));
      cb.cp().process({"qqH_1J_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.041,0.974));
      cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"em_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"ggH_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.971,1.024));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.983,1.004));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.952,1.037));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.980,1.025));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.952,1.036));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.122,0.929));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.026,0.977));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.043,0.957));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.925,1.075));
      cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.920,1.087));
      cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.084,0.955));
      cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"qqH_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.011,0.994));
      cb.cp().process({"qqH_0J_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.980,1.011));
      cb.cp().process({"qqH_1J_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.011,0.995));
      cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.909,1.012));
      cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.000,0.952));
      cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.009,0.986));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.169,0.926));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.120,0.934));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"ggH_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.017,0.977));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.038,0.984));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.013,0.996));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.033,0.979));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.041,0.955));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.980,0.983));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.012,0.979));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.027,0.972));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.082,0.907));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.047,0.909));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.038,0.975));
      cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.978,1.001));
      cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.054,0.941));
      cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"qqH_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.985,1.013));
      cb.cp().process({"qqH_0J_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_1J_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.053,0.964));
      cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.943,1.033));
      cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.969,1.020));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.898,1.081));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.031,0.997));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"ggH_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.061,0.957));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.037,0.973));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.023,0.977));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.205,0.885));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.039,0.974));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.088,0.924));
      cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.073,0.944));
      cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"qqH_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.986,1.009));
      cb.cp().process({"qqH_0J_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_1J_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.026,0.995));
      cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.957,1.031));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.960,1.023));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.976,1.004));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.984,1.010));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.009,0.994));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.004,0.994));
      cb.cp().process({"ggH_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.099,0.961));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.006,0.997));
      cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.099,0.961));
      cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.081,0.972));
      cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.177,0.911));
      cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
      cb.cp().process({"qqH_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.978,1.008));
      cb.cp().process({"qqH_0J_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_1J_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.038,0.980));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
      cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.980,1.005));
    }
  //pdf acceptance uncertainties.
cb.cp().process({ggH_STXS}).bin({"em_0jet"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.007));
  cb.cp().process({ggH_STXS}).bin({"em_boosted1"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.004));
  cb.cp().process({ggH_STXS}).bin({"em_boosted2"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.005));
  cb.cp().process({ggH_STXS}).bin({"em_vbflow"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.010));
  cb.cp().process({ggH_STXS}).bin({"em_vbfhigh"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.010));

  cb.cp().process({qqH_STXS}).bin({"em_0jet"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.015));
  cb.cp().process({qqH_STXS}).bin({"em_boosted1"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.006));
  cb.cp().process({qqH_STXS}).bin({"em_boosted2"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.006));
  cb.cp().process({qqH_STXS}).bin({"em_vbflow"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.007));
  cb.cp().process({qqH_STXS}).bin({"em_vbfhigh"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.007));

  //Electron ID efficiency
  cb.cp().process(JoinStr({{"ZT","TTT","VVT","TTL","VVL","STT","STL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_eff_e_2016","lnN",SystMap<>::init(1.02));
  cb.cp().process({"W"}).AddSyst(cb,"CMS_eff_e_2016","lnN",SystMap<>::init(1.01)); 

  //Muon ID efficiency
  cb.cp().process(JoinStr({{"ZT","TTT","VVT","TTL","VVL","STT","STL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_eff_m_2016","lnN",SystMap<>::init(1.02));
  cb.cp().process({"W"}).AddSyst(cb,"CMS_eff_m_2016","lnN",SystMap<>::init(1.01));

  // b-tagging efficiency
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_eta","lnN",SystMap<>::init(1.005));
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hf","lnN",SystMap<>::init(0.993));
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hfstats1_2016","lnN",SystMap<>::init(1.03));
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hfstats2_2016","lnN",SystMap<>::init(1.015));
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_jes","lnN",SystMap<>::init(0.98));
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lf","lnN",SystMap<>::init(0.90));
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lfstats1_2016","lnN",SystMap<>::init(0.995));
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lfstats2_2016","lnN",SystMap<>::init(0.995));

  // XSection Uncertainties
  cb.cp().process({"TTT","TTL"}).AddSyst(cb,"CMS_htt_tjXsec", "lnN", SystMap<>::init(1.042));
  cb.cp().process({"VVT","VVL"}).AddSyst(cb,"CMS_htt_vvXsec", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"STT","STL"}).AddSyst(cb,"CMS_htt_stXsec", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"ZT"}).AddSyst(cb,"CMS_htt_zjXsec", "lnN", SystMap<>::init(1.02));

  cb.cp().process({"ZL"}).AddSyst(cb,"CMS_htt_elltoellFR_2016", "lnN", SystMap<>::init(1.3));
 
  //Luminosity Uncertainty
  cb.cp().process(JoinStr({sig_procs,{"W","VVL","VVT","STT","STL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_Run2016", "lnN", SystMap<>::init(1.022));
  cb.cp().process(JoinStr({sig_procs,{"W","VVL","VVT","STT","STL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_XYfactorization", "lnN", SystMap<>::init(1.009));
  cb.cp().process(JoinStr({sig_procs,{"W","VVL","VVT","STT","STL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_beamBeamDeflection", "lnN", SystMap<>::init(1.004));
  cb.cp().process(JoinStr({sig_procs,{"W","VVL","VVT","STT","STL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_dynamicBeta", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({sig_procs,{"W","VVL","VVT","STT","STL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_ghostsAndSatellites", "lnN", SystMap<>::init(1.004));

  cb.cp().process({"TTL","TTT"}).bin({"em_0jet"}).AddSyst(cb,"CMS_ttbar_njet_2016","lnN",SystMap<>::init(1.10));
  //cb.cp().process({"TTL","TTT"}).bin({"em_0jethigh"}).AddSyst(cb,"CMS_ttbar_njet_2016","lnN",SystMap<>::init(1.10));
  cb.cp().process({"TTL","TTT"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_ttbar_njet_2016","lnN",SystMap<>::init(1.05));
  cb.cp().process({"TTL","TTT"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_ttbar_njet_2016","lnN",SystMap<>::init(0.95));
  cb.cp().process({"TTL","TTT"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_ttbar_njet_2016","lnN",SystMap<>::init(0.95));
  cb.cp().process({"TTL","TTT"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_ttbar_njet_2016","lnN",SystMap<>::init(0.95));

  cb.cp().process({"QCD"}).bin({"em_0jet"}).AddSyst(cb,"CMS_QCDsyst_0jet_2016","lnN",SystMap<>::init(1.10));
  //cb.cp().process({"QCD"}).bin({"em_0jethigh"}).AddSyst(cb,"CMS_QCDsyst_0jethigh_2016","lnN",SystMap<>::init(1.10));
  //cb.cp().process({"QCD"}).bin({"em_0jet"}).AddSyst(cb,"CMS_QCDsyst_0jet_2016","lnN",SystMap<>::init(1.10));
  cb.cp().process({"QCD"}).bin({"em_boosted1"}).AddSyst(cb,"CMS_QCDsyst_boosted1_2016","lnN",SystMap<>::init(1.10));
  cb.cp().process({"QCD"}).bin({"em_boosted2"}).AddSyst(cb,"CMS_QCDsyst_boosted2_2016","lnN",SystMap<>::init(1.10));
  cb.cp().process({"QCD"}).bin({"em_vbflow"}).AddSyst(cb,"CMS_QCDsyst_vbflow_2016","lnN",SystMap<>::init(1.10));
  cb.cp().process({"QCD"}).bin({"em_vbfhigh"}).AddSyst(cb,"CMS_QCDsyst_vbfhigh_2016","lnN",SystMap<>::init(1.10));

  //***********************************************************
  //shape uncertainties
  //***********************************************************
  if(not Input.OptionExists("-s"))
    {
      std::cout<<"Adding Shapes..."<<std::endl;
      //uses custom defined utility function that only adds the shape if at least one shape inside is not empty.
      
      // Prefiring
      AddShapesIfNotEmpty({"CMS_prefiring"},
			  JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}), 
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);

      // QCD shape            
      if (Input.OptionExists("-dp") || Input.OptionExists("-dn") || Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
	{
	  AddShapesIfNotEmpty({"CMS_QCD_njet0_intercept_2016","CMS_QCD_njet0_slope_2016","CMS_QCD_stat_pte15to25_ptmu15to25_2016","CMS_QCD_stat_pte15to25_ptmugt25_2016","CMS_QCD_stat_pte25to35_ptmu15to25_2016","CMS_QCD_stat_pte25to35_ptmugt25_2016","CMS_QCD_stat_ptegt35_ptmu15to25_2016","CMS_QCD_stat_ptegt35_ptmugt25_2016","CMS_QCD_njet1_intercept_2016","CMS_QCD_njet1_slope_2016","CMS_QCD_stat_pte15to25_ptmu15to25_2016","CMS_QCD_stat_pte15to25_ptmugt25_2016","CMS_QCD_stat_pte25to35_ptmu15to25_2016","CMS_QCD_stat_pte25to35_ptmugt25_2016","CMS_QCD_stat_ptegt35_ptmu15to25_2016","CMS_QCD_stat_ptegt35_ptmugt25_2016","CMS_QCD_njet2_intercept_2016","CMS_QCD_njet2_slope_2016","CMS_QCD_stat_pte15to25_ptmu15to25_2016","CMS_QCD_stat_pte15to25_ptmugt25_2016","CMS_QCD_stat_pte25to35_ptmu15to25_2016","CMS_QCD_stat_pte25to35_ptmugt25_2016","CMS_QCD_stat_ptegt35_ptmu15to25_2016","CMS_QCD_stat_ptegt35_ptmugt25_2016"},
			      {"QCD"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs);
	}
      else
	{
	  std::cout<<"QCD shapes"<<std::endl;
	  AddShapesIfNotEmpty({"CMS_QCD_njet0_intercept_2016","CMS_QCD_njet0_slope_2016","CMS_QCD_stat_pte15to25_ptmu15to25_2016","CMS_QCD_stat_pte15to25_ptmugt25_2016","CMS_QCD_stat_pte25to35_ptmu15to25_2016","CMS_QCD_stat_pte25to35_ptmugt25_2016","CMS_QCD_stat_ptegt35_ptmu15to25_2016","CMS_QCD_stat_ptegt35_ptmugt25_2016"},
			      {"QCD"},
			      &cb,
			      1.00,
			      TheFile,
			      {"em_0jet"});
			      //{"em_0jetlow","em_0jethigh"});
      
	  AddShapesIfNotEmpty({"CMS_QCD_njet1_intercept_2016","CMS_QCD_njet1_slope_2016","CMS_QCD_stat_pte15to25_ptmu15to25_2016","CMS_QCD_stat_pte15to25_ptmugt25_2016","CMS_QCD_stat_pte25to35_ptmu15to25_2016","CMS_QCD_stat_pte25to35_ptmugt25_2016","CMS_QCD_stat_ptegt35_ptmu15to25_2016","CMS_QCD_stat_ptegt35_ptmugt25_2016"},
			      {"QCD"},
			      &cb,
			      1.00,
			      TheFile,
			      {"em_boosted1"});
	  AddShapesIfNotEmpty({"CMS_QCD_njet2_intercept_2016","CMS_QCD_njet2_slope_2016","CMS_QCD_stat_pte15to25_ptmu15to25_2016","CMS_QCD_stat_pte15to25_ptmugt25_2016","CMS_QCD_stat_pte25to35_ptmu15to25_2016","CMS_QCD_stat_pte25to35_ptmugt25_2016","CMS_QCD_stat_ptegt35_ptmu15to25_2016","CMS_QCD_stat_ptegt35_ptmugt25_2016"},
			      {"QCD"},
			      &cb,
			      1.00,
			      TheFile,
			      {"em_boosted2","em_vbflow","em_vbfhigh"});
	}

      //MET Unclustered Energy Scale      
      std::cout<<"MET UES"<<std::endl;
      AddShapesIfNotEmpty({"CMS_scale_met_unclustered_2016"},
			  {"TTT","TTL","VVT","VVL","STT","STL"},
			  &cb,
			  1.00,
			  TheFile,CategoryArgs);
      
      //Recoil Shapes:                  
      //check which signal processes this should be applied to. If any.
      std::cout<<"Recoil shapes"<<std::endl;
      if (Input.OptionExists("-c"))
	{
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2016","CMS_htt_boson_scale_met_0jet_2016",
		"CMS_htt_boson_reso_met_1jet_2016","CMS_htt_boson_scale_met_1jet_2016",
		"CMS_htt_boson_reso_met_2jet_2016","CMS_htt_boson_scale_met_2jet_2016"},
	    JoinStr({ggH_STXS,qqH_STXS,{"ZT"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      else if(Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
	{
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2016","CMS_htt_boson_scale_met_0jet_2016","CMS_htt_boson_reso_met_1jet_2016","CMS_htt_boson_scale_met_1jet_2016","CMS_htt_boson_reso_met_2jet_2016","CMS_htt_boson_scale_met_2jet_2016"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZT","ggH_hww125","qqH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs);
	}
      else
	{
	  std::cout<<"met recoil uncertainties"<<std::endl;
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2016","CMS_htt_boson_scale_met_0jet_2016"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZT","ggH_hww125","qqH_hww125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      {"em_0jet"});
			      //{"em_0jetlow","em_0jethigh"});
      
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_1jet_2016","CMS_htt_boson_scale_met_1jet_2016"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZT","ggH_hww125","qqH_hww125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      {"em_boosted1"});

	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_2jet_2016","CMS_htt_boson_scale_met_2jet_2016"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZT","ggH_hww125","qqH_hww125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      {"em_boosted2","em_vbflow","em_vbfhigh"});            
	}

      //ZPT Reweighting Shapes:      
      std::cout<<"ZPT Reweighting"<<std::endl;
      AddShapesIfNotEmpty({"CMS_htt_dyShape_2016"},
			  {"ZT"},
			  &cb,
			  1.00,
			  TheFile,CategoryArgs);

      //Top Pt Reweighting      
      std::cout<<"ttbar shape"<<std::endl;
      AddShapesIfNotEmpty({"CMS_htt_ttbarShape"},
			  {"TTL","TTT"},
			  &cb,
			  1.00,
			  TheFile,CategoryArgs);

      //JES Uncertainties
      std::cout<<"JES"<<std::endl;
      AddShapesIfNotEmpty({"CMS_JetAbsolute","CMS_JetAbsolute_2016","CMS_JetBBEC1","CMS_JetBBEC1_2016","CMS_JetEC2","CMS_JetEC2_2016",
	    "CMS_JetFlavorQCD","CMS_JetHF","CMS_JetHF_2016","CMS_JetRelativeBal"},
	JoinStr({sig_procs,{"ZT","VVL","TTL","VVT","STL","STT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
	&cb,
	1.000,
	TheFile,CategoryArgs);      

      //JER
      std::cout<<"JER"<<std::endl;
      AddShapesIfNotEmpty({"CMS_JER_2016"},
			  JoinStr({sig_procs,{"ZT","VVL","TTL","VVT","STL","STT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
			  &cb,
			  1.000,
			  TheFile,CategoryArgs);

      //ggH Theory Uncertainties
      // scaled uncertainties for a stage 0 cross section measurement
      if (Input.OptionExists("-x0"))
	{
	  std::cout<<"Scaled ggH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_ggH_Mu_norm","THU_ggH_Res_norm","THU_ggH_Mig01_norm","THU_ggH_Mig12_norm","THU_ggH_VBF2j_norm",
		"THU_ggH_VBF3j_norm","THU_ggH_qmtop_norm","THU_ggH_PT60_norm","THU_ggH_PT120_norm"},
	    JoinStr({ggH_STXS,{"ggH_hww125","ggH_htt_nonfid125"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);            

	  //qqH theory uncertainties
	  //NOTE, We explicitly removed THU_qqH_yield from this.
	  std::cout<<"qqH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_qqH_PTH200","THU_qqH_Mjj60","THU_qqH_Mjj120","THU_qqH_Mjj350","THU_qqH_Mjj700",
		"THU_qqH_Mjj1000","THU_qqH_Mjj1500","THU_qqH_PTH25","THU_qqH_JET01"},
	    JoinStr({qqH_STXS,{"qqH_hww125","qqH_htt_nonfid125",}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      //unscaled for either mu measurement
      else if(not(Input.OptionExists("-x0")||Input.OptionExists("-x1")))
	{
	  std::cout<<"ggH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_ggH_Mu","THU_ggH_Res","THU_ggH_Mig01","THU_ggH_Mig12","THU_ggH_VBF2j",
		"THU_ggH_VBF3j","THU_ggH_qmtop","THU_ggH_PT60","THU_ggH_PT120"},
	    JoinStr({ggH_STXS,{"ggH_hww125","ggH_htt_nonfid125"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);            
	  
	  //qqH theory uncertainties
	  std::cout<<"qqH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_qqH_yield","THU_qqH_PTH200","THU_qqH_Mjj60","THU_qqH_Mjj120","THU_qqH_Mjj350","THU_qqH_Mjj700",
		"THU_qqH_Mjj1000","THU_qqH_Mjj1500","THU_qqH_PTH25","THU_qqH_JET01"},
	    JoinStr({qqH_STXS,{"qqH_hww125","qqH_htt_nonfid125",}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}

      //Muon Energy scale uncertainties
      AddShapesIfNotEmpty({"CMS_scale_m_etalt1p2","CMS_scale_m_eta1p2to2p1","CMS_scale_m_eta2p1to2p4"},
	JoinStr({sig_procs,{"ZT","VVT","TTT","VVL","TTL","STT","STL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
	&cb,
	1.00,
	TheFile,CategoryArgs);

      //Electron Energy scale uncertainties
      AddShapesIfNotEmpty({"CMS_scale_e"},
			  JoinStr({sig_procs,{"ZT","VVT","TTT","VVL","TTL","STT","STL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
        &cb,
        1.00,
        TheFile,CategoryArgs);

      // Trigger uncertainties
      AddShapesIfNotEmpty({"CMS_mu8e23trg_2016","CMS_mu23e12trg_2016","CMS_bothmuetrg_2016"},
                          JoinStr({sig_procs,{"ZT","VVT","TTT","VVL","TTL","STT","STL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
        &cb,
        1.00,
        TheFile,CategoryArgs);

      //new theory shapes
      //inclusive shapes are removable now?
      /*
      AddShapesIfNotEmpty({"ggH_scale"},
			  {"ggH_htt125",
			      "ggZH_had_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"vbf_scale"},
			  {"qqH_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      */      
      AddShapesIfNotEmpty({"VH_scale"},
			  {"WH_had_htt125",
			      "ZH_had_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"WHlep_scale"},
			  {"WH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ZHlep_scale"},
			  {"ZH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      //individual STXS bin shapes
      AddShapesIfNotEmpty({"ggH_scale_0jet"},
			  {"ggH_PTH_0_200_0J_PTH_10_200_htt125",
			      "ggH_PTH_0_200_0J_PTH_0_10_htt125",
			      "ggZH_PTH_0_200_0J_PTH_10_200_htt125",
			      "ggZH_PTH_0_200_0J_PTH_0_10_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ggH_scale_1jet_lowpt"},
			  {"ggH_PTH_0_200_1J_PTH_0_60_htt125",
			      "ggH_PTH_0_200_1J_PTH_60_120_htt125",
			      "ggH_PTH_0_200_1J_PTH_120_200_htt125",
			      "ggZH_PTH_0_200_1J_PTH_0_60_htt125",
			      "ggZH_PTH_0_200_1J_PTH_60_120_htt125",
			      "ggZH_PTH_0_200_1J_PTH_120_200_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ggH_scale_2jet_lowpt"},
			  {"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",		   
			      "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125",		   
			      "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"
			      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",		   
			      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125",		   
			      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ggH_scale_vbf"},
			  {"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",
			      "ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125",
			      "ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125",
			      "ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125",
			      "ggZH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",
			      "ggZH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125",
			      "ggZH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125",
			      "ggZH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ggH_scale_highpt"},
			  {"ggH_PTH_200_300_htt125",
			      "ggH_PTH_300_450_htt125",
			      "ggZH_PTH_200_300_htt125",
			      "ggZH_PTH_300_450_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ggH_scale_very_highpt"},
			  {"ggH_PTH_450_650_htt125",
			      "ggH_PTH_GE650_htt125",
			      "ggZH_PTH_450_650_htt125",
			      "ggZH_PTH_GE650_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );

      AddShapesIfNotEmpty({"vbf_scale_0jet"},
			  {"qqH_0J_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"vbf_scale_1jet"},
			  {"qqH_1J_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"vbf_scale_lowmjj"},
			  {"qqH_GE2J_MJJ_0_60_htt125",
			      "qqH_GE2J_MJJ_60_120_htt125",
			      "qqH_GE2J_MJJ_120_350_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );

      AddShapesIfNotEmpty({"vbf_scale_highmjj_lowpt"},
			  {"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
			      "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
			      "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
			      "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );

      AddShapesIfNotEmpty({"vbf_scale_highmjj_highpt"},
			  {"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );

      AddShapesIfNotEmpty({"VH_scale_0jet"},
			  {"WH_0J_htt125",
			      "ZH_0J_htt125,"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"VH_scale_1jet"},
			  {"WH_1J_htt125",
			      "ZH_1J_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"VH_scale_lowmjj"},
			  {"WH_GE2J_MJJ_0_60_htt125",
			      "WH_GE2J_MJJ_60_120_htt125",
			      "WH_GE2J_MJJ_120_350_htt125",
			      "ZH_GE2J_MJJ_0_60_htt125",
			      "ZH_GE2J_MJJ_60_120_htt125",
			      "ZH_GE2J_MJJ_120_350_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );

      AddShapesIfNotEmpty({"VH_scale_highmjj_lowpt"},
			  {"WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
			      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
			      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
			      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
			      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
			      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
			      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
			      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );

      AddShapesIfNotEmpty({"VH_scale_highmjj_highpt"},
			  {"WH_GE2J_MJJ_GE350_PTH_GE200_htt125",
			      "ZH_GE2J_MJJ_GE350_PTH_GE200_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
	
      //FIX ME: shapes are valid on split VH_lep, but we do not use seperated VH_lep at the moment.
      /*
      AddShapesIfNotEmpty({"WH_scale_lowpt"},
			  {"WH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ZH_scale_lowpt"},
			  {"ZH_lep_htt125",
			      "ggZH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      
      
      AddShapesIfNotEmpty({"WH_scale_highpt"},
			  {"WH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ZH_scale_highpt"},
			  {"ZH_lep_htt125",
			      "ggZH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      */
    }

  //**********************************************************************************************************
  //embedded uncertainties. 
  //**********************************************************************************************************
  if(not Input.OptionExists("-e"))
    {

      //50% correlation with ID unc in MC
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_e_2016","lnN",SystMap<>::init(1.010));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_e_embedded_2016","lnN",SystMap<>::init(1.01732));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_m_2016","lnN",SystMap<>::init(1.010));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_m_embedded_2016","lnN",SystMap<>::init(1.01732));

      // Embedded normalization
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_htt_doublemutrg_2016", "lnN", SystMap<>::init(1.04));

      //ttbar contamination in embedded
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_htt_emb_ttbar_2016", "shape", SystMap<>::init(1.00));
      
      //muon energy scale      
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_m_etalt1p2_2016","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_m_eta1p2to2p1_2016","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_m_eta2p1to2p4_2016","shape",SystMap<>::init(0.866));
      
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_m_etalt1p2_2016","shape",SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_m_eta1p2to2p1_2016","shape",SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_m_eta2p1to2p4_2016","shape",SystMap<>::init(0.500));

      /*cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_m_etalt1p2_2016","shape",SystMap<>::init(1.0));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_m_eta1p2to2p1_2016","shape",SystMap<>::init(1.0));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_m_eta2p1to2p4_2016","shape",SystMap<>::init(1.0));*/

      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_e_barrel_2016","shape",SystMap<>::init(1.0));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_e_endcap_2016","shape",SystMap<>::init(1.0));

      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_mu8e23trg_emb_2016","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_mu23e12trg_emb_2016","shape",SystMap<>::init(0.866));
      if(Input.OptionExists("-dm")||Input.OptionExists("-dn")||Input.OptionExists("-dp")||Input.OptionExists("-dljpt"))
	{
	  AddShapesIfNotEmpty({"CMS_bothmuetrg_emb_2016"},{"embedded"},&cb,0.866,TheFile,CategoryArgs);
	  AddShapesIfNotEmpty({"CMS_bothmuetrg_2016"},{"embedded"},&cb,0.5,TheFile,CategoryArgs);
	}
      else
	{
	  AddShapesIfNotEmpty({"CMS_bothmuetrg_emb_2016"},{"embedded"},&cb,0.866,TheFile,{"em_boosted1","em_boosted2","em_vbflow","em_vbfhigh"});
	  AddShapesIfNotEmpty({"CMS_bothmuetrg_2016"},{"embedded"},&cb,0.5,TheFile,{"em_boosted1","em_boosted2","em_vbflow","em_vbfhigh"});
	}
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_mu8e23trg_2016","shape",SystMap<>::init(0.5));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_mu23e12trg_2016","shape",SystMap<>::init(0.5));      

    }

  //****************************************************************************************************
  if (Input.OptionExists("-c"))
    {      
      cb.cp().backgrounds().ExtractShapes(
					  aux_shapes + "em_controls_2016.root",
					  "$BIN/$PROCESS",
					  "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
				      aux_shapes + "em_controls_2016.root",
				      "$BIN/$PROCESS$MASS",
				      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else if(Input.OptionExists("-gf"))
    {
      cb.cp().backgrounds().ExtractShapes(
                      aux_shapes + "smh2016em_GOF.root",
                      "$BIN/$PROCESS",
                      "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
                      aux_shapes + "smh2016em_GOF.root",
                      "$BIN/$PROCESS$MASS",
                      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else if(Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
    {
      cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "smh2016em_Differential.root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
      aux_shapes + "smh2016em_Differential.root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else
    {
      cb.cp().backgrounds().ExtractShapes(
					  aux_shapes + "smh2016em.root",
					  "$BIN/$PROCESS",
					  "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
				      aux_shapes + "smh2016em.root",
				      "$BIN/$PROCESS$MASS",
				      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  //! [part7]

  //! [part8]
  
  if (not Input.OptionExists("-b"))
    {
      auto bbb = ch::BinByBinFactory()
	.SetAddThreshold(0.05)
	.SetMergeThreshold(0.5)
	.SetFixNorm(false);
      bbb.MergeBinErrors(cb.cp().backgrounds());
      bbb.AddBinByBin(cb.cp().backgrounds(), cb);
      bbb.AddBinByBin(cb.cp().signals(), cb);
    }  

  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the htt analyses
  ch::SetStandardBinNames(cb);
  //! [part8]

  //! [part9]
  // First we generate a set of bin names:
  set<string> bins = cb.bin_set();
  // This method will produce a set of unique bin names by considering all
  // Observation, Process and Systematic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
  TFile output((Input.ReturnToken(0)+"/"+"smh2016_em.input.root").c_str(), "RECREATE");

  // Finally we iterate through each bin,mass combination and write a
  // datacard.
  for (auto b : bins) {
    for (auto m : masses) {
      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remember to include the "*" mass entry to get
      // all the data and backgrounds.
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(Input.ReturnToken(0)+"/"+b + "_" + m + ".txt", output);
    }
  }
  //! [part9]

}
