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

  //Global object for TLegend
  TH1F* hRed = new TH1F("hRed","hRed",100,200,300);
  hRed->SetMarkerColor(kRed); hRed->SetMarkerSize(1.2); hRed->SetMarkerStyle(kFullCircle);
  TH1F* hBlue = new TH1F("hBlue","hBlue",100,200,300); 
  hBlue->SetMarkerColor(kBlue); hBlue->SetMarkerSize(1.2); hBlue->SetMarkerStyle(kFullCircle);
  TH1F* hGreen = new TH1F("hGreen","hGreen",100,200,300);
  hGreen->SetMarkerColor(kGreen); hGreen->SetMarkerSize(1.2); hGreen->SetMarkerStyle(kFullCircle);
  
  // Open RooDataFile
  TFile fInData("./outRoo_Data_Pbp_newcut_v1/outRoo_Data_Pbp_newcut_v1.root");
  if (fInData.IsZombie()) { cout << "CANNOT open data root file\n"; return 1; }
  fInData.cd();

  RooDataSet *dataSS;
  dataSS = (RooDataSet*)fInData.Get("dataJpsiSSWeight");  //Weighted same sign
  dataSS->SetName("dataSS");

  RooDataSet *dataWeight;
  dataWeight = (RooDataSet*)fInData.Get("dataJpsiWeight");  //Weighted
  dataWeight->SetName("dataWeight");

  // Create work space
  RooWorkspace *ws = new RooWorkspace("workspace");
  ws->import(*dataSS);
  ws->import(*dataWeight);

  //print var and num of events in data
  cout << "## unweighted!\n";
  dataSS->Print();
  cout << "## weighted!\n";
  dataWeight->Print();

  //// construct plot frame
  // z vtx dist.
  RooBinning rbM(2.6,3.5);
  rbM.addUniform(60,2.6,3.5);
  cout << "1" << endl;
  
  //RooPlot *mFrame = ws->var("Jpsi_Mass")->frame(Range(2.6,3.5));
  RooPlot *mFrame = ws->var("Jpsi_Mass")->frame();
  ws->var("Jpsi_Mass")->setBinning(rbM);
  mFrame->GetXaxis()->SetTitle("M (GeV)");
  mFrame->GetXaxis()->CenterTitle();
  mFrame->GetYaxis()->SetTitleOffset(1.5);
  cout << "1" << endl;
  ws->data("dataSS")->plotOn(mFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerSize(1),Binning(rbM));
  ws->data("dataWeight")->plotOn(mFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kRed),MarkerSize(1),Binning(rbM));
  cout << "1" << endl;
  double zmax = mFrame->GetMaximum();
  mFrame->SetMaximum(1.2*zmax);
  mFrame->SetMinimum(0.);
  //ws->data("data")->plotOn(mFrame);

  //Draw 1D
  gStyle->SetTitleSize(0.048,"xyz");

  //TLegend *legUR = new TLegend(0.58,0.71,0.90,0.90,NULL,"brNDC");
  TLegend *legUR = new TLegend(0.62,0.75,0.90,0.90,NULL,"brNDC");
  legUR->SetFillStyle(0); legUR->SetBorderSize(0);  legUR->SetShadowColor(0); legUR->SetMargin(0.2); 
  legUR->SetTextSize(0.040);

  TCanvas* c1 = new TCanvas("c1","c1",600,600) ;
  c1->cd() ;
  gPad->SetLogy(0);
  gPad->SetLeftMargin(0.15) ; 
  mFrame->Draw();
  legUR->AddEntry(hRed,"opposite sign","p");
  legUR->AddEntry(hBlue,"same sign","p");
  legUR->Draw("same");
  c1->SaveAs("Data_pPb_m_1file.pdf"); 
  legUR->Clear();

  return 0;  

}

