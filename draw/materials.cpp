#include <CanvasHelper.h>

#include <TFile.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TVirtualPad.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>
#include <TApplication.h>
#include "include/utilities.h"

void materials() {
    TFile *pwoC = new TFile("./output/materials/PWO-CRYTUR.root");
    TFile *pwoS = new TFile("./output/materials/PWO-SICCAS.root");
    TFile *pwoB = new TFile("./output/materials/PWO-BOGO.root"); // https://sci-hub.do/10.1016/0168-9002(96)00286-0

    // PWO transmittance
    {
        TGraph *graph1 = (TGraph*) pwoC->Get("MY_TRANSMITTANCE");
        graph1->SetTitle("PWO CRYTUR, 20 cm (JLab?)");
        graph1->SetMarkerStyle(kStar); // https://root.cern.ch/doc/master/classTAttMarker.html

        TGraph *graph2 = (TGraph*) pwoS->Get("MY_TRANSMITTANCE");
        graph2->SetTitle("PWO SICCAS, 20 cm (JLab?)");
        graph2->SetMarkerStyle(kCircle);

        TGraph *graph4 = (TGraph*) pwoB->Get("MY_TRANSMITTANCE");
        graph4->SetTitle("PWO Bogoroditsk, 21.3 cm (R.Zhu)");
        graph4->SetMarkerStyle(kOpenSquare);

        TMultiGraph *mg = new TMultiGraph();
        mg->SetTitle("PWO Transmittance Spectra;Wavelength, nm;Transmittance, %");
        mg->Add(graph1);
        mg->Add(graph2);
        mg->Add(graph4);

        TCanvas *canvas = new TCanvas();
        canvas->SetLeftMargin(0.15);
        canvas->SetRightMargin(0.05);

        mg->Draw("apl");
        canvas->BuildLegend();
        alignLegend(canvas, Alignment::BOTTOM_RIGHT);
        addGrid(canvas);
    }

    // PWO calculated attenuation length
    {
        TGraph *graph1 = (TGraph*) pwoC->Get("ABSLENGTH");
        graph1->SetTitle("PWO CRYTUR, 20 cm (JLab?)");
        graph1->SetMarkerStyle(kStar); // https://root.cern.ch/doc/master/classTAttMarker.html

        TGraph *graph2 = (TGraph*) pwoS->Get("ABSLENGTH");
        graph2->SetTitle("PWO SICCAS, 20 cm (JLab?)");
        graph2->SetMarkerStyle(kCircle);

        TGraph *graph4 = (TGraph*) pwoB->Get("ABSLENGTH");
        graph4->SetTitle("PWO Bogoroditsk, 21.3 cm (R.Zhu)");
        graph4->SetMarkerStyle(kOpenSquare);

        TMultiGraph *mg = new TMultiGraph();
        mg->SetTitle("Calculated PWO Absorption Length;Wavelength, nm;Absorption length, mm");
        mg->Add(graph1);
        mg->Add(graph2);
        mg->Add(graph4);

        TCanvas *canvas = new TCanvas();
        canvas->SetLeftMargin(0.15);
        canvas->SetRightMargin(0.05);

        mg->Draw("apl");
        canvas->BuildLegend();
        alignLegend(canvas);
        addGrid(canvas);
    }

    // Emission spectra
    // TFile* pwo = new TFile("../../output/materials/PWO-CRYTUR.root");
    TFile *s1t = new TFile("./output/materials/SciGlass-4-1-L.root");
    TFile *cst = new TFile("./output/materials/CSGlass-0-T.root");        // https://sci-hub.do/10.1016/0168-9002(96)00286-0

    {
//		TGraph* graph1 = (TGraph*)pwo->Get("FASTCOMPONENT");
//		graph1->SetTitle("PWO samples");
//		graph1->SetMarkerStyle(kStar); // https://root.cern.ch/doc/master/classTAttMarker.html

        TGraph *graph2 = (TGraph*) s1t->Get("FASTCOMPONENT");
        graph2->SetTitle("SciGlass (Cube 6, Cube 9)");
        graph2->SetMarkerStyle(kFullDotMedium);

        TGraph *graph3 = (TGraph*) cst->Get("FASTCOMPONENT");
        graph3->SetTitle("CSGlass (Cube 4, Cube 10)");
        graph3->SetMarkerStyle(kFullCircle);

        TMultiGraph *mg = new TMultiGraph();
        mg->SetTitle("Photon Emission Spectra;Wavelength, nm;Normalized amplitude, a.u.");
//		mg->Add(graph1);
        mg->Add(graph2);
        mg->Add(graph3);

        TCanvas *canvas = new TCanvas();
        canvas->SetLeftMargin(0.15);
        canvas->SetRightMargin(0.05);

        mg->Draw("apl");
        canvas->BuildLegend();
        alignLegend(canvas);
        addGrid(canvas);
    }

    // Transmittance SciGlass
    TFile *csgla = new TFile("./output/materials/CSGlass-0-T.root");
    TFile *cube4 = new TFile("./output/materials/Cube-4-1.root");
    TFile *cube6 = new TFile("./output/materials/Cube-6-1.root");
    TFile *cube9 = new TFile("./output/materials/Cube-9-1.root");
    TFile *cube10 = new TFile("./output/materials/Cube-10-1.root");
    TFile *sg41l = new TFile("./output/materials/SciGlass-4-1-L.root");

    {
        TGraph *graph1 = (TGraph*) csgla->Get("MY_TRANSMITTANCE");
        graph1->SetTitle("CSGlass-T, 20 mm");
        graph1->SetMarkerStyle(kStar);
        graph1->SetMarkerSize(1.2);

        TGraph *graph2 = (TGraph*) cube4->Get("MY_TRANSMITTANCE");
        graph2->SetTitle("Cube 4, 48 mm");
        graph2->SetMarkerStyle(kOpenCircle);
        graph2->SetMarkerSize(1.2);

        TGraph *graph3 = (TGraph*) cube6->Get("MY_TRANSMITTANCE");
        graph3->SetTitle("Cube 6, 45.5 mm");
        graph3->SetMarkerStyle(kFullDiamond);
        graph3->SetMarkerSize(2);

        TGraph *graph4 = (TGraph*) cube9->Get("MY_TRANSMITTANCE");
        graph4->SetTitle("Cube 9, 46.4 mm");
        graph4->SetMarkerStyle(kOpenDiamond);
        graph4->SetMarkerSize(2);

        TGraph *graph5 = (TGraph*) cube10->Get("MY_TRANSMITTANCE");
        graph5->SetTitle("Cube 10, 40.87 mm");
        graph5->SetMarkerStyle(kFullSquare);
        graph5->SetMarkerSize(1.2);

        TGraph *graph6 = (TGraph*) sg41l->Get("MY_TRANSMITTANCE");
        graph6->SetTitle("SciGlass-4-1-L, 200 mm");
        graph6->SetMarkerStyle(kOpenSquare);
        graph6->SetMarkerSize(1.2);

        TMultiGraph *mg = new TMultiGraph();
        mg->SetTitle("SciGlass Transmittance Spectra;Wavelength, nm;Transmittance, %");
        mg->Add(graph1);
        mg->Add(graph2);
        mg->Add(graph3);
        mg->Add(graph4);
        mg->Add(graph5);
        mg->Add(graph6);

        TCanvas *canvas = new TCanvas();
        canvas->SetLeftMargin(0.15);
        canvas->SetRightMargin(0.05);

        mg->Draw("apl");
        canvas->BuildLegend();
        alignLegend(canvas);
        addGrid(canvas);
    }

    {
        TGraph *graph1 = (TGraph*) csgla->Get("ABSLENGTH");
        graph1->SetTitle("CSGlass-0-T");
        graph1->SetMarkerStyle(kStar);
        graph1->SetMarkerSize(1.2);

//		TGraph* graph2 = (TGraph*)sg11t->Get("ABSLENGTH");
//		graph2->SetTitle("SciGlass-1-1-T");
//		graph2->SetMarkerStyle(kOpenCircle);
//		graph2->SetMarkerSize(1.2);
//
//		TGraph* graph3 = (TGraph*)sg21l->Get("ABSLENGTH");
//		graph3->SetTitle("SciGlass-2-1-L");
//		graph3->SetMarkerStyle(kFullDiamond);
//		graph3->SetMarkerSize(2);
//
//		TGraph* graph4 = (TGraph*)sg22t->Get("ABSLENGTH");
//		graph4->SetTitle("SciGlass-2-2-T");
//		graph4->SetMarkerStyle(kOpenDiamond);
//		graph4->SetMarkerSize(2);
//
//		TGraph* graph5 = (TGraph*)sg41l->Get("ABSLENGTH");
//		graph5->SetTitle("SciGlass-4-1-L");
//		graph5->SetMarkerStyle(kFullSquare);
//		graph5->SetMarkerSize(1.2);

        TGraph *graph6 = (TGraph*) sg41l->Get("ABSLENGTH");
        graph6->SetTitle("SciGlass-4-1-L");
        graph6->SetMarkerStyle(kOpenSquare);
        graph6->SetMarkerSize(1.2);

        TMultiGraph *mg = new TMultiGraph();
        mg->SetTitle("Calculated SciGlass Absorption Length;Wavelength, nm;Absorption length, mm");
        mg->Add(graph1);
//		mg->Add(graph2);
//		mg->Add(graph3);
//		mg->Add(graph4);
//		mg->Add(graph5);
        mg->Add(graph6);

        TCanvas *canvas = new TCanvas();
        canvas->SetLeftMargin(0.15);
        canvas->SetRightMargin(0.05);

        mg->Draw("apl");
        canvas->BuildLegend();
        alignLegend(canvas);
        addGrid(canvas);
    }

    {
        TGraph *graph1 = (TGraph*) cube9->Get("ABSLENGTH");
        graph1->SetTitle("Cube-9-1 (SciGlass)");
        graph1->SetMarkerStyle(kOpenDiamond);
        graph1->SetMarkerSize(1.2);

        TGraph *graph2 = (TGraph*) cube10->Get("ABSLENGTH");
        graph2->SetTitle("Cube-10-1 (CSGlass");
        graph2->SetMarkerStyle(kFullDiamond);
        graph2->SetMarkerSize(1.2);

        //    TGraph* graph3 = (TGraph*)pwoC->Get("ABSLENGTH");
        //    graph3->SetTitle("PWO CRYTUR");
        //    graph3->SetMarkerStyle(kStar);
        //    graph3->SetMarkerSize(1.2);

        TMultiGraph *mg = new TMultiGraph();
        mg->SetTitle("Calculated Absorption Lengths;Wavelength, nm;Absorption length, mm");
        mg->Add(graph1);
        mg->Add(graph2);
        //    mg->Add(graph3);

        TCanvas *canvas = new TCanvas();
        canvas->SetLeftMargin(0.15);
        canvas->SetRightMargin(0.05);

        mg->Draw("apl");
        canvas->BuildLegend();
        alignLegend(canvas);
        addGrid(canvas);
    }

    {

        TGraph *graph2 = (TGraph*) sg41l->Get("ABSLENGTH");
        graph2->SetTitle("Calculated Absorption Length");
        graph2->SetMarkerStyle(kFullDiamond);
        graph2->SetMarkerSize(1.2);

        gStyle->SetOptFit(11);
        TCanvas *canvas = new TCanvas();
        canvas->Divide(2, 1, 1E-8, 1E-8); // last two parameters - margin
        {
            TGraph *graph = (TGraph*) sg41l->Get("FASTCOMPONENT");
            // graph->SetTitle("Emission Spectrum");
            graph->SetTitle("");
            graph->SetMarkerStyle(kDot);
            graph->SetMarkerSize(1.2);
            graph->GetXaxis()->SetLimits(370, 640);
            TVirtualPad* pad = canvas->cd(1);
            addGrid(pad);
            graph->GetYaxis()->SetTitle("Scintillation Intensity, a.u.");
            graph->Fit("gaus", "0", "", 416, 475); // 0 - do not draw but store the fitted function
            TF1* gaus = graph->GetFunction("gaus");
            // Once fixed, parameters are not shown in the stat box
            gaus->FixParameter(0, gaus->GetParameter(0));
            gaus->FixParameter(2, gaus->GetParameter(2));
            graph->Draw();
//            alignStats(pad);
        }
        {
            TGraph *graph = (TGraph*) sg41l->Get("ABSLENGTH");
            // graph->SetTitle("Calculated Absorption Length");
            graph->SetTitle("");
            graph->SetMarkerStyle(kStar);
            graph->SetMarkerSize(1.2);
            TVirtualPad* pad = canvas->cd(2);
            graph->Draw();
            graph->GetYaxis()->SetTitle("Absorption Length, mm");
            //graph->Fit("gaus", "", "", 475, 575);
            addGrid(pad);
        }
        CanvasHelper::getInstance()->addCanvas(canvas);
    }
}

#ifndef __CINT__

int main(int argc, char **argv) {
    // Instantiate TApplication
    TApplication *app = new TApplication("energyResolution", &argc, argv);

    materials();

    // Enter the event loop
    app->Run();
    // Return success
    return 0;
}

#endif
