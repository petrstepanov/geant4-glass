#include <iostream>
#include <fstream>
#include <TStyle.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TRandom3.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TPad.h>
#include <TGraphErrors.h>
#include <TApplication.h>

using namespace std;
void ene_edep_res_test(double crys_x = 0, double crys_y = 0, double crys_z = 0){

  gStyle->SetOptStat(000000);

  TChain *ch_10GeV  = new TChain("t");
  TChain *ch_775GeV = new TChain("t");
  TChain *ch_55GeV  = new TChain("t");
  TChain *ch_325GeV = new TChain("t");
  TChain *ch_1GeV   = new TChain("t");
  TChain *ch_05GeV  = new TChain("t");

  for(int i = 0 ; i < 10 ; i++){
    ch_10GeV->Add(Form("./resolution_x_%.0fmm_y_%.0fmm_z_%.0fmm_10GeV_4m_%d.root", crys_x, crys_y, crys_z, i+1));
    ch_775GeV->Add(Form("./resolution_x_%.0fmm_y_%.0fmm_z_%.0fmm_7.75GeV_4m_%d.root", crys_x, crys_y, crys_z, i+1));
    ch_55GeV->Add(Form("./resolution_x_%.0fmm_y_%.0fmm_z_%.0fmm_5.5GeV_4m_%d.root", crys_x, crys_y, crys_z, i+1));
    ch_325GeV->Add(Form("./resolution_x_%.0fmm_y_%.0fmm_z_%.0fmm_3.25GeV_4m_%d.root", crys_x, crys_y, crys_z, i+1));
    ch_1GeV->Add(Form("./resolution_x_%.0fmm_y_%.0fmm_z_%.0fmm_1GeV_4m_%d.root", crys_x, crys_y, crys_z, i+1));
    ch_05GeV->Add(Form("./resolution_x_%.0fmm_y_%.0fmm_z_%.0fmm_0.5GeV_4m_%d.root", crys_x, crys_y, crys_z, i+1));
  }

  /////////////
  //constants//
  /////////////

  const int array = 9;
  int x, y;


  /////////////
  //variables//
  /////////////

  double edep_10GeV[array];  ch_10GeV->SetBranchAddress("edep", edep_10GeV);
  double edep_775GeV[array]; ch_775GeV->SetBranchAddress("edep", edep_775GeV);
  double edep_55GeV[array];  ch_55GeV->SetBranchAddress("edep", edep_55GeV);
  double edep_325GeV[array]; ch_325GeV->SetBranchAddress("edep", edep_325GeV);
  double edep_1GeV[array];   ch_1GeV->SetBranchAddress("edep", edep_1GeV);
  double edep_05GeV[array];  ch_05GeV->SetBranchAddress("edep", edep_05GeV);

  double totedep_10GeV  = 0.;
  double totedep_775GeV = 0.;
  double totedep_55GeV  = 0.;
  double totedep_325GeV = 0.;
  double totedep_1GeV   = 0.;
  double totedep_05GeV  = 0.;

  /////////////////
  //file to write//
  /////////////////

  TFile *f_out = new TFile(Form("output/ene/files/ene_edep_res_x_%.0fmm_y_%.0fmm_z_%.0fmm.root", crys_x, crys_y, crys_z), "RECREATE");

  //////////////
  //histograms//
  //////////////
  TH1D *h_edep_10GeV = new TH1D("h_edep_10GeV", "h_edep_10GeV", 100, 0, 10);
  TH1D *h_edep_775GeV = new TH1D("h_edep_775GeV", "h_edep_775GeV", 100, 0, 7.75);
  TH1D *h_edep_55GeV = new TH1D("h_edep_55GeV", "h_edep_55GeV", 100, 0, 5.5);
  TH1D *h_edep_325GeV = new TH1D("h_edep_325GeV", "h_edep_325GeV", 100, 0, 3.25);
  TH1D *h_edep_1GeV = new TH1D("h_edep_1GeV", "h_edep_1GeV", 100, 0, 1);
  TH1D *h_edep_05GeV = new TH1D("h_edep_05GeV", "h_edep_05GeV", 100, 0, 0.5);

  /////////////////
  //start filling//
  /////////////////
  int nent = ch_10GeV->GetEntries();

  for(int i = 0 ; i < nent ; i++){
    ch_10GeV->GetEntry(i);
    ch_775GeV->GetEntry(i);
    ch_55GeV->GetEntry(i);
    ch_325GeV->GetEntry(i);
    ch_1GeV->GetEntry(i);
    ch_05GeV->GetEntry(i);

    totedep_10GeV = 0.;
    totedep_775GeV = 0.;
    totedep_55GeV = 0.;
    totedep_325GeV = 0.;
    totedep_1GeV = 0.;
    totedep_05GeV = 0.;

    for(int j = 0 ; j < array ; j++){
      totedep_10GeV += edep_10GeV[j]*1e-3;//MeV to GeV
      totedep_775GeV += edep_775GeV[j]*1e-3;//MeV to GeV
      totedep_55GeV += edep_55GeV[j]*1e-3;//MeV to GeV
      totedep_325GeV += edep_325GeV[j]*1e-3;//MeV to GeV
      totedep_1GeV += edep_1GeV[j]*1e-3;//MeV to GeV
      totedep_05GeV += edep_05GeV[j]*1e-3;//MeV to GeV
    }

    h_edep_10GeV->Fill(totedep_10GeV);
    h_edep_775GeV->Fill(totedep_775GeV);
    h_edep_55GeV->Fill(totedep_55GeV);
    h_edep_325GeV->Fill(totedep_325GeV);
    h_edep_1GeV->Fill(totedep_1GeV);
    h_edep_05GeV->Fill(totedep_05GeV);
  }

  //////////////////
  //plot the plots//
  //////////////////

  TCanvas *c_edep = new TCanvas("c_edep", "c_edep", 200, 10, 3000, 2000);
  c_edep->Divide(3,2);
  c_edep->cd(1);
  h_edep_10GeV->Draw();

  c_edep->cd(2);
  h_edep_775GeV->Draw();

  c_edep->cd(3);
  h_edep_55GeV->Draw();

  c_edep->cd(4);
  h_edep_325GeV->Draw();

  c_edep->cd(5);
  h_edep_1GeV->Draw();

  c_edep->cd(6);
  h_edep_05GeV->Draw();

  ////////////////
  //fit the fits//
  ////////////////
  TF1 *f_10GeV;// = new TF1("f_10GeV", "gaus");
  TF1 *f_775GeV;// = new TF1("f_775GeV", "gaus");
  TF1 *f_55GeV;// = new TF1("f_55GeV", "gaus");
  TF1 *f_325GeV;// = new TF1("f_325GeV", "gaus");
  TF1 *f_1GeV;// = new TF1("f_1GeV", "gaus");
  TF1 *f_05GeV;// = new TF1("f_05GeV", "gaus");

  f_10GeV = new TF1("f_10GeV", "gaus");
  f_775GeV = new TF1("f_775GeV", "gaus");
  f_55GeV = new TF1("f_55GeV", "gaus");
  f_325GeV = new TF1("f_325GeV", "gaus");
  f_1GeV = new TF1("f_1GeV", "gaus");
  f_05GeV = new TF1("f_05GeV", "gaus");

  c_edep->cd(1);
  h_edep_10GeV->Fit("f_10GeV");
  c_edep->cd(2);
  h_edep_775GeV->Fit("f_775GeV");
  c_edep->cd(3);
  h_edep_55GeV->Fit("f_55GeV");
  c_edep->cd(4);
  h_edep_325GeV->Fit("f_325GeV");
  c_edep->cd(5);
  h_edep_1GeV->Fit("f_1GeV");
  c_edep->cd(6);
  h_edep_05GeV->Fit("f_05GeV");

  double mean_10GeV = h_edep_10GeV->GetFunction("f_10GeV")->GetParameter(1);
  double mean_err_10GeV = h_edep_10GeV->GetFunction("f_10GeV")->GetParError(1);
  double sigma_10GeV = h_edep_10GeV->GetFunction("f_10GeV")->GetParameter(2);
  double sigma_err_10GeV = h_edep_10GeV->GetFunction("f_10GeV")->GetParError(2);
  double mean_775GeV = h_edep_775GeV->GetFunction("f_775GeV")->GetParameter(1);
  double mean_err_775GeV = h_edep_775GeV->GetFunction("f_775GeV")->GetParError(1);
  double sigma_775GeV = h_edep_775GeV->GetFunction("f_775GeV")->GetParameter(2);
  double sigma_err_775GeV = h_edep_775GeV->GetFunction("f_775GeV")->GetParError(2);
  double mean_55GeV = h_edep_55GeV->GetFunction("f_55GeV")->GetParameter(1);
  double mean_err_55GeV = h_edep_55GeV->GetFunction("f_55GeV")->GetParError(1);
  double sigma_55GeV = h_edep_55GeV->GetFunction("f_55GeV")->GetParameter(2);
  double sigma_err_55GeV = h_edep_55GeV->GetFunction("f_55GeV")->GetParError(2);
  double mean_325GeV = h_edep_325GeV->GetFunction("f_325GeV")->GetParameter(1);
  double mean_err_325GeV = h_edep_325GeV->GetFunction("f_325GeV")->GetParError(1);
  double sigma_325GeV = h_edep_325GeV->GetFunction("f_325GeV")->GetParameter(2);
  double sigma_err_325GeV = h_edep_325GeV->GetFunction("f_325GeV")->GetParError(2);
  double mean_1GeV = h_edep_1GeV->GetFunction("f_1GeV")->GetParameter(1);
  double mean_err_1GeV = h_edep_1GeV->GetFunction("f_1GeV")->GetParError(1);
  double sigma_1GeV = h_edep_1GeV->GetFunction("f_1GeV")->GetParameter(2);
  double sigma_err_1GeV = h_edep_1GeV->GetFunction("f_1GeV")->GetParError(2);
  double mean_05GeV = h_edep_05GeV->GetFunction("f_05GeV")->GetParameter(1);
  double mean_err_05GeV = h_edep_05GeV->GetFunction("f_05GeV")->GetParError(1);
  double sigma_05GeV = h_edep_05GeV->GetFunction("f_05GeV")->GetParameter(2);
  double sigma_err_05GeV = h_edep_05GeV->GetFunction("f_05GeV")->GetParError(2);

  ///////////////////
  //plot resolution//
  ///////////////////

  double x_res[6] = {0.5, 1., 3.25, 5.5, 7.75, 10.};
  double y_res[6] = {sigma_05GeV/mean_05GeV*100, sigma_1GeV/mean_1GeV*100, sigma_325GeV/mean_325GeV*100, sigma_55GeV/mean_55GeV*100, sigma_775GeV/mean_775GeV*100, sigma_10GeV/mean_10GeV*100};
  double x_res_err[6] = {0., 0., 0., 0., 0., 0.};
  double y_res_err[6] = {(sigma_05GeV/mean_05GeV)*TMath::Sqrt((mean_err_05GeV/mean_05GeV)*(mean_err_05GeV/mean_05GeV) + (sigma_err_05GeV/sigma_05GeV)*(sigma_err_05GeV/sigma_05GeV))*100,
  			 (sigma_05GeV/mean_1GeV)*TMath::Sqrt((mean_err_1GeV/mean_1GeV)*(mean_err_1GeV/mean_1GeV) + (sigma_err_1GeV/sigma_1GeV)*(sigma_err_1GeV/sigma_1GeV))*100,
  			 (sigma_05GeV/mean_325GeV)*TMath::Sqrt((mean_err_325GeV/mean_325GeV)*(mean_err_325GeV/mean_325GeV) + (sigma_err_325GeV/sigma_325GeV)*(sigma_err_325GeV/sigma_325GeV))*100,
  			 (sigma_05GeV/mean_55GeV)*TMath::Sqrt((mean_err_55GeV/mean_55GeV)*(mean_err_55GeV/mean_55GeV) + (sigma_err_55GeV/sigma_55GeV)*(sigma_err_55GeV/sigma_55GeV))*100,
  			 (sigma_05GeV/mean_775GeV)*TMath::Sqrt((mean_err_775GeV/mean_775GeV)*(mean_err_775GeV/mean_775GeV) + (sigma_err_775GeV/sigma_775GeV)*(sigma_err_775GeV/sigma_775GeV))*100,
  			 (sigma_05GeV/mean_10GeV)*TMath::Sqrt((mean_err_10GeV/mean_10GeV)*(mean_err_10GeV/mean_10GeV) + (sigma_err_10GeV/sigma_10GeV)*(sigma_err_10GeV/sigma_10GeV))*100};

  TCanvas *c_res = new TCanvas("c_res", "c_res", 200, 10, 500, 500);
  c_res->cd();
  gPad->SetGridx(1);
  gPad->SetGridy(1);
  TGraphErrors* gr_res = new TGraphErrors(6, x_res, y_res, x_res_err, y_res_err);
  gr_res->Draw("APE");
  gr_res->Write("gr_res");

  ////////////////////
  //print the prints//
  ////////////////////
  c_edep->Print(Form("output/ene/pics/dist_edep_x_%.0fmm_y_%.0fmm_z_%.0fmm.pdf", crys_x, crys_y, crys_z));
  c_edep->Print(Form("output/ene/pics/png/dist_edep_x_%.0fmm_y_%.0fmm_z_%.0fmm.png", crys_x, crys_y, crys_z));

  c_res->Print(Form("output/ene/pics/edep_res_x_%.0fmm_y_%.0fmm_z_%.0fmm.pdf", crys_x, crys_y, crys_z));
  c_res->Print(Form("output/ene/pics/png/edep_res_x_%.0fmm_y_%.0fmm_z_%.0fmm.png", crys_x, crys_y, crys_z));

  f_out->Write();

  return;
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("energyResolution", &argc, argv);

  ene_edep_res_test();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
