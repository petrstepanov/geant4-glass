#include "CanvasHelper.h"
#include <TROOT.h>
#include <TPad.h>
#include <TCanvas.h>
#include <THistPainter.h>
#include <TObjString.h>
#include <TLatex.h>
#include <iostream>
#include <sstream>

ClassImp(CanvasHelper)

// Instance
CanvasHelper* CanvasHelper::fgInstance = nullptr;

// Constructor
CanvasHelper::CanvasHelper(){
  // canvasesToBeExported = new TMap();
  registeredCanvases = new TList();

  // Only accept resized signels from TCanvas. Child pads will also send these signals
  // However we want to omit them because they will duplicate the functinoality
  TQObject::Connect(TCanvas::Class_Name(), "Resized()", this->Class_Name(), this, "onCanvas Resized()");
}

// Destructor
CanvasHelper::~CanvasHelper(){
}

// Instance provider
CanvasHelper* CanvasHelper::GetInstance(){
  static CanvasHelper instance;
  if (fgInstance == nullptr){
    fgInstance = &instance;
  }
  return fgInstance;
}

// All sizes in pixels
const Int_t CanvasHelper::FONT_SIZE_NORMAL = 16;
const Int_t CanvasHelper::FONT_SIZE_SMALL = FONT_SIZE_NORMAL*3./4.;
const Int_t CanvasHelper::FONT_SIZE_SMALLER = FONT_SIZE_SMALL*3./4.;
const Int_t CanvasHelper::FONT_SIZE_LARGE = FONT_SIZE_NORMAL*4./3.;
const Int_t CanvasHelper::FONT_SIZE_LARGER = FONT_SIZE_LARGE*4./3.;
const Int_t CanvasHelper::FONT_SIZE_HUGE = FONT_SIZE_LARGER*4./3.;

const Double_t CanvasHelper::LINE_HEIGHT = 1.2;

const Double_t CanvasHelper::TITLE_OFFSET = 64;
const Double_t CanvasHelper::LABEL_OFFSET = 6;
const Double_t CanvasHelper::TICK_LENGTH = 10;

const Double_t CanvasHelper::MARGIN_LEFT = 50;
const Double_t CanvasHelper::MARGIN_TOP = 20;
const Double_t CanvasHelper::MARGIN_RIGHT = 20;
const Double_t CanvasHelper::MARGIN_BOTTOM = 50;

Style_t CanvasHelper::getFont(EFontFace fontFace){
    // https://root.cern.ch/doc/master/classTAttText.html#autotoc_md31
  const Int_t precision = 3;  // scalable and rotatable fonts (if "1" then problems rendering, "3" - non scalable fonts look bad on big screens)
  return (Int_t)fontFace * 10 + precision;
}

TAxis* CanvasHelper::getYAxis(TVirtualPad* pad){
  TList* primitives = pad->GetListOfPrimitives();
  TListIter next(primitives);
  TObject *object;
  while ((object=next())) {
    if (object->InheritsFrom(TH1::Class())){
      TH1* hist = (TH1*)object;
      return hist->GetYaxis();
    }
    if (object->InheritsFrom(THStack::Class())){
      THStack* stack = (THStack*)object;
      TAxis* yAxis = stack->GetYaxis();
      // Fix TStack not having Y axis limits
      yAxis->SetLimits(stack->GetMinimum(), stack->GetMaximum());
      return yAxis;
    }
    // TODO: for TGraph
  }
  return NULL;
}

Double_t CanvasHelper::getYAxisMaxLabelWidthPx(TVirtualPad* pad){
  // Obtain axis from pad (could be histogram, Stack, Graph...)
  TAxis* axis = getYAxis(pad);
  if (!axis) return 0;

  // Determine longest axis label
  Int_t axisMin = pad->GetLogy() ? 100 : axis->GetXmin();
  Int_t axisMax = pad->GetLogy() ? 100 : axis->GetXmax();
  std::stringstream bufferMin;
  bufferMin << axisMin;
  std::string axisMinLabel = bufferMin.str();
  std::stringstream bufferMax;
  bufferMax << axisMax;
  std::string axisMaxLabel = bufferMax.str();
  std::string axisLongestLabel = (Int_t)axisMinLabel.length() > (Int_t)axisMaxLabel.length() ? axisMinLabel : axisMaxLabel;

  // Determine longest label width in pixels
  UInt_t w=0,h=0;
  TText* t = new TText(0, 0, axisLongestLabel.c_str());
  t->SetTextFont(getFont());
  t->SetTextSize(FONT_SIZE_NORMAL);
  t->GetBoundingBox(w,h);

  // Return longest label width
  return w;
}

Double_t CanvasHelper::getLabelHeigthPx(){
  UInt_t w=0,h=0;
  TText* t = new TText(0, 0, "1");
  t->SetTextFont(getFont());
  t->SetTextSize(FONT_SIZE_NORMAL);
  t->GetBoundingBox(w,h);

  // Return label height
  return h;
}

Double_t CanvasHelper::getPadWidthPx(TVirtualPad* virtualPad){
  if (virtualPad->InheritsFrom(TCanvas::Class())){
    return virtualPad->GetWw();
  }
  if (virtualPad->InheritsFrom(TPad::Class())){
    Double_t parentCanvasWidth = virtualPad->GetWw();
    Double_t widthRatio = virtualPad->GetAbsWNDC();
    return parentCanvasWidth*widthRatio;
  }
  return 0;
}

Double_t CanvasHelper::getPadHeightPx(TVirtualPad* virtualPad){
  if (virtualPad->InheritsFrom(TCanvas::Class())){
    return virtualPad->GetWh();
  }
  if (virtualPad->InheritsFrom(TPad::Class())){
    Double_t parentCanvasHeight = virtualPad->GetWh();
    Double_t heightRatio = virtualPad->GetAbsHNDC();
    return parentCanvasHeight*heightRatio;
  }
  return 0;
}

void CanvasHelper::tweakCanvas(TCanvas* canvas){
  registeredCanvases->Add(canvas);
  processCanvas(canvas);
}

void CanvasHelper::onCanvasResized(){
  // Every Pad will emit this signal. Supposedly child canvas pads as well.
  // We need to listen to only parent canvas signal to eliminate doing things multiple times

//  TSeqCollection* canvases = gROOT->GetListOfCanvases();
//  if (canvases == nullptr) return;

  TIter next(registeredCanvases);
  TObject *object;
  while ((object=next())) {
    if (object!=nullptr && object->InheritsFrom(TCanvas::Class())){
      TCanvas* canvas = (TCanvas*)object;
      processCanvas(canvas);
    }
  }
}

void CanvasHelper::processCanvas(TCanvas* canvas){
  // Process canvas itself
  if (canvas->IsModified()){
    std::cout << "Updating pad \"" << canvas->GetName() << std::endl;
    processPad(canvas);
  }

  // canvas->Paint();
  // canvas->Update();

  // Find and process child pads
  for (Int_t i = 1; ; i++){
    TString childPadName = TString::Format("%s_%d", canvas->GetName(), i);
    TPad* childPad = (TPad*)gROOT->FindObject(childPadName);
    if (childPad){
      std::cout << "  Found child pad \"" << childPadName << std::endl;
      processPad(childPad);

      // Quicker redraw
      childPad->SetFillStyle(EFillStyle::kFEmpty);
//      childPad->Modified();
      // childPad->Paint();
      // childPad->Update();
    }
    else {
      break;
    }
  }

  // Refresh canvas - copied code from the ROOT GUI Refresh button
  canvas->Paint();
  canvas->Update();

  // Check if this particular canvas needed to be saved
//  TObjString* canvasFileName = (TObjString*)canvasesToBeExported->FindObject(canvas);
//  if (canvasFileName != nullptr){
//    // Save canvas
//    doExportCanvas(canvas, canvasFileName->GetString().Data());
//    // Pop canvas from saved
//    canvasesToBeExported->Remove(canvas);
//  }
}

void CanvasHelper::processPad(TVirtualPad* pad){
  // Tweak pad margins
  setPadMargins(pad);

  // Update pad primitives - histograms, Stacks, Graphs...
  TList* primitives = pad->GetListOfPrimitives();
  TListIter next(primitives);
  TObject *object;

  while ((object=next())) {
    if (object->InheritsFrom(TH1::Class())){
      TH1* hist = (TH1*)object;
      convertHistToPxSize(hist, pad);
    }
    if (object->InheritsFrom(THStack::Class())){
      THStack* stack = (THStack*)object;
      convertHistStackToPxSize(stack, pad);
    }
    // TODO: for TGraph
  }
}

void CanvasHelper::setPadMargins(TVirtualPad* pad){
  // Problem: child pads return Ww (width) and Wh (height) from parent's canvas object 🥵
  // Solution: wrote custom functions to get actual size
  Double_t padWidth = getPadWidthPx(pad);
  Double_t padHeight = getPadHeightPx(pad);


  Double_t leftMargin = MARGIN_LEFT + getYAxisMaxLabelWidthPx(pad);
  Double_t leftMarginNDC = leftMargin/padWidth;
  pad->SetLeftMargin(leftMarginNDC);

  Double_t topMargin = MARGIN_TOP;
  Double_t topMarginNDC = topMargin/padHeight;
  pad->SetTopMargin(topMarginNDC);

  Double_t rightMargin = MARGIN_RIGHT;
  Double_t rightMarginNDC = rightMargin/padWidth;
  pad->SetRightMargin(rightMarginNDC);

  Double_t bottomMargin = MARGIN_BOTTOM + getLabelHeigthPx();
  Double_t bottomMarginNDC = bottomMargin/padHeight;
  pad->SetBottomMargin(bottomMarginNDC);
}

void CanvasHelper::convertHistToPxSize(TH1* hist, TVirtualPad* pad){
  convertAxisToPxSize(hist->GetXaxis(), 'x',pad);
  convertAxisToPxSize(hist->GetYaxis(), 'y',pad);
}

void CanvasHelper::convertHistStackToPxSize(THStack* stack, TVirtualPad* pad){
  convertAxisToPxSize(stack->GetXaxis(), 'x', pad);
  convertAxisToPxSize(stack->GetYaxis(), 'y', pad);
}

void CanvasHelper::convertAxisToPxSize(TAxis* axis, const char type, TVirtualPad* pad){
  // Determine pad size in pixels
  Double_t padWidth = getPadWidthPx(pad);
  Double_t padHeight = getPadHeightPx(pad);

  // Style axis title - font, alignment, offset
//  axis->SetTitleFont(getFont());
//  axis->SetTitleSize(FONT_SIZE_NORMAL);
//  axis->CenterTitle(true);
//  if (type == 'x'){
//    // TODO: try to estimate the width of the TAxisPainter's asis from canvas primitive
//    Double_t offset = TITLE_OFFSET/padHeight*20;
//    axis->SetTitleOffset(offset);
//  }
//  else {
//    Double_t yAxisMaxLabelWidth = getYAxisMaxLabelWidthPx(pad);
//    Double_t offset = (TITLE_OFFSET+yAxisMaxLabelWidth)/padWidth*20;
//    axis->SetTitleOffset(offset);
//  }
  const Int_t titleOffset = 15; //px
  if (type == 'x'){
    if (strlen(axis->GetTitle()) > 0){
      TText* xAxisText = new TText(0.5, titleOffset/padHeight, axis->GetTitle());
      xAxisText->SetNDC();
      xAxisText->SetTextFont(getFont());
      xAxisText->SetTextSize(FONT_SIZE_NORMAL);
      xAxisText->SetTextAlign(ETextAlign::kHAlignCenter+ETextAlign::kVAlignCenter);
      pad->cd();
      xAxisText->Draw();
      axis->SetTitle("");
    }
  }
  else {
    if (strlen(axis->GetTitle()) > 0){
      Double_t yaxisYPos = (50 + (padHeight - 50-20)/2)/padHeight;
      TText* yAxisText = new TText(titleOffset/padWidth, yaxisYPos, axis->GetTitle());
      yAxisText->SetNDC();
      yAxisText->SetTextFont(getFont());
      yAxisText->SetTextSize(FONT_SIZE_NORMAL);
      yAxisText->SetTextAngle(90);
      yAxisText->SetTextAlign(ETextAlign::kHAlignCenter+ETextAlign::kVAlignCenter);
      pad->cd();
      yAxisText->Draw();
      axis->SetTitle("");
    }
  }

  // Style labels - font and offset
  axis->SetLabelFont(getFont());
  axis->SetLabelSize(FONT_SIZE_NORMAL);
  if (type == 'x'){
    Double_t offset = LABEL_OFFSET/padHeight;
    axis->SetLabelOffset(offset);
  }
  else {
    Double_t offset = LABEL_OFFSET/padWidth;
    axis->SetLabelOffset(offset);
  }

  // Set tick length
  if (type == 'x'){
    Double_t length = TICK_LENGTH/padHeight*2;
    axis->SetTickLength(length);
  }
  else {
    Double_t length = TICK_LENGTH/padWidth*2;
    axis->SetTickLength(length);
  }
}

// Signals

//void CanvasHelper::canvasReady(TCanvas* canvas){
//  Emit("canvasReady(Int_t)", canvas);
//}
