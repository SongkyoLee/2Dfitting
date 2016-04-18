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
  
  if (argc!=3) { 
    cout << "select option ( Usage : ./Executable (char)szFinal (int)weight" << endl; 
    return 0; 
  }
  const char* szFinal =  argv[1];
  int weight =  atoi(argv[2]);
  
  // Open RooDataFile
  TFile *fInData  = new TFile(Form("./outRoo_%s/outRoo_%s.root",szFinal,szFinal));
  if (fInData->IsZombie()) { cout << "CANNOT open data root file\n"; return 1; }
  fInData->cd();
  RooDataSet *data;
  if (weight==1) data = (RooDataSet*)fInData->Get("dataJpsiWeight");
  else  data = (RooDataSet*)fInData->Get("dataJpsi");  //Unweighted
  data->SetName("data");


  // Create work space
  RooWorkspace *ws = new RooWorkspace("workspace");
  ws->import(*data);

  //print var and num of events in data
  data->Print();

  // construct plot frame
  RooBinning rbpt(0.0,40.0);
  rbpt.addUniform(400,0.0,45.0);
  RooBinning rby(-2.5,2.5);
  rby.addUniform(50,-2.5,2.5);
  RooBinning rbm(2.6,3.5);
  rbm.addUniform(40,2.6,3.5);
  RooBinning rbethf(0.,120.);
  rbethf.addUniform(60,0.,120.);
  RooBinning rbntrk(0.,350.);
  rbntrk.addUniform(60,0.,350.);
  
  RooPlot *ptFrame = ws->var("Jpsi_Pt")->frame(Bins(45),Range(0.0,45.0));
  ws->var("Jpsi_Pt")->setBinning(rbpt);
  ptFrame->GetXaxis()->SetTitle("p_{T} (GeV/c)");
  ptFrame->GetXaxis()->CenterTitle();
  ptFrame->GetYaxis()->SetTitleOffset(1.5);
  ws->data("data")->plotOn(ptFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerSize(1),Binning(rbpt));
  double ptmax = ptFrame->GetMaximum();
  ptFrame->SetMaximum(4*ptmax);
  ptFrame->SetMinimum(1.);
  //ws->data("data")->plotOn(ptFrame);

  RooPlot *yFrame = ws->var("Jpsi_Y")->frame(Bins(50),Range(-2.5,2.5));
  yFrame->GetXaxis()->SetTitle("y_{lab}");
  yFrame->GetXaxis()->CenterTitle();
  yFrame->GetYaxis()->SetTitleOffset(1.9);
  double ymax = yFrame->GetMaximum();
  yFrame->SetMaximum(1.1*ymax);
  ws->data("data")->plotOn(yFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerSize(1),Binning(rby));

  RooPlot *mFrame = ws ->var("Jpsi_Mass")->frame(Bins(40),Range(2.6,3.5));
  mFrame->GetXaxis()->SetTitle("m_{#mu#mu} (GeV/c^{2})");
  mFrame->GetXaxis()->CenterTitle();
  mFrame->GetYaxis()->SetTitleOffset(1.9);
  double mmax = mFrame->GetMaximum();
  mFrame->SetMaximum(1.1*mmax);
  ws->data("data")->plotOn(mFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerSize(1),Binning(rbm));


  //Draw 1D
  gStyle->SetTitleSize(0.048,"xyz");
  TCanvas* c_pt = new TCanvas("c_pt","c_pt",400,400) ;
  TCanvas* c_y = new TCanvas("c_y","c_y",400,400) ;
  TCanvas* c_m = new TCanvas("c_m","c_m",400,400) ;
  c_pt->cd() ;
  gPad->SetLogy(1);
  gPad->SetLeftMargin(0.15) ; 
  ptFrame->Draw() ;
  c_pt->SaveAs(Form("dir_var/%s_1D_pT.pdf",szFinal)); 
  c_y->cd() ; 
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.18) ; 
  yFrame->Draw() ;
  c_y->SaveAs(Form("dir_var/%s_1D_y.pdf",szFinal)); 
  c_m->cd() ; 
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.19) ; 
  mFrame->Draw() ;
  c_m->SaveAs(Form("dir_var/%s_1D_m.pdf",szFinal)); 


  // Draw 2D

  TCanvas* c_pt_y = new TCanvas("c_pt_y","c_pt_y",600,600) ;
  TH1* h2D_pt_y = ws->data("data")->createHistogram("h2D_pt_y",*(ws->var("Jpsi_Y")),Binning(rby),YVar(*(ws->var("Jpsi_Pt")),Binning(rbpt)));

  c_pt_y->cd();
  gPad->SetRightMargin(0.19) ; 
  gPad->SetLeftMargin(0.12) ; 
  h2D_pt_y -> GetXaxis()->SetTitle("y_{lab}");
  h2D_pt_y -> GetXaxis()->CenterTitle();
  h2D_pt_y -> GetYaxis()->SetTitle("p_{T} (GeV/c)");
  h2D_pt_y -> GetZaxis()->SetTitle("");
  h2D_pt_y -> Draw("colz");
  c_pt_y->SaveAs(Form("dir_var/%s_2D_pT_y.pdf",szFinal));

  TCanvas* c_m_pt = new TCanvas("c_m_pt","c_m_pt",600,600) ;
  TH1* h2D_m_pt = ws->data("data")->createHistogram("h2D_m_pt",*(ws->var("Jpsi_Pt")),Binning(rbpt),YVar(*(ws->var("Jpsi_Mass")),Binning(rbm)));

  c_m_pt->cd();
  gPad->SetRightMargin(0.19) ; 
  gPad->SetLeftMargin(0.12) ; 
  h2D_m_pt -> GetXaxis()->SetTitle("p_{T} (GeV/c)");
  h2D_m_pt -> GetYaxis()->SetTitleOffset(1.1);
  h2D_m_pt -> GetXaxis()->CenterTitle();
  h2D_m_pt -> GetYaxis()->SetTitle("m_{#mu#mu} (GeV/c^{2})");
  h2D_m_pt -> GetZaxis()->SetTitle("");
  h2D_m_pt -> Draw("colz");
  c_m_pt->SaveAs(Form("dir_var/%s_2D_m_pT.pdf",szFinal));

  //only for data
  TCanvas* c_ethf_ntrk;
  TH1* h2D_ethf_ntrk;
  c_ethf_ntrk = new TCanvas("c_ethf_ntrk","c_ethf_ntrk",600,600) ;
  h2D_ethf_ntrk = ws->data("data")->createHistogram("h2D_ethf_ntrk",*(ws->var("Ntracks")),Binning(rbntrk),YVar(*(ws->var("SumET_HFEta4")),Binning(rbethf)));

  c_ethf_ntrk->cd();
  gPad->SetRightMargin(0.19) ; 
  gPad->SetLeftMargin(0.12) ; 
  h2D_ethf_ntrk -> GetXaxis()->SetTitle("N_{tracks}");
  h2D_ethf_ntrk -> GetYaxis()->SetTitleOffset(1.1);
  h2D_ethf_ntrk -> GetXaxis()->CenterTitle();
  h2D_ethf_ntrk -> GetYaxis()->SetTitle("E_{T}^{HF} (GeV/c)");
  h2D_ethf_ntrk -> GetZaxis()->SetTitle("");
  h2D_ethf_ntrk -> Draw("colz");
  c_ethf_ntrk->SaveAs(Form("dir_var/%s_2D_ethf_ntrk.pdf",szFinal));

  return 0;  

}

