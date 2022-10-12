#ifndef utilities_hh
#define utilities_hh 1

#include <Math/PdfFuncMathCore.h>
#include <RtypesCore.h>
#include <TVirtualPad.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TMultiGraph.h>
#include <TObjString.h>
#include <TGMsgBox.h>
#include <TFile.h>
#include <TPRegexp.h>
#include <TGFileDialog.h>
#include <TGraph.h>
#include <TSystemDirectory.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TPaletteAxis.h>
#include <TColor.h>
#include <TSystem.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <RooConstVar.h>
#include <TVector3.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TArrayI.h>
#include <TStyle.h>
#include <fstream>
#include <iomanip>
#include <limits>
#include <TClass.h>
#include <functional>

namespace RoundMe {
	int getFirstDigit();
	std::pair<double, double> valueError(const double value, const double error);
}

double nmToMeV(double nm);

void printNmVectorForGPS(std::vector<double> vector);

// Bool_t invertedFlag;

Color_t getGradientColor(Int_t i, Int_t ncolors, EColorPalette palette = EColorPalette::kCherry, Bool_t invert = kFALSE);

void utility();

// END Ho San Code

void msgBoxInfo(const char* title, const char* text);
void msgBoxError(const char* title, const char* text);
Int_t msgBoxQuestion(const char* title, const char* text);

enum class Alignment {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};

enum class Decoration {
	DEFAULT,
	TRANSPARENT
};

void addPaveStatsEntry(TVirtualPad* pad, const char* text);

Int_t getNumberOfLines(TPave* pave);

TLegend* getPadLegend(TVirtualPad* pad);

TPaveStats* getPadStats(TVirtualPad* pad);

void alignPave(TPave* pave, TVirtualPad* pad, Alignment alignment, Decoration decoration, Double_t statsLineHeight = 0.07, Double_t statsWidth = 0);

void alignStats(TVirtualPad* pad, Alignment alignment = Alignment::TOP_RIGHT, Decoration decoration = Decoration::DEFAULT, Double_t statsLineHeight = 0.07, Double_t statsWidth = 0);

void alignLegend(TVirtualPad* pad, Alignment alignment = Alignment::TOP_RIGHT, Decoration decoration = Decoration::DEFAULT, Double_t statsLineHeight = 0.07, Double_t statsWidth = 0);

void alignLegend(TVirtualPad* pad, Double_t statsWidth, Alignment alignment = Alignment::TOP_RIGHT, Decoration decoration = Decoration::DEFAULT, Double_t statsLineHeight = 0.07);

void addGrid(TVirtualPad* pad);

TPad* addOverlayPad(TCanvas* canvas, Int_t padNumberToOverlay /* 1st = 1, 2nd = 2, ...*/, Int_t currentNPads /* current number of pads */);

void addGraph(TMultiGraph *mg, TFile* file, EMarkerStyle markerStyle, Color_t markerColor=1);

void increaseAxisFontSize(TAxis* axis, Float_t factor);

void increaseGraphFontSize(TGraph* graph, Float_t factor);

struct EnergyValueUnit {
  Double_t value;
  std::string unit;
};

void movePalleteAxis(TVirtualPad* canvas, TH1* hist);

EnergyValueUnit formatEnergy(Double_t value);

TFile* openFile(const char* fileName);

TTree* getTree(TFile* file, const char* treeName);

TCanvas* getCanvas();

void addMultiCanvasTitle(TCanvas* canvas, const char* title, const char* subtitle = 0);

void printParameters(TF1* func);

TList* getFileNamesList(const char* text = "", Bool_t isMultiple = true);

const char* getFileName(const char* text = "");

Int_t exportCurve(TF1* func, TString filename, const char* delimeter=",");

Int_t findFirstNonZeroBin(TH1* hist);

Int_t findFirstNonZeroBin(std::vector<TH1*> histList);

Int_t findLastNonZeroBin(TH1* hist);

Int_t findLastNonZeroBin(std::vector<TH1*> histList);

TH2* makeHistQueue(std::vector<TH1*> histList, const char* name, Double_t crystalLength);

// Interpolation for the TGraph
Double_t getTGraphValue(Double_t x, TGraph* graph);

TGraph* getEfficiencyGraph(const char* detector);

TH1* convertOpToPe(TH1* histOp, const char* detector);

TH1* cloneHist(TH1* hist, const char* newName);

void setCanvasTitle(TVirtualPad *pad, TH1* firstHist, const char *newTitle);

TString formatMaterialAndSize(TFile* file);

Double_t findHistsMax(std::vector<TH1*> hists);

// template function to iterate over a tlist
// we select objects from TList that inherited from T
/* https://stackoverflow.com/questions/2298242/callback-functions-in-c\ */
// 3.3.2 Lambda expressions
template<class T>
void iterateTList(TList* list, std::function<void(T*)> fp);

// Get Graph maximum
Double_t getGraphMax(TGraph* graph);

// This function returns a list of filenames in directory with certain extension
TList* getFilesListInDrectory(const char* dirPath, const char* ext);

// Minimum  column width
// int minAsciiColWidth = 10;

Int_t exportTreeToGnuplot(TTree* tree, TString filename, const char* delimeter="   ", Bool_t append = kFALSE);

Int_t exportPointToGnuplot(TString filename, std::vector<std::string> colNames, std::vector<double> values, const char* delimeter="   ");

Int_t exportGraphToGnuplot(TGraph* graph, const char* filename, const char* delimeter = "   ");

void saveCanvasToDisk(TCanvas* canvas, const char* name, Bool_t savePNG = kTRUE, Bool_t saveC = kTRUE, Bool_t saveROOT = kTRUE);

void saveGraphToDisk(TGraph* graph, const char* name, Bool_t saveGNUPLOT = kTRUE, Bool_t saveROOT = kTRUE);

TString removeFileExtension(const char* fileNamePath);

TString getMaterialName(TFile* file);

TString getMaterialName(const char* uri);

TString getAssemblySize(TFile* file);

TString getAssemblySize(const char* uri);

TString getCrystalSize(TFile* file);

TString getCrystalSize(const char* uri);

TString getIncidentParticleType(TFile* file);

TString getIncidentParticleType(const char* uri);

Double_t getIncidentParticleEnergy(TFile* file);

Double_t getIncidentParticleEnergy(const char* uri);

TString getIncidentParticlePosition(TFile* file);

TString getIncidentParticlePosition(const char* uri);

Int_t getNumberOfEvents(TFile* file);

Int_t getNumberOfEvents(const char* uri);

TString getFileNameFromPath(const char* fileNamePath);

TTree* sortTree(TTree* tree, const char* branchName);

// Determine maximum emitted scintillation and cherenkov photons
// Here argument needs to be "branch/leaf". Get leaf name like this:
// tree->FindBranch("prod_op_chere")->GetListOfLeaves()->Print("V");
// P.S. all our branches have one leaf
Double_t getBranchMaximum(TTree* tree, const char* branchName);

TText* makeLabelAtHistogramPeak(TH1* hist, const char* text = "", Double_t ndcXOffset = 0.02);

std::pair<Double_t, Double_t> getBranchDistributionPerMeV(const char* fileName, const char* branch, Bool_t draw=kFALSE);

class CrystalBallFunctionObject {
  public:
	CrystalBallFunctionObject(Bool_t _isInverted = kFALSE);
    Double_t operator() (double *_x, double *par);

  private:
    Bool_t isReversed;
};

class GaussFunctionObject {
  public:
    GaussFunctionObject(TH1* hist);
    Double_t operator() (double *_x, double *par);

  private:
    Double_t normalization;
};

TF1* getCrystalBallFunction(TH1* hist, Bool_t reversed = kFALSE);

TF1* fitHistWithROOTCrystalBall(TH1* hist, Bool_t inverted = kFALSE);

#endif
