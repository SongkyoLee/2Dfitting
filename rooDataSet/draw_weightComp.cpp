#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <TString.h>
#include <string>
#include <math.h>

#include <TROOT.h>
#include "TSystem.h"
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TMath.h>
#include <math.h>
#include <TH1.h>
#include <TH2.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include "TClonesArray.h"
#include <TAxis.h>
#include <cmath>
#include <TLorentzRotation.h>

#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TRandom.h>
#include <TLine.h>

#include <RooFit.h>
#include <RooGlobalFunc.h>
#include <RooCategory.h>
#include <RooHistPdfConv.h>
#include <RooGenericPdf.h>
#include <RooFFTConvPdf.h>
#include <RooWorkspace.h>
#include <RooBinning.h>
#include <RooHistPdf.h>
#include <RooProdPdf.h>
#include <RooAddPdf.h>
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooHist.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooConstVar.h>

//#include <fit2DData.h>

using namespace std;
using namespace RooFit;

int main (int argc, char* argv[])
{
  // set style
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  gStyle->SetTitleFillColor(0);
  gStyle->SetStatColor(0);

  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameLineColor(kBlack);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetPadColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);

  gStyle->SetTextSize(0.04);
  gStyle->SetTextFont(42);
  gStyle->SetLabelFont(42,"xyz");
  gStyle->SetTitleFont(42,"xyz");
  gStyle->SetTitleSize(0.048,"xyz");

  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.12) ; 

  gStyle->SetEndErrorSize(0);

  //Global object for TLegend
  TH1F* hRed = new TH1F("hRed","hRed",100,200,300);
  hRed->SetMarkerColor(kRed); hRed->SetMarkerSize(1.2); hRed->SetMarkerStyle(kOpenCircle);
  TH1F* hBlue = new TH1F("hBlue","hBlue",100,200,300); 
  hBlue->SetMarkerColor(kBlue); hBlue->SetMarkerSize(1.2); hBlue->SetMarkerStyle(kFullCircle);
  TH1F* hGreen = new TH1F("hGreen","hGreen",100,200,300);
  hGreen->SetMarkerColor(kGreen); hGreen->SetMarkerSize(1.2); hGreen->SetMarkerStyle(kFullCircle);

  bool isPrompt=0;
  bool is1stRun=0;
    
  //if (argc==1) { 
  if (argc!=3) { 
    cout << "select option ( Usage : ./Executable isPrompt is1stRun)" << endl; 
    return 0; 
  }
  isPrompt =  atoi(argv[1]);
  is1stRun =  atoi(argv[2]);

  // Open RooDataFile
  TFile *fInData;
  const char* sampleName;
  if (isPrompt && is1stRun){
    cout << "****** Prompt MC $ 1st Run is selected ******" <<endl; 
    sampleName = "PRMC_Pbp";
    fInData = new TFile("outRoo_PRMC_Pbp_newcut_off8M/outRoo_PRMC_Pbp_newcut_off8M.root");
  }
  else if (isPrompt && !is1stRun){
    cout << "****** Prompt MC $ 2nd Run is selected ******" <<endl; 
    sampleName = "PRMC_pPb";
    fInData  = new TFile("outRoo_PRMC_pPb_newcut_off8M/outRoo_PRMC_pPb_newcut_off8M.root");
  }
  else if (!isPrompt && is1stRun){
    cout << "****** Non-Prompt MC $ 1st Run is selected ******" <<endl; 
    sampleName = "NPMC_Pbp";
    fInData = new TFile("outRoo_NPMC_Pbp_newcut_off8M/outRoo_NPMC_Pbp_newcut_off8M.root");
  }
  else {
    cout << "****** Non-Prompt MC $ 2nd Run is selected ******" <<endl; 
    sampleName = "NPMC_pPb";
    fInData = new TFile("outRoo_NPMC_pPb_newcut_off8M/outRoo_NPMC_pPb_newcut_off8M.root");
  }

  if (fInData->IsZombie()) { cout << "CANNOT open data root file\n"; return 1; }
  fInData->cd();
  RooDataSet *data;
  data = (RooDataSet*)fInData->Get("dataJpsi");  //Unweighted
  data->SetName("data");

  RooDataSet *dataWeight;
  dataWeight = (RooDataSet*)fInData->Get("dataJpsiWeight");  //Weighted
  dataWeight->SetName("dataWeight");

  // Create work space
  RooWorkspace *ws = new RooWorkspace("workspace");
  ws->import(*data);
  ws->import(*dataWeight);

  //print var and num of events in data
  cout << "## unweighted!\n";
  data->Print();
  cout << "## weighted!\n";
  dataWeight->Print();

  //// construct plot frame

  //---- pT 
  RooBinning rbpt(0.0,30.0);
  //rbpt.addUniform(45,0.0,30.0);
  rbpt.addUniform(15,0.0,4.0);
  rbpt.addUniform(50,4.0,30.0);
  //RooPlot *ptFrame = ws->var("Jpsi_Pt")->frame(Bins(45),Range(0.0,35.0));
  RooPlot *ptFrame = ws->var("Jpsi_Pt")->frame(Range(0.0,30.0));
  //RooPlot *ptFrame = ws->var("Jpsi_Pt")->frame();
  ws->var("Jpsi_Pt")->setBinning(rbpt);
  ptFrame->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  ptFrame->GetXaxis()->CenterTitle();
  ptFrame->GetYaxis()->SetTitleOffset(1.5);
  ws->data("data")->plotOn(ptFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerStyle(kFullCircle),LineColor(kBlue),MarkerSize(1),Binning(rbpt));
  ws->data("dataWeight")->plotOn(ptFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kRed),MarkerStyle(kOpenCircle),LineColor(kRed),MarkerSize(1),Binning(rbpt));
  double ptmax = ptFrame->GetMaximum();
//  ptFrame->SetMaximum(4*ptmax);
  ptFrame->SetMaximum(1.1*ptmax);
  ptFrame->SetMinimum(1.);
  //ws->data("data")->plotOn(ptFrame);

  //---- rapidity 
  RooBinning rby(-2.5,2.5);
  rby.addUniform(50,-2.5,2.5);
  RooPlot *yFrame = ws->var("Jpsi_Y")->frame();
  yFrame->GetXaxis()->SetTitle("y_{lab}");
  yFrame->GetXaxis()->CenterTitle();
  yFrame->GetYaxis()->SetTitleOffset(1.9);
  ws->data("data")->plotOn(yFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerStyle(kFullCircle),LineColor(kBlue),MarkerSize(1),Binning(rby));
  ws->data("dataWeight")->plotOn(yFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kRed),MarkerStyle(kOpenCircle),LineColor(kRed),MarkerSize(1),Binning(rby));
  double ymax = yFrame->GetMaximum();
  yFrame->SetMaximum(1.1*ymax);

  //---- mass 
  RooBinning rbm(2.6,3.5);
//  rbm.addUniform(40,2.6,3.5);
  rbm.addUniform(80,2.6,3.5);
  RooPlot *mFrame = ws ->var("Jpsi_Mass")->frame();
  mFrame->GetXaxis()->SetTitle("m_{#mu#mu} (GeV/c^{2})");
  mFrame->GetXaxis()->CenterTitle();
  mFrame->GetYaxis()->SetTitleOffset(1.9);
  ws->data("data")->plotOn(mFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerStyle(kFullCircle),LineColor(kBlue),MarkerSize(1),Binning(rbm));
  ws->data("dataWeight")->plotOn(mFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kRed),MarkerStyle(kOpenCircle),LineColor(kRed),MarkerSize(1),Binning(rbm));
  double mmax = mFrame->GetMaximum();
  mFrame->SetMaximum(1.1*mmax);
  
  //---- Ct 
    RooBinning rbct(-0.5,2.0);
    RooPlot *ctFrame;
  if (isPrompt){
    rbct.addUniform(80,-0.3,0.3);
    ctFrame = ws->var("Jpsi_Ct")->frame(Range(-0.3,0.3));
  }
  else {
    rbct.addUniform(80,-0.5,2.0);
    ctFrame = ws->var("Jpsi_Ct")->frame(Range(-0.5,2.0));
  }
  ctFrame->GetXaxis()->SetTitle("#font[12]{l}_{J/#psi} (mm)");
  ctFrame->GetXaxis()->CenterTitle();
  ctFrame->GetYaxis()->SetTitleOffset(1.9);
  ws->data("data")->plotOn(ctFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerStyle(kFullCircle),LineColor(kBlue),MarkerSize(1),Binning(rbct));
  ws->data("dataWeight")->plotOn(ctFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kRed),MarkerStyle(kOpenCircle),LineColor(kRed),MarkerSize(1),Binning(rbct));
  double ctmax = ctFrame->GetMaximum();
  ctFrame->SetMaximum(1.1*ctmax);

  //---- CtErr  
  RooBinning rbcterr(0.,0.3);
  rbcterr.addUniform(80,0.,0.3);
  RooPlot *ctErrFrame = ws->var("Jpsi_CtErr")->frame(Range(0.,0.3));
  ctErrFrame->GetXaxis()->SetTitle("#font[12]{l}_{J/#psi} error (mm)");
  ctErrFrame->GetXaxis()->CenterTitle();
  ctErrFrame->GetYaxis()->SetTitleOffset(1.9);
  ws->data("data")->plotOn(ctErrFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerStyle(kFullCircle),LineColor(kBlue),MarkerSize(1),Binning(rbcterr));
  ws->data("dataWeight")->plotOn(ctErrFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kRed),MarkerStyle(kOpenCircle),LineColor(kRed),MarkerSize(1),Binning(rbcterr));
  double cterrmax = ctErrFrame->GetMaximum();
  ctErrFrame->SetMaximum(1.1*cterrmax);

  //---- CtTrue 
  RooBinning rbcttrue(-0.1,2.);
  RooPlot *ctTrueFrame;
  if (isPrompt){
    rbcttrue.addUniform(80,-0.1,0.1);
    ctTrueFrame = ws->var("Jpsi_CtTrue")->frame(-0.1,0.1);
  } else {
    rbcttrue.addUniform(80,-0.1,2.);
    ctTrueFrame = ws->var("Jpsi_CtTrue")->frame(-0.1,2);
  }
  ctTrueFrame->GetXaxis()->SetTitle("#font[12]{l}_{J/#psi} True (mm)");
  ctTrueFrame->GetXaxis()->CenterTitle();
  ctTrueFrame->GetYaxis()->SetTitleOffset(1.9);
  ws->data("data")->plotOn(ctTrueFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerStyle(kFullCircle),LineColor(kBlue),MarkerSize(1),Binning(rbcttrue));
  ws->data("dataWeight")->plotOn(ctTrueFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kRed),MarkerStyle(kOpenCircle),LineColor(kRed),MarkerSize(1),Binning(rbcttrue));
  double cttruemax = ctTrueFrame->GetMaximum();
  ctTrueFrame->SetMaximum(1.1*cttruemax);

  
  /////Draw 1D
  gStyle->SetTitleSize(0.048,"xyz");
  
  TLegend *legUR = new TLegend(0.62,0.75,0.90,0.90,NULL,"brNDC");
  legUR->SetFillStyle(0); legUR->SetBorderSize(0);  legUR->SetShadowColor(0); legUR->SetMargin(0.2); 
  legUR->SetTextSize(0.040);
  
  TCanvas* c_pt = new TCanvas("c_pt","c_pt",600,600) ;
  TCanvas* c_y = new TCanvas("c_y","c_y",600,600) ;
  TCanvas* c_m = new TCanvas("c_m","c_m",600,600) ;
  TCanvas* c_ct = new TCanvas("c_ct","c_ct",600,600) ;
  TCanvas* c_cterr = new TCanvas("c_cterr","c_cterr",600,600) ;
  TCanvas* c_cttrue = new TCanvas("c_cttrue","c_cttrue",600,600) ;
  
  c_pt->cd() ;
  gPad->SetLeftMargin(0.15) ; 
  ptFrame->Draw() ;
  legUR->AddEntry(hRed,"Weight","p");
  legUR->AddEntry(hBlue,"No Weight","p");
  legUR->Draw("same");
  gPad->SetLogy(0);
  c_pt->SaveAs(Form("weightComp/%s_pT.pdf",sampleName));  
  gPad->SetLogy(1);
  ptFrame->SetMaximum(4*ptmax);
  c_pt->SaveAs(Form("weightComp/%s_pT_log1.pdf",sampleName)); 
  
  c_y->cd() ; 
  gPad->SetLeftMargin(0.18) ; 
  gPad->SetLogy(0);
  yFrame->Draw() ;
  legUR->Draw("same");
  c_y->SaveAs(Form("weightComp/%s_y.pdf",sampleName));  
  
  c_m->cd() ; 
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.19) ; 
  mFrame->Draw() ;
  legUR->Draw("same");
  gPad->SetLogy(0);
  c_m->SaveAs(Form("weightComp/%s_m.pdf",sampleName));  
  gPad->SetLogy(1);
  mFrame->SetMaximum(4.*mmax);
  c_m->SaveAs(Form("weightComp/%s_m_log1.pdf",sampleName)); 

  c_ct->cd() ; 
  gPad->SetLeftMargin(0.18) ; 
  gPad->SetLogy(0);
  ctFrame->Draw() ;
  legUR->Draw("same");
  c_ct->SaveAs(Form("weightComp/%s_ct.pdf",sampleName));  
  gPad->SetLogy(1);
  ctFrame->SetMaximum(4.*mmax);
  c_ct->SaveAs(Form("weightComp/%s_ct_log1.pdf",sampleName)); 

  c_cterr->cd() ; 
  gPad->SetLeftMargin(0.18) ; 
  gPad->SetLogy(0);
  ctErrFrame->Draw() ;
  legUR->Draw("same");
  c_cterr->SaveAs(Form("weightComp/%s_ctErr.pdf",sampleName));  

  c_cttrue->cd() ; 
  gPad->SetLeftMargin(0.18) ; 
  gPad->SetLogy(0);
  ctTrueFrame->Draw() ;
  legUR->Draw("same");
  c_cttrue->SaveAs(Form("weightComp/%s_ctTrue.pdf",sampleName));  

  return 0;  

}

