#ifndef CanvasHelper_HH_
#define CanvasHelper_HH_

#include <RtypesCore.h>
#include <Rtypes.h>
#include <TColor.h>
#include <TVirtualPad.h>
#include <TH1.h>
#include <THStack.h>
#include <TMap.h>
#include <TQObject.h>

struct Margin {
  Double_t left;
  Double_t right;
  Double_t bottom;
  Double_t top;
};

enum EFontFace {
    TimesItalic = 1,
    TimesBold = 2,
    TimesBoldItalic = 3,
    Helvetica = 4,
    HelveticaItalic = 5,
    HelveticaBold = 6,
    HelveticaBoldItalic = 7,
    Courier = 8,
    CourierItalic = 9,
    CourierBold = 10,
    CourierBoldItalic = 11,
    Symbol = 12,
    Times = 13,
    Wingdings = 14,
    SymbolItalic = 15
};

class CanvasHelper /* : public TQObject */ {
  public:
    static CanvasHelper* GetInstance();
    virtual ~CanvasHelper();

  protected:
    CanvasHelper();

  // Had to make public? or use this??
  public:
    static CanvasHelper* fgInstance;

  public:
    // Font sizes in pixels
    static const Int_t FONT_SIZE_NORMAL;
    static const Int_t FONT_SIZE_SMALL;
    static const Int_t FONT_SIZE_SMALLER;
    static const Int_t FONT_SIZE_LARGE;
    static const Int_t FONT_SIZE_LARGER;
    static const Int_t FONT_SIZE_HUGE;

    static const Double_t LINE_HEIGHT;

    static const Double_t TITLE_OFFSET;
    static const Double_t LABEL_OFFSET;
    static const Double_t TICK_LENGTH;

    static const Double_t MARGIN_LEFT;
    static const Double_t MARGIN_TOP;
    static const Double_t MARGIN_RIGHT;
    static const Double_t MARGIN_BOTTOM;

    static Style_t getFont(EFontFace fontFace = EFontFace::Helvetica);

    void tweakCanvas(TCanvas* canvas);

    // Slot for canvas resizing
    void onCanvasResized();

//    void canvasReady(TCanvas*);      // *SIGNAL*
//    void exportCanvas(TCanvas* canvas, const char* filename);

private:
    // TMap *canvasesToBeExported;
    TList *registeredCanvases;

    void processCanvas(TCanvas* canvas);
    void processPad(TVirtualPad* pad);
    void setPadMargins(TVirtualPad* pad);
    void convertHistToPxSize(TH1* hist, TVirtualPad* pad);
    void convertHistStackToPxSize(THStack* stack, TVirtualPad* pad);

    void convertAxisToPxSize(TAxis* axis, const char type, TVirtualPad* pad);

    Double_t getPadWidthPx(TVirtualPad* pad);
    Double_t getPadHeightPx(TVirtualPad* pad);

    TAxis*   getYAxis(TVirtualPad* pad);
    Double_t getYAxisMaxLabelWidthPx(TVirtualPad* pad);
    Double_t getLabelHeigthPx();


public:
    ClassDef(CanvasHelper, 0)
};

#endif
