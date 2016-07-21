#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>

#include <TROOT.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TRandom.h>

#include <RooFit.h>
#include <RooGlobalFunc.h>
#include <RooCategory.h>
#include <RooHistPdfConv.h>
#include <RooGenericPdf.h>
#include <RooFFTConvPdf.h>
#include <RooWorkspace.h>
#include <RooBinning.h>
#include <RooHistPdf.h>
#include <RooKeysPdf.h>
#include <RooProdPdf.h>
#include <RooAddPdf.h>
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooHist.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooConstVar.h>

using namespace std;
using namespace RooFit;

struct InputOpt {
  string FileNameData, FileNameData2, FileNameData3;
  string FileNamePRMC, FileNameNPMC, FileNamePRMC2, FileNameNPMC2;
  string sysString, mSigFunct, mBkgFunct;
  
  int dataMerge; //1:(for pp, pPb), 2:Pbp_v1 + Pbp_v2 (for Pbp), 3: Pbpv1 +Pbp_v2 + pPbFlip (for whole PA)
  int mcMerge; //1:(for pp, pPb, Pbp), 2: Pbpv1 +Pbp_v2 + pPbFlip (for whole PA)
  int doMCWeight;
  int EventActivity; // 0: centrality, 1 : Ntracks, 2 : ET^{HF}
  int isPA; //0:pp, 1:Pbp 2:pPb 3:PbPb- just for legend
  int absoluteY;
  string ptrange, yrange, lrange, errrange, etrange, ntrrange;
  string dirName;
  char beamEn[512], lumi[512]; //not used
  char rapString[512], ptString[512], etString[512], ntrkString[512];
  string psmin_latex, psmax_latex;
 

  int readCtErr;
  string ctErrFile;

  double combinedWidth, combinedWidthErr; //CB + Gaus combined width/width error
  double PcombinedWidth, PcombinedWidthErr; //CB + Gaus combined width/width error, scaling for presentation
} inOpt;

//// Global objects for drawing
TGraphErrors *gfake1;
TH1F hfake11, hfake21, hfake31, hfake311, hfake41;
//// Input arguments, text parsing, formatting functions
void parseInputArg(int argc, char* argv[], InputOpt &opt);
void getOptRange(string &ran,float *min,float *max);
void formTitle(InputOpt &opt) ;
void formRapidity(InputOpt &opt, float ymin, float ymax) ;
void formPt(InputOpt &opt, float pmin, float pmax) ;
void formNtrk(InputOpt &opt, float ntrmin, float ntrmax) ;
void formEt(InputOpt &opt, float etmin, float etmax) ;

//// calculate of read CtErrRange
void getCtErrRange(RooDataSet *data, InputOpt &opt, const char *reduceDSOrig, float lmin, float lmax, float *errmin, float *errmax);
int readCtErrRange(InputOpt &opt, float *errmin, float *errmax);

//// Define essential fit functions
void setWSRange(RooWorkspace *ws, float lmin, float lmax, float errmin, float errmax);
RooBinning setCtBinning(float lmin,float lmax);
RooDataHist* subtractSidebands(RooWorkspace* ws, RooDataHist* subtrData, RooDataHist* all, RooDataHist* side, float scalefactor, string varName);

//// define PDF
void defineMassSig(RooWorkspace *ws, InputOpt &opt);
void defineMassBkg(RooWorkspace *ws);
void defineCtPRRes(RooWorkspace *ws, InputOpt &opt);
void defineCtBkg(RooWorkspace *ws, InputOpt &opt);
void defineCtNP(RooWorkspace *ws, RooDataSet *redNPMC, string titlestr, InputOpt &opt);
void drawCtNPTrue(RooWorkspace *ws, RooDataSet *redNPMC, string titlestr, InputOpt &opt);
void drawCtNPReco(RooWorkspace *ws, RooDataSet *redNPMC, string titlestr, InputOpt &opt);

// Drawing functions: Plotting
void drawSBRightLeft(RooWorkspace *ws, RooDataSet *redDataSBL, RooDataSet *redDataSBR, InputOpt &opt);
void drawInclusiveMassPlots(RooWorkspace *ws, RooDataSet* redDataCut, RooFitResult *fitM, InputOpt &opt, bool isMC);
void drawCtauErrPdf(RooWorkspace *ws, RooDataHist *binDataCtErrSB, RooDataHist *binDataCtErrSIG, RooDataHist *subtrData, RooDataHist *weightedBkg, InputOpt &opt);
void drawCtauResolPlots(RooWorkspace *ws, bool fitMC, RooPlot *tframePR, InputOpt &opt);
void drawCtauSBPlots(RooWorkspace *ws, RooDataSet *redDataSB, RooDataHist *binDataCtErr, RooFitResult *fitSB, float lmin, float lmax, InputOpt &opt, double* UnNormChi2_side_t, int* nFitParam_side_t, int* nFullBinsPull_side_t, int* Dof_side_t,double* Chi2_side_t) ;
//// final plots
void drawFinalMass(RooWorkspace *ws, RooDataSet* redDataCut, float NSigNP_fin, float NBkg_fin, RooFitResult *fitM, InputOpt &opt, double* UnNormChi2_mass_t, int* nFitParam_mass_t, int* nFullBinsPull_mass_t, int* Dof_mass_t,double* Chi2_mass_t);
void drawFinalCtau(RooWorkspace *ws, RooDataSet *redDataCut, RooDataHist* binDataCtErr, float NSigNP_fin, float NBkg_fin, float Bfrac_fin, float ErrBfrac_fin, RooFitResult *fit2D, float lmin, float lmax, InputOpt &opt, double* UnNormChi2_time_t, int* nFitParam_time_t, int* nFullBinsPull_time_t, int* Dof_time_t,double* Chi2_time_t);
void drawMassCtau2DPlots(RooWorkspace *ws, InputOpt &opt) ;

  /////////////////////////////////// fit quality information /////////////////////////////////// 
  // 1) for mass distributions
  double UnNormChi2_mass = 0; // un normalized chi2
  int nFitParam_mass = 0; // # of floating fit parameters
  int nFullBinsPull_mass = 0; // # of bins in pull dist
  int Dof_mass = 0; //ndf = nFullBinsPull - nFitPram
  double Chi2_mass = 0; //chi2/ndf
  double theChi2Prob_mass = 0; // check the chi2 fit probability
  // 2) for time distributions
  double UnNormChi2_time = 0; // un normalized chi2
  int nFitParam_time = 0; // # of floating fit parameters
  int nFullBinsPull_time = 0; // # of bins in pull dist
  int Dof_time = 0; //ndf = nFullBinsPull - nFitPram
  double Chi2_time = 0; //chi2/ndf
  double theChi2Prob_time = 0; // check the chi2 fit probability
  // 3) for time side distributions
  double UnNormChi2_side = 0; // un normalized chi2
  int nFitParam_side = 0; // # of floating fit parameters
  int nFullBinsPull_side = 0; // # of bins in pull dist
  int Dof_side = 0; //ndf = nFullBinsPull - nFitPram
  double Chi2_side = 0; //chi2/ndf
  double theChi2Prob_side = 0; // check the chi2 fit probability
  
/////////////////////////////////////////////////////////
//////////////////// Sub-routines ///////////////////////
/////////////////////////////////////////////////////////

void defineMassSig(RooWorkspace *ws, InputOpt &opt) {
  //// 1) Normal gaussians with meanSig, sigmaSig1
  ws->factory("Gaussian::G1Sig(Jpsi_Mass,meanSig[3.0975,3.05,3.15],sigmaSig1[0.03,0.008,0.075])");
  //// 2)  Crystall Ball with meanSig, sigmaSig2
  ws->factory("CBShape::CB1Sig(Jpsi_Mass,meanSig,sigmaSig2[0.03,0.0008,0.075],alpha[1.9,1.,3.],enne[2.5,1.0,4.0])");
  // Sum of G1 and CB1
  ws->factory("SUM::G1CB1Sig(fracG1[0.5,0.11,0.89]*G1Sig,CB1Sig)");
  return;
}
/////////////////////////////////////////////////////////

void defineMassBkg(RooWorkspace *ws) {
  //// 1st order polynomial
  ws->factory("Polynomial::polBkg(Jpsi_Mass,{coefPol[-0.05,-5.,5.]})");
  //// Exponential
  ws->factory("Exponential::expBkg(Jpsi_Mass,coefExp[-1.,-3.,1.])");
  return;
}
/////////////////////////////////////////////////////////

void defineCtPRRes(RooWorkspace *ws, InputOpt &opt) {
      ws->factory("GaussModel::GW_PRRes(Jpsi_Ct,meanPRResW[0.,-0.01,0.01],sigmaPRResW[2.3,1.1,5.5],one[1.0],Jpsi_CtErr)");
      ws->factory("GaussModel::GN_PRRes(Jpsi_Ct,meanPRResN[0.,-0.01,0.01],sigmaPRResN[0.8,0.6,1.2],one,Jpsi_CtErr)");
      ws->factory("AddModel::CtPRRes({GW_PRRes,GN_PRRes},{fracRes[0.01,0.001,0.999]})");
  return;
}
/////////////////////////////////////////////////////////

void defineCtBkg(RooWorkspace *ws, InputOpt &opt) {
    //ws->factory("Decay::CtBkgPos(Jpsi_Ct,lambdap[0.42,0.05,1.5],CtPRRes,RooDecay::SingleSided)");
    //ws->factory("Decay::CtBkgNeg(Jpsi_Ct,lambdam[0.79,0.02,1.5],CtPRRes,RooDecay::Flipped)");
    //ws->factory("Decay::CtBkgDbl(Jpsi_Ct,lambdasym[0.69,0.02,5.0],CtPRRes,RooDecay::DoubleSided)");
    ws->factory("Decay::CtBkgPos(Jpsi_Ct,lambdap[0.42,0.1,1.0],CtPRRes,RooDecay::SingleSided)");
    ws->factory("Decay::CtBkgNeg(Jpsi_Ct,lambdam[0.02,0.001,0.1],CtPRRes,RooDecay::Flipped)");
    ws->factory("Decay::CtBkgDbl(Jpsi_Ct,lambdasym[0.2,0.001,0.8],CtPRRes,RooDecay::DoubleSided)");
    ws->factory("SUM::CtBkgSum1(fracCtBkg1[0.9,0.0,1.0]*CtBkgPos,CtBkgNeg)");
    ws->factory("SUM::CtBkgSum2(fracCtBkg2[0.9,0.0,1.0]*CtBkgSum1,CtBkgDbl)");
    ws->factory("SUM::CtBkgTot(fracCtBkg3[0.29,0.0,1.0]*CtPRRes,CtBkgSum2)");
  return;
}
/////////////////////////////////////////////////////////

void defineCtNP(RooWorkspace *ws, RooDataSet *redNPMC, string titlestr, InputOpt &opt) {

  RooDataHist* binMCCutNP = new RooDataHist("binMCCutNP","MC distribution for NP signal",RooArgSet(*(ws->var("Jpsi_CtTrue"))),*redNPMC);
  if (inOpt.sysString=="sys04_01") {
    //// **** option1) NP histogram simply convolved with PRRes
    /*RooHistPdfConv GN_NPResW("GN_NPResW","Non-prompt signal with wide gaussian",*(ws->var("Jpsi_Ct")),*(ws->var("meanPRResW")),*(ws->var("sigmaPRResW")),*(ws->var("one")),*(ws->var("Jpsi_CtErr")),*binMCCutNP);  ws->import(GN_NPResW);
    RooHistPdfConv GN_NPResN("GN_NPResN","Non-prompt signal with narrow gaussian",*(ws->var("Jpsi_Ct")),*(ws->var("meanPRResN")),*(ws->var("sigmaPRResN")),*(ws->var("one")),*(ws->var("Jpsi_CtErr")),*binMCCutNP); ws->import(GN_NPResN);
    RooAddPdf CtNPTot("CtNPTot","Non-prompt signal",RooArgSet(*(ws->pdf("GN_NPResW")),*(ws->pdf("GN_NPResN"))),RooArgSet(*(ws->var("fracRes"))));  ws->import(CtNPTot); */
    //// **** option2) RooKeysPdf to smoothen NP histograms (not convolved with PRRes)
    RooKeysPdf CtNPTot("CtNPTot","Non-prompt signal",*(ws->var("Jpsi_Ct")),*redNPMC,RooKeysPdf::MirrorBoth); ws->import(CtNPTot);
    //// **** check NPMC Reco 
    drawCtNPReco(ws, redNPMC, titlestr, opt);
  } else {  
    //// **** define PDF
//    ws->factory("GaussModel::BResTrue(Jpsi_CtTrue,mean[0.0],sigmaNPTrue[0.002,0.00001,0.02])"); //resolution from B meson (before CtPRRes) 
    ws->factory("GaussModel::BResTrue(Jpsi_CtTrue,mean[0.0],sigmaNPTrue[0.00002,0.0000001,0.00003])"); //resolution from B meson (before CtPRRes) 
//    ws->factory("Decay::CtNPTrue(Jpsi_CtTrue,coefExpNPTrue[0.04,0.01,1.0],BResTrue,RooDecay::SingleSided)");
    ws->factory("Decay::CtNPTrue(Jpsi_CtTrue,coefExpNPTrue[0.4,0.25,0.65],BResTrue,RooDecay::SingleSided)");
    
    //// **** get NPMCTrue
    drawCtNPTrue(ws, redNPMC, titlestr, opt);
    
    //// **** convolution NPMCTrue with PRRes
    RooFormulaVar sigmaNPResW("sigmaNPResW", "sqrt((@0*@1)**2+(@2)**2)", RooArgList(*(ws->var("sigmaPRResW")), *(ws->var("Jpsi_CtErr")), *(ws->var("sigmaNPTrue"))));  ws->import(sigmaNPResW);
    RooFormulaVar sigmaNPResN("sigmaNPResN", "sqrt((@0*@1)**2+(@2)**2)", RooArgList(*(ws->var("sigmaPRResN")), *(ws->var("Jpsi_CtErr")),*(ws->var("sigmaNPTrue"))));  ws->import(sigmaNPResN);
    ws->factory("GaussModel::GW_NPRes(Jpsi_Ct,meanPRResW,sigmaNPResW)");
    ws->factory("GaussModel::GN_NPRes(Jpsi_Ct,meanPRResN,sigmaNPResN)");
    ws->factory("AddModel::CtNPRes({GW_NPRes,GN_NPRes},{fracRes})");
    //// **** final CtNP 
    float coefExpNPTrueVal = ws->var("coefExpNPTrue")->getVal();
    ws->factory("Decay::CtNPTot(Jpsi_Ct,coefExpNPTrue,CtNPRes,RooDecay::SingleSided)");
    //// **** check NPMC Reco 
    drawCtNPReco(ws, redNPMC, titlestr, opt);
  } 
  return;
}
/////////////////////////////////////////////////////////
void drawCtNPReco(RooWorkspace *ws, RooDataSet *redNPMC, string titlestr, InputOpt &opt) {
  RooPlot *tframereco = ws->var("Jpsi_Ct")->frame();
  redNPMC->plotOn(tframereco);
  ws->pdf("CtNPTot")->plotOn(tframereco,LineColor(kRed),Normalization(redNPMC->sumEntries(),RooAbsReal::NumEvent));
  tframereco->GetXaxis()->CenterTitle(1);
  tframereco->GetYaxis()->CenterTitle(1);
  
  char titlestr_lin[512];
  TCanvas c0f;
  c0f.cd(); c0f.SetLogy(0); tframereco->Draw();
  sprintf(titlestr_lin,"%s_CtNPReco_Lin.pdf",titlestr.c_str());
  c0f.SaveAs(titlestr_lin);
  
  c0f.SetLogy(1);
  double recomax = tframereco->GetMaximum();
  tframereco->SetMinimum(0.5);
  tframereco->SetMaximum(recomax*5);
  sprintf(titlestr_lin,"%s_CtNPReco_Log.pdf",titlestr.c_str());
  c0f.SaveAs(titlestr_lin);
  delete tframereco;
  return ;
}
/////////////////////////////////////////////////////////

void drawCtNPTrue(RooWorkspace *ws, RooDataSet *redNPMC, string titlestr, InputOpt &opt) {

  RooDataSet* redNPMCTrue = (RooDataSet*) redNPMC->reduce(RooArgSet(*(ws->var("Jpsi_CtTrue"))));
  //// **** fitting  
  cout <<" "<< endl;
  cout << " -*-*-*-*-*-*-*-*-*-*-*-*-*- !!!!!FITTING!!!!! *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-  " << endl;
  ws->pdf("CtNPTrue")->fitTo(*redNPMCTrue,Minos(0),SumW2Error(kTRUE),NumCPU(8));
  
  //// **** Draw
  RooPlot *tframetrue = ws->var("Jpsi_CtTrue")->frame(Bins(150));
  redNPMCTrue->plotOn(tframetrue);
  ws->pdf("CtNPTrue")->plotOn(tframetrue,LineColor(kOrange),Normalization(redNPMCTrue->sumEntries(),RooAbsReal::NumEvent));
  tframetrue->GetXaxis()->CenterTitle(1);
  tframetrue->GetYaxis()->CenterTitle(1);
  
  char titlestr_lin[512];
  TLatex t;
  t.SetNDC(); t.SetTextAlign(12); t.SetTextSize(0.035);

  TCanvas c0f;
  c0f.cd(); c0f.SetLogy(0); tframetrue->Draw();
  sprintf(titlestr_lin,"coefExpNPTrue: %.2f #pm %.1e",ws->var("coefExpNPTrue")->getVal(),ws->var("coefExpNPTrue")->getError());
  t.DrawLatex(0.43,0.84,titlestr_lin);
  sprintf(titlestr_lin,"sigmaNPTrue: %.1e #pm %.1e",ws->var("sigmaNPTrue")->getVal(),ws->var("sigmaNPTrue")->getError());
  t.DrawLatex(0.43,0.76,titlestr_lin);
  sprintf(titlestr_lin,"%s_CtNPTrue_Lin.pdf",titlestr.c_str());
  c0f.SaveAs(titlestr_lin);

  c0f.SetLogy(1);
  double truemax = tframetrue->GetMaximum();
  tframetrue->SetMinimum(0.5);
  tframetrue->SetMaximum(truemax*5);
  sprintf(titlestr_lin,"%s_CtNPTrue_Log.pdf",titlestr.c_str());
  c0f.SaveAs(titlestr_lin);
  delete tframetrue;

  ws->var("sigmaNPTrue")->setConstant(kTRUE);

  return ;
}
/////////////////////////////////////////////////////////


RooDataHist* subtractSidebands(RooWorkspace* ws, RooDataHist* binSubtrSIG, RooDataHist* binSIG, RooDataHist* binSB, float scalefactor, string varName = "Jpsi_CtErr") {
 
  if (binSIG->numEntries() != binSB->numEntries()) {
    cout << "ERROR subtractSidebands : different binning!" << endl;
    return 0;
  }
  RooDataHist* binScaleBKG = new RooDataHist("binScaleBKG","scaled SB",RooArgSet(*(ws->var(varName.c_str())))); 

  //// **** bin-by-bin scaling
  const RooArgSet* argSIG;
  const RooArgSet* argSB;
  for (Int_t i=0; i<binSIG->numEntries(); i++) {
    argSIG = binSIG->get(i);
    argSB = binSB->get(i);
    RooRealVar* thisVar = (RooRealVar*)argSIG->find(varName.c_str());
    ws->var(varName.c_str())->setVal(thisVar->getVal());
    //// *** set minimum as 0.1 to prevent empty PDF
    float wBkg = binSB->weight(*argSB,0,false);
    if (wBkg <= 0.1) wBkg = 0.1;
    binScaleBKG->add(RooArgSet(*(ws->var(varName.c_str()))),wBkg);
    float newWeight = binSIG->weight(*argSIG,0,false) - scalefactor*binSB->weight(*argSB,0,false);
    if (newWeight <= 0.1) newWeight = 0.1;
    binSubtrSIG->add(RooArgSet(*(ws->var(varName.c_str()))),newWeight);
  }
  return binScaleBKG;
}
