#include <fit2DData.h>
#include "CMS_lumi.h"

void CMS_lumi( TPad* pad, int iPeriod, int iPosX );

int main (int argc, char* argv[]) {
	
	RooMsgService::instance().getStream(0).removeTopic(Plotting);
  RooMsgService::instance().getStream(0).removeTopic(InputArguments);
  RooMsgService::instance().getStream(1).removeTopic(InputArguments);
  RooMsgService::instance().getStream(1).removeTopic(Plotting);
  RooMsgService::instance().getStream(1).removeTopic(NumIntegration);
  RooMsgService::instance().getStream(1).removeTopic(Minimization);
  RooMsgService::instance().getStream(1).removeTopic(Caching);
  
	gROOT->Macro("/afs/cern.ch/work/k/kyolee/private/CMSSW_8_0_0/src/2Dfitting/JpsiStyle6.C");
	//gROOT->Macro("/home/songkyo/kyo/2Dfitting/JpsiStyle6.C");
	//gROOT->Macro("../JpsiStyle6.C");
	
	/////////////////////////////////// check input options ///////////////////////////////////	
  parseInputArg(argc, argv, inOpt);
  float pmin=0, pmax=0, ymin=0, ymax=0, lmin=0, lmax=0, errmin=0, errmax=0; //pt, y, ct, ctErr
  float etmin=0, etmax=0, ntrmin=0, ntrmax=0; // ETHF, Ntrk
	getOptRange(inOpt.ptrange,&pmin,&pmax);
  getOptRange(inOpt.yrange,&ymin,&ymax);
  getOptRange(inOpt.lrange,&lmin,&lmax);
  getOptRange(inOpt.etrange,&etmin,&etmax);
  getOptRange(inOpt.ntrrange,&ntrmin,&ntrmax);
//  if (!inOpt.ptrange.compare("2.0-3.0") ){ lmin = 3.0; lmax = 5.0; } // TEST

  ////// **** Strings for plot formatting
  formTitle(inOpt);
  formRapidity(inOpt, ymin, ymax);
  formPt(inOpt, pmin, pmax);
	formEt(inOpt, etmin, etmax);
	formNtrk(inOpt, ntrmin, ntrmax);

  ////// **** Global objects for legend
  TLatex *t = new TLatex();  t->SetNDC();  t->SetTextAlign(12); t->SetTextFont(42);
  Double_t fx[2], fy[2], fex[2], fey[2];
  //// black points (data, etc)
	gfake1 = new TGraphErrors(2,fx,fy,fex,fey);
  gfake1->SetMarkerStyle(20); gfake1->SetMarkerSize(1);
  //// blue line (background)
	hfake11 = TH1F("hfake11","hfake1",100,200,300);
  hfake11.SetLineColor(kBlue); hfake11.SetLineWidth(4); hfake11.SetLineStyle(7); hfake11.SetFillColor(kAzure-9); hfake11.SetFillStyle(1001);
  //// black line (total fit)
	hfake21 = TH1F("hfake21","hfake2",100,200,300);
  hfake21.SetLineColor(kBlack); hfake21.SetLineWidth(4); hfake21.SetFillColor(kBlack); hfake21.SetFillStyle(3354);
  //// red line (non-prompt)
	hfake31 = TH1F("hfake31","hfake3",100,200,300);
  hfake31.SetLineColor(kRed); hfake31.SetMarkerStyle(kCircle); hfake31.SetLineWidth(4); hfake31.SetMarkerColor(kRed); hfake31.SetLineStyle(9); hfake31.SetFillColor(kRed-7); hfake31.SetFillStyle(3444);
  hfake311 = TH1F("hfake311","hfake311",100,200,300);
  hfake311.SetLineColor(kRed); hfake311.SetMarkerStyle(kCircle); hfake311.SetLineWidth(4); hfake311.SetMarkerColor(kRed); hfake311.SetLineStyle(kDashed); hfake311.SetFillColor(kRed-7); hfake311.SetFillStyle(3444);
	//// green line (prompt)
  hfake41 = TH1F("hfake41","hfake4",100,200,300);
  hfake41.SetLineColor(kGreen+1); hfake41.SetMarkerStyle(kCircle); hfake41.SetLineWidth(4); hfake41.SetMarkerColor(kGreen+1); hfake41.SetLineStyle(kDashDotted); hfake41.SetFillColor(kGreen-7); hfake41.SetFillStyle(3444);

	/////////////////////////////////// read MC and Data files ///////////////////////////////////	
	
	//// prompt MC /////////////////////////////////////////////////
  TFile fInPRMC(inOpt.FileNamePRMC.c_str());
  cout << inOpt.FileNamePRMC.c_str() << endl;
  if (fInPRMC.IsZombie()) { cout << "CANNOT open PRMC file\n"; return 1; }
  fInPRMC.cd();
  RooDataSet *dataPRMC;
  if (inOpt.doMCWeight == 1) {
    dataPRMC = (RooDataSet*)fInPRMC.Get("dataJpsiWeight");
    cout << "## WEIGHTED PRMC is used\n";
  } else {
    dataPRMC = (RooDataSet*)fInPRMC.Get("dataJpsi");
    cout << "## UN-WEIGHTED PRMC is used!\n";
 	} 
  dataPRMC->SetName("dataPRMC");

	TFile* fInPRMC2;	
	if (inOpt.mcMerge==2) {
		fInPRMC2 = new TFile(inOpt.FileNamePRMC2.c_str()); //// Data2
		cout << "## NOTICE : 2nd PRMC to be merged \n";
		cout << inOpt.FileNamePRMC2.c_str() << endl;
  	if (fInPRMC2->IsZombie()) { cout << "CANNOT open 2nd PRMC file\n"; return 1; }
  	fInPRMC2->cd();
  	RooDataSet *dataPRMC2;
  	if (inOpt.doMCWeight == 1) {
    	dataPRMC2 = (RooDataSet*)fInPRMC2->Get("dataJpsiWeight");
  	} else {
    	dataPRMC2 = (RooDataSet*)fInPRMC2->Get("dataJpsi");
	 	} 
  	dataPRMC2->SetName("dataPRMC2");
		dataPRMC->append(*dataPRMC2); //// merge "data2" to "data"
	}
	
	//// non-prompt MC /////////////////////////////////////////////////
  TFile fInNPMC(inOpt.FileNameNPMC.c_str());
  cout << inOpt.FileNameNPMC.c_str() << endl;
  if (fInNPMC.IsZombie()) { cout << "CANNOT open non-prompt MC file\n"; return 1; }
  fInNPMC.cd();
  RooDataSet *dataNPMC;
  if (inOpt.doMCWeight == 1) {
    dataNPMC = (RooDataSet*)fInNPMC.Get("dataJpsiWeight");
    cout << "## WEIGHTED NPMC is used\n";
  } else {
    dataNPMC = (RooDataSet*)fInNPMC.Get("dataJpsi");
    cout << "## UN-WEIGHTED NPMC is used!\n";
 	} 
  dataNPMC->SetName("dataNPMC");
	
	TFile* fInNPMC2;	
	if (inOpt.mcMerge==2) {
		fInNPMC2 = new TFile(inOpt.FileNameNPMC2.c_str()); //// Data2
		cout << "## NOTICE : 2nd NPMC to be merged \n";
		cout << inOpt.FileNameNPMC2.c_str() << endl;
  	if (fInNPMC2->IsZombie()) { cout << "CANNOT open 2nd NPMC file\n"; return 1; }
  	fInNPMC2->cd();
  	RooDataSet *dataNPMC2;
  	if (inOpt.doMCWeight == 1) {
    	dataNPMC2 = (RooDataSet*)fInNPMC2->Get("dataJpsiWeight");
  	} else {
    	dataNPMC2 = (RooDataSet*)fInNPMC2->Get("dataJpsi");
	 	} 
  	dataNPMC2->SetName("dataNPMC2");
		dataNPMC->append(*dataNPMC2); //// merge "data2" to "data"
	}

	//// Data /////////////////////////////////////////////////
  TFile fInData(inOpt.FileNameData.c_str());
  cout << inOpt.FileNameData.c_str() << endl;
  if (fInData.IsZombie()) { cout << "CANNOT open Data file\n"; return 1; }
  fInData.cd();
  RooDataSet *data;
  data = (RooDataSet*)fInData.Get("dataJpsi");
  cout << "## UN-WEIGHTED dataset is used!\n";
  data->SetName("data");

	TFile* fInData2;
	TFile* fInData3;
	if (inOpt.dataMerge==2 || inOpt.dataMerge==3) {
		fInData2 = new TFile(inOpt.FileNameData2.c_str()); //// Data2
		cout << "## NOTICE : 2nd data to be merged \n";
		cout << inOpt.FileNameData2.c_str() << endl;
  	if (fInData2->IsZombie()) { cout << "CANNOT open 2nd Data file\n"; return 1; }
  	fInData2->cd();
  	RooDataSet *data2;
	 	data2 = (RooDataSet*)fInData2->Get("dataJpsi");
 		data2->SetName("data2");
		data->append(*data2); //// merge "data2" to "data"
		if (inOpt.dataMerge==3) {
			fInData3 = new TFile(inOpt.FileNameData3.c_str()); //// Data3
			cout << "## NOTICE : 3rd data to be merged \n";
			cout << inOpt.FileNameData3.c_str() << endl;
 	 		if (fInData3->IsZombie()) { cout << "CANNOT open 3rd Datadata file\n"; return 1; }
 	 		fInData3->cd();
 	 		RooDataSet *data3;
		 	data3 = (RooDataSet*)fInData3->Get("dataJpsi");
 			data3->SetName("data3");
			data->append(*data3); //// merge "data2" to "data"
		}
	}

  //// **** Create workspace to play with
  RooWorkspace *ws = new RooWorkspace("workspace");

	/////////////////////////////////// reduce [data] to [redData] ///////////////////////////////////	
  char reduceDS[3000], reduceDS_woCtErr[3000], reduceDSMC[3000], reduceDSMC_woCtErr[3000];
	if (inOpt.absoluteY==1) {
		//// **** string without CtErrCut
		if (inOpt.EventActivity==0) {
			sprintf(reduceDS_woCtErr, 
						"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && TMath::Abs(Jpsi_Y)>=%.2f && TMath::Abs(Jpsi_Y)<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f", 
						pmin,pmax,ymin,ymax,-lmin,lmax);
		} else {
			sprintf(reduceDS_woCtErr, 
						"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && TMath::Abs(Jpsi_Y)>=%.2f && TMath::Abs(Jpsi_Y)<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f && Ntracks >=%.1f && Ntracks < %.1f && SumET_HFEta4 >=%.2f && SumET_HFEta4 < %.2f", 
						pmin,pmax,ymin,ymax,-lmin,lmax,ntrmin,ntrmax,etmin,etmax);
		}
		sprintf(reduceDSMC_woCtErr, 
					"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && TMath::Abs(Jpsi_Y)>=%.2f && TMath::Abs(Jpsi_Y)<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f", 
					pmin,pmax,ymin,ymax,-lmin,lmax);
		//// **** check ctErr Range
		if (inOpt.readCtErr ==0) {
			getCtErrRange(data, inOpt, reduceDS_woCtErr, lmin, lmax, &errmin, &errmax);
		} else if (inOpt.readCtErr ==1) {
			if (readCtErrRange(inOpt, &errmin, &errmax)<0) return -1;
		} else {
			errmin = 0.008; errmax = 1.0;
		} 
		//// **** string with CtErrCut
		if (inOpt.EventActivity==0) {
 			sprintf(reduceDS,
							"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && TMath::Abs(Jpsi_Y)>=%.2f && TMath::Abs(Jpsi_Y)<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f && Jpsi_CtErr>=%.3f && Jpsi_CtErr<%.3f",
 		     	  pmin,pmax,ymin,ymax,-lmin,lmax,errmin,errmax);
		} else {
			 	sprintf(reduceDS,
						"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && TMath::Abs(Jpsi_Y)>=%.2f && TMath::Abs(Jpsi_Y)<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f && Jpsi_CtErr>=%.3f && Jpsi_CtErr<%.3f && Ntracks >=%.1f && Ntracks < %.1f && SumET_HFEta4 >=%.2f && SumET_HFEta4 < %.2f",
 	   	    pmin,pmax,ymin,ymax,-lmin,lmax,errmin,errmax,ntrmin,ntrmax,etmin,etmax);
		}
 		sprintf(reduceDSMC,
		     "Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && TMath::Abs(Jpsi_Y)>=%.2f && TMath::Abs(Jpsi_Y)<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f && Jpsi_CtErr>=%.3f && Jpsi_CtErr<%.3f",
 	 	    pmin,pmax,ymin,ymax,-lmin,lmax,errmin,errmax);
 	}else{
		//// **** string without CtErrCut
		if (inOpt.EventActivity==0) {
			sprintf(reduceDS_woCtErr, 
						"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && Jpsi_Y>=%.2f && Jpsi_Y<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f", 
						pmin,pmax,ymin,ymax,-lmin,lmax);
		} else {
			sprintf(reduceDS_woCtErr, 
						"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && Jpsi_Y>=%.2f && Jpsi_Y<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f && Ntracks >=%.1f && Ntracks < %.1f && SumET_HFEta4 >=%.2f && SumET_HFEta4 < %.2f", 
						pmin,pmax,ymin,ymax,-lmin,lmax,ntrmin,ntrmax,etmin,etmax);
		}
		sprintf(reduceDSMC_woCtErr, 
					"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && Jpsi_Y>=%.2f && Jpsi_Y<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f", 
					pmin,pmax,ymin,ymax,-lmin,lmax);
		//// **** check ctErr Range
		if (inOpt.readCtErr ==0) {
			getCtErrRange(data, inOpt, reduceDS_woCtErr, lmin, lmax, &errmin, &errmax);
		} else if (inOpt.readCtErr ==1) {
			if (readCtErrRange(inOpt, &errmin, &errmax)<0) return -1;
		} else {
			errmin = 0.008; errmax = 1.0;
		} 
		//// **** string with CtErrCut
		if (inOpt.EventActivity==0) {
 			sprintf(reduceDS,
						"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && Jpsi_Y>=%.2f && Jpsi_Y<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f && Jpsi_CtErr>=%.3f && Jpsi_CtErr<%.3f",
 	     	  pmin,pmax,ymin,ymax,-lmin,lmax,errmin,errmax);
		} else {
		 	sprintf(reduceDS,
						"Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && Jpsi_Y>=%.2f && Jpsi_Y<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f && Jpsi_CtErr>=%.3f && Jpsi_CtErr<%.3f && Ntracks >=%.1f && Ntracks < %.1f && SumET_HFEta4 >=%.2f && SumET_HFEta4 < %.2f",
 	   	    pmin,pmax,ymin,ymax,-lmin,lmax,errmin,errmax,ntrmin,ntrmax,etmin,etmax);
		}
 	 sprintf(reduceDSMC,
		     "Jpsi_Pt>=%.2f && Jpsi_Pt<%.2f && Jpsi_Y>=%.2f && Jpsi_Y<%.2f && Jpsi_Ct>=%.2f && Jpsi_Ct<%.2f && Jpsi_CtErr>=%.3f && Jpsi_CtErr<%.3f",
 	 	    pmin,pmax,ymin,ymax,-lmin,lmax,errmin,errmax);
	} 
  cout << "reduceDS: " << reduceDS << endl;
  cout << "reduceDSMC: " << reduceDSMC << endl;
	
	RooDataSet *redPRMC, *redNPMC, *redData, *redData_woCtErr; // with CtErr cut  for for isPEE==1
  redPRMC = (RooDataSet*)dataPRMC->reduce(reduceDSMC);
  redNPMC = (RooDataSet*)dataNPMC->reduce(reduceDSMC);
  redData = (RooDataSet*)data->reduce(reduceDS);
  redData_woCtErr = (RooDataSet*)data->reduce(reduceDS_woCtErr);

  ws->import(*redPRMC);
  ws->import(*redNPMC);
  ws->import(*redData);
  
	setWSRange(ws, lmin, lmax, errmin, errmax);
	
	/////////////////////////////////// Binning /////////////////////////////////// 
  string titlestr;
  TCanvas c0;
  ws->var("Jpsi_Mass")->SetTitle("m_{#mu#mu}");
  ws->var("Jpsi_Ct")->SetTitle("#font[12]{l}_{J/#psi}");
 
 	RooBinning rbmass(2.6,3.5);
	rbmass.addUniform(10, 2.6,2.8);
	rbmass.addUniform(40, 2.8,3.2);
	rbmass.addUniform(15, 3.2,3.5);
	//ws->var("Jpsi_Mass")->setBins(rbmass);
	ws->var("Jpsi_Mass")->setBins(45);
	//ws->var("Jpsi_Mass")->setBins(90);
	RooBinning rbtrue(-0.1,4.0);
  rbtrue.addUniform(5,-0.1,0.0);
  rbtrue.addUniform(100,0.0,0.5);
  rbtrue.addUniform(15,0.5,1.0);
  rbtrue.addUniform(20,1.0,2.5);
  rbtrue.addUniform(5,2.5,4.0);
	if (inOpt.sysString=="sys04_01") {ws->var("Jpsi_CtTrue")->setBinning(rbtrue);} 
	else { ws->var("Jpsi_CtTrue")->setBins(1000); }
  
	RooBinning rbct = setCtBinning(lmin,lmax);
  ws->var("Jpsi_Ct")->setBinning(rbct);
//	if (!inOpt.ptrange.compare("2.0-3.0")){ws->var("Jpsi_Ct")->setBins(100);}  // TEST

  ws->var("Jpsi_CtErr")->setBins(25);
	
	//////////////////////////////////////////////////////////////////////////////////////////////

  //RooDataSet* redNPMC = (RooDataSet*) redNPMCtmp->reduce(RooArgSet(*(ws->var("Jpsi_CtTrue"))));
  cout << " *** DATA :: N events to fit: " << redData->sumEntries() << endl;
  cout << " *** PRMC :: N events to fit: " << redPRMC->sumEntries() << endl;
  cout << " *** NPMC :: N events to fit: " << redNPMC->sumEntries() << endl;

  RooDataSet *redDataSIG = (RooDataSet*)redData->reduce("Jpsi_Mass > 2.9 && Jpsi_Mass < 3.3");
	RooDataSet* redDataSB = (RooDataSet*) redData->reduce("Jpsi_Mass<2.9 || Jpsi_Mass>3.3");
  RooDataSet* redDataSBL = (RooDataSet*) redData->reduce("Jpsi_Mass<2.9");
  RooDataSet* redDataSBR = (RooDataSet*) redData->reduce("Jpsi_Mass>3.3");

  drawSBRightLeft(ws, redDataSBL, redDataSBR, inOpt);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////// Define PDFs with params (mass and ctau) ///////////////////////////// 
  //// *** J/psi mass parameterization
  defineMassBkg(ws);
  defineMassSig(ws, inOpt);
  //// *** J/psi ctau parameterization
  defineCtPRRes(ws, inOpt);              // R(l) : resolution function
  defineCtBkg(ws, inOpt);                // theta(l') convolution R(l')
  titlestr = inOpt.dirName + "_rap" + inOpt.yrange + "_pT" + inOpt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange;
  defineCtNP(ws,redNPMC,titlestr,inOpt); // F_B(l) : X_mc(l')

  struct PARAM {
  	double fracG1; double fracG1Err;
    double meanSig;  double meanSigErr;
    double sigmaSig1; double sigmaSig1Err; // G1
    double sigmaSig2; double sigmaSig2Err; // CB
    double alpha;     double alphaErr;
    double enne;      double enneErr;
	};
	
	char funct[100];
	double initBkg = redDataSB->sumEntries()*9.0/5.0;
	double initSig = redData->sumEntries() - initBkg;
	initBkg = initBkg+10000;
	////// *** mSigFunct + mBkgFunct
	sprintf(funct,"SUM::MassPDF(NSig[%f,1.0,5000000.0]*%s,NBkg[%f,1.0,50000000.0]*%s)",initSig,initBkg,inOpt.mSigFunct.c_str(),inOpt.mBkgFunct.c_str());

	//////////////////////////// Special parameter setting for each bin //////////////////////////// 
  //// sys03 : Remove fracRes, meanPRResW, sigmaPRResW!!!
  //// sys04 : Remove sigmaNPTrue!!!
	ws->var("enne")->setVal(2.1);
  ws->var("enne")->setConstant(kTRUE);
	if (inOpt.sysString == "sys01_01") {
		ws->var("alpha")->setVal(1.0);
	 	ws->var("alpha")->setConstant(kTRUE);
	} else if (inOpt.sysString == "sys01_02") {
		ws->var("alpha")->setVal(2.0);
	 	ws->var("alpha")->setConstant(kTRUE);
	} else if (inOpt.sysString == "sys01_03") {
		ws->var("alpha")->setVal(3.0);
	 	ws->var("alpha")->setConstant(kTRUE);
	} else if (inOpt.sysString == "sys01_04") {
		ws->var("enne")->setVal(1.6);
  	ws->var("enne")->setConstant(kTRUE);
	 	ws->var("alpha")->setConstant(kFALSE);
	} else if (inOpt.sysString == "sys01_05") {
		ws->var("enne")->setVal(2.6);
  	ws->var("enne")->setConstant(kTRUE);
	 	ws->var("alpha")->setConstant(kFALSE);
	} else {
	 	ws->var("alpha")->setConstant(kFALSE);
	}

			if (inOpt.EventActivity == 0) { // no multiplicity
				if (inOpt.isPA == 3) {
					if (!inOpt.yrange.compare("0.43-1.03")) {
						if (!inOpt.ptrange.compare("7.5-8.5")){
							ws->var("sigmaSig1")->setRange(0.015,0.050);
							ws->var("sigmaSig1")->setVal(0.040);
							ws->var("sigmaSig2")->setRange(0.015,0.050);
							ws->var("sigmaSig2")->setVal(0.040);
						}
					}
				} //isPA==3
			} // EventActivity==0

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////// [[1]] fitting mass ///////////////////////////////////  
	/////////////////////////////////////////////////////////////////////////////////////////
	sprintf(funct,"SUM::MassPDF(NSig[%f,1.0,5000000.0]*%s,NBkg[%f,1.0,50000000.0]*%s)",initSig,initBkg,inOpt.mSigFunct.c_str(),inOpt.mBkgFunct.c_str());
	ws->factory(funct);
	RooFitResult *fitMass;
	fitMass = ws->pdf("MassPDF")->fitTo(*redData,Extended(1),Minos(0),Save(1),SumW2Error(kTRUE),NumCPU(8));
	fitMass->Print("v");
  ws->var("alpha")->setConstant(kTRUE);
  ws->var("enne")->setConstant(kTRUE);
  ws->var("fracG1")->setConstant(kTRUE);
  ws->var("sigmaSig1")->setConstant(kTRUE);
  ws->var("sigmaSig2")->setConstant(kTRUE);
  ws->var("meanSig")->setConstant(kTRUE);
  ws->var("coefExp")->setConstant(kTRUE);
  ws->var("coefPol")->setConstant(kTRUE);
  ws->var("NSig")->setConstant(kTRUE);
  ws->var("NBkg")->setConstant(kTRUE);

  //// **** sigWidth : combined width of mass peak CB + Gaus
  double tmpFracG = 0, tmpFracGErr = 0;
  double tmpSigG = 0, tmpSigGErr = 0;
  double tmpSigCB = 0,   tmpSigCBErr = 0;
  if (inOpt.mSigFunct == "G1CB1Sig") {
    tmpFracG = ws->var("fracG1")->getVal();
    tmpSigG = ws->var("sigmaSig1")->getVal();
    tmpSigCB = ws->var("sigmaSig2")->getVal();
    tmpFracGErr = ws->var("fracG1")->getError();
    tmpSigGErr = ws->var("sigmaSig1")->getError();
    tmpSigCBErr = ws->var("sigmaSig2")->getError();
		inOpt.combinedWidth = sqrt( (1-tmpFracG)*pow(tmpSigCB,2) + tmpFracG*pow(tmpSigG,2) );
    inOpt.combinedWidthErr = (0.5/inOpt.combinedWidth) *
    sqrt ( pow(tmpFracG*tmpSigG*tmpSigGErr,2) +
           pow((1-tmpFracG)*tmpSigCB*tmpSigCBErr,2) +
           pow(0.5*tmpFracGErr*( pow(tmpSigG,2)-pow(tmpSigCB,2) ),2) );
  }

	inOpt.PcombinedWidth = inOpt.combinedWidth*1000;
  inOpt.PcombinedWidthErr = inOpt.combinedWidthErr*1000;
  if(inOpt.PcombinedWidthErr < 1) inOpt.PcombinedWidthErr = 1;
  
	//// *** Draw inclusive mass plots
	drawInclusiveMassPlots(ws, redData, fitMass, inOpt);
  
	Double_t NSig_fin = ws->var("NSig")->getVal();
  Double_t ErrNSig_fin = ws->var("NSig")->getError();
  Double_t NBkg_fin = ws->var("NBkg")->getVal();
  Double_t ErrNBkg_fin = ws->var("NBkg")->getError();

	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////// [[2]] get ctau error PDF for PEE method /////////////////////////  
	/////////////////////////////////////////////////////////////////////////////////////////
  
 	//// *** scaleF to scale down ct err dist in 2.9-3.3 GeV/c2
  float bc;
  if (!inOpt.mBkgFunct.compare("expBkg")){  bc = ws->var("coefExp")->getVal(); }
  else if (!inOpt.mBkgFunct.compare("polBkg")) { bc = ws->var("coefPol")->getVal(); }
  else {cout << "select correct mBkgFunct" << endl; return 0;}
	float scaleF = (exp(2.9*bc)-exp(3.3*bc))/(exp(2.6*bc)-exp(2.9*bc)+exp(3.3*bc)-exp(3.5*bc));
  
 	//// **** RooDataSet(unbinned) to RooDataHist (binned) 
  RooDataHist *binDataCtErr = new RooDataHist("binDataCtErr","binDataCtErr",RooArgSet(*(ws->var("Jpsi_CtErr"))),*redData);
  RooDataHist *binDataCtErrSB = new RooDataHist("binDataCtErrSB","Data ct error distribution for bkg",RooArgSet(*(ws->var("Jpsi_CtErr"))),*redDataSB);
  RooDataHist *binDataCtErrSIG = new RooDataHist("binDataCtErrSIG","Data ct error distribution for sig",RooArgSet(*(ws->var("Jpsi_CtErr"))),*redDataSIG);

  //// **** (tbinSubtractedSIG) = (binDataCtErrSIG) - scaleF*(binDataCtErrSB)
	RooDataHist *binSubtractedSIG, *binScaledBKG;
	binSubtractedSIG = new RooDataHist("binSubtractedSIG","Subtracted data",RooArgSet(*(ws->var("Jpsi_CtErr")))); 
  binScaledBKG = subtractSidebands(ws,binSubtractedSIG,binDataCtErrSIG,binDataCtErrSB,scaleF,"Jpsi_CtErr");
   
  RooHistPdf errPdfSig("errPdfSig","Error PDF signal",RooArgSet(*(ws->var("Jpsi_CtErr"))),*binSubtractedSIG);  ws->import(errPdfSig);
//  RooHistPdf errPdfBkgRaw("errPdfBkg","Error PDF bkg before scaling",RooArgSet(*(ws->var("Jpsi_CtErr"))),*binDataCtErrSB);  ws->import(errPdfBkg);
  RooHistPdf errPdfBkg("errPdfBkg","Error PDF bkg scaled",RooArgSet(*(ws->var("Jpsi_CtErr"))),*binScaledBKG);  ws->import(errPdfBkg);

  //// **** Draw CtErr PDF
  drawCtauErrPdf(ws, binDataCtErrSB, binDataCtErrSIG, binSubtractedSIG, binScaledBKG, inOpt);
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////// preparing total 2D PDFs with PEE method ///////////////////////// 
	//////////////////////////     e.g.) AAA_PEE : AAA x errPdf        /////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	//// *** Conditional to select Ct and Mass only) :: for step[[3]] and step[[4]]
  RooProdPdf CtPR_PEE("CtPR_PEE","CtPDF with PEE", *(ws->pdf("errPdfSig")),
                      Conditional(*(ws->pdf("CtPRRes")), RooArgList(*(ws->var("Jpsi_Ct"))))
                      );  ws->import(CtPR_PEE);
	RooProdPdf CtBkgTot_PEE("CtBkgTot_PEE","PDF with PEE", *(ws->pdf("errPdfBkg")),
                              Conditional(*(ws->pdf("CtBkgTot")),RooArgList(*(ws->var("Jpsi_Ct"))))
                             );  ws->import(CtBkgTot_PEE);  
	
	//// *** 2D PDF (mass x ctau) :: for step[[5]]
    sprintf(funct,"PROD::MassCtPR(%s,CtPRRes)",inOpt.mSigFunct.c_str()); ws->factory(funct);
    sprintf(funct,"PROD::MassCtNP(%s,CtNPTot)",inOpt.mSigFunct.c_str()); ws->factory(funct);
    sprintf(funct,"PROD::MassCtBkg(%s,CtBkgTot)",inOpt.mBkgFunct.c_str());  ws->factory(funct);

	RooProdPdf MassCtPR_PEE("MassCtPR_PEE","PDF with PEE", *(ws->pdf("errPdfSig")),
                           Conditional( *(ws->pdf("MassCtPR")), RooArgList(*(ws->var("Jpsi_Ct")),*(ws->var("Jpsi_Mass"))) )
                           );  ws->import(MassCtPR_PEE);
	RooProdPdf MassCtNP_PEE("MassCtNP_PEE","PDF with PEE", *(ws->pdf("errPdfSig")),
                           Conditional( *(ws->pdf("MassCtNP")), RooArgList(*(ws->var("Jpsi_Ct")),*(ws->var("Jpsi_Mass"))))
                           );  ws->import(MassCtNP_PEE);    
  RooProdPdf MassCtBkg_PEE("MassCtBkg_PEE","PDF with PEE", *(ws->pdf("errPdfBkg")),
                         Conditional( *(ws->pdf("MassCtBkg")), RooArgList(*(ws->var("Jpsi_Ct")),*(ws->var("Jpsi_Mass"))) )
                         );  ws->import(MassCtBkg_PEE);
   
	//// **** total PDF  = promptPDF + non-promptPDF + bkgPDF 
	RooFormulaVar fracBkg("fracBkg","@0/(@0+@1)",RooArgList(*(ws->var("NBkg")),*(ws->var("NSig"))));    ws->import(fracBkg);
  ws->factory("RSUM::totPDF_PEE(fracBkg*MassCtBkg_PEE,Bfrac[0.25,0.0,1.]*MassCtNP_PEE,MassCtPR_PEE)");

  
	/////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////// [[3]] prompt MC ctau function (resolution) ////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
  
	RooFitResult *fitCt_PRMC = ws->pdf("CtPR_PEE")->fitTo(*redPRMC,Range("promptMCfit"),SumW2Error(kTRUE),ConditionalObservables(RooArgSet(*(ws->var("Jpsi_CtErr")))),Save(1),NumCPU(8));
  fitCt_PRMC->Print("v");
	ws->var("sigmaPRResW")->setConstant(kTRUE);
  ws->var("meanPRResW")->setConstant(kTRUE);
	ws->var("meanPRResN")->setConstant(kTRUE);
  ws->var("meanPRResN")->setConstant(kTRUE);

  //// *** Check promptMCfit is fine with per event error fit //// CtWeighted = l/(l_err)
  
	RooRealVar* CtWeighted = new RooRealVar("CtWeighted","#font[12]{l}_{J/#psi} / #sigma( #font[12]{l}_{J/#psi} )",-5.,5.); ws->import(*CtWeighted);
  const RooArgSet* thisRow = (RooArgSet*)redPRMC->get(0); // prompt MC rooData 
  RooArgSet* newRow = new RooArgSet(*CtWeighted);
  RooDataSet* tempSet = new RooDataSet("tempSet","new data set with CtWeighted",*newRow);

  for (Int_t iSamp = 0; iSamp < redPRMC->numEntries(); iSamp++) {
    thisRow = (RooArgSet*)redPRMC->get(iSamp);
    RooRealVar* myct = (RooRealVar*)thisRow->find("Jpsi_Ct");
    RooRealVar* mycterr = (RooRealVar*)thisRow->find("Jpsi_CtErr");
    CtWeighted->setVal(myct->getVal()/mycterr->getVal());
    RooArgSet* tempRow = new RooArgSet(*CtWeighted);
    tempSet->add(*tempRow);
  }

  ws->factory("Gaussian::tmpGW_PRRes(CtWeighted,meanPRResW,sigmaPRResW)");
  ws->factory("Gaussian::tmpGN_PRRes(CtWeighted,meanPRResN,sigmaPRResN)");
  ws->factory("SUM::tmpCtPRRes(fracRes*tmpGW_PRRes,tmpGN_PRRes)");

  RooPlot* tempFramePR = ws->var("CtWeighted")->frame();
  tempSet->plotOn(tempFramePR,DataError(RooAbsData::SumW2));  // data points
  ws->pdf("tmpCtPRRes")->plotOn(tempFramePR,LineColor(kGreen+1),Normalization(tempSet->sumEntries(),RooAbsReal::NumEvent)); // pdf line

  drawCtauResolPlots(ws, true, tempFramePR, inOpt); // fit to PRMC
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////// [[4]] background ctau distributions /////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
  
	cout << " *** DATA :: N events to fit on SIDEBANDS : " << redDataSB->sumEntries() << endl;
	RooFitResult* fitCt_Bkg = ws->pdf("CtBkgTot_PEE")->fitTo(*redDataSB,SumW2Error(kTRUE),Minos(0),NumCPU(8),Save(1),ConditionalObservables(RooArgSet(*(ws->var("Jpsi_CtErr")))),Optimize(0));
  fitCt_Bkg->Print("v");
  ws->var("fracCtBkg1")->setConstant(kTRUE);
  ws->var("fracCtBkg2")->setConstant(kTRUE);
  ws->var("fracCtBkg3")->setConstant(kTRUE);
  ws->var("lambdap")->setConstant(kTRUE);
  ws->var("lambdam")->setConstant(kTRUE);
  ws->var("lambdasym")->setConstant(kTRUE);
	
	drawCtauSBPlots(ws, redDataSB, binDataCtErrSB, fitCt_Bkg, lmin, lmax, inOpt, &UnNormChi2_side, &nFitParam_side, &nFullBinsPull_side, &Dof_side, &Chi2_side);
	
	////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// [[5]] final 2D fits ///////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////

		//// *** release some parameters
		if (inOpt.sysString!="sys04_01") {ws->var("coefExpNPTrue") ->setConstant(kFALSE);}
		ws->var("fracRes")->setConstant(kFALSE);
		ws->var("meanPRResW")->setConstant(kFALSE);
		ws->var("meanPRResN")->setConstant(kFALSE);
		ws->var("sigmaPRResW") ->setConstant(kTRUE);
		ws->var("sigmaPRResN") ->setConstant(kFALSE);
		if (inOpt.sysString=="sys03_01") { ws->var("sigmaPRResW") ->setConstant(kFALSE); } //release all sigma
		else if (inOpt.sysString=="sys03_02") { ws->var("sigmaPRResN") ->setConstant(kTRUE); } //fix all sigma

/*
		//// **** Special fixing for Bfrac
		if ((!inOpt.yrange.compare("-2.4--1.93")) ){
			if (!inOpt.ptrange.compare("5.0-6.5")){
				ws->var("Bfrac")->setRange(0.138,0.158);
				ws->var("Bfrac")->setVal(0.14);
			}
		}
*/

  //// *** Get NSig, NBkg, Bfraction and their errors
  Double_t NSigPR_fin, ErrNSigPR_fin;
  Double_t NSigNP_fin, ErrNSigNP_fin;
  Double_t Bfrac_fin, ErrBfrac_fin;
  int nFitPar;
  Double_t theNLL;
  double resol, Errresol;
  RooFitResult *fit2D;
  
	cout << "" << endl;
	cout << " -*-*-*-*-*-*-*-*-*-*-*- total fitting -*-*-*-*-*-*-*-*-*-*-*-  " << endl;
  fit2D = ws->pdf("totPDF_PEE")->fitTo(*redData,Minos(0),Save(1),SumW2Error(kTRUE),NumCPU(8),ConditionalObservables(RooArgSet(*(ws->var("Jpsi_CtErr")))));
  fit2D->Print("v");
  
	nFitPar = fit2D->floatParsFinal().getSize(); //floating param.
  theNLL = fit2D->minNll(); 
  Bfrac_fin = ws->var("Bfrac")->getVal();
  ErrBfrac_fin = ws->var("Bfrac")->getError();
  NSigNP_fin = NSig_fin * Bfrac_fin;
  NSigPR_fin = NSig_fin * (1-Bfrac_fin);
  ErrNSigNP_fin = NSigNP_fin * sqrt( pow(ErrNSig_fin/NSig_fin,2)+pow(ErrBfrac_fin/Bfrac_fin,2) );
  ErrNSigPR_fin = NSigPR_fin * sqrt ( pow(ErrNSig_fin/NSig_fin,2)+pow(ErrBfrac_fin/(1.0-Bfrac_fin),2) );

  //// **** Resolution : combined width of 2G (PRRes)
  const double fracR = ws->var("fracRes")->getVal();
  const double fracRErr = ws->var("fracRes")->getError();
  const double sigN = ws->var("sigmaPRResN")->getVal();
  const double sigNErr = ws->var("sigmaPRResN")->getError();
  const double sigW = ws->var("sigmaPRResW")->getVal();
  const double sigWErr = ws->var("sigmaPRResW")->getError();
  resol = sqrt( fracR*pow(sigW,2) + (1-fracR)*pow(sigN,2) );
  Errresol = (0.5/resol) *
                sqrt( pow(sigW*fracR*sigWErr,2) +
                      pow(sigN*(1-fracR)*sigNErr,2) +
                      pow(0.5*(pow(sigW,2)-pow(sigN,2))*fracRErr,2) );

  //// **** To check values of fit parameters
  cout << endl << "J/psi yields:" << endl;
  cout << "NSig :       Fit :"  << NSig_fin << " +/- " << ErrNSig_fin << endl;
  cout << "PROMPT :     Fit : " << NSigPR_fin << " +/- " << ErrNSigPR_fin << endl;
  cout << "NON-PROMPT : Fit : " << NSigNP_fin << " +/- " << ErrNSigNP_fin << endl;
  cout << "Bfraction : Fit : " << Bfrac_fin << " +/- " << ErrBfrac_fin << endl;
  cout << "Resolution : Fit : " << resol << " +/- " << Errresol << endl;

	////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////// output text file /////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	titlestr = inOpt.dirName + "_rap" + inOpt.yrange + "_pT" + inOpt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + ".txt";

  int cutEntry = redData->sumEntries();
  int noCutEntry = redData_woCtErr->sumEntries();

  ofstream outputFile(titlestr.c_str());
  if (!outputFile.good()) {cout << "Fail to open result file." << endl; return 1;}
  cout << "" << endl;
	cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
	cout << " -*-*-*-*-*-*-*-*-*-*-*- Write text file -*-*-*-*-*-*-*-*-*-*-*-  " << endl;
	cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
	outputFile
  << "ctauErrMin "   << errmin                            << " 0 " << "\n"
  << "ctauErrMax "   << errmax                            << " 0 " << "\n"
  << "NoCutEntry "   << noCutEntry                        << " 0 " << "\n"
  << "CutEntry "     << cutEntry                          << " 0 " << "\n"
  << "NSig "         << NSig_fin                          << " " << ErrNSig_fin << "\n"
  << "NBkg "         << NBkg_fin                          << " " << ErrNBkg_fin << "\n"
  << "coefExp "      << ws->var("coefExp")->getVal()      << " " << ws->var("coefExp")->getError() << "\n"
  << "coefPol "     << ws->var("coefPol")->getVal()     << " " << ws->var("coefPol")->getError() << "\n"
  << "fracG1 "    << ws->var("fracG1")->getVal()    << " " << ws->var("fracG1")->getError() << "\n"
  << "meanSig "     << ws->var("meanSig")->getVal()     << " " << ws->var("meanSig")->getError() << "\n"
  << "sigmaSig1 "    << ws->var("sigmaSig1")->getVal()    << " " << ws->var("sigmaSig1")->getError() << "\n"
  << "sigmaSig2 "    << ws->var("sigmaSig2")->getVal()    << " " << ws->var("sigmaSig2")->getError()<< "\n"
  << "alpha "        << ws->var("alpha")->getVal()        << " " << ws->var("alpha")->getError() << "\n"
  << "enne "        << ws->var("enne")->getVal()        << " " << ws->var("enne")->getError() << "\n"
  << "sigWidth "     << inOpt.combinedWidth               << " " << inOpt.combinedWidthErr << "\n";
	if (inOpt.sysString=="sys04_01") {
  	outputFile
		<< "sigmaNPTrue "  << "0"  << " " << "0" <<  "\n"
  	<< "coefExpNPTrue " << "0" << " " << "0" <<  "\n";
	}else {
  	outputFile
		<< "sigmaNPTrue "  << ws->var("sigmaNPTrue")->getVal()  << " " << ws->var("sigmaNPTrue")->getError() <<  "\n"
  	<< "coefExpNPTrue " << ws->var("coefExpNPTrue")->getVal() << " " << ws->var("coefExpNPTrue")->getError() <<  "\n";
	}
  outputFile
  << "fracRes "      << ws->var("fracRes")->getVal()      << " " << ws->var("fracRes")->getError() <<  "\n"
  << "meanPRResW "  << ws->var("meanPRResW")->getVal()  << " " << ws->var("meanPRResW")->getError() << "\n"
  << "meanPRResN "  << ws->var("meanPRResN")->getVal()  << " " << ws->var("meanPRResN")->getError() << "\n"
  << "sigmaPRResW " << ws->var("sigmaPRResW")->getVal() << " " << ws->var("sigmaPRResW")->getError() <<  "\n"
  << "sigmaPRResN " << ws->var("sigmaPRResN")->getVal() << " " << ws->var("sigmaPRResN")->getError() << "\n"
  << "fracCtBkg1 "          << ws->var("fracCtBkg1")->getVal()          << " " << ws->var("fracCtBkg1")->getError() << "\n"
  << "fracCtBkg2 "      << ws->var("fracCtBkg2")->getVal()      << " " << ws->var("fracCtBkg2")->getError() << "\n"
  << "fracCtBkg3 "    << ws->var("fracCtBkg3")->getVal()    << " " << ws->var("fracCtBkg3")->getError() << "\n"
  << "lambdam "      << ws->var("lambdam")->getVal()      << " " << ws->var("lambdam")->getError() << "\n"
  << "lambdap "      << ws->var("lambdap")->getVal()      << " " << ws->var("lambdap")->getError() << "\n"
  << "lambdasym "    << ws->var("lambdasym")->getVal()    << " " << ws->var("lambdasym")->getError() << "\n"
  << "NLL "          << theNLL                            << endl
  << "Resolution "   << resol				                      << " " << Errresol << endl
  << "PROMPT "       << NSigPR_fin                        << " " << ErrNSigPR_fin << endl
  << "NON-PROMPT "   << NSigNP_fin                        << " " << ErrNSigNP_fin << endl
  << "Bfraction "    << Bfrac_fin                         << " " << ErrBfrac_fin << endl;
 
	////////////////////////////////////////////////////////////////////////////////////////////
	cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
	cout << " -*-*-*-*-*-*-*-*-*-*-*- Draw final plots  -*-*-*-*-*-*-*-*-*-*-*-  " << endl;
	cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
  
	///////////////////////////////// ****  1) Final mass plots **** /////////////////////////////////
	drawFinalMass(ws, redData, NSigNP_fin, NBkg_fin, fitMass, inOpt,&UnNormChi2_mass, &nFitParam_mass, &nFullBinsPull_mass, &Dof_mass, &Chi2_mass); 
  ///////////////////////////////// ****  2) Final ctau plots **** /////////////////////////////////
  drawFinalCtau(ws, redData, binDataCtErr, NSigNP_fin, NBkg_fin, fit2D, lmin, lmax, inOpt, &UnNormChi2_time, &nFitParam_time, &nFullBinsPull_time, &Dof_time, &Chi2_time);
	//////////////////////////////////////////////////////////////////////////////////////////////////

	//////// **** Check1) final resoltuion function fitted to data instead of PRMC
  const RooArgSet *thisRowD = (RooArgSet*)redData->get(0); 
  RooArgSet *newRowD = new RooArgSet(*CtWeighted);
  RooDataSet *tempSetD = new RooDataSet("tempSetD","new data",*newRowD);
  for (Int_t iSamp = 0; iSamp < redData->numEntries(); iSamp++) {
    thisRowD = (RooArgSet*)redData->get(iSamp);
    RooRealVar *myct = (RooRealVar*)thisRowD->find("Jpsi_Ct");
    RooRealVar *mycterr = (RooRealVar*)thisRowD->find("Jpsi_CtErr");
    CtWeighted->setVal(myct->getVal()/mycterr->getVal());
    RooArgSet* tempRowD = new RooArgSet(*CtWeighted);
    tempSetD->add(*tempRowD);
  }
  ws->factory("Gaussian::tmpGW_PRResD(CtWeighted,meanPRResW,sigmaPRResW)");
  ws->factory("Gaussian::tmpGN_PRResD(CtWeighted,meanPRResN,sigmaPRResN)");
  ws->factory("SUM::tmpCtPRResD(fracRes*tmpGW_PRRes,tmpGN_PRRes)");

  RooPlot* tempFrameD = ws->var("CtWeighted")->frame();
  tempSetD->plotOn(tempFrameD,DataError(RooAbsData::SumW2));
  ws->pdf("tmpCtPRResD")->plotOn(tempFrameD,LineColor(kGreen+1),Normalization(tempSetD->sumEntries(),RooAbsReal::NumEvent));
	// Plot resolution functions
   drawCtauResolPlots(ws, false, tempFrameD, inOpt); //// resolFit (overlaied to Data)
 
	//////// **** Check2) mass-lifetime 2D plots
  drawMassCtau2DPlots(ws, inOpt) ;
	
	///////////////////////////////// fitting quality check  /////////////////////////////////
 	theChi2Prob_mass = TMath::Prob(UnNormChi2_mass, Dof_mass); // get from fitting
 	theChi2Prob_time = TMath::Prob(UnNormChi2_time, Dof_time);
 	theChi2Prob_side = TMath::Prob(UnNormChi2_side, Dof_side);
		outputFile
		<< "UnNormChi2_mass "       	<< UnNormChi2_mass                         << endl
		<< "nFitParam_mass "          << nFitParam_mass                         << endl
		<< "nFullBinsPull_mass "      << nFullBinsPull_mass                         << endl
		<< "Dof_mass "          			<< Dof_mass                         << endl
		<< "Chi2_mass "          			<< Chi2_mass                         << endl
		<< "theChi2Prob_mass "        << theChi2Prob_mass                         << endl
		<< "UnNormChi2_time "       	<< UnNormChi2_time                         << endl
		<< "nFitParam_time "          << nFitParam_time                         << endl
		<< "nFullBinsPull_time "      << nFullBinsPull_time                         << endl
		<< "Dof_time "          			<< Dof_time                         << endl
		<< "Chi2_time "          			<< Chi2_time                         << endl
		<< "theChi2Prob_time "        << theChi2Prob_time                         << endl
		<< "UnNormChi2_side "       	<< UnNormChi2_side                         << endl
		<< "nFitParam_side "          << nFitParam_side                         << endl
		<< "nFullBinsPull_side "      << nFullBinsPull_side                         << endl
		<< "Dof_side "          			<< Dof_side                         << endl
		<< "Chi2_side "          			<< Chi2_side                         << endl
		<< "theChi2Prob_side "        << theChi2Prob_side                         << endl;

	outputFile.close();
  fInPRMC.Close();
  fInNPMC.Close();
  fInData.Close();
  if (inOpt.mcMerge==2) {fInPRMC2->Close(); fInNPMC2->Close();}
  if (inOpt.dataMerge==2 || inOpt.dataMerge==3) {fInData2->Close();}
  if (inOpt.dataMerge==3) {fInData3->Close();}
	
	return 0;

} // end of main function

///////////////////////////////////////////////////////////////////
///////////////// Sub-routines for plotting ///////////////////////
///////////////////////////////////////////////////////////////////
void parseInputArg(int argc, char* argv[], InputOpt &opt) {
	cout << ""<< endl;
	cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
	cout << " -*-*-*-*-*-*-*-*-*- Parsing input argument -*-*-*-*-*-*-*-*-*-  " << endl;
	cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
	opt.dataMerge = 1; //num of data to be merged
	opt.mcMerge = 1; //num of mc to be merged
  opt.doMCWeight = 0;   // not use weighting
  opt.isPA = 0;	//pA (0:pp, 1:Pbp, 2:pPb, 3 :pAMerged)
	opt.EventActivity = 0;
	opt.absoluteY= 0; 
	opt.readCtErr = 0;

  for (int i=1; i<argc; i++) {
    char *tmpargv = argv[i];
    switch (tmpargv[0]) {
      case '-':{
        switch (tmpargv[1]) {
          case 'f':
            opt.dataMerge = atoi(argv[i+1]);
            opt.FileNameData = argv[i+2];
            cout << "   ** Data file 1st: " << opt.FileNameData << endl;
            if (opt.dataMerge == 1) {
              cout << " * One rooData file used: (default for pp, pPb)" << endl;
            } else if (opt.dataMerge == 2) {
							opt.FileNameData2 = argv[i+3]; 
							cout << "   ** Data file 2nd: "<< opt.FileNameData2 << endl;
              cout << " * Two rooData files used: Pbp_v1 + Pbp_v2 (default for Pbp)" << endl;
            } else if (opt.dataMerge == 3) {
							opt.FileNameData2 = argv[i+3]; 
							cout << "   ** Data file 2nd: "<< opt.FileNameData2 << endl;
							opt.FileNameData3 = argv[i+4]; 
							cout << "   ** Data file 3rd: "<< opt.FileNameData3 << endl;
              cout << " * Three rooData files used: Pbp_v1 + Pbp_v2 + pPbFlip (for whole pA)" << endl;
						} else {
							cout << " * WRONG :: Select among dataMerge = 1, 2, or 3 " << endl;
						} 
						break;
          case 'm':
            opt.mcMerge = atoi(argv[i+1]);
            opt.FileNamePRMC = argv[i+2];
            opt.FileNameNPMC = argv[i+3];
            cout << "   ** Prompt MC file: " << opt.FileNamePRMC << endl;
            cout << "   ** Non-prompt MC file: " << opt.FileNameNPMC << endl;
           	if (opt.mcMerge == 1) {
              cout << " * One rooData MC file used: (default for pp, pPb, Pbp)" << endl;
           	} else if (opt.mcMerge == 2) {
            	opt.FileNamePRMC2 = argv[i+4];
            	opt.FileNameNPMC2 = argv[i+5];
            	cout << "   ** Prompt MC file 2nd: " << opt.FileNamePRMC2 << endl;
            	cout << "   ** Non-prompt MC file 2nd: " << opt.FileNameNPMC2 << endl;
              cout << " * Two rooData MC files used: Pbp + pPbFlip (for whole pA)" << endl;
						} else {
							cout << " * WRONG :: Select among mcMerge = 1 or 2" << endl;
            }
            break;
          case 'w':
            opt.doMCWeight = atoi(argv[i+1]);
            cout << " * (Zvtx) Weighting MC: " << opt.doMCWeight << endl;
            break;
          case 'c':
            opt.isPA = atoi(argv[i+1]);
            cout << " * beam info (0:pp, 1:Pbp, 2:pPb, 3:pAMerged): " << opt.isPA << endl;
						opt.EventActivity = atoi(argv[i+2]);
						cout << " * event-activity (0:nothing, 1:Ntrk, 2:ET) : " << opt.EventActivity << endl;
            break;
          case 'd':
            opt.dirName = argv[i+1];
            cout << " * Directory Name: " << opt.dirName << endl;
            break;
          case 's':
						opt.sysString = argv[i+1];
						opt.mSigFunct = "G1CB1Sig";
            if ( opt.sysString == "sys02_01") {opt.mBkgFunct = "polBkg";}
						else {opt.mBkgFunct = "expBkg";}
            cout << " * systematic String: " << opt.sysString << endl;
            cout << "  ** Mass signal: " << opt.mSigFunct << endl;
            cout << "  ** Mass background: " << opt.mBkgFunct << endl;
            break;
          case 'p':
            opt.ptrange = argv[i+1];
            cout << "  ** p_{T} range: " << opt.ptrange << " [GeV]" << endl;
            break;
          case 'y':
            opt.absoluteY = atoi(argv[i+1]);
						opt.yrange = argv[i+2];
						cout << "  ** Use absolute Y: " << opt.absoluteY << endl;
            cout << "  ** y_{lab} range: " << opt.yrange << endl;
            break;
					case 'l':
            opt.lrange = argv[i+1];
            cout << "  ** l_{J/#psi} range: " << opt.lrange << " [mm]" << endl; 
            break;
					case 'n':
						opt.ntrrange = argv[i+1];
						cout << "  ** Ntracks range : " << opt.ntrrange << endl;
            break;
          case 'h':
						opt.etrange = argv[i+1];
						cout << "  ** E_{T}^{HF} range : " << opt.etrange << " [GeV]" << endl;
            break;
          case 'x':
						opt.readCtErr = atoi(argv[i+1]);
						cout << " * ctau error range read from text or not: " << opt.readCtErr << endl;
						opt.ctErrFile = argv[i+2];
						cout << "  ** ctau error range input file: " << opt.ctErrFile << endl;
            break;
        }
      }
    }
  }// End check options

	cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
	cout << ""<< endl;
} 

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void getOptRange(string &ran, float *min, float *max) {
  if (sscanf(ran.c_str(), "%f-%f", min, max) == 0) {
    cout << ran.c_str() << ": not valid!" << endl;
    assert(0);
  }
  return ;
}

void formTitle(InputOpt &opt) {
	//not used
	if (opt.isPA == 0) {
    sprintf(opt.beamEn,"pp  #sqrt{s} = 5.02 TeV");
    sprintf(opt.lumi,"L_{int} = 26.3 pb^{-1}");
  } else if (opt.isPA == 1) {
    sprintf(opt.beamEn,"pPb  #sqrt{s_{NN}} = 5.02 TeV");
    sprintf(opt.lumi,"L_{int} = 21 nb^{-1}");
  } else if (opt.isPA == 2) {
		sprintf(opt.beamEn,"pPb  #sqrt{s_{NN}} = 5.02 TeV");
		sprintf(opt.lumi,"L_{int} = 14 nb^{-1}");
  } else if (opt.isPA == 3) {
		sprintf(opt.beamEn,"pPb  #sqrt{s_{NN}} = 5.02 TeV");
		sprintf(opt.lumi,"L_{int} = 34.6 nb^{-1}");
	} else {
		sprintf(opt.beamEn,"PbPb  #sqrt{s_{NN}} = 5.02 TeV");
		sprintf(opt.lumi,"L_{int} = 346 #mub^{-1}");
	}
}

void formRapidity(InputOpt &opt, float ymin, float ymax) {
  double inteMin, inteMax;
  double fracMin = modf(ymin,&inteMin);
  double fracMax = modf(ymax,&inteMax);
  if (opt.absoluteY==1) {
		if (ymin==0 && fracMax!=0) sprintf(opt.rapString,"|y_{lab}| < %.2f",ymax);
		else if (fracMin==0 && fracMax!=0) sprintf(opt.rapString,"%.0f < |y_{lab}| < %.2f",ymin,ymax);
	  else if (fracMin!=0 && fracMax==0) sprintf(opt.rapString,"%.2f < |y_{lab}| < %.0f",ymin,ymax);
	  else if (fracMin==0 && fracMax==0) sprintf(opt.rapString,"%.0f < |y_{lab}| < %.0f",ymin,ymax);
	  else sprintf(opt.rapString,"%.2f < |y_{lab}| < %.2f",ymin,ymax);
	}else {
		if (fracMin==0 && fracMax!=0) sprintf(opt.rapString,"%.0f < y_{lab} < %.2f",ymin,ymax);
	  else if (fracMin!=0 && fracMax==0) sprintf(opt.rapString,"%.2f < y_{lab} < %.0f",ymin,ymax);
	  else if (fracMin==0 && fracMax==0) sprintf(opt.rapString,"%.0f < y_{lab} < %.0f",ymin,ymax);
	  else sprintf(opt.rapString,"%.2f < y_{lab} < %.2f",ymin,ymax);
	}
}

void formPt(InputOpt &opt, float pmin, float pmax) {
  double pminD, pmaxD, pminF, pmaxF;
  pminF = modf(pmin,&pminD);
  pmaxF = modf(pmax,&pmaxD);

  if (pmin == 0) { 
    if (pmaxF == 0) sprintf(opt.ptString,"p_{T} < %.0f GeV/c",pmax);
    else  sprintf(opt.ptString,"p_{T} < %.1f GeV/c",pmax);
  } else {
    if (pminF == 0 && pmaxF == 0) sprintf(opt.ptString,"%.0f < p_{T} < %.0f GeV/c",pmin,pmax);
    else if (pminF == 0 && !pmaxF == 0) sprintf(opt.ptString,"%.0f < p_{T} < %.1f GeV/c",pmin,pmax);
    else if (!pminF == 0 && pmaxF == 0) sprintf(opt.ptString,"%.1f < p_{T} < %.0f GeV/c",pmin,pmax);
    else sprintf(opt.ptString,"%.1f < p_{T} < %.1f GeV/c",pmin,pmax);
  }
}

void formEt(InputOpt &opt, float etmin, float etmax) {
  double etminD, etmaxD, etminF, etmaxF;
  etminF = modf(etmin,&etminD);
  etmaxF = modf(etmax,&etmaxD);

  if (etmin == 0) { 
    if (etmaxF == 0) sprintf(opt.etString,"#Sigma E_{T}^{HF |#eta>4|} < %.0f GeV",etmax);
    else  sprintf(opt.etString,"#Sigma E_{T}^{HF |#eta>4|} < %.1f GeV",etmax);
  } else {
    if (etminF == 0 && etmaxF == 0) sprintf(opt.etString,"%.0f < #Sigma E_{T}^{HF |#eta>4|} < %.0f GeV",etmin,etmax);
    else if (etminF == 0 && !etmaxF == 0) sprintf(opt.etString,"%.0f < #Sigma E_{T}^{HF |#eta>4|} < %.1f GeV",etmin,etmax);
    else if (!etminF == 0 && etmaxF == 0) sprintf(opt.etString,"%.1f < #Sigma E_{T}^{HF |#eta>4|} < %.0f GeV",etmin,etmax);
    else sprintf(opt.etString,"%.1f < #Sigma E_{T}^{HF |#eta>4|} < %.1f GeV",etmin,etmax);
  }
}

void formNtrk(InputOpt &opt, float ntrmin, float ntrmax) {
  double ntrminD, ntrmaxD, ntrminF, ntrmaxF;
  ntrminF = modf(ntrmin,&ntrminD);
  ntrmaxF = modf(ntrmax,&ntrmaxD);

  if (ntrmin == 0) { 
    if (ntrmaxF == 0) sprintf(opt.ntrkString,"N_{tracks}^{|#eta|<2.4} < %.0f ",ntrmax);
    else  sprintf(opt.ntrkString,"N_{tracks}^{|#eta|<2.4} < %.1f ",ntrmax);
  } else {
    if (ntrminF == 0 && ntrmaxF == 0) sprintf(opt.ntrkString,"%.0f < N_{tracks}^{|#eta|<2.4} < %.0f ",ntrmin,ntrmax);
    else if (ntrminF == 0 && !ntrmaxF == 0) sprintf(opt.ntrkString,"%.0f < N_{tracks}^{|#eta|<2.4} < %.1f ",ntrmin,ntrmax);
    else if (!ntrminF == 0 && ntrmaxF == 0) sprintf(opt.ntrkString,"%.1f < N_{tracks}^{|#eta|<2.4} < %.0f ",ntrmin,ntrmax);
    else sprintf(opt.ntrkString,"%.1f < N_{tracks}^{|#eta|<2.4} < %.1f ",ntrmin,ntrmax);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void setWSRange(RooWorkspace *ws, float lmin, float lmax, float errmin, float errmax){
  float minRangeForPF = -4*errmax;
  if (minRangeForPF < -lmin) minRangeForPF = -lmin;

  ws->var("Jpsi_CtTrue")->setRange(-0.1,lmax);
  ws->var("Jpsi_Ct")->setRange("promptMCfit",minRangeForPF,4*errmax);
  ws->var("Jpsi_Ct")->setRange(-lmin,lmax);
  ws->var("Jpsi_CtErr")->setRange(errmin,errmax);

  return;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

RooBinning setCtBinning(float lmin,float lmax) {
  RooBinning rbct(-lmin,lmax);
  if (lmax+lmin>4.9) {
    rbct.addBoundary(-1.5);
    rbct.addBoundary(-1.0);
    rbct.addBoundary(-0.8);
    rbct.addBoundary(-0.6);
    rbct.addBoundary(-0.5);
    rbct.addUniform(6,-0.5,-0.2);
    rbct.addUniform(12,-0.2,0.1);
    rbct.addUniform(8,0.1,0.5);
    rbct.addUniform(5,0.5,1.0);
    rbct.addUniform(15,1.0,lmax);
  } else if (lmax+lmin > 4.4) { //this is what we use KYO!!!
    rbct.addBoundary(-1.5);
    rbct.addBoundary(-1.0);
    rbct.addBoundary(-0.8);
    rbct.addBoundary(-0.6);
    rbct.addBoundary(-0.5);
    rbct.addUniform(9,-0.5,-0.2);
    rbct.addUniform(20,-0.2,0.1);
    rbct.addUniform(11,0.1,0.5);
    rbct.addUniform(5,0.5,1.0);
    rbct.addUniform(5,1.0,lmax);
  } else {
    rbct.addBoundary(-lmin);
    rbct.addBoundary(-0.7);
    rbct.addBoundary(-0.6);
    rbct.addBoundary(-0.5);
    rbct.addUniform(9,-0.5,-0.2);
    rbct.addUniform(28,-0.2,0.1);
    rbct.addUniform(11,0.1,0.5);
    rbct.addUniform(15,0.5,1.2);
    rbct.addUniform(8,1.2,lmax);
  }
  return rbct;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

int readCtErrRange(InputOpt &opt, float *errmin, float *errmax) {
	ifstream errinput;
	errinput.open(opt.ctErrFile.c_str(),ifstream::in);
	if (!errinput.good()) {
		cout << "readCtErrRange:: CANNOT read ctau error list file. Exit." << endl;
		return -1;
	}
	string headers;
	getline(errinput, headers); // remove prefix
	getline(errinput, headers); // remove column names

	string rap, pt, ntrk, et, emin, emax;
	while (!errinput.eof()) {
		errinput >> rap >> pt >> ntrk >> et >> emin >> emax;
		if (!rap.compare(opt.yrange) && !pt.compare(opt.ptrange) && !ntrk.compare(opt.ntrrange) && !et.compare(opt.etrange)) {
			*errmin = atof(emin.c_str());
			*errmax = atof(emax.c_str());
			return 0;
		} else continue;
	}
	cout << "readCtErrRange:: CANNOT read ctau error range from a file. Exit." << endl;
	return -2;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void getCtErrRange(RooDataSet *data, InputOpt &opt, const char *t_reduceDS_woCtErr, float lmin, float lmax, float *errmin, float *errmax) {
  RooWorkspace *ws = new RooWorkspace("ctauerrorcheckWS");
  RooDataSet *redDataCut = (RooDataSet*)data->reduce(t_reduceDS_woCtErr);
  ws->import(*redDataCut);
  
  ws->var("Jpsi_Mass")->setRange(2.6,3.5);
  ws->var("Jpsi_Mass")->setBins(45);
  ws->var("Jpsi_Ct")->setRange(-lmin,lmax);
  ws->var("Jpsi_CtErr")->setRange(0.0,0.992);
  ws->var("Jpsi_CtErr")->setBins(124);

	cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*- getCtErrRange -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
  cout << " *** DATA :: N events to fit (woCtErrRange) : " << redDataCut->sumEntries() << endl;

  RooDataSet *redDataSB = (RooDataSet*) redDataCut->reduce("Jpsi_Mass < 2.9 || Jpsi_Mass > 3.3");
  RooDataSet *redDataSIG = (RooDataSet*)redDataCut->reduce("Jpsi_Mass > 2.9 && Jpsi_Mass < 3.3");
  
	//// *** RooDataHist
  RooDataHist *tbinDataCtErrSB = new RooDataHist("tbinDataCtErrSB","Data ct error distribution for bkg",RooArgSet(*(ws->var("Jpsi_CtErr"))),*redDataSB);
  RooDataHist *tbinDataCtErrSIG = new RooDataHist("tbinDataCtErrSIG","Data ct error distribution for sig",RooArgSet(*(ws->var("Jpsi_CtErr"))),*redDataSIG);

  //// *** mass fit to get coefExp of coefPol for scaleF
  defineMassBkg(ws);
  defineMassSig(ws, opt);
  struct PARAM {
    double fracG1; double fracG1Err;
    double meanSig;  double meanSigErr;
    double sigmaSig1; double sigmaSig1Err;
    double sigmaSig2; double sigmaSig2Err;
    double alpha;     double alphaErr;
    double enne;      double enneErr;
  };
  double cutValue_merged;

	char funct[100];
  double initBkg = redDataSB->sumEntries()*9.0/5.0;
  double initSig = redDataCut->sumEntries() - initBkg;
  sprintf(funct,"SUM::MassPDF(NSig[%f,1.0,50000.0]*G1CB1Sig,NBkg[%f,1.0,500000.0]*expBkg)",initSig,initBkg);
  ws->factory(funct);
	ws->pdf("MassPDF")->fitTo(*redDataCut,Extended(1),Minos(0),Save(1),SumW2Error(kTRUE),NumCPU(8));

 	//// ****  scaleF to scale down ct err dist in 2.9-3.3 GeV/c2
  float bc;
  if (!inOpt.mBkgFunct.compare("expBkg")) bc = ws->var("coefExp")->getVal();
  else if (!inOpt.mBkgFunct.compare("polBkg")) bc = ws->var("coefPol")->getVal();
  float scaleF = (exp(2.9*bc)-exp(3.3*bc))/(exp(2.6*bc)-exp(2.9*bc)+exp(3.3*bc)-exp(3.5*bc));
  
  //// *** (tbinSubtractedSIG) = (tbinDataCtErrSIG) - scaleF*(tbinDataCtErrSB)
	RooDataHist* tbinSubtractedSIG = new RooDataHist("tbinSubtractedSIG","Subtracted data",RooArgSet(*(ws->var("Jpsi_CtErr")))); 
  RooDataHist* tbinScaledBKG = subtractSidebands(ws,tbinSubtractedSIG,tbinDataCtErrSIG,tbinDataCtErrSB,scaleF,"Jpsi_CtErr");
  
  //// **** Check the minimum and maximum of the ctau error in signal and background regions
  TH1* histDataCtErrSIG = tbinDataCtErrSIG->createHistogram("histDataCtErrSIG",*ws->var("Jpsi_CtErr"));
  TH1* histSubtractedSIG = tbinSubtractedSIG->createHistogram("histSubtractedSIG",*ws->var("Jpsi_CtErr"));
  TH1* histScaledBKG = tbinScaledBKG->createHistogram("histScaledBKG",*ws->var("Jpsi_CtErr"));
  
  double minSig = 0.5, maxSig = 0.0, minBkg = 0.5, maxBkg = 0.0;
  double cutValue = 0.2;

  int maxBinSig = histSubtractedSIG->GetMaximumBin();
  int maxBinBkg = histScaledBKG->GetMaximumBin();
  
  minSig = histSubtractedSIG->GetBinLowEdge(maxBinSig);
  minBkg = histScaledBKG->GetBinLowEdge(maxBinBkg);
  // pick up lower bound next to other non-zero bins
  for (int xbins = maxBinSig; xbins > 0; xbins--) {
    if (histSubtractedSIG->GetBinContent(xbins) > cutValue) {
      minSig = histSubtractedSIG->GetBinLowEdge(xbins);
//          cout << "getCtErrRange:: SIG binContent: " << histSubtractedSIG->GetBinContent(xbins) << endl;
//          cout << "getCtErrRange:: SIG low edge: " << histSubtractedSIG->GetBinLowEdge(xbins) << endl;
    } else break;
  }
  for (int xbins = maxBinBkg; xbins > 0; xbins--) {
    if (histScaledBKG->GetBinContent(xbins) > cutValue) {
      minBkg = histScaledBKG->GetBinLowEdge(xbins);
//          cout << "getCtErrRange:: BKG binContent: " << histScaledBKG->GetBinContent(xbins) << endl;
//          cout << "getCtErrRange:: BKG low edge: " << histScaledBKG->GetBinLowEdge(xbins) << endl;
    } else break;
  }

  // pick up upper bound next to other non-zero bins
  maxSig = histSubtractedSIG->GetBinLowEdge(maxBinSig+1);
  maxBkg = histScaledBKG->GetBinLowEdge(maxBinBkg+1);
  for (int xbins = maxBinSig; xbins < histSubtractedSIG->GetNbinsX() ; xbins++) {
    if (histSubtractedSIG->GetBinContent(xbins) > cutValue) {
      maxSig = histSubtractedSIG->GetBinLowEdge(xbins+1);
//          cout << "getCtErrRange:: SIG binContent: " << histSubtractedSIG->GetBinContent(xbins) << endl;
//          cout << "getCtErrRange:: SIG upper edge: " << histSubtractedSIG->GetBinLowEdge(xbins+1) << endl;
    } else break;
  }
  for (int xbins = maxBinSig; xbins < histScaledBKG->GetNbinsX() ; xbins++) {
    if (histScaledBKG->GetBinContent(xbins) > cutValue) {
      maxBkg = histScaledBKG->GetBinLowEdge(xbins+1);
//          cout << "getCtErrRange:: BKG binContent: " << histScaledBKG->GetBinContent(xbins) << endl;
//          cout << "getCtErrRange:: BKG upper edge: " << histScaledBKG->GetBinLowEdge(xbins+1) << endl;
    } else break;
  }

  // choose the higher lower limit, lower upper limit
  double tmpMin = 0, tmpMax = 0;
  if (minSig > minBkg) tmpMin = minSig; else tmpMin = minBkg;
  if (maxSig < maxBkg) tmpMax = maxSig; else tmpMax = maxBkg;

  // round off lower limit -> allow more entries on the lower limits
  tmpMin = TMath::Floor(tmpMin*1000);
  tmpMin = tmpMin/(double)1000.0;

  // round up upper limit -> allow more entries on the upper limits
  tmpMax = TMath::Ceil(tmpMax*1000);
  tmpMax = tmpMax/(double)1000.0;

  char reduceDS[512];
  sprintf(reduceDS,"Jpsi_CtErr > %.3f && Jpsi_CtErr < %.3f",tmpMin,tmpMax);
  RooDataSet *redDataTmp = (RooDataSet*)redDataCut->reduce(reduceDS);
  if (redDataTmp->sumEntries() < redDataCut->sumEntries()*0.9) { // if ctau error range cuts off >10% events
    delete redDataTmp;
    sprintf(reduceDS,"Jpsi_CtErr > %.3f && Jpsi_CtErr < %.3f",minSig,maxSig);
    redDataTmp = (RooDataSet*)redDataCut->reduce(reduceDS);
    tmpMin = minSig; tmpMax = maxSig; 
  }
  if ((tmpMax - tmpMin) < 0.008) {
      cout << "getCtErrRange:: Maximum is less than minimum! Possibly there are few events in this bin.\n";
      tmpMax = tmpMin + 0.008;
  }

  //// *** draw final ctau error plot
  TCanvas c0("ctau_err","ctau_err",500,500);
  c0.Draw(); c0.cd(); c0.SetLogy(1); 
  RooPlot *errframe2 = ws->var("Jpsi_CtErr")->frame();
  //tbinDataCtErrSIG->plotOn(errframe2,DataError(RooAbsData::SumW2),MarkerColor(kRed),LineColor(kRed));
  //tbinDataCtErrSB->plotOn(errframe2,DataError(RooAbsData::SumW2),MarkerColor(kGreen+2),LineColor(kGreen+2),MarkerStyle(24));
  //tbinScaledBKG->plotOn(errframe2,DataError(RooAbsData::SumW2),MarkerColor(kBlue),MarkerStyle(24),LineColor(kBlue));
  //tbinSubtractedSIG->plotOn(errframe2,DataError(RooAbsData::SumW2),LineColor(kWhite));
  const double max = errframe2->GetMaximum() * 1.3;
  errframe2->SetMaximum(max);
  errframe2->SetMinimum(0.2);
  errframe2->Draw();
	histDataCtErrSIG->SetMarkerColor(kRed);
	histDataCtErrSIG->SetLineColor(kWhite);
	histDataCtErrSIG->SetMarkerStyle(24);
  histDataCtErrSIG->GetXaxis()->CenterTitle(1);
  histDataCtErrSIG->GetYaxis()->CenterTitle(1);
	histDataCtErrSIG->Draw("pe");
 	histScaledBKG->SetMarkerColor(kBlue);
 	histScaledBKG->SetLineColor(kWhite);
 	histScaledBKG->SetMarkerStyle(24);
  histScaledBKG->Draw("pe same");
  histSubtractedSIG->SetLineColor(kWhite);
  histSubtractedSIG->Draw("pe same");

  TLatex *t = new TLatex();
  t->SetNDC(); t->SetTextAlign(32); t->SetTextSize(0.035);

  t->DrawLatex(0.92,0.84,opt.rapString);
  t->DrawLatex(0.92,0.78,opt.ptString);
	if (opt.EventActivity ==1) t->DrawLatex(0.92,0.72,opt.ntrkString);
	else if (opt.EventActivity ==2) t->DrawLatex(0.92,0.72,opt.etString);

  char comment[200];
  sprintf(comment,"Range: %.3f-%.3f (mm)",tmpMin,tmpMax);
  t->SetTextSize(0.04);
	t->SetTextColor(kRed);
	t->DrawLatex(0.92,0.6,comment);
	t->SetTextColor(kBlack);
  
	TLegend legsb(0.6,0.19,0.9,0.35,NULL,"brNDC");
  legsb.SetFillStyle(0); legsb.SetBorderSize(0); legsb.SetShadowColor(0); legsb.SetMargin(0.2);
	legsb.SetTextFont(42); legsb.SetTextSize(0.035);
	legsb.AddEntry(histDataCtErrSIG,"sig cands","p");
  legsb.AddEntry(histScaledBKG,"scaled bkg","p");
  legsb.AddEntry(histSubtractedSIG,"sig (= cands - bkg)","p");
  legsb.Draw("same");

  string titlestr = opt.dirName+ "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtErrGetRange_Log.pdf";
  c0.SaveAs(titlestr.c_str());

  *errmin = tmpMin; *errmax = tmpMax;
//  cout << "getCtErrRange:: " << t_reduceDS_woCtErr << " " << lmin << " " << lmax << " " << *errmin << " " << *errmax << endl;
  
  delete ws;
  delete redDataCut;
  delete redDataTmp;
  //delete binData;
  //delete binDataCtErr;
  //delete binDataSB;
  delete tbinDataCtErrSB;
  delete tbinDataCtErrSIG;
  delete tbinSubtractedSIG;
  delete tbinScaledBKG;
  delete t;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void drawSBRightLeft(RooWorkspace *ws, RooDataSet *redDataSBL, RooDataSet *redDataSBR, InputOpt &opt) {
  TH1 *binDataSBL = redDataSBL->createHistogram("sidebandL",*(ws->var("Jpsi_Ct")),Binning(45));
  TH1 *binDataSBR = redDataSBR->createHistogram("sidebandR",*(ws->var("Jpsi_Ct")),Binning(45));
  
  TCanvas c0; c0.SetLogy(0);
  TLatex *t = new TLatex();  t->SetNDC();  t->SetTextAlign(12);
  string titlestr;
  
  binDataSBL->GetXaxis()->CenterTitle(1);
  binDataSBL->GetYaxis()->CenterTitle(1);
  binDataSBL->GetXaxis()->SetTitle("#font[12]{l}_{J/#psi} (mm)");
  binDataSBL->SetMarkerColor(kBlack);
  binDataSBL->SetLineColor(kBlack);
  binDataSBR->SetMarkerColor(kRed);
  binDataSBR->SetMarkerStyle(kOpenCircle);
  binDataSBR->SetLineColor(kRed);
  binDataSBR->Scale(binDataSBL->GetMaximum()/binDataSBR->GetMaximum());
  double originalmax = binDataSBL->GetMaximum();
  binDataSBL->SetMaximum(originalmax*1.9);
  binDataSBL->Draw("p");
  binDataSBR->Draw("p,same");

	//CMS_lumi(&c0, opt.isPA, 0);
	t->SetTextSize(0.035);
  t->DrawLatex(0.18,0.84,opt.rapString);
  t->DrawLatex(0.18,0.78,opt.ptString);
	if (opt.EventActivity ==1) t->DrawLatex(0.18,0.72,opt.ntrkString);
	else if (opt.EventActivity ==2) t->DrawLatex(0.18,0.72,opt.etString);

  TLegend legsb(0.49,0.75,0.9,0.88,NULL,"brNDC");
  legsb.SetFillStyle(0); legsb.SetBorderSize(0); legsb.SetShadowColor(0); legsb.SetMargin(0.2); 
	legsb.SetTextFont(42); legsb.SetTextSize(0.040);
 	legsb.AddEntry(binDataSBL,"2.6 < m_{#mu#mu} < 2.9 (GeV)","p");
  legsb.AddEntry(binDataSBR,"3.3 < m_{#mu#mu} < 3.5 (GeV)","p");
  legsb.Draw("same");

  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtSBRL_Lin.pdf";
  c0.SaveAs(titlestr.c_str());
  c0.SetLogy(1);
  binDataSBL->SetMaximum(originalmax*65);
  c0.Update();
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtSBRL_Log.pdf";
  c0.SaveAs(titlestr.c_str()); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void drawInclusiveMassPlots(RooWorkspace *ws, RooDataSet* redDataCut, RooFitResult *fitMass, InputOpt &opt) {
  
	//// **** Temporary variables for plotting
  TLatex *t = new TLatex();  t->SetNDC();  t->SetTextAlign(32);
  TLatex *ty = new TLatex();  ty->SetNDC();  ty->SetTextAlign(12);
  char reduceDS[512];
  string titlestr;
  
  //// *** Mass plot
  RooBinning rb(ws->var("Jpsi_Mass")->getBinning().numBins(), ws->var("Jpsi_Mass")->getBinning().array());
  RooPlot *mframe_wob = ws->var("Jpsi_Mass")->frame();
  redDataCut->plotOn(mframe_wob,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerSize(1),Binning(rb));
  
	double avgBinWidth = rb.averageBinWidth();
  mframe_wob->GetYaxis()->SetTitle(Form("Counts / %.2f (GeV/c^{2})",avgBinWidth));
  mframe_wob->GetXaxis()->SetTitle("m_{#mu#mu} (GeV/c^{2})");
  mframe_wob->GetXaxis()->CenterTitle(1);
  mframe_wob->GetYaxis()->CenterTitle(1);
  const double max = mframe_wob->GetMaximum() * 1.3;
  mframe_wob->SetMaximum(max);
  mframe_wob->SetMinimum(0);

	ws->pdf("MassPDF")->plotOn(mframe_wob,DrawOption("F"),FillColor(kBlack),FillStyle(3354),Normalization(redDataCut->sumEntries(),RooAbsReal::NumEvent));
  ws->pdf("MassPDF")->plotOn(mframe_wob,Components(opt.mBkgFunct.c_str()),DrawOption("F"),FillColor(kAzure-9),FillStyle(1001),Normalization(redDataCut->sumEntries(),RooAbsReal::NumEvent));
  ws->pdf("MassPDF")->plotOn(mframe_wob,Components(opt.mBkgFunct.c_str()),LineColor(kBlue),LineStyle(7),LineWidth(5),Normalization(redDataCut->sumEntries(),RooAbsReal::NumEvent));
  ws->pdf("MassPDF")->plotOn(mframe_wob,LineColor(kBlack),LineWidth(2),Normalization(redDataCut->sumEntries(),RooAbsReal::NumEvent));
  redDataCut->plotOn(mframe_wob,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerSize(1),Binning(rb));

  TH1 *hdata = redDataCut->createHistogram("hdata",*ws->var("Jpsi_Mass"),Binning(rb));
  // *** Calculate chi2/nDof for mass fitting
  int nBins = hdata->GetNbinsX();
  RooHist *hpullm; hpullm = mframe_wob->pullHist(); hpullm->SetName("hpullM");
  double chi2 = 0;
  int nFullBinsPull = 0;
  double *ypull = hpullm->GetY();
  for (unsigned int i=0; i < nBins; i++) {
    if (hdata->GetBinContent(i+1) == 0) continue;
    chi2 += pow(ypull[i],2);
    nFullBinsPull++;
  }
  double UnNormChi2 = chi2;
  int nFitParam = fitMass->floatParsFinal().getSize();
  int Dof = nFullBinsPull - nFitParam;
  chi2 /= (nFullBinsPull - nFitParam);

  TCanvas c1wop; c1wop.Draw();
  mframe_wob->SetTitleOffset(1.47,"Y");
  mframe_wob->Draw();
  //// **** no pT & y information for massfit_wopull
  //lumiTextOffset   = 0.45;
	//CMS_lumi(&c1wop, opt.isPA, 0);
	//t->SetTextSize(0.040);
  //t->DrawLatex(0.20,0.89,opt.rapString);
  //t->DrawLatex(0.20,0.83,opt.ptString);
	ty->SetTextSize(0.040);
  sprintf(reduceDS,"N_{J/#psi}: %0.0f #pm %0.0f",ws->var("NSig")->getVal(),ws->var("NSig")->getError());
  ty->DrawLatex(0.20,0.85,reduceDS);
  sprintf(reduceDS,"#sigma = %0.0f #pm %0.0f MeV/c^{2}", opt.PcombinedWidth, opt.PcombinedWidthErr);
  ty->DrawLatex(0.20,0.79,reduceDS);

  TLegend * leg11 = new TLegend(0.18,0.51,0.54,0.72,NULL,"brNDC");
  leg11->SetFillStyle(0); leg11->SetBorderSize(0); leg11->SetShadowColor(0);
  leg11->SetTextSize(0.035); leg11->SetTextFont(42);
	leg11->SetMargin(0.2);
  leg11->AddEntry(gfake1,"data","p");
  leg11->AddEntry(&hfake21,"total fit","lf");
  leg11->AddEntry(&hfake11,"background","lf");
  leg11->Draw("same");

  titlestr = opt.dirName + "_rap" + opt.yrange  + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_massfitIncl_wopull.pdf";
  c1wop.SaveAs(titlestr.c_str());
  titlestr = opt.dirName + "_rap" + opt.yrange  + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_massfitIncl_wopull.root";
  c1wop.SaveAs(titlestr.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void drawCtauErrPdf(RooWorkspace *ws, RooDataHist *binDataCtErrSB, RooDataHist *binDataCtErrSIG, RooDataHist *binSubtractedSIG, RooDataHist *binScaledBKG, InputOpt &opt) {
  RooPlot *errframe = ws->var("Jpsi_CtErr")->frame();

  binDataCtErrSB->plotOn(errframe,DataError(RooAbsData::SumW2),LineColor(kBlue),MarkerColor(kBlue),MarkerStyle(kOpenCircle));
  ws->pdf("errPdfBkg")->plotOn(errframe,LineColor(kViolet+3),Normalization(binDataCtErrSB->sumEntries(),RooAbsReal::NumEvent));
  //ws->pdf("errPdfBkg")->plotOn(errframe);
  
	TCanvas c0;
  string titlestr;
  c0.Clear(); c0.SetLogy(1); errframe->Draw();
	errframe->GetXaxis()->CenterTitle(1);
  errframe->GetYaxis()->CenterTitle(1);
	errframe->SetMinimum(0.01);
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtErrPdfBkg_Log.pdf";
  c0.SaveAs(titlestr.c_str());
  delete errframe;

  errframe = ws->var("Jpsi_CtErr")->frame();
  binSubtractedSIG->plotOn(errframe,DataError(RooAbsData::SumW2),DrawOption("F"),FillColor(kWhite),LineColor(kWhite)); // just for axis
  ws->pdf("errPdfSig")->plotOn(errframe,LineColor(kViolet+3),Normalization(binSubtractedSIG->sumEntries(),RooAbsReal::NumEvent));
  binDataCtErrSIG->plotOn(errframe,DataError(RooAbsData::SumW2),LineColor(kRed),MarkerColor(kRed),MarkerStyle(kOpenCircle));
  c0.Clear(); c0.SetLogy(1); errframe->Draw();
	errframe->GetXaxis()->CenterTitle(1);
  errframe->GetYaxis()->CenterTitle(1);
	errframe->SetMinimum(0.01);
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtErrPdfSig_Log.pdf";
  c0.SaveAs(titlestr.c_str());
  delete errframe;

}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void drawCtauResolPlots(RooWorkspace *ws, bool fitMC, RooPlot *tframePR, InputOpt &opt) {
  TLatex *t = new TLatex();  t->SetNDC();  t->SetTextAlign(22);
  char reduceDS[512];
  string titlestr;

  t->SetTextSize(0.035);
  TCanvas c00; c00.cd(); tframePR->Draw();
  sprintf(reduceDS,"#sigma(G_{N}): %.2f",ws->var("sigmaPRResN")->getVal());
  t->DrawLatex(0.55,0.31,reduceDS);
 	sprintf(reduceDS,"#sigma(G_{W}): %.2f",ws->var("sigmaPRResW")->getVal());
  t->DrawLatex(0.55,0.26,reduceDS);
  sprintf(reduceDS,"frac(G_{W}): %.2f",ws->var("fracRes")->getVal());
  t->DrawLatex(0.55,0.21,reduceDS);
	tframePR->GetXaxis()->CenterTitle(1);
	tframePR->GetYaxis()->CenterTitle(1);
  if (fitMC)
    titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtPRResMC_Lin.pdf";
  else
    titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtPRResData_Lin.pdf";
  c00.SaveAs(titlestr.c_str());
  
	c00.SetLogy(1);
	tframePR->GetXaxis()->CenterTitle(1);
	tframePR->GetYaxis()->CenterTitle(1);
	double prmax = tframePR->GetMaximum();
	tframePR->SetMinimum(0.5);
	tframePR->SetMaximum(prmax*5);
	if (fitMC)
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtPRResMC_Log.pdf";
  else
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtPRResData_Log.pdf";
  c00.SaveAs(titlestr.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void drawCtauSBPlots(RooWorkspace *ws, RooDataSet *redDataSB, RooDataHist *binDataCtErrSB, RooFitResult *fitCt_Bkg, float lmin, float lmax, InputOpt &opt, double* UnNormChi2_side_t, int* nFitParam_side_t, int* nFullBinsPull_side_t, int* Dof_side_t,double* Chi2_side_t) {
  
	char reduceDS[512];
  string titlestr;
  double unNormChi2;
  int dof;

  RooBinning rb(ws->var("Jpsi_Ct")->getBinning().numBins(), ws->var("Jpsi_Ct")->getBinning().array());
  
  TLegend leg11(0.64,0.65,0.9,0.74,NULL,"brNDC");
  leg11.SetFillStyle(0); leg11.SetBorderSize(0); leg11.SetShadowColor(0);
  leg11.SetMargin(0.2);
	leg11.SetTextSize(0.040); leg11.SetTextFont(42);
  leg11.AddEntry(gfake1,"sideband data","p");
  leg11.AddEntry(&hfake11,"background","l");

  RooPlot *tframe1 = ws->var("Jpsi_Ct")->frame();
  double avgBinWidth = rb.averageBinWidth();
  tframe1->GetYaxis()->SetTitle(Form("Counts / %.2f (mm)",avgBinWidth));
  tframe1->GetYaxis()->CenterTitle(1);
  redDataSB->plotOn(tframe1,DataError(RooAbsData::SumW2));
  ws->pdf("CtBkgTot_PEE")->plotOn(tframe1,ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErrSB,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent),LineStyle(7));
    
  TCanvas *c3 = new TCanvas("c3","The Canvas",200,10,600,750);
  c3->cd();
  TPad *pad1 = new TPad("pad1","This is pad1",0.00,0.3,1.0,1.0);
  pad1->SetLeftMargin(0.14);
  pad1->SetRightMargin(0.03);
  pad1->SetTopMargin(0.075);
  pad1->SetBottomMargin(0);  pad1->Draw();
  TPad *pad2 = new TPad("pad2","This is pad2",0.00,0.00,1.0,0.3);
  pad2->SetLeftMargin(0.14);
  pad2->SetRightMargin(0.03);
  pad2->SetTopMargin(0);  
	pad2->SetBottomMargin(0.30);  
	pad2->Draw();

  pad1->cd(); tframe1->Draw();
  lumiTextOffset   = 0.45;
	CMS_lumi(c3, opt.isPA, 0);
  TLatex *t = new TLatex();  t->SetNDC();  t->SetTextAlign(32);
	t->SetTextSize(0.040);
  t->DrawLatex(0.92,0.89,opt.rapString);
  t->DrawLatex(0.92,0.83,opt.ptString);
	if (opt.EventActivity ==1) t->DrawLatex(0.92,0.78,opt.ntrkString);
	else if (opt.EventActivity ==2) t->DrawLatex(0.92,0.78,opt.etString);
  leg11.Draw("same");

	//// *** pull
  TH1 *hdatasb = redDataSB->createHistogram("hdatasb",*ws->var("Jpsi_Ct"),Binning(rb));
  RooHist *hpullsb = tframe1->pullHist(); hpullsb->SetName("hpullSB");
  int nFitPar = fitCt_Bkg->floatParsFinal().getSize();
  double chi2 = 0;
  double *ypullssb = hpullsb->GetY();
  unsigned int nBins = ws->var("Jpsi_Ct")->getBinning().numBins();
  unsigned int nFullBins = 0;
  for (unsigned int i = 0; i < nBins; i++) {
    if (hdatasb->GetBinContent(i+1) == 0) continue;
    chi2 += ypullssb[i]*ypullssb[i];
    nFullBins++;
  }
  unNormChi2 = chi2;
  *UnNormChi2_side_t = chi2;
	dof = nFullBins - nFitPar;
  chi2 /= (nFullBins - nFitPar);
  int nDOF = ws->var("Jpsi_Ct")->getBinning().numBins() - nFitPar;
	*nFitParam_side_t = nFitPar;
	*nFullBinsPull_side_t = nFullBins;
	*Dof_side_t =dof;
	*Chi2_side_t = chi2;

  RooPlot* tframepull =  ws->var("Jpsi_Ct")->frame(Title("Pull")) ;
  tframepull->GetYaxis()->SetTitle("Pull");
  tframepull->GetYaxis()->CenterTitle(1);
  tframepull->SetLabelSize(0.04*2.5,"XYZ");
  tframepull->SetTitleSize(0.048*2.5,"XYZ");
  tframepull->SetTitleOffset(0.47,"Y");
  tframepull->addPlotable(hpullsb,"PX") ;
  double tframemax = 0;
  if (tframepull->GetMinimum()*-1 > tframepull->GetMaximum()) tframemax = tframepull->GetMinimum()*-1;
  else tframemax = tframepull->GetMaximum();
  tframepull->SetMaximum(tframemax); 
  tframepull->SetMinimum(-1*tframemax); 
  tframepull->GetXaxis()->SetTitle("#font[12]{l}_{J/#psi} (mm)");
  tframepull->GetXaxis()->CenterTitle(1);

  pad2->cd(); tframepull->Draw();
	TLine* line1 = new TLine(-lmin,0,lmax,0.); line1->SetLineStyle(7); line1->Draw();

  TLatex *t2 = new TLatex();
  t2->SetNDC(); t2->SetTextAlign(22); t2->SetTextSize(0.035*3);
  sprintf(reduceDS,"#chi^{2}/dof = %.2f/%d",unNormChi2,dof);
  t2->DrawLatex(0.76,0.90,reduceDS);

  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtSB_Lin.pdf";
  c3->SaveAs(titlestr.c_str());
  pad1->SetLogy(1);
	double originalmax = tframe1->GetMaximum();
  tframe1->SetMaximum(originalmax*10);
  tframe1->SetMinimum(0.5);
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_CtSB_Log.pdf";
  c3->SaveAs(titlestr.c_str());

  delete pad1;
  delete pad2;
  delete c3;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void drawFinalMass(RooWorkspace *ws, RooDataSet* redDataCut, float NSigNP_fin, float NBkg_fin, RooFitResult *fitMass, InputOpt &opt, double* UnNormChi2_mass_t, int* nFitParam_mass_t, int* nFullBinsPull_mass_t, int* Dof_mass_t,double* Chi2_mass_t) {
  //// **** Temporary variables for plotting
  TLatex *t = new TLatex();  t->SetNDC();  t->SetTextAlign(32);
  TLatex *ty = new TLatex();  ty->SetNDC();  ty->SetTextAlign(12);
  char reduceDS[512];
  string titlestr;
  
  RooBinning rb(ws->var("Jpsi_Mass")->getBinning().numBins(), ws->var("Jpsi_Mass")->getBinning().array());
  RooRealVar tmpVar1("tmpVar1","tmpVar1",NSigNP_fin);
  RooRealVar tmpVar2("tmpVar2","tmpVar2",NBkg_fin);

  //// *** Mass plot
  RooPlot *mframe = ws->var("Jpsi_Mass")->frame();
  redDataCut->plotOn(mframe,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerSize(1),Binning(rb));
  double avgBinWidth = rb.averageBinWidth();
  mframe->GetYaxis()->SetTitle(Form("Counts / %.0f MeV/c^{2}",avgBinWidth*1000));
  mframe->GetXaxis()->SetTitle("m_{#mu#mu} [GeV/c^{2}]");
  mframe->GetXaxis()->CenterTitle(1);
  mframe->GetYaxis()->CenterTitle(1);
  const double max = mframe->GetMaximum() * 1.3;
  mframe->SetMaximum(max);
  mframe->SetMinimum(0);

    //// **** Fill color
    ws->pdf("totPDF_PEE")->plotOn(mframe,DrawOption("F"),FillColor(kBlack),FillStyle(3354),Normalization(redDataCut->sumEntries(),RooAbsReal::NumEvent));
    RooAddPdf tmpPDF("tmpPDF","tmpPDF",RooArgList(*(ws->pdf(opt.mSigFunct.c_str())),*(ws->pdf(opt.mBkgFunct.c_str()))),RooArgList(tmpVar1,tmpVar2));
    tmpPDF.plotOn(mframe,LineColor(kRed),DrawOption("F"),FillColor(kWhite),FillStyle(1001),Normalization(NSigNP_fin+NBkg_fin,RooAbsReal::NumEvent));
    tmpPDF.plotOn(mframe,LineColor(kRed),DrawOption("F"),FillColor(kRed),FillStyle(3444),Normalization(NSigNP_fin+NBkg_fin,RooAbsReal::NumEvent));
    gStyle->SetHatchesLineWidth(2);
    ws->pdf("totPDF_PEE")->plotOn(mframe,Components(opt.mBkgFunct.c_str()),DrawOption("F"),FillColor(kAzure-9),FillStyle(1001),Normalization(redDataCut->sumEntries(),RooAbsReal::NumEvent));
    //// **** Line color
    ws->pdf("totPDF_PEE")->plotOn(mframe,Components(opt.mBkgFunct.c_str()),LineColor(kBlue),LineStyle(7),LineWidth(5),Normalization(redDataCut->sumEntries(),RooAbsReal::NumEvent));
    tmpPDF.plotOn(mframe,LineColor(kRed),LineStyle(9),LineWidth(5),Normalization(NSigNP_fin+NBkg_fin,RooAbsReal::NumEvent));
    ws->pdf("totPDF_PEE")->plotOn(mframe,LineColor(kBlack),LineWidth(2),Normalization(redDataCut->sumEntries(),RooAbsReal::NumEvent));
  redDataCut->plotOn(mframe,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerSize(1),Binning(rb));

  TH1 *hdata = redDataCut->createHistogram("hdata",*ws->var("Jpsi_Mass"),Binning(rb));
  //// *** Calculate chi2/nDof for mass fitting
  int nBins = hdata->GetNbinsX();
  RooHist *hpullm; hpullm = mframe->pullHist(); hpullm->SetName("hpullM");
  double Chi2 = 0;
  int nFullBinsPull = 0;
  double *ypull = hpullm->GetY();
  for (unsigned int i=0; i < nBins; i++) {
    if (hdata->GetBinContent(i+1) == 0) continue;
    nFullBinsPull++;
    Chi2 = Chi2 + pow(ypull[i],2);
  }
  double UnNormChi2 = Chi2;
	*UnNormChi2_mass_t = Chi2;
  int nFitParam = fitMass->floatParsFinal().getSize();
	int Dof = nFullBinsPull - nFitParam;
  Chi2 /= (nFullBinsPull - nFitParam);
  *nFitParam_mass_t = nFitParam;
  *nFullBinsPull_mass_t = nFullBinsPull;
  *Dof_mass_t = Dof;
  *Chi2_mass_t = Chi2;

  TCanvas c1wop; c1wop.Draw();
  mframe->SetTitleOffset(1.47,"Y");
  mframe->Draw();
  //// **** no pT & y information for massfit_wopull
  //lumiTextOffset   = 0.45;
	//CMS_lumi(&c1wop, opt.isPA, 0);
	//t->SetTextSize(0.040);
  //t->DrawLatex(0.20,0.89,opt.rapString);
  //t->DrawLatex(0.20,0.83,opt.ptString);
	ty->SetTextSize(0.040);
  sprintf(reduceDS,"N_{J/#psi}: %0.0f #pm %0.0f",ws->var("NSig")->getVal(),ws->var("NSig")->getError());
  ty->DrawLatex(0.20,0.85,reduceDS);
  sprintf(reduceDS,"#sigma = %0.0f #pm %0.0f MeV/c^{2}", opt.PcombinedWidth, opt.PcombinedWidthErr);
  ty->DrawLatex(0.20,0.79,reduceDS);

  TLegend * leg11 = new TLegend(0.18,0.51,0.54,0.72,NULL,"brNDC");
  leg11->SetFillStyle(0); leg11->SetBorderSize(0); leg11->SetShadowColor(0);
  leg11->SetTextSize(0.035); leg11->SetTextFont(42);
	leg11->SetMargin(0.2);
  leg11->AddEntry(gfake1,"data","p");
  leg11->AddEntry(&hfake21,"total fit","lf");
  leg11->AddEntry(&hfake31,"bkg + non-prompt","lf"); 
  leg11->AddEntry(&hfake11,"background","lf");
  leg11->Draw("same");

  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_massfit_wopull.pdf";
  c1wop.SaveAs(titlestr.c_str());
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_massfit_wopull.root";
	c1wop.SaveAs(titlestr.c_str());
	/////////////////////////////////////////////////////////////////////////////////  

  TCanvas c1("c1","The mass Canvas",200,10,600,750);
  c1.cd();
  TPad *padm1 = new TPad("padm1","This is pad1",0.0,0.3,1.0,1.0);
  padm1->SetLeftMargin(0.14);
  padm1->SetRightMargin(0.03);
  padm1->SetTopMargin(0.075);
  padm1->SetBottomMargin(0);  padm1->Draw();
  TPad *padm2 = new TPad("padm2","This is pad2",0.00,0.00,1.0,0.3);
  padm2->SetLeftMargin(0.14);
  padm2->SetRightMargin(0.03);
  padm2->SetTopMargin(0);  
	padm2->SetBottomMargin(0.30);  padm2->Draw();

  padm1->cd();  mframe->Draw();
  lumiTextOffset   = 0.45;
	CMS_lumi(&c1, opt.isPA, 0);
	t->SetTextSize(0.035);
  t->DrawLatex(0.91,0.90,opt.rapString);
  t->DrawLatex(0.91,0.85,opt.ptString);
	if (opt.EventActivity ==1) t->DrawLatex(0.91,0.80,opt.ntrkString);
	else if (opt.EventActivity ==2) t->DrawLatex(0.91,0.80,opt.etString);
  
	ty->SetTextSize(0.040);
  sprintf(reduceDS,"N_{J/#psi}: %0.0f #pm %0.0f",ws->var("NSig")->getVal(),ws->var("NSig")->getError());
  ty->DrawLatex(0.20,0.89,reduceDS);
  sprintf(reduceDS,"#sigma = %0.0f #pm %0.0f MeV/c^{2}", opt.PcombinedWidth, opt.PcombinedWidthErr);
  ty->DrawLatex(0.20,0.84,reduceDS);

  leg11->Draw("same");
  c1.Update();

	//// **** pull
  RooPlot* mframepull =  ws->var("Jpsi_Mass")->frame(Title("Pull")) ;
  mframepull->GetYaxis()->SetTitle("Pull");
  mframepull->GetYaxis()->CenterTitle(1);
  mframepull->SetLabelSize(0.04*2.5,"XYZ");
  mframepull->SetTitleSize(0.048*2.5,"XYZ");
  mframepull->SetTitleOffset(0.47,"Y");
  mframepull->addPlotable(hpullm,"PX") ;
  double mframemax = 0;
  if (mframepull->GetMinimum()*-1 > mframepull->GetMaximum()) mframemax = mframepull->GetMinimum()*-1;
  else mframemax = mframepull->GetMaximum();
  mframepull->SetMaximum(mframemax); 
  mframepull->SetMinimum(-1*mframemax); 
  mframepull->GetXaxis()->SetTitle("m_{#mu#mu} (GeV/c^{2})");
  mframepull->GetXaxis()->CenterTitle(1);

  padm2->cd(); mframepull->Draw();
	TLine* line1 = new TLine(2.6,0,3.5,0.); line1->SetLineStyle(7); line1->Draw();

  TLatex *t2 = new TLatex();
  t2->SetNDC(); t2->SetTextAlign(22); t2->SetTextSize(0.035*3);
  sprintf(reduceDS,"#chi^{2}/dof = %.1f/%d",UnNormChi2,Dof);
  t2->DrawLatex(0.78,0.86,reduceDS);
  c1.Update();

  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_massfit.pdf";
  c1.SaveAs(titlestr.c_str());
	/////////////////////////////////////////////////////////////////////////////////  
	mframe->SetMinimum(0.5);
	mframe->SetMaximum(max*50);
	padm1->cd();
	padm1->SetLogy(1);
	titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_massfit_Log.pdf";
	c1.SaveAs(titlestr.c_str());
	/////////////////////////////////////////////////////////////////////////////////  
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void drawFinalCtau(RooWorkspace *ws, RooDataSet *redDataCut, RooDataHist* binDataCtErr, float NSigNP_fin, float NBkg_fin, RooFitResult *fit2D, float lmin, float lmax, InputOpt &opt, double* UnNormChi2_time_t, int* nFitParam_time_t, int* nFullBinsPull_time_t, int* Dof_time_t,double* Chi2_time_t) {
  char reduceDS[512];
  string titlestr;
  
  RooBinning rb(ws->var("Jpsi_Ct")->getBinning().numBins(), ws->var("Jpsi_Ct")->getBinning().array());

  RooRealVar tmpVar1("tmpVar1","tmpVar1",NSigNP_fin);
  RooRealVar tmpVar2("tmpVar2","tmpVar2",NBkg_fin);
  
  RooPlot *tframe = ws->var("Jpsi_Ct")->frame();
  tframe->SetTitleOffset(1.47,"Y");
  double avgBinWidth = rb.averageBinWidth();
  //tframe->GetYaxis()->SetTitle(Form("Counts / %.2f (mm)",avgBinWidth));
  tframe->GetYaxis()->SetTitle(Form("Counts / %.0f (#mum)",avgBinWidth*1000));
  tframe->GetXaxis()->SetTitle("#font[12]{l}_{J/#psi} (mm)");
  tframe->GetXaxis()->CenterTitle(1);
  tframe->GetYaxis()->CenterTitle(1);

  //// **** Ctau total distributions
  RooHist *hpulltot;
  redDataCut->plotOn(tframe,DataError(RooAbsData::SumW2),Binning(rb),MarkerSize(1));

    ws->pdf("totPDF_PEE")->plotOn(tframe,LineColor(kBlack),LineWidth(2),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent));
    hpulltot = tframe->pullHist(); hpulltot->SetName("hpulltot");
    ws->pdf("totPDF_PEE")->plotOn(tframe,Components("MassCtBkg"),LineColor(kBlue),LineWidth(5),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent),LineStyle(7));
    ws->pdf("totPDF_PEE")->plotOn(tframe,Components("MassCtNP"),LineColor(kRed),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent),LineStyle(kDashed));
    ws->pdf("totPDF_PEE")->plotOn(tframe,Components("MassCtPR"),LineColor(kGreen+1),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent),LineStyle(kDashDotted));
    ws->pdf("totPDF_PEE")->plotOn(tframe,LineColor(kBlack),LineWidth(2),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent));

  TH1 *hdatact = redDataCut->createHistogram("hdatact",*ws->var("Jpsi_Ct"),Binning(rb));
  double chi2 = 0, unNormChi2 = 0;
  int dof = 0;
  double *ypulls = hpulltot->GetY();
  unsigned int nBins = ws->var("Jpsi_Ct")->getBinning().numBins();
  unsigned int nFullBins = 0;
  for (unsigned int i = 0; i < nBins; i++) {
    if (hdatact->GetBinContent(i+1) == 0) continue;
    chi2 += ypulls[i]*ypulls[i];
    nFullBins++;
  }
  unNormChi2 = chi2;
  *UnNormChi2_time_t = chi2;
	int nFitPar = fit2D->floatParsFinal().getSize();
	dof = nFullBins - nFitPar;
  chi2 /= (nFullBins - nFitPar);
  *nFitParam_time_t = nFitPar;
	*nFullBinsPull_time_t = nFullBins;
	*Dof_time_t =dof;
	*Chi2_time_t = chi2;
  
  TCanvas* c2 = new TCanvas("c2","The Canvas",200,10,600,750);
  c2->cd();
  TPad *pad1 = new TPad("pad1","This is pad1",0.0,0.3,1.0,1.0);
	pad1->SetLeftMargin(0.14);
  pad1->SetRightMargin(0.03);
  pad1->SetTopMargin(0.075);
  pad1->SetBottomMargin(0);  pad1->Draw();
  TPad *pad2 = new TPad("pad2","This is pad2",0.0,0.0,1.0,0.3);
  pad2->SetLeftMargin(0.14);
  pad2->SetRightMargin(0.03);
  pad2->SetTopMargin(0);  
	pad2->SetBottomMargin(0.30);  pad2->Draw();

  pad1->cd(); tframe->Draw();
  
	lumiTextOffset   = 0.45;
	CMS_lumi(c2, opt.isPA, 0);
  TLatex *ty = new TLatex();  ty->SetNDC();  ty->SetTextAlign(32);
	ty->SetTextSize(0.035);
  ty->DrawLatex(0.91,0.90,opt.rapString);
  ty->DrawLatex(0.91,0.85,opt.ptString);
	if (opt.EventActivity ==1) ty->DrawLatex(0.91,0.80,opt.ntrkString);
	else if (opt.EventActivity ==2) ty->DrawLatex(0.91,0.80,opt.etString);

  TLegend * leg = new TLegend(0.66,0.56,0.85,0.75,NULL,"brNDC");
  leg->SetFillStyle(0); leg->SetBorderSize(0); leg->SetShadowColor(0);
  leg->SetTextSize(0.035); leg->SetTextFont(42);
  leg->SetMargin(0.2);
  leg->AddEntry(gfake1,"data","p");
  leg->AddEntry(&hfake21,"total fit","l");
  leg->AddEntry(&hfake41,"prompt","l"); 
  leg->AddEntry(&hfake311,"non-prompt","l"); 
  leg->AddEntry(&hfake11,"background","l");
  leg->Draw("same"); 

  RooPlot* tframepull =  ws->var("Jpsi_Ct")->frame(Title("Pull")) ;
  tframepull->GetYaxis()->SetTitle("Pull");
  tframepull->GetYaxis()->CenterTitle(1);
  tframepull->SetLabelSize(0.04*2.5,"XYZ");
  tframepull->SetTitleSize(0.048*2.5,"XYZ");
  tframepull->SetTitleOffset(0.47,"Y");
  tframepull->addPlotable(hpulltot,"PX") ;
  double tframemax = 0;
  if (tframepull->GetMinimum()*-1 > tframepull->GetMaximum()) tframemax = tframepull->GetMinimum()*-1;
  else tframemax = tframepull->GetMaximum();
  tframepull->SetMaximum(tframemax); 
  tframepull->SetMinimum(-1*tframemax);
  tframepull->GetXaxis()->SetTitle("#font[12]{l}_{J/#psi} (mm)");
  tframepull->GetXaxis()->CenterTitle(1);

  pad2->cd(); tframepull->Draw();
	TLine* line1 = new TLine(-lmin,0,lmax,0.); line1->SetLineStyle(7); line1->Draw();

  int nDOF = ws->var("Jpsi_Ct")->getBinning().numBins() - nFitPar;

  TLatex *t2 = new TLatex();
  t2->SetNDC(); t2->SetTextAlign(22); t2->SetTextSize(0.035*3);
  sprintf(reduceDS,"#chi^{2}/dof = %.2f/%d",unNormChi2,dof);
  t2->DrawLatex(0.78,0.90,reduceDS);
  
  c2->Update();
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_timefit_Lin.pdf";
  c2->SaveAs(titlestr.c_str());
	/////////////////////////////////////////////////////////////////////////
	tframe->SetMaximum(tframe->GetMaximum()*9); 
  tframe->SetMinimum(0.5); 
	pad1->SetLogy(1);
	titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_timefit_Log.pdf";
  c2->SaveAs(titlestr.c_str());
	/////////////////////////////////////////////////////////////////////////

  TCanvas* c2b = new TCanvas("c2b","The Canvas",200,10,540,546);
  c2b->cd(); c2b->Draw(); c2b->SetLogy(1);

  RooPlot *tframefill = ws->var("Jpsi_Ct")->frame();
  tframefill->GetYaxis()->SetTitle(Form("Counts / %.2f (mm)",avgBinWidth));
  redDataCut->plotOn(tframefill,DataError(RooAbsData::SumW2),Binning(rb),MarkerSize(1));
    ws->pdf("totPDF_PEE")->plotOn(tframefill,DrawOption("F"),FillColor(kBlack),FillStyle(3354),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent));
    ws->pdf("totPDF_PEE")->plotOn(tframefill,LineColor(kBlack),LineWidth(2),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent));
    RooAddPdf tmpPDF2("tmpPDF2","tmpPDF2",RooArgList(*(ws->pdf("MassCtNP")),*(ws->pdf("MassCtBkg"))),RooArgList(tmpVar1,tmpVar2));
    tmpPDF2.plotOn(tframefill,DrawOption("F"),FillColor(kWhite),FillStyle(1001),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization((NSigNP_fin+NBkg_fin)/redDataCut->sumEntries(),RooAbsReal::NumEvent));
    tmpPDF2.plotOn(tframefill,DrawOption("F"),FillColor(kRed),FillStyle(3444),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization((NSigNP_fin+NBkg_fin)/redDataCut->sumEntries(),RooAbsReal::NumEvent));
    gStyle->SetHatchesLineWidth(2);
    ws->pdf("totPDF_PEE")->plotOn(tframefill,Components("MassCtBkg"),DrawOption("F"),FillColor(kAzure-9),FillStyle(1001),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent));
    ws->pdf("totPDF_PEE")->plotOn(tframefill,Components("MassCtBkg"),LineColor(kBlue),LineWidth(5),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization(1,RooAbsReal::NumEvent),LineStyle(7));
    tmpPDF2.plotOn(tframefill,LineColor(kRed),ProjWData(RooArgList(*(ws->var("Jpsi_CtErr"))),*binDataCtErr,kTRUE),NumCPU(8),Normalization((NSigNP_fin+NBkg_fin)/redDataCut->sumEntries(),RooAbsReal::NumEvent),LineWidth(5),LineStyle(9));
  
	redDataCut->plotOn(tframefill,DataError(RooAbsData::SumW2),Binning(rb),MarkerSize(1));

  tframefill->GetXaxis()->SetTitle("#font[12]{l}_{J/#psi} [mm]");
  tframefill->GetXaxis()->CenterTitle(1);
  tframefill->GetYaxis()->CenterTitle(1);
  tframefill->GetYaxis()->SetTitle(Form("Counts / %.0f #mum",avgBinWidth*1000));
  tframefill->SetMaximum(tframefill->GetMaximum()*9); 
  tframefill->SetMinimum(0.5); 
  tframefill->Draw();
  
  TLegend * leg11 = new TLegend(0.58,0.53,0.90,0.72,NULL,"brNDC");
  leg11->SetFillStyle(0); leg11->SetBorderSize(0); leg11->SetShadowColor(0);
  leg11->SetTextSize(0.035); leg11->SetTextFont(42);
  leg11->SetMargin(0.2);
  leg11->AddEntry(gfake1,"data","p");
  leg11->AddEntry(&hfake21,"total fit","lf");
  leg11->AddEntry(&hfake31,"bkg + non-prompt","lf"); 
  leg11->AddEntry(&hfake11,"background","lf");
  leg11->Draw("same");
  
	c2b->Update();
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_timefit_Log_wopull.pdf";
  c2b->SaveAs(titlestr.c_str());
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_timefit_Log_wopull.root";
  c2b->SaveAs(titlestr.c_str());
  delete c2b;

}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void drawMassCtau2DPlots(RooWorkspace *ws, InputOpt &opt) {

  RooBinning rb(ws->var("Jpsi_Ct")->getBinning().numBins(), ws->var("Jpsi_Ct")->getBinning().array());

  string titlestr;
  RooAbsPdf *hist2D;
	hist2D = ws->pdf("totPDF_PEE");
  TH1* h2D = hist2D->createHistogram("2DHist",*(ws->var("Jpsi_Mass")),YVar(*(ws->var("Jpsi_Ct")),Binning(rb)));
  h2D->GetZaxis()->SetTitle("");
  h2D->GetXaxis()->SetTitleOffset(1.6);
  h2D->GetYaxis()->SetTitleOffset(1.8);
  h2D->GetXaxis()->CenterTitle(1);
  h2D->GetYaxis()->CenterTitle(1);
  //gStyle->SetPadLeftMargin(0.2);
  gStyle->SetTitleSize(0.04,"xyz");
  gStyle->SetLabelSize(0.03,"xyz");
  TCanvas* c2c = new TCanvas("c2c","The Canvas",1000,1000);
  c2c->cd();
	//c2c->SetLogz(1);
  h2D->Draw("SURF1");
  titlestr = opt.dirName + "_rap" + opt.yrange + "_pT" + opt.ptrange + "_ntrk" + inOpt.ntrrange + "_ET" + inOpt.etrange + "_2D.pdf";
  c2c->SaveAs(titlestr.c_str());
  delete c2c;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void CMS_lumi( TPad* pad, int iPeriod, int iPosX )
{            
  bool outOfFrame    = false;
  if( iPosX/10==0 ) { outOfFrame = true; }
  int alignY_=3;
  int alignX_=2;
  if( iPosX/10==0 ) alignX_=1;
  if( iPosX==0    ) alignX_=1;
  if( iPosX==0    ) alignY_=1;
  if( iPosX/10==1 ) alignX_=1;
  if( iPosX/10==2 ) alignX_=2;
  if( iPosX/10==3 ) alignX_=3;
  //if( iPosX == 0  ) relPosX = 0.12;
  if( iPosX == 0  ) relPosX = 0.15; // KYO
  int align_ = 10*alignX_ + alignY_;

  float H = pad->GetWh();
  float W = pad->GetWw();
  float l = pad->GetLeftMargin();
  float t = pad->GetTopMargin();
  float r = pad->GetRightMargin();
  float b = pad->GetBottomMargin();
  //  float e = 0.025;

  pad->cd();
  
	TString lumiText;
 	if( iPeriod==0 ) {
    lumiText += lumi_pp502TeV;
    lumiText += " (pp 5.02 TeV)";
  }  else if( iPeriod==1 || iPeriod==2 || iPeriod==3){
    lumiText += lumi_pPb502TeV;
    lumiText += " (pPb 5.02 TeV)";
  } else {
  	lumiText += "LumiText Not Selected";
	}
  cout << lumiText << endl;

  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);    

  //float extraTextSize = extraOverCmsTextSize*cmsTextSize;
  float extraTextSize = extraOverCmsTextSize*cmsTextSize*1.1;

  latex.SetTextFont(42);
  latex.SetTextAlign(31); 
  latex.SetTextSize(lumiTextSize*t);    
  latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);
  if( outOfFrame ) {
    latex.SetTextFont(cmsTextFont);
    latex.SetTextAlign(11); 
    latex.SetTextSize(cmsTextSize*t);    
    latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);
  }
  
  pad->cd();

  float posX_=0;
  if( iPosX%10<=1 ) { posX_ =   l + relPosX*(1-l-r); }
  else if( iPosX%10==2 ) { posX_ =  l + 0.5*(1-l-r); }
  else if( iPosX%10==3 ) { posX_ =  1-r - relPosX*(1-l-r); }
  float posY_ = 1-t - relPosY*(1-t-b);
  if( !outOfFrame ) {
    if( drawLogo ) {
		  posX_ =   l + 0.045*(1-l-r)*W/H;
	 	 	posY_ = 1-t - 0.045*(1-t-b);
	 	 	float xl_0 = posX_;
		  float yl_0 = posY_ - 0.15;
		  float xl_1 = posX_ + 0.15*H/W;
		  float yl_1 = posY_;
		  //TASImage* CMS_logo = new TASImage("CMS-BW-label.png");
		  TPad* pad_logo = new TPad("logo","logo", xl_0, yl_0, xl_1, yl_1 );
		  pad_logo->Draw();
		  pad_logo->cd();
		  //CMS_logo->Draw("X");
		  pad_logo->Modified();
		  pad->cd();
		} else {
		  latex.SetTextFont(cmsTextFont);
		  latex.SetTextSize(cmsTextSize*t);
		  latex.SetTextAlign(align_);
		  latex.DrawLatex(posX_, posY_, cmsText);
		  if( writeExtraText ) {
	      latex.SetTextFont(extraTextFont);
	      latex.SetTextAlign(align_);
	      latex.SetTextSize(extraTextSize*t);
	      latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, extraText);
	    }
		}
  } else if( writeExtraText ) {
    if( iPosX==0) {
		  posX_ =   l +  relPosX*(1-l-r);
		  posY_ =   1-t+lumiTextOffset*t;
		}
    latex.SetTextFont(extraTextFont);
    latex.SetTextSize(extraTextSize*t);
    latex.SetTextAlign(align_);
    latex.DrawLatex(posX_, posY_, extraText);      
  }
  return;
}

