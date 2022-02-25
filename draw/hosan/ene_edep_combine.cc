#include <iostream>
#include <fstream>
#include <TStyle.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TPad.h>
#include <TF1.h>
#include <TLegend.h>
#include <TApplication.h>

using namespace std;

void ene_edep_combine(){

  gStyle->SetOptStat(000000);

  //////////////////
  //get resolutions/
  //////////////////
  TFile *f_x_20_y_20_z_200  = new TFile("output/ene/files/ene_edep_res_x_20mm_y_20mm_z_200mm.root", "READ");
  TFile *f_x_20_y_20_z_300  = new TFile("output/ene/files/ene_edep_res_x_20mm_y_20mm_z_300mm.root", "READ");
  TFile *f_x_20_y_20_z_400  = new TFile("output/ene/files/ene_edep_res_x_20mm_y_20mm_z_400mm.root", "READ");
  TFile *f_x_30_y_30_z_200  = new TFile("output/ene/files/ene_edep_res_x_30mm_y_30mm_z_200mm.root", "READ");
  TFile *f_x_30_y_30_z_300  = new TFile("output/ene/files/ene_edep_res_x_30mm_y_30mm_z_300mm.root", "READ");
  TFile *f_x_30_y_30_z_400  = new TFile("output/ene/files/ene_edep_res_x_30mm_y_30mm_z_400mm.root", "READ");
  TFile *f_x_40_y_40_z_200  = new TFile("output/ene/files/ene_edep_res_x_40mm_y_40mm_z_200mm.root", "READ");
  TFile *f_x_40_y_40_z_300  = new TFile("output/ene/files/ene_edep_res_x_40mm_y_40mm_z_300mm.root", "READ");
  TFile *f_x_40_y_40_z_400  = new TFile("output/ene/files/ene_edep_res_x_40mm_y_40mm_z_400mm.root", "READ");

  TFile *f_x_20_y_20_z_500  = new TFile("output/ene/files/ene_edep_res_x_20mm_y_20mm_z_500mm.root", "READ");
  TFile *f_x_30_y_30_z_500  = new TFile("output/ene/files/ene_edep_res_x_30mm_y_30mm_z_500mm.root", "READ");
  TFile *f_x_40_y_40_z_500  = new TFile("output/ene/files/ene_edep_res_x_40mm_y_40mm_z_500mm.root", "READ");
  TFile *f_x_20_y_20_z_600  = new TFile("output/ene/files/ene_edep_res_x_20mm_y_20mm_z_600mm.root", "READ");
  TFile *f_x_30_y_30_z_600  = new TFile("output/ene/files/ene_edep_res_x_30mm_y_30mm_z_600mm.root", "READ");
  TFile *f_x_40_y_40_z_600  = new TFile("output/ene/files/ene_edep_res_x_40mm_y_40mm_z_600mm.root", "READ");


  TGraphErrors *gr_x_20_y_20_z_200  = (TGraphErrors*)f_x_20_y_20_z_200->Get("gr_res");
  TGraphErrors *gr_x_20_y_20_z_300  = (TGraphErrors*)f_x_20_y_20_z_300->Get("gr_res");
  TGraphErrors *gr_x_20_y_20_z_400  = (TGraphErrors*)f_x_20_y_20_z_400->Get("gr_res");
  TGraphErrors *gr_x_30_y_30_z_200  = (TGraphErrors*)f_x_30_y_30_z_200->Get("gr_res");
  TGraphErrors *gr_x_30_y_30_z_300  = (TGraphErrors*)f_x_30_y_30_z_300->Get("gr_res");
  TGraphErrors *gr_x_30_y_30_z_400  = (TGraphErrors*)f_x_30_y_30_z_400->Get("gr_res");
  TGraphErrors *gr_x_40_y_40_z_200  = (TGraphErrors*)f_x_40_y_40_z_200->Get("gr_res");
  TGraphErrors *gr_x_40_y_40_z_300  = (TGraphErrors*)f_x_40_y_40_z_300->Get("gr_res");
  TGraphErrors *gr_x_40_y_40_z_400  = (TGraphErrors*)f_x_40_y_40_z_400->Get("gr_res");

  TGraphErrors *gr_x_20_y_20_z_500  = (TGraphErrors*)f_x_20_y_20_z_500->Get("gr_res");
  TGraphErrors *gr_x_30_y_30_z_500  = (TGraphErrors*)f_x_30_y_30_z_500->Get("gr_res");
  TGraphErrors *gr_x_40_y_40_z_500  = (TGraphErrors*)f_x_40_y_40_z_500->Get("gr_res");
  TGraphErrors *gr_x_20_y_20_z_600  = (TGraphErrors*)f_x_20_y_20_z_600->Get("gr_res");
  TGraphErrors *gr_x_30_y_30_z_600  = (TGraphErrors*)f_x_30_y_30_z_600->Get("gr_res");
  TGraphErrors *gr_x_40_y_40_z_600  = (TGraphErrors*)f_x_40_y_40_z_600->Get("gr_res");

  ////////////////////
  //draw resolutions//
  ////////////////////
  TCanvas *c_res = new TCanvas("c_res", "c_res", 200, 10, 1000, 500);
  c_res->Divide(2,1);
  c_res->cd(1);
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  gr_x_20_y_20_z_200->SetMarkerStyle(24);
  gr_x_20_y_20_z_300->SetMarkerStyle(24);
  gr_x_20_y_20_z_400->SetMarkerStyle(24);
  gr_x_30_y_30_z_200->SetMarkerStyle(25);
  gr_x_30_y_30_z_300->SetMarkerStyle(25);
  gr_x_30_y_30_z_400->SetMarkerStyle(25);
  gr_x_40_y_40_z_200->SetMarkerStyle(26);
  gr_x_40_y_40_z_300->SetMarkerStyle(26);
  gr_x_40_y_40_z_400->SetMarkerStyle(26);

  gr_x_20_y_20_z_500->SetMarkerStyle(24);
  gr_x_30_y_30_z_500->SetMarkerStyle(25);
  gr_x_40_y_40_z_500->SetMarkerStyle(26);
  gr_x_20_y_20_z_600->SetMarkerStyle(24);
  gr_x_30_y_30_z_600->SetMarkerStyle(25);
  gr_x_40_y_40_z_600->SetMarkerStyle(26);

  gr_x_20_y_20_z_200->SetMarkerColor(1);
  gr_x_20_y_20_z_300->SetMarkerColor(2);
  gr_x_20_y_20_z_400->SetMarkerColor(3);
  gr_x_30_y_30_z_200->SetMarkerColor(1);
  gr_x_30_y_30_z_300->SetMarkerColor(2);
  gr_x_30_y_30_z_400->SetMarkerColor(3);
  gr_x_40_y_40_z_200->SetMarkerColor(1);
  gr_x_40_y_40_z_300->SetMarkerColor(2);
  gr_x_40_y_40_z_400->SetMarkerColor(3);

  gr_x_20_y_20_z_500->SetMarkerColor(4);
  gr_x_30_y_30_z_500->SetMarkerColor(4);
  gr_x_40_y_40_z_500->SetMarkerColor(4);
  gr_x_20_y_20_z_600->SetMarkerColor(6);
  gr_x_30_y_30_z_600->SetMarkerColor(6);
  gr_x_40_y_40_z_600->SetMarkerColor(6);

  gr_x_20_y_20_z_200->SetTitle("");
  gr_x_20_y_20_z_200->GetXaxis()->SetTitle("Photon energy [GeV]");
  gr_x_20_y_20_z_200->GetYaxis()->SetTitle("Resolution [%]");
  gr_x_20_y_20_z_200->GetYaxis()->SetTitleOffset(1.5);
  gr_x_20_y_20_z_200->GetXaxis()->SetLimits(0, 11);
  gr_x_20_y_20_z_200->GetYaxis()->SetRangeUser(0, 30);
  gr_x_20_y_20_z_200->Draw("AP");//"X" : do not draw error bars
  gr_x_20_y_20_z_300->Draw("P SAME");//"X" : do not draw error bars
  gr_x_20_y_20_z_400->Draw("P SAME");//"X" : do not draw error bars
  gr_x_30_y_30_z_200->Draw("P SAME");//"X" : do not draw error bars
  gr_x_30_y_30_z_300->Draw("P SAME");//"X" : do not draw error bars
  gr_x_30_y_30_z_400->Draw("P SAME");//"X" : do not draw error bars
  gr_x_40_y_40_z_200->Draw("P SAME");//"X" : do not draw error bars
  gr_x_40_y_40_z_300->Draw("P SAME");//"X" : do not draw error bars
  gr_x_40_y_40_z_400->Draw("P SAME");//"X" : do not draw error bars

  gr_x_20_y_20_z_500->Draw("P SAME");//"X" : do not draw error bars
  gr_x_30_y_30_z_500->Draw("P SAME");//"X" : do not draw error bars
  gr_x_40_y_40_z_500->Draw("P SAME");//"X" : do not draw error bars
  gr_x_20_y_20_z_600->Draw("P SAME");//"X" : do not draw error bars
  gr_x_30_y_30_z_600->Draw("P SAME");//"X" : do not draw error bars
  gr_x_40_y_40_z_600->Draw("P SAME");//"X" : do not draw error bars


  TLegend *leg = new TLegend(0.1, 0.1, 0.5, 0.9);
  leg->AddEntry(gr_x_20_y_20_z_200,  "20X20X200 mm^{3}", "p");
  leg->AddEntry(gr_x_20_y_20_z_300,  "20X20X300 mm^{3}", "p");
  leg->AddEntry(gr_x_20_y_20_z_400,  "20X20X400 mm^{3}", "p");
  leg->AddEntry(gr_x_20_y_20_z_500,  "20X20X500 mm^{3}", "p");
  leg->AddEntry(gr_x_20_y_20_z_600,  "20X20X600 mm^{3}", "p");
  leg->AddEntry(gr_x_30_y_30_z_200,  "30X30X200 mm^{3}", "p");
  leg->AddEntry(gr_x_30_y_30_z_300,  "30X30X300 mm^{3}", "p");
  leg->AddEntry(gr_x_30_y_30_z_400,  "30X30X400 mm^{3}", "p");
  leg->AddEntry(gr_x_30_y_30_z_500,  "30X30X500 mm^{3}", "p");
  leg->AddEntry(gr_x_30_y_30_z_600,  "30X30X600 mm^{3}", "p");
  leg->AddEntry(gr_x_40_y_40_z_200,  "40X40X200 mm^{3}", "p");
  leg->AddEntry(gr_x_40_y_40_z_300,  "40X40X300 mm^{3}", "p");
  leg->AddEntry(gr_x_40_y_40_z_400,  "40X40X400 mm^{3}", "p");
  leg->AddEntry(gr_x_40_y_40_z_500,  "40X40X500 mm^{3}", "p");
  leg->AddEntry(gr_x_40_y_40_z_600,  "40X40X600 mm^{3}", "p");
  c_res->cd(2);
  leg->Draw();

  c_res->Print("output/ene/pics/combine/ene_edep_resolution.pdf");
  c_res->Print("output/ene/pics/combine/png/ene_edep_resolution.png");

  return;
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("energyResolution", &argc, argv);

  ene_edep_combine();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
