#include <iostream>
#include <math.h>
#include <fstream>

#include <TList.h>

#include <TROOT.h>
#include "TSystem.h"
#include <TFile.h>
#include <TChain.h>
#include <TGraphErrors.h>
#include <TVector3.h>
#include "TFitResult.h"
#include "TH1.h"
#include "TH2.h"
#include "TH2D.h"
#include "TH3.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TRandom3.h"
#include "TClonesArray.h"
#include "TChain.h"
#include "TLatex.h"

#include "TArrayI.h"
#include "RooFit.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooPlot.h"

#include <TCanvas.h>
#include "TStyle.h"
#include <TPaveStats.h>

static int trigType = 1; //(bit1=DoubleMuOpen)
static int accCutType = 2; //// 1=oldcut, 2=newcut, 3=preLooseCut, 4=2015PbPb
static int mergeData = 0; //// 0=default, 1=Pbp_v1, 2=Pbp_v2, select runNo. (only for Pbp 1st run!)
static bool doZWeighting = false; //0=no weight, 1=z vertex weight (only for MC)

static const double Jpsi_MassMin=2.6;
static const double Jpsi_MassMax=3.5;
static const double Jpsi_PtMin=0.0;
static const double Jpsi_PtMax=100;
static const double Jpsi_YMin=-2.4;
static const double Jpsi_YMax=2.4;
static const double Jpsi_CtMin = -3.0;
static const double Jpsi_CtMax = 5.0;
static const double Jpsi_CtErrMin = 0.0;
static const double Jpsi_CtErrMax = 1.0;

using namespace RooFit;
using namespace std;

// Variables for a dimuon
struct VariableStruct {
  double theMass, thePt, theRapidity;
  double theVtxProb, theZvtx, theCt, theCtErr, theWeight;
	double theETHF; //SumET_HFplusEta4 + SumET_HFminusEta4
	int theNtrk; // Ntracks
  int theSign, theHLTrig, theQQTrig;
  bool mupl_TrkMuArb, mumi_TrkMuArb;
  bool mupl_TMOneStaTight, mumi_TMOneStaTight;
  bool mupl_highPurity, mumi_highPurity;
  int mupl_nTrkWMea, mumi_nTrkWMea;
  int mupl_nPixWMea, mumi_nPixWMea;
  double mupl_dxy, mumi_dxy;
  double mupl_dz, mumi_dz;
} ;

bool checkTriggers(const struct VariableStruct Jpsi);
bool checkAccCut(const TLorentzVector* m1P, const TLorentzVector* m2P);
bool checkMuID(const struct VariableStruct Jpsi);

int main(int argc, char* argv[]) {
  
  string fileName, outputDir;
  int initev = 0;
  int nevt = -1;

  if (argc == 1) {
    cout << "====================================================================\n";
    cout << "Use the program with this commend:" << endl;
    cout << "./Tree2Datasets =t [trigType] =a [accCutType] =w [doZWeighting] =m [mergeData] =n initev nevt =f [input TTree file] [output directory]" << endl;
    cout << "====================================================================\n";
    return 0;
  }
  
  for (int i=1; i<argc; i++) {
    char *tmpargv = argv[i];
    switch (tmpargv[0]) {
      case '=':{
        switch (tmpargv[1]) {
					case 't':
						trigType = atoi(argv[i+1]);
						break;
					case 'a':
						accCutType = atoi(argv[i+1]);
						break;
          case 'w':
            if (atoi(argv[i+1]) == 0) doZWeighting = false;
            else doZWeighting =true;
            break;
					case 'm':
						mergeData = atoi(argv[i+1]);
						break;
          case 'n':
            initev = atoi(argv[i+1]);
            nevt = atoi(argv[i+2]);
            break;
          case 'f':
            fileName = argv[i+1];
            outputDir = argv[i+2];
            break;
        }
      }
    } 
  } // end of option inputs

  cout << "* fileName: " << fileName << endl;
  cout << "* output directory: " << outputDir << endl;
  cout << "* trigType: "<< trigType << endl;
  cout << "* accCutType: " << accCutType << endl;
  cout << "* doZWeighting: " << doZWeighting << endl;
  cout << "* mergeData (in this step, just making RooDataSet separately): " << mergeData << endl;
  cout << "* start event #: " << initev << endl;
  cout << "* end event #: " << nevt << endl;


  TFile *file=TFile::Open(fileName.c_str());
  //TTree *myTree=(TTree*)file->Get("myTree"); //for_pa
  TTree *myTree=(TTree*)file->Get("hionia/myTree"); //for_pp
  
	UInt_t          runNb; // Check run number
	Int_t 					Ntracks; // multiplicity dependence
	Float_t 				SumET_HFplusEta4; // multiplicity dependence
	Float_t 				SumET_HFminusEta4; // multiplicity dependence
  Float_t         zVtx;         //Primary vertex position
  ULong64_t       HLTriggers; //for_pp
  //Int_t 		      HLTriggers; //for_pa
  Int_t           Reco_QQ_size;
  Int_t           Reco_QQ_sign[210];   //[Reco_QQ_size]
 	ULong64_t       Reco_QQ_trig[210];   //[Reco_QQ_size] //for_pp
 	//Int_t	       		Reco_QQ_trig[210];   //[Reco_QQ_size] //for_pa
  Float_t         Reco_QQ_VtxProb[210];   //[Reco_QQ_size]
  Float_t         Reco_QQ_ctau[210];   //[Reco_QQ_size]
  Float_t         Reco_QQ_ctauErr[210];   //[Reco_QQ_size]

  Bool_t          Reco_QQ_mupl_TrkMuArb[210]; 
  Bool_t          Reco_QQ_mumi_TrkMuArb[210];  
  Bool_t          Reco_QQ_mupl_TMOneStaTight[210];  
  Bool_t          Reco_QQ_mumi_TMOneStaTight[210]; 
  Bool_t          Reco_QQ_mupl_highPurity[210];
  Bool_t          Reco_QQ_mumi_highPurity[210]; 
  Int_t           Reco_QQ_mupl_nTrkWMea[210];  
  Int_t           Reco_QQ_mumi_nTrkWMea[210];  
  Int_t           Reco_QQ_mupl_nPixWMea[210];  
  Int_t           Reco_QQ_mumi_nPixWMea[210]; 
  Float_t         Reco_QQ_mupl_dxy[210]; 
  Float_t         Reco_QQ_mumi_dxy[210];
  Float_t         Reco_QQ_mupl_dz[210]; 
  Float_t         Reco_QQ_mumi_dz[210]; 

  TClonesArray    *Reco_QQ_4mom;
  TClonesArray    *Reco_QQ_mupl_4mom;
  TClonesArray    *Reco_QQ_mumi_4mom;

//  Float_t         Reco_QQ_ctauTrue[210];   //[Reco_QQ_size]

  TBranch        *b_runNb;
  TBranch        *b_Ntracks;   //!
  TBranch        *b_SumET_HFplusEta4;   //!
  TBranch        *b_SumET_HFminusEta4;   //!
  TBranch        *b_zVtx;
  TBranch        *b_HLTriggers;   //!
  TBranch        *b_Reco_QQ_size;   //!
  TBranch        *b_Reco_QQ_sign;   //!
  TBranch        *b_Reco_QQ_trig;   //!
  TBranch        *b_Reco_QQ_VtxProb;   //!
  TBranch        *b_Reco_QQ_ctau;   //!
  TBranch        *b_Reco_QQ_ctauErr;   //!
 
  TBranch        *b_Reco_QQ_mupl_TrkMuArb;   //!
  TBranch        *b_Reco_QQ_mumi_TrkMuArb;   //!
  TBranch        *b_Reco_QQ_mupl_TMOneStaTight;   //!
  TBranch        *b_Reco_QQ_mumi_TMOneStaTight;   //!
  TBranch        *b_Reco_QQ_mupl_highPurity;   //!
  TBranch        *b_Reco_QQ_mumi_highPurity;   //!
  TBranch        *b_Reco_QQ_mupl_nTrkWMea;   //!
  TBranch        *b_Reco_QQ_mumi_nTrkWMea;   //!
  TBranch        *b_Reco_QQ_mumi_nPixWMea;   //!
  TBranch        *b_Reco_QQ_mupl_nPixWMea;   //!
  TBranch        *b_Reco_QQ_mupl_dxy;   //!
  TBranch        *b_Reco_QQ_mumi_dxy;   //!
  TBranch        *b_Reco_QQ_mupl_dz;   //!
  TBranch        *b_Reco_QQ_mumi_dz;   //!

  TBranch        *b_Reco_QQ_4mom;   //!
  TBranch        *b_Reco_QQ_mupl_4mom;   //!
  TBranch        *b_Reco_QQ_mumi_4mom;   //!

//  TBranch        *b_Reco_QQ_ctauTrue;   //!

  TLorentzVector* JP= new TLorentzVector;
  TLorentzVector* m1P= new TLorentzVector;
  TLorentzVector* m2P= new TLorentzVector;


  TH1I *PassingEvent; // counting
  //// **** Normal datasets
  RooDataSet* dataJpsi;
  RooDataSet* dataJpsiSS;
  //// **** Have efficiency for every events <- just for Check
  RooDataSet* dataJpsiWCheck;
  RooDataSet* dataJpsiSSWCheck;
  //// **** Efficiencies are applied to datasets as a weight
  RooDataSet* dataJpsiWeight;
  RooDataSet* dataJpsiSSWeight;
  
  RooRealVar* Jpsi_Mass = new RooRealVar("Jpsi_Mass","J/#psi mass",Jpsi_MassMin,Jpsi_MassMax,"GeV/c^{2}");
  RooRealVar* Jpsi_Pt = new RooRealVar("Jpsi_Pt","J/#psi pt",Jpsi_PtMin,Jpsi_PtMax,"GeV/c");
  RooRealVar* Jpsi_Y = new RooRealVar("Jpsi_Y","J/#psi y",-Jpsi_YMax,Jpsi_YMax);
  RooRealVar* Jpsi_Ct = new RooRealVar("Jpsi_Ct","J/#psi c#tau",Jpsi_CtMin,Jpsi_CtMax,"mm");
  RooRealVar* Jpsi_CtErr = new RooRealVar("Jpsi_CtErr","J/#psi c#tau error",Jpsi_CtErrMin,Jpsi_CtErrMax,"mm");
  RooRealVar* Jpsi_ETHF = new RooRealVar("SumET_HFEta4","sum of ET in HF plusEta4 and minusEta4",0.,400.);
  RooRealVar* Jpsi_Ntrk = new RooRealVar("Ntracks","number of tracks in the event",0.,400.);
  RooRealVar* Jpsi_Zvtx = new RooRealVar("Jpsi_Zvtx","primary Z vertex for each events",-30.,30.);
  RooRealVar* Jpsi_Weight = new RooRealVar("Jpsi_Weight","J/#psi efficiency weight",0.,100.);
  RooCategory* Jpsi_Sign = new RooCategory("Jpsi_Sign","Charge combination of Jpsi_");
//  RooRealVar* Jpsi_CtTrue = new RooRealVar("Jpsi_CtTrue","J/#psi c#tau true",Jpsi_CtMin,Jpsi_CtMax,"mm");


  Jpsi_Sign->defineType("OS",0);
  Jpsi_Sign->defineType("PP",1);
  Jpsi_Sign->defineType("MM",2);

  Reco_QQ_4mom = 0;
  Reco_QQ_mupl_4mom = 0;
  Reco_QQ_mumi_4mom = 0;

  myTree->SetBranchAddress("runNb", &runNb, &b_runNb);
  myTree->SetBranchAddress("Ntracks", &Ntracks, &b_Ntracks);
  myTree->SetBranchAddress("SumET_HFplusEta4", &SumET_HFplusEta4, &b_SumET_HFplusEta4);
  myTree->SetBranchAddress("SumET_HFminusEta4", &SumET_HFminusEta4, &b_SumET_HFminusEta4);
  myTree->SetBranchAddress("zVtx", &zVtx, &b_zVtx);
  myTree->SetBranchAddress("HLTriggers", &HLTriggers, &b_HLTriggers);
  myTree->SetBranchAddress("Reco_QQ_size", &Reco_QQ_size, &b_Reco_QQ_size);
  myTree->SetBranchAddress("Reco_QQ_trig", Reco_QQ_trig, &b_Reco_QQ_trig);
  myTree->SetBranchAddress("Reco_QQ_sign", Reco_QQ_sign, &b_Reco_QQ_sign);
  myTree->SetBranchAddress("Reco_QQ_VtxProb", Reco_QQ_VtxProb, &b_Reco_QQ_VtxProb);
  myTree->SetBranchAddress("Reco_QQ_ctau", Reco_QQ_ctau, &b_Reco_QQ_ctau);
  myTree->SetBranchAddress("Reco_QQ_ctauErr", Reco_QQ_ctauErr, &b_Reco_QQ_ctauErr);
  
  myTree->SetBranchAddress("Reco_QQ_mupl_TrkMuArb", Reco_QQ_mupl_TrkMuArb, &b_Reco_QQ_mupl_TrkMuArb);
  myTree->SetBranchAddress("Reco_QQ_mumi_TrkMuArb", Reco_QQ_mumi_TrkMuArb, &b_Reco_QQ_mumi_TrkMuArb);
  myTree->SetBranchAddress("Reco_QQ_mupl_TMOneStaTight", Reco_QQ_mupl_TMOneStaTight, &b_Reco_QQ_mupl_TMOneStaTight);
  myTree->SetBranchAddress("Reco_QQ_mumi_TMOneStaTight", Reco_QQ_mumi_TMOneStaTight, &b_Reco_QQ_mumi_TMOneStaTight);
  //// for_pp
	myTree->SetBranchAddress("Reco_QQ_mupl_highPurity", Reco_QQ_mupl_highPurity, &b_Reco_QQ_mupl_highPurity);
  myTree->SetBranchAddress("Reco_QQ_mumi_highPurity", Reco_QQ_mumi_highPurity, &b_Reco_QQ_mumi_highPurity);
  //// for_pa
	//myTree->SetBranchAddress("Reco_QQ_mupl_isHighPurity", Reco_QQ_mupl_highPurity, &b_Reco_QQ_mupl_highPurity);
  //myTree->SetBranchAddress("Reco_QQ_mumi_isHighPurity", Reco_QQ_mumi_highPurity, &b_Reco_QQ_mumi_highPurity);
  myTree->SetBranchAddress("Reco_QQ_mupl_nTrkWMea", Reco_QQ_mupl_nTrkWMea, &b_Reco_QQ_mupl_nTrkWMea);
  myTree->SetBranchAddress("Reco_QQ_mumi_nTrkWMea", Reco_QQ_mumi_nTrkWMea, &b_Reco_QQ_mumi_nTrkWMea);
  myTree->SetBranchAddress("Reco_QQ_mupl_nPixWMea", Reco_QQ_mupl_nPixWMea, &b_Reco_QQ_mupl_nPixWMea);
  myTree->SetBranchAddress("Reco_QQ_mumi_nPixWMea", Reco_QQ_mumi_nPixWMea, &b_Reco_QQ_mumi_nPixWMea);
  myTree->SetBranchAddress("Reco_QQ_mupl_dxy", Reco_QQ_mupl_dxy, &b_Reco_QQ_mupl_dxy);
  myTree->SetBranchAddress("Reco_QQ_mumi_dxy", Reco_QQ_mumi_dxy, &b_Reco_QQ_mumi_dxy);
  myTree->SetBranchAddress("Reco_QQ_mupl_dz", Reco_QQ_mupl_dz, &b_Reco_QQ_mupl_dz);
  myTree->SetBranchAddress("Reco_QQ_mumi_dz", Reco_QQ_mumi_dz, &b_Reco_QQ_mumi_dz);

  myTree->SetBranchAddress("Reco_QQ_4mom", &Reco_QQ_4mom, &b_Reco_QQ_4mom);
  myTree->SetBranchAddress("Reco_QQ_mupl_4mom", &Reco_QQ_mupl_4mom, &b_Reco_QQ_mupl_4mom);
  myTree->SetBranchAddress("Reco_QQ_mumi_4mom", &Reco_QQ_mumi_4mom, &b_Reco_QQ_mumi_4mom);

//  myTree->SetBranchAddress("Reco_QQ_ctauTrue", Reco_QQ_ctauTrue, &b_Reco_QQ_ctauTrue);

  //// list without a weighting factor
  RooArgList varlist(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Zvtx,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_Ntrk,*Jpsi_ETHF);
  //// list with a weighting factor
  RooArgList varlistW(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Zvtx,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_Ntrk,*Jpsi_ETHF,*Jpsi_Weight);

  PassingEvent = new TH1I("NumPassingEvent",";;total number of events",1,1,2); // for counting
  dataJpsi = new RooDataSet("dataJpsi","A sample",varlist); // actual
  dataJpsiSS = new RooDataSet("dataJpsiSS","A sample",varlist); // same sign
  if (doZWeighting) {
    dataJpsiWCheck = new RooDataSet("dataJpsiWCheck","A sample",varlistW);
    dataJpsiSSWCheck = new RooDataSet("dataJpsiSSWCheck","A sample",varlistW);
  }

  if (nevt == -1) nevt = myTree->GetEntries();
  for (int ev=initev; ev<nevt; ++ev) {
    if (ev%100000==0) cout << ">>>>> EVENT " << ev << " / " << myTree->GetEntries() <<  endl;

    myTree->GetEntry(ev);

		//// **** dataMerge Check!! (only for Pbp 1st run)
		//// mergeData==1 (v1) : 1st 7 runs excluded, mergeData==2 (v2) : 1st 7 runs only 
		if (mergeData == 1 && runNb >= 210498 && 210658 >= runNb) { continue; }
		else if (mergeData == 2 && (! (runNb >= 210498 && 210658 >= runNb))) { continue; }

		for (int i=0; i<Reco_QQ_size; ++i) {
      
			struct VariableStruct Jpsi; //Contains all condition variables
      JP = (TLorentzVector*) Reco_QQ_4mom->At(i);
      m1P = (TLorentzVector*) Reco_QQ_mupl_4mom->At(i);
      m2P = (TLorentzVector*) Reco_QQ_mumi_4mom->At(i);
      
			Jpsi.theNtrk = Ntracks;
      Jpsi.theETHF = SumET_HFplusEta4 + SumET_HFminusEta4;
      Jpsi.theZvtx = zVtx;
      Jpsi.theHLTrig = HLTriggers;
      Jpsi.theSign = Reco_QQ_sign[i];
      Jpsi.theQQTrig = Reco_QQ_trig[i];
			Jpsi.theVtxProb = Reco_QQ_VtxProb[i];
			Jpsi.theCt = Reco_QQ_ctau[i];
      Jpsi.theCtErr = Reco_QQ_ctauErr[i];
      
			Jpsi.mupl_TrkMuArb = Reco_QQ_mupl_TrkMuArb[i];
      Jpsi.mumi_TrkMuArb = Reco_QQ_mumi_TrkMuArb[i];
			Jpsi.mupl_TMOneStaTight = Reco_QQ_mupl_TMOneStaTight[i];
      Jpsi.mumi_TMOneStaTight = Reco_QQ_mumi_TMOneStaTight[i];
			Jpsi.mupl_highPurity = Reco_QQ_mupl_highPurity[i];
      Jpsi.mumi_highPurity = Reco_QQ_mumi_highPurity[i];
			Jpsi.mupl_nTrkWMea = Reco_QQ_mupl_nTrkWMea[i];
      Jpsi.mumi_nTrkWMea = Reco_QQ_mumi_nTrkWMea[i];
      Jpsi.mupl_nPixWMea = Reco_QQ_mupl_nTrkWMea[i];
      Jpsi.mumi_nPixWMea = Reco_QQ_mumi_nTrkWMea[i];
			Jpsi.mupl_dxy = Reco_QQ_mupl_dxy[i];
      Jpsi.mumi_dxy = Reco_QQ_mumi_dxy[i];
			Jpsi.mupl_dz = Reco_QQ_mupl_dz[i];
      Jpsi.mumi_dz = Reco_QQ_mumi_dz[i];

      Jpsi.theMass =JP->M();
      Jpsi.thePt=JP->Pt();
      Jpsi.theRapidity=JP->Rapidity();

      Jpsi.theWeight = 1.0;
        
			/*
      //// *** Cowboy or Sailor (get delta Phi between 2 muons)
      double mumuPhi = m1P->Phi() - m2P->Phi();
      while (mumuPhi > TMath::Pi()) mumuPhi -= 2*TMath::Pi();
      while (mumuPhi <= -TMath::Pi()) mumuPhi += 2*TMath::Pi();
      bool cowboy = false, sailor = false;
      if ( 1*mumuPhi > 0. ) cowboy = true;
      else sailor = true;
      */

			//// *** Check all cuts (trigger, single mu acc cut, muID..)
      if (Jpsi.theMass > Jpsi_MassMin && Jpsi.theMass < Jpsi_MassMax && 
        Jpsi.thePt > Jpsi_PtMin && Jpsi.thePt < Jpsi_PtMax && 
        Jpsi.theRapidity > Jpsi_YMin && Jpsi.theRapidity < Jpsi_YMax &&
        Jpsi.theCt > Jpsi_CtMin && Jpsi.theCt < Jpsi_CtMax && 
        Jpsi.theCtErr > Jpsi_CtErrMin && Jpsi.theCtErr < Jpsi_CtErrMax && 
        checkAccCut(m1P,m2P) &&
       	checkTriggers(Jpsi) &&
        checkMuID(Jpsi)
        ) {

        Jpsi_Mass->setVal(Jpsi.theMass);
        Jpsi_Pt->setVal(Jpsi.thePt); 
        Jpsi_Y->setVal(Jpsi.theRapidity); 
        Jpsi_Ct->setVal(Jpsi.theCt);
        Jpsi_CtErr->setVal(Jpsi.theCtErr);
        Jpsi_Ntrk->setVal(Jpsi.theNtrk);
        Jpsi_ETHF->setVal(Jpsi.theETHF);
        Jpsi_Zvtx->setVal(Jpsi.theZvtx);
        Jpsi_Weight->setVal(Jpsi.theWeight);
        
				if(Jpsi.theSign== 0){ Jpsi_Sign->setIndex(Jpsi.theSign,kTRUE); }
        else { Jpsi_Sign->setIndex(Jpsi.theSign,kTRUE); }
        //// list without a weighting factor
        RooArgList varlist_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Zvtx,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_Ntrk,*Jpsi_ETHF);
        //// list with a weighting factor
        RooArgList varlistW_tmp(*Jpsi_Mass,*Jpsi_Pt,*Jpsi_Y,*Jpsi_Zvtx,*Jpsi_Ct,*Jpsi_CtErr,*Jpsi_Ntrk,*Jpsi_ETHF,*Jpsi_Weight);

        if (Jpsi.theSign == 0) { // Check opposite sign! (0=+-,1=++,2=+-)
           dataJpsi->add(varlist_tmp);
           if (doZWeighting) dataJpsiWCheck->add(varlistW_tmp); //add to RooDataSets
           PassingEvent->Fill(1); //for counting
        } else {
           dataJpsiSS->add(varlist_tmp);
           if (doZWeighting) dataJpsiSSWCheck->add(varlistW_tmp);
        }
      } // End of if() statement for all cuts
    } // End of Reco_QQ_size loop
  } // End of tree event loop

	//// make new RooDataSet actually weighted!!
  if (doZWeighting) {
    dataJpsiWeight = new RooDataSet("dataJpsiWeight","A sample",*dataJpsiWCheck->get(),Import(*dataJpsiWCheck),WeightVar(*Jpsi_Weight));
    dataJpsiSSWeight = new RooDataSet("dataJpsiSSWeight","A sample",*dataJpsiSSWCheck->get(),Import(*dataJpsiSSWCheck),WeightVar(*Jpsi_Weight));
		cout << " *** weighted! *** " << endl;
  }

  /// *** Fill TFiles with RooDataSet
  TFile* Out;
  char namefile[531];
  sprintf(namefile,"%s/%s.root",outputDir.c_str(),outputDir.c_str());
  Out = new TFile(namefile,"RECREATE");
  Out->cd();
  dataJpsi->Write();
  dataJpsiSS->Write();
  if (doZWeighting) {
    dataJpsiWCheck->Write();
    dataJpsiSSWCheck->Write();
    dataJpsiWeight->Write();
    dataJpsiSSWeight->Write();
  }
  Out->Close();

  cout << "PassingEvent: " << PassingEvent->GetEntries() << endl;
  delete PassingEvent;

  return 0;
}

////////////////////////////////////////////////////////////////////
/////////////////////////// sub-routines ///////////////////////////
////////////////////////////////////////////////////////////////////

bool checkTriggers(const struct VariableStruct Jpsi){
  		return ( (Jpsi.theHLTrig&trigType)==trigType && (Jpsi.theQQTrig&trigType)==trigType );
}

bool checkAccCut(const TLorentzVector* m1P, const TLorentzVector* m2P) {
  if (accCutType == 1) { //old cut
     return (
		 	( fabs(m1P->Eta()) < 2.4 &&
		 	((fabs(m1P->Eta()) < 1.3 && m1P->Pt() >= 3.3) ||
			(1.3 <= fabs(m1P->Eta()) && fabs(m1P->Eta()) < 2.2 && m1P->P() >= 2.9) ||
			(2.2 <= fabs(m1P->Eta()) && m1P->Pt() >= 0.8)) ) &&
		 	( fabs(m2P->Eta()) < 2.4 &&
		 	((fabs(m2P->Eta()) < 1.3 && m2P->Pt() >= 3.3) ||
			(1.3 <= fabs(m2P->Eta()) && fabs(m2P->Eta()) < 2.2 && m2P->P() >= 2.9) ||
			(2.2 <= fabs(m2P->Eta()) && m2P->Pt() >= 0.8)) )
		);
  }
  else if (accCutType == 2) { // new cut
     return (
		 	( fabs(m1P->Eta()) < 2.4 &&
		 	((fabs(m1P->Eta()) < 1.2 && m1P->Pt() >= 3.3) ||
			(1.2 <= fabs(m1P->Eta()) && fabs(m1P->Eta()) < 2.1 && m1P->Pt() >= -(1.0/0.9)*fabs(m1P->Eta())+1.2*(1.0/0.9)+2.6) ||
			//(1.2 <= fabs(m1P->Eta()) && fabs(m1P->Eta()) < 2.1 && m1P->Pt() >= 3.93-1.1*fabs(m1P->Eta())) ||
			(2.1 <= fabs(m1P->Eta()) && m1P->Pt() >= 1.3)) ) &&
		 	( fabs(m2P->Eta()) < 2.4 &&
		 	((fabs(m2P->Eta()) < 1.2 && m2P->Pt() >= 3.3) ||
			(1.2 <= fabs(m2P->Eta()) && fabs(m2P->Eta()) < 2.1 && m2P->Pt() >= -(1.0/0.9)*fabs(m2P->Eta())+1.2*(1.0/0.9)+2.6) ||
			//(1.2 <= fabs(m2P->Eta()) && fabs(m2P->Eta()) < 2.1 && m2P->Pt() >= 3.93-1.1*fabs(m2P->Eta())) ||
			(2.1 <= fabs(m2P->Eta()) && m2P->Pt() >= 1.3)) )
		);
  }
  else if (accCutType == 3) { //pre-loose cut A on pA data
     return (
		 	( fabs(m1P->Eta()) < 2.4 &&
		 	((fabs(m1P->Eta()) < 1.2 && m1P->Pt() >= 3.3) ||
			(1.2 <= fabs(m1P->Eta()) && fabs(m1P->Eta()) < 1.6 && m1P->Pt() >= -(1.4/0.4)*fabs(m1P->Eta())+1.2*(1.4/0.4)+2.6) ||
			(1.6 <= fabs(m1P->Eta()) && fabs(m1P->Eta()) < 2.1 && m1P->Pt() >= -(0.4/0.5)*fabs(m1P->Eta())+1.6*(0.4/0.5)+1.2) ||
			(2.1 <= fabs(m1P->Eta()) && m1P->Pt() >= 0.8)) ) &&
		 	( fabs(m2P->Eta()) < 2.4 &&
		 	((fabs(m2P->Eta()) < 1.2 && m2P->Pt() >= 3.3) ||
			(1.2 <= fabs(m2P->Eta()) && fabs(m2P->Eta()) < 1.6 && m2P->Pt() >= -(1.4/0.4)*fabs(m2P->Eta())+1.2*(1.4/0.4)+2.6) ||
			(1.6 <= fabs(m2P->Eta()) && fabs(m2P->Eta()) < 2.1 && m2P->Pt() >= -(0.4/0.5)*fabs(m2P->Eta())+1.6*(0.4/0.5)+1.2) ||
			(2.1 <= fabs(m2P->Eta()) && m2P->Pt() >= 0.8)) )
		);
  }
  else if (accCutType == 4) { // 2015 PbPb GlbTrk muons
     return (
		 	( fabs(m1P->Eta()) < 2.4 &&
		 	((fabs(m1P->Eta()) < 1.2 && m1P->Pt() >= 3.5) ||
			(1.2 <= fabs(m1P->Eta()) && fabs(m1P->Eta()) < 2.1 && m1P->Pt() >= 5.77-(1.89)*fabs(m1P->Eta())) ||
			(2.1 <= fabs(m1P->Eta()) && m1P->Pt() >= 1.8)) ) &&
		 	( fabs(m2P->Eta()) < 2.4 &&
		 	((fabs(m2P->Eta()) < 1.2 && m2P->Pt() >= 3.5) ||
			(1.2 <= fabs(m2P->Eta()) && fabs(m2P->Eta()) < 2.1 && m2P->Pt() >= 5.77-(1.89)*fabs(m2P->Eta())) ||
			(2.1 <= fabs(m2P->Eta()) && m2P->Pt() >= 1.8)) )
		);
  }
	else {return false;}
}

bool checkMuID(const struct VariableStruct Jpsi){
	return (
					Jpsi.theVtxProb > 0.01 &&
					Jpsi.mupl_TrkMuArb &&
					Jpsi.mupl_TMOneStaTight &&
					Jpsi.mupl_highPurity &&
					Jpsi.mupl_nTrkWMea > 5 &&
					Jpsi.mupl_nPixWMea > 0 &&
					fabs(Jpsi.mupl_dxy) < 0.3 &&
					fabs(Jpsi.mupl_dz) < 20.0 &&
					Jpsi.mumi_TrkMuArb &&
					Jpsi.mumi_TMOneStaTight &&
					Jpsi.mumi_highPurity &&
					Jpsi.mumi_nTrkWMea > 5 &&
					Jpsi.mumi_nPixWMea > 0 &&
					fabs(Jpsi.mumi_dxy) < 0.3 &&
					fabs(Jpsi.mumi_dz) < 20.0
				);
}
