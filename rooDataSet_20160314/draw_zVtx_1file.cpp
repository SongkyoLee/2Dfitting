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
	TFile fInData("/home/songkyo/kyo/2Dfitting/rooDataSet_v60402/outRoo_NPMC_Pbp_newcut_zvtxtest/outRoo_NPMC_Pbp_newcut_zvtxtest.root");
	if (fInData.IsZombie()) { cout << "CANNOT open data root file\n"; return 1; }
	fInData.cd();
	RooDataSet *data;
	data = (RooDataSet*)fInData.Get("dataJpsi");  //Unweighted
	data->SetName("data");

	RooDataSet *dataWeight;
	dataWeight = (RooDataSet*)fInData.Get("dataJpsiWeight");  //Weighted
	dataWeight->SetName("dataWeight");

	RooDataSet *dataW;
	dataW = (RooDataSet*)fInData.Get("dataJpsiWCheck");  //only to check weight factor
	dataW->SetName("dataW");


	// Create work space
	RooWorkspace *ws = new RooWorkspace("workspace");
	ws->import(*data);
	ws->import(*dataWeight);

	//print var and num of events in data
	cout << "## unweighted!\n";
	data->Print();
	cout << "## weighted!\n";
	dataWeight->Print();
	cout << "## weight check!\n";
	dataW->Print();

	//// construct plot frame
	// z vtx dist.
	RooBinning rbZ(-30.0,30.0);
	rbZ.addUniform(60,-30.0,30.0);
	
	//RooPlot *zFrame = ws->var("Jpsi_Zvtx")->frame(Range(-30.0,30.0));
	RooPlot *zFrame = ws->var("Jpsi_Zvtx")->frame();
	ws->var("Jpsi_Zvtx")->setBinning(rbZ);
	zFrame->GetXaxis()->SetTitle("primary Z vertex (cm)");
	zFrame->GetXaxis()->CenterTitle();
	zFrame->GetYaxis()->SetTitleOffset(1.5);
	ws->data("data")->plotOn(zFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kBlue),MarkerSize(1),Binning(rbZ));
	ws->data("dataWeight")->plotOn(zFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kRed),MarkerSize(1),Binning(rbZ));
	double zmax = zFrame->GetMaximum();
	zFrame->SetMaximum(1.2*zmax);
	zFrame->SetMinimum(0.);
	//ws->data("data")->plotOn(zFrame);

	//Weight
	RooBinning rbW(0.0,2.0);
	rbW.addUniform(44,0.0,2.0);
	RooWorkspace *ws2 = new RooWorkspace("workspace2");
	ws2->import(*dataW);
//	RooPlot *wFrame = ws2->var("Jpsi_Weight")->frame(Bins(44),Range(0.0,2.0));
	RooPlot *wFrame = ws2->var("Jpsi_Weight")->frame(Range(0.0,2.0));
//	RooPlot *wFrame = ws2->var("Jpsi_Weight")->frame();
	ws2->var("Jpsi_Weight")->setBinning(rbW);
	wFrame->GetXaxis()->SetTitle("weighting factor");
	wFrame->GetXaxis()->CenterTitle();
	wFrame->GetYaxis()->SetTitleOffset(1.5);
	ws2->data("dataW")->plotOn(wFrame,DataError(RooAbsData::SumW2),XErrorSize(0),MarkerColor(kGreen),MarkerSize(1),Binning(rbW));
	double wmax = wFrame->GetMaximum();
	wFrame->SetMaximum(1.2*wmax);
	wFrame->SetMinimum(0.);

	//Draw 1D
  gStyle->SetTitleSize(0.048,"xyz");

	//TLegend *legUR = new TLegend(0.58,0.71,0.90,0.90,NULL,"brNDC");
	TLegend *legUR = new TLegend(0.62,0.75,0.90,0.90,NULL,"brNDC");
	legUR->SetFillStyle(0); legUR->SetBorderSize(0);  legUR->SetShadowColor(0); legUR->SetMargin(0.2); 
	legUR->SetTextSize(0.040);

//	TLine* t1 = new TLine(-10,0,-10,2000);
//	t1->SetLineWidth(2.5); t1->SetLineStyle(7);
//	TLine* t2 = new TLine(10,0,10,2000);
//	t2->SetLineWidth(2.5); t2->SetLineStyle(7);
		
	TCanvas* c1 = new TCanvas("c1","c1",600,600) ;
  c1->cd() ;
	gPad->SetLogy(0);
	gPad->SetLeftMargin(0.15) ; 
	zFrame->Draw();
	legUR->AddEntry(hRed,"Weight","p");
	legUR->AddEntry(hBlue,"No Weight","p");
	legUR->Draw("same");
//	t1->Draw();
//	t2->Draw();
	c1->SaveAs("NPMC_Pbp_zVtx_1file.pdf");	
	legUR->Clear();

	TCanvas* c2 = new TCanvas("c2","c2",600,600) ;
  c2->cd() ;
	gPad->SetLogy(0);
	gPad->SetLeftMargin(0.15) ; 
	wFrame->Draw() ;
	c2->SaveAs("NPMC_Pbp_weightF_1file.pdf");	

	return 0;	 

}

