#include <RtypesCore.h>
#include "./../include/utilities.h"
#include <cmath>

namespace RoundMe {
	int getFirstDigit(double number){
		// We obtain the first digit of the error (always positive)
		// Like they teached in MEPhI
		number = std::abs(number);
		if (number == 1) return 1;
		if (number > 1){
			while(number >= 10){
				number = number/10;
			}
		}
		else if (number < 1){
			while(number < 1){
				number = number*10;
			}
		}
		return (int)number;
	}

	std::pair<double, double> valueError(const double value, const double error){
		// First we find the decimal point shift
		int decimalPointShift = int(log10(error)); // This will give "-0.6" for 0.0234 error, "3" for
		// For 0 < error < 1 we need to manually shift to the right
		if (error < 1) decimalPointShift--;
		// MEPhI - keep second digit if first is "1"
		if (getFirstDigit(error) == 1) decimalPointShift--;

		// Round error
		double errorRounded = round(error*pow(10, (double)-decimalPointShift));
		double errorReverted = errorRounded*pow(10, (double)decimalPointShift);

		// Do the same operation with value
		double valueRounded = int(value*pow(10, (double)-decimalPointShift));
		double valueReverted = valueRounded*pow(10, (double)decimalPointShift);

		return std::make_pair(valueReverted, errorReverted);
	}
}

double nmToMeV(double nm) {
    double hc = 1239.84193;
    double energy = hc / nm; // E (eV) = 1239.8 / l (nm)
    energy = energy * 1E-6;       // E (MeV)
    return energy;
}

void printNmVectorForGPS(std::vector<double> vector) {
    // Must be in increasing order!
    for (int i = (int) vector.size() - 2; i >= 0; i -= 2) {
        // for (int i = 0; i <= (int)vector.size()-2; i+=2){
        std::cout << "/gps/hist/point " << nmToMeV(vector[i]) << " " << vector[i + 1] << std::endl;
    }
}

Bool_t invertedFlag = kFALSE;

// Get gradient color. Approach same as arrays. starts with 0.
Color_t getGradientColor(Int_t i, Int_t ncolors, EColorPalette palette, Bool_t invert) {
    gStyle->SetPalette(palette);
    if (invert && !invertedFlag) {
        TColor::InvertPalette();
        invertedFlag = kTRUE;
    }
    Int_t nPalletteColors = TColor::GetNumberOfColors();
    Int_t colorIndex = (Double_t) i / (Double_t) (ncolors - 1) * (Double_t) (nPalletteColors - 1);
    return TColor::GetColorPalette(colorIndex);
}

void utility() {
    std::vector<double> pwoEmissionSpectrum = { 349.7, 0, 358.5, 0.0013858556703, 367.4, 0.002899146344766, 376.7, 0.004523942647876, 385.8, 0.006021303946821,
            390.4, 0.006738125845252, 397.5, 0.00759831212337, 404.9, 0.008331063397322, 412, 0.008968238418149, 415.1, 0.009270896553042, 419.7,
            0.00944611968377, 424.5, 0.009541695936894, 429.7, 0.009541695936894, 434.8, 0.009414260932729, 441.6, 0.008904520916067, 447.6, 0.008346992772842,
            454.1, 0.007741676503056, 461, 0.00713636023327, 467.8, 0.006419538334839, 474.9, 0.005638998934325, 482.6, 0.004794742031728, 490,
            0.003966414504652, 496.5, 0.003265521981742, 500.2, 0.002851358218204, 508.7, 0.002166395070814, 514.7, 0.001768160682797, 521, 0.001401785045821,
            531.2, 0.000939833155721, 540.3, 0.000541598767704, 549.7, 0.00023894063281, 559.1, 0 };

    std::vector<double> sciGlass4lEmissionSpectrum = { 364, 0, 365, 0.000216196129458, 366, 0.00019669833529, 367, 0.000217343058527, 368, 0.000216769593993,
            369, 0.000223651168405, 370, 0.000223077703871, 371, 0.00021906345213, 372, 0.000209314555046, 373, 0.000244869356177, 374, 0.000244295891643, 375,
            0.000263793685811, 376, 0.000276409905568, 377, 0.000285585338118, 378, 0.000309670848561, 379, 0.000297054628805, 380, 0.000313111635767, 381,
            0.000339491004349, 382, 0.000384794702564, 383, 0.00042493721997, 384, 0.000394543599648, 385, 0.000424363755436, 386, 0.000463932808307, 387,
            0.000478269421667, 388, 0.000518985403607, 389, 0.000568876818098, 390, 0.00058493382506, 391, 0.000569450282632, 392, 0.000684143189507, 393,
            0.000750091610959, 394, 0.000817186961481, 395, 0.000858476407956, 396, 0.001041411594421, 397, 0.00103739734268, 398, 0.001098184583324, 399,
            0.001292589060476, 400, 0.001401547322007, 401, 0.001516240228881, 402, 0.001618890380534, 403, 0.00177315234028, 404, 0.001990495398807, 405,
            0.002171136727135, 406, 0.002373569707768, 407, 0.002567400720386, 408, 0.002824886296319, 409, 0.002979148256066, 410, 0.003179287378562, 411,
            0.003425303663808, 412, 0.0037338275833, 413, 0.003940274815674, 414, 0.004243064089823, 415, 0.004536677931422, 416, 0.004854377283464, 417,
            0.005014947353088, 418, 0.005458235438158, 419, 0.005693929361786, 420, 0.005965178086544, 421, 0.006241587992111, 422, 0.006554126163344, 423,
            0.00692343732348, 424, 0.007141353846542, 425, 0.007523854690969, 426, 0.007703349090227, 427, 0.007974597814985, 428, 0.008213159061284, 429,
            0.008436236765155, 430, 0.008740172968373, 431, 0.008992497363497, 432, 0.00915421436219, 433, 0.009405965292779, 434, 0.009579725046694, 435,
            0.009622161422238, 436, 0.00995419738764, 437, 0.010063155649171, 438, 0.010083226907874, 439, 0.010123942889814, 440, 0.010297129179195, 441,
            0.010629165144596, 442, 0.01040149972445, 443, 0.010645222151559, 444, 0.01057984719464, 445, 0.010644648687024, 446, 0.010754180413089, 447,
            0.01072149293463, 448, 0.010710597108477, 449, 0.010586728769053, 450, 0.010738123406127, 451, 0.010660132229452, 452, 0.010798910646771, 453,
            0.010602212311481, 454, 0.010599344988809, 455, 0.010661279158521, 456, 0.010432466809306, 457, 0.010512751844119, 458, 0.010373399962266, 459,
            0.010243796977498, 460, 0.010230607293207, 461, 0.01008666769508, 462, 0.010071757617186, 463, 0.009867604242949, 464, 0.009820006686596, 465,
            0.009828035190078, 466, 0.009683522127416, 467, 0.009476501430507, 468, 0.00944668127472, 469, 0.009189769163321, 470, 0.009106616805837, 471,
            0.009027478700094, 472, 0.008731571000357, 473, 0.008805547925291, 474, 0.008542901168549, 475, 0.008352510943137, 476, 0.008374302595443, 477,
            0.008107068122426, 478, 0.00788341695402, 479, 0.007775032157024, 480, 0.007611594764728, 481, 0.007531309729915, 482, 0.007482565244494, 483,
            0.007167733215123, 484, 0.007195259512773, 485, 0.007176335183139, 486, 0.006976769525177, 487, 0.006807024023003, 488, 0.006660790566738, 489,
            0.006671112928357, 490, 0.006567889312169, 491, 0.006382660267567, 492, 0.006263379644418, 493, 0.006125174691634, 494, 0.006039728476012, 495,
            0.005903243916832, 496, 0.00577192053846, 497, 0.005691062039114, 498, 0.005588985351995, 499, 0.005419239849821, 500, 0.005337234421406, 501,
            0.00530626733655, 502, 0.005231716947081, 503, 0.005060251051304, 504, 0.004982833339164, 505, 0.00484864263812, 506, 0.004819969411402, 507,
            0.004596891707531, 508, 0.004581408165103, 509, 0.00442886659896, 510, 0.004364065106576, 511, 0.004306145188604, 512, 0.004205788895089, 513,
            0.004128944647483, 514, 0.004064716619633, 515, 0.003814112618112, 516, 0.003897264975596, 517, 0.003734974512369, 518, 0.003672466878122, 519,
            0.003592755307844, 520, 0.003419569018464, 521, 0.003379426501058, 522, 0.003332975873774, 523, 0.003283657923818, 524, 0.003205093282608, 525,
            0.00310301659549, 526, 0.003039935496709, 527, 0.003027892741487, 528, 0.002879938891619, 529, 0.002766392913813, 530, 0.002726250396407, 531,
            0.002640804180786, 532, 0.002599514734311, 533, 0.002590339301761, 534, 0.002572561901196, 535, 0.002387906321128, 536, 0.002362673881615, 537,
            0.002293284672956, 538, 0.00233055986769, 539, 0.002284109240406, 540, 0.002112643344629, 541, 0.002222748535228, 542, 0.00203407870342, 543,
            0.001968130281967, 544, 0.001970424140104, 545, 0.001914224615736, 546, 0.001858025091367, 547, 0.00175078722344, 548, 0.001827631471045, 549,
            0.001704336596155, 550, 0.001712365099637, 551, 0.001637241245634, 552, 0.001621757703206, 553, 0.001570719359647, 554, 0.001534017629447, 555,
            0.001429647084191, 556, 0.001445130626619, 557, 0.001445130626619, 558, 0.00131667457092, 559, 0.001331011184279, 560, 0.001362551733669, 561,
            0.001312660319179, 562, 0.001267930085498, 563, 0.001220905993679, 564, 0.001182483869876, 565, 0.001171588043723, 566, 0.001146929068745, 567,
            0.001063203246727, 568, 0.001068364427536, 569, 0.001038544271749, 570, 0.000956538843334, 571, 0.001017326083977, 572, 0.000941055300905, 573,
            0.000884855776537, 574, 0.000865357982368, 575, 0.000903206641637, 576, 0.000865357982368, 577, 0.000796542238244, 578, 0.000792527986503, 579,
            0.000774750585937, 580, 0.000749518146425, 581, 0.000731167281325, 582, 0.000776470979541, 583, 0.000646294530238, 584, 0.000641133349429, 585,
            0.000694465551125, 586, 0.000659484214528, 587, 0.000659484214528, 588, 0.000610739729107, 589, 0.000583213431457, 590, 0.000584360360526, 591,
            0.000559127921013, 592, 0.000568303353563, 593, 0.000528734300692, 594, 0.000493752964095, 595, 0.000526440442554, 596, 0.000478842886201, 597,
            0.000488591783285, 598, 0.000422069897298, 599, 0.000451316588551, 600, 0.000417482181023, 601, 0.000427231078108, 602, 0.000411174071145, 603,
            0.000423790290901, 604, 0.000377913128152, 605, 0.000392823206045, 606, 0.000373325411877, 607, 0.00040257210313, 608, 0.00036587037293, 609,
            0.00035669494038, 610, 0.000334903288074, 611, 0.000313111635767, 612, 0.000334329823539, 613, 0.000325154390989, 614, 0.000291319983461, 615,
            0.000305656596821, 616, 0.00029246691253, 617, 0.000287879196255, 618, 0.000264367150346, 619, 0.000268954866621, 620, 0.000254618253261, 621,
            0.000263793685811, 622, 0.000255191717796, 623, 0.000232826600955, 624, 0.000251177466055, 625, 0.000221930774802, 626, 0.000220783845733, 627,
            0.000221357310268, 628, 0.00021504920039, 629, 0.000190390225412, 630, 0.000202432980634, 631, 0.00020071258703, 632, 0.000184655580068, 633,
            0.000183508650999, 634, 0.000161716998693, 635, 0.000155982353349, 636, 0.000163437392296, 637, 0.000158849676021, 638, 0.000154261959746, 639,
            0.000140498810921, 640, 0.000138778417318, 641, 0.000151394637074, 642, 0.00013247030744, 643, 0.000134190701043, 644, 0.000145659991731, 645,
            0.000151968101609, 646, 0.000122147945821, 647, 0.000123868339424, 648, 0.000127882591165, 649, 0.000133617236509, 650, 0.000127882591165, 651, 0 };
    printNmVectorForGPS(pwoEmissionSpectrum);
}

void msgBoxInfo(const char *title, const char *text) {
    new TGMsgBox(gClient->GetRoot(), NULL, title, text, EMsgBoxIcon::kMBIconAsterisk, EMsgBoxButton::kMBOk);
}

void msgBoxError(const char *title, const char *text) {
    new TGMsgBox(gClient->GetRoot(), NULL, title, text, EMsgBoxIcon::kMBIconStop, EMsgBoxButton::kMBOk);
}

Int_t msgBoxQuestion(const char *title, const char *text) {
    int ret = EMsgBoxButton::kMBNo;
    new TGMsgBox(gClient->GetRoot(), NULL, title, text, EMsgBoxIcon::kMBIconQuestion, EMsgBoxButton::kMBYes | EMsgBoxButton::kMBNo, &ret);
    return ret;
}

void addPaveStatsEntry(TVirtualPad *pad, const char *text) {
    TPaveStats *pave = (TPaveStats*) pad->GetPrimitive("stats");
    if (!pave) {
        return;
    }
    TText *tText = new TText(0, 0, text);
    pave->GetListOfLines()->Add(tText);
    pad->Modified();
}

Int_t getNumberOfLines(TPave *pave) {
    if (TPaveText *paveText = dynamic_cast<TPaveText*>(pave)) {
        return paveText->GetListOfLines()->GetSize();
    } else if (TLegend *legend = dynamic_cast<TLegend*>(pave)) {
        return legend->GetNRows();
    }
    return 1;
}

TLegend* getPadLegend(TVirtualPad *pad) {
    // ROOT creates legend always with name "TPave" - thats how we find it on the pad
    TLegend *legend = (TLegend*) pad->FindObject("TPave");
    return legend;
}

TPaveStats* getPadStats(TVirtualPad *pad) {
    // Update Pad - in case the histogram was just drawn - need to update
    pad->Update();

    // By default ROOT names the statistics box "stats". So we just find and return this object.
    TPaveStats *pave = (TPaveStats*) pad->GetPrimitive("stats");

    // SCENARIO 1: If statistics box was found and not produced for histogram (say, made for TGraph) - we just return it
    if (pave && !pave->GetParent()->InheritsFrom("TH1")) {
        return pave;
    }

    // SCENARIO 2: If statistics box was drawn for a histogram then it is not a direct primitive of a canvas.
    //             Stats is a primitive of a histogram. pad->getPrimitive() searches across primitives recursively.
    //             Therefore we need to "disconnect it" from the histogram, rename it to "mystats" and add to canvas with different name.

    // If previously renamed PaveStats, detached it from histogram and added to the pad primitives - simply return it
    TPaveStats *paveDetachedFromHistAndRenamed = (TPaveStats*) pad->GetPrimitive("mystats");
    if (paveDetachedFromHistAndRenamed)
        return paveDetachedFromHistAndRenamed;

    // Otherwise perform this trick:
    // Remove PaveText from histogram primitives, rename it and add to the list of Pad primitives
    if (!pave)
        return NULL;

    pave->SetName("mystats");                     // rename
    ((TH1*) pave->GetParent())->SetStats(kFALSE); // disconnect from the histogram.
    pad->GetListOfPrimitives()->Add(pave);        // attach to pad primitives

    return pave;

    // If not found search object by file type
    // for (TObject* object : *(pad->GetListOfPrimitives())){
    //  std::cout << object->GetName() << std::endl;
    //  if (object->InheritsFrom(TPaveStats::Class())){
    //    return (TPaveStats*) object;
    //  }
    // }

    // return NULL;
}

void alignPave(TPave *pave, TVirtualPad *pad, Alignment alignment, Decoration decoration, Double_t statsLineHeight, Double_t statsWidth) {
    if (!pave) {
        // Try finding Legend first
        pave = getPadLegend(pad);
        if (!pave) {
            // Next try finding pad
            pave = getPadStats(pad);
        }
        if (!pave) {
            std::cout << "alignPave: could not find neither legend nor pave on canvas" << std::endl;
            return;
        }
    }

    // Update
    pad->Modified();
    pad->Update();

    // Legend height needs recalculation
    // it depends on the number of hte lines in the legend
    // std::cout << "Number of lines: " << getNumberOfLines(pave) << std::endl;
    Double_t legendHeight = statsLineHeight * getNumberOfLines(pave); // stats->GetY2NDC() - stats->GetY1NDC();
    Double_t legendWidth = statsWidth == 0 ? (pave->GetX2NDC() - pave->GetX1NDC()) : statsWidth;
    // std::cout << legendWidth << "x" << legendHeight << std::endl;

    Double_t dx = (decoration == Decoration::TRANSPARENT) ? 0.04 : 0;
    Double_t dy = (decoration == Decoration::TRANSPARENT) ? 0.04 : 0;
    if (alignment == Alignment::TOP_LEFT) {
        pave->SetX1NDC(pad->GetLeftMargin() + dx);
        pave->SetY1NDC(1 - pad->GetTopMargin() - legendHeight - dy);
        pave->SetX2NDC(pad->GetLeftMargin() + legendWidth + dx);
        pave->SetY2NDC(1 - pad->GetTopMargin() - dy);
    } else if (alignment == Alignment::BOTTOM_RIGHT) {
        pave->SetX1NDC(1 - pad->GetRightMargin() - legendWidth);
        pave->SetY1NDC(pad->GetBottomMargin() + 2 * dy);
        pave->SetX2NDC(1 - pad->GetRightMargin());
        pave->SetY2NDC(pad->GetBottomMargin() + 2 * dy + legendHeight);
    } else if (alignment == Alignment::TOP_RIGHT) {
        pave->SetX1NDC(1 - pad->GetRightMargin() - legendWidth - dx);
        pave->SetY1NDC(1 - pad->GetTopMargin() - legendHeight - dy);
        pave->SetX2NDC(1 - pad->GetRightMargin() - dx);
        pave->SetY2NDC(1 - pad->GetTopMargin() - dy);
    }

    if (decoration == Decoration::TRANSPARENT) {
        pave->SetLineWidth(0);
        pave->SetFillStyle(0);
    }
    pad->Modified();
    pad->Update();
}

void alignStats(TVirtualPad *pad, Alignment alignment, Decoration decoration, Double_t statsLineHeight, Double_t statsWidth) {
    // Find stats box on canvas
    TPave *stats = getPadStats(pad);
    alignPave(stats, pad, alignment, decoration, statsLineHeight, statsWidth);
}

void alignLegend(TVirtualPad *pad, Alignment alignment, Decoration decoration, Double_t statsLineHeight, Double_t statsWidth) {
    // Find legend on canvas
    TPave *legend = getPadLegend(pad);
    alignPave(legend, pad, alignment, decoration, statsLineHeight, statsWidth);
}

void alignLegend(TVirtualPad *pad, Double_t statsWidth, Alignment alignment, Decoration decoration, Double_t statsLineHeight) {
    alignLegend(pad, alignment, decoration, statsLineHeight, statsWidth);
}

void addGrid(TVirtualPad *pad) {
    pad->SetGridx();
    pad->SetGridy();
}

TPad* addOverlayPad(TCanvas *canvas, Int_t padNumberToOverlay /* 1st = 1, 2nd = 2, ...*/, Int_t currentNPads /* current number of pads */) {
    // Add (n+1)-th extera pad to canvas that overlays n-th - refer to TPad::Divide()
    TString bottomPadName = TString::Format("%s_%d", canvas->GetName(), padNumberToOverlay);
    TString overlayPadName = TString::Format("%s_%d", canvas->GetName(), currentNPads + 1);
    // TPad* pad = new TPad(padName.Data(), "", 0.4, 0.6, 0.4, 0.6);
    TVirtualPad *bottomPad = (TVirtualPad*) canvas->GetListOfPrimitives()->FindObject(bottomPadName.Data());
    TPad *overlayPad = new TPad(overlayPadName.Data(), "", bottomPad->GetXlowNDC(), bottomPad->GetYlowNDC(), bottomPad->GetXlowNDC() + bottomPad->GetWNDC(),
            bottomPad->GetYlowNDC() + bottomPad->GetHNDC());
    overlayPad->SetNumber(currentNPads + 1);
    overlayPad->SetFillStyle(0);
    overlayPad->SetFrameFillStyle(0);
    overlayPad->Draw();
    canvas->Modified();
    return overlayPad;
}

void addGraph(TMultiGraph *mg, TFile *file, EMarkerStyle markerStyle, Color_t markerColor) {
    TGraph *graph = (TGraph*) file->Get("efficiency");
    TObjString *name = (TObjString*) file->Get("name");
    graph->SetTitle(name->GetString().Data());
    graph->SetMarkerStyle(markerStyle); // https://root.cern.ch/doc/master/classTAttMarker.html
    graph->SetMarkerColor(markerColor);
    graph->SetLineColor(markerColor);
    mg->Add(graph);
}

void increaseAxisFontSize(TAxis *axis, Float_t factor) {
    axis->SetLabelSize(factor * (axis->GetLabelSize()));
    axis->SetTitleSize(factor * (axis->GetTitleSize()));
}

void increaseGraphFontSize(TGraph *graph, Float_t factor) {
    increaseAxisFontSize(graph->GetXaxis(), factor);
    increaseAxisFontSize(graph->GetYaxis(), factor);
}

void movePalleteAxis(TVirtualPad *canvas, TH1 *hist) {
    // Move PalleteAxis to reveal 10^3 label
    canvas->Update();
    TPaletteAxis *palette = (TPaletteAxis*) hist->GetListOfFunctions()->FindObject("palette");
    Double_t paletteShiftPx = 30;
    Double_t canvasHeight = canvas->GetWh();
    palette->SetY1NDC(canvas->GetBottomMargin() + paletteShiftPx / canvasHeight);

    canvas->Modified();
    canvas->Update();
}

EnergyValueUnit formatEnergy(Double_t value) {
    if (value >= 1E6) {
        EnergyValueUnit evu;
        evu.unit = "TeV";
        evu.value = value / 1E6;
        return evu;
    }
    if (value >= 1E3) {
        EnergyValueUnit evu;
        evu.unit = "GeV";
        evu.value = value / 1E3;
        return evu;
    }
    if (value >= 1) {
        EnergyValueUnit evu;
        evu.unit = "MeV";
        evu.value = value;
        return evu;
    }
    EnergyValueUnit evu;
    evu.unit = "keV";
    evu.value = value * 1000;
    return evu;
}

TFile* openFile(const char *fileName) {
    std::cout << "Opening file \"" << fileName << "\"..." << std::endl;
    TFile *file = new TFile(fileName);
    if (file->IsZombie()) {
        file->Close();
        std::cout << "Error opening file \"" << fileName << "\". Aborting." << std::endl;
        exit(1);
    }
    return file;
}

TTree* getTree(TFile *file, const char *treeName) {
    TTree *tree = (TTree*) file->Get(treeName);
    if (!tree) {
        std::cout << "Cannot find tree with name \"" << treeName << "\". Aborting." << std::endl;
        return NULL;
    }
    // std ::cout << "Number of events in the tree: " << tree->GetEntries() << std::endl;
    // tree->Print();
    return tree;
}

TCanvas* getCanvas() {
    // TODO: finish if needed
    // static Int_t canvasCounter = 0;
    TCanvas *canvas = new TCanvas();
    return canvas;
}

void addMultiCanvasTitle(TCanvas *canvas, const char *title, const char *subtitle) {
    // Define heights for title and subtitle (in pixels)
    const Int_t titleHeightPx = 40;
    const Int_t subtitleHeightPx = 25;

    Int_t titlePadHeight = titleHeightPx + (subtitle == 0 ? 0 : subtitleHeightPx);

    Int_t canvasHeightPx = canvas->GetWh();
    Double_t titlePadHeightNDC = (Double_t) titlePadHeight / (Double_t) canvasHeightPx;

    // If canvas has no child pads - create child pad and copy canvas into its child pad
    if (canvas->GetPad(1) == NULL) {
        canvas->Divide(1, 1);
        TVirtualPad *pad = canvas->cd(1);
        pad->SetPad(0, 0, 1, 1); // Remove subpad margin
        canvas->DrawClonePad(); // Clone on its own subpad?

        // Delete all primitives but keep subpads
        TList *primitives = canvas->GetListOfPrimitives();
        if (primitives) {
            TListIter next(primitives);
            TObject *object;
            while ((object = next())) {
                if (!object->InheritsFrom(TPad::Class())) {
                    // Not deleting primitives but removing them from the list - safer.
                    // https://root.cern/root/roottalk/roottalk00/2082.html
                    primitives->Remove(object);
                }
            }
        }

        // TODO: do we need this?
        canvas->Modified();
        canvas->Update();
    }

    // Resize child canvases to free space on to for title
    // If Canvas has child pads
    for (Int_t i = 1; canvas->GetPad(i) != NULL; i++) {
        TVirtualPad *childPad = canvas->GetPad(i);
        Double_t yLow = childPad->GetAbsYlowNDC();
        Double_t xLow = childPad->GetAbsXlowNDC();
        Double_t height = childPad->GetAbsHNDC();
        Double_t width = childPad->GetAbsWNDC();
        childPad->SetPad(xLow, yLow * (1 - titlePadHeightNDC), xLow + width, (yLow + height) * (1 - titlePadHeightNDC));
        canvas->Modified();
        canvas->Update();
    }

    // Add extra pad for the title on top
    canvas->cd();

    // Add title text (fixed size in px)
    Double_t titleY = 1. - titleHeightPx / 2. / canvasHeightPx;
    TText *t = new TText(0.5, titleY, title);
    t->SetNDC(kTRUE);
    t->SetTextFont(63);
    t->SetTextSizePixels(21);
    t->SetTextAlign(ETextAlign::kHAlignCenter + ETextAlign::kVAlignCenter);
    t->Draw();

    // Add subtitle text
    if (subtitle == 0)
        return;
    Double_t subtitleY = 1. - (titleHeightPx + subtitleHeightPx / 2.) / canvasHeightPx;
    TText *s = new TText(0.5, subtitleY, subtitle);
    s->SetNDC(kTRUE);
    s->SetTextFont(63);
    s->SetTextColor(kGray + 2);
    s->SetTextSizePixels(16);
    s->SetTextAlign(ETextAlign::kHAlignCenter + ETextAlign::kVAlignCenter);
    s->Draw();

    canvas->Modified();
    canvas->Update();
}

void printParameters(TF1 *func) {
    std::cout << "Function \"" << func->GetName() << "\"" << std::endl;
    std::cout << std::setw(20) << std::left << "Parameter name" << std::setw(10) << "Value" << std::setw(10) << "Min" << std::setw(10) << "Max" << std::endl;

    for (Int_t i = 0; i < func->GetNpar(); i++) {
        Double_t min, max;
        func->GetParLimits(i, min, max);
        std::cout << std::setw(20) << std::left << func->GetParName(i) << func->GetParameter(i) << min << max << std::endl;
    }
}

TList* getFileNamesList(const char *text, Bool_t isMultiple) {
    // Show information message box
    if (strlen(text)) {
        msgBoxInfo("Open Files", text);
    }

    // Show file picker
    const char *filetypes[] = { // "All files",     "*",
            "ROOT files", "*.root", 0, 0 };
    TGFileInfo fi;
    fi.fFileTypes = filetypes;
    if (isMultiple)
        fi.SetMultipleSelection(kTRUE);

    // Usability: open the dialog in my home directory (if running on my laptop)
    char dir[] = "/home/petrstepanov/Yandex.Disk/20_Tanja_Horn/Data";
    if (!gSystem->AccessPathName(dir)) {
        // Not works wit old farm ROOT
        fi.SetIniDir(dir);
        // Crashes when debuging new root 6.24
        // fi.fIniDir = dir;
    }

    new TGFileDialog(gClient->GetRoot(), 0, kFDOpen, &fi);

    // If selected multiple files then output energy resolution plot
    if (fi.fMultipleSelection && fi.fFileNamesList) {
        TList *fileNamesList = new TList();
        for (TObject *obj : *(fi.fFileNamesList)) {
            if (!obj->InheritsFrom(TObjString::Class()))
                continue;
            TObjString *objString = (TObjString*) obj;
            TObjString *newObjString = new TObjString(objString->GetString().Data());
            fileNamesList->Add(newObjString);
        }
        fileNamesList->Sort(kSortAscending);
        std::cout << "getFileNamesList(): selected multiple files." << std::endl;
        fileNamesList->Print();
        return fileNamesList;
    }
    // If one file is selected
    else if (fi.fFilename) {
        TList *fileNamesList = new TList();
        TObjString *fileName = new TObjString(fi.fFilename);
        fileNamesList->Add(fileName);
        std::cout << "getFileNamesList(): selected one file." << std::endl;
        fileNamesList->Print();
        return fileNamesList;
    }
    // If no files selected
    std::cout << "getFileNamesList(): no files selected." << std::endl;
    return new TList();
}

const char* getFileName(const char *text) {
    TList *fileNames = getFileNamesList(text, kFALSE);
    TObject *obj = fileNames->At(0);
    TObjString *objString = (TObjString*) obj;
    const char *fileName = objString->GetString().Data();
    return fileName;
}

Int_t exportCurve(TF1 *func, TString filename, const char *delimeter) {
    std::ofstream myfile;
    myfile.open(filename.Data());
    if (!myfile.is_open())
        return 1;

    Double_t step = (func->GetXmax() - func->GetXmin()) / (func->GetNpx() - 1);
    for (Double_t x = func->GetXmin(); x <= func->GetXmax(); x += step) {
        myfile << x << delimeter << func->Eval(x) << "\n";
    }
    myfile.close();
    return 0;
}

Int_t findFirstNonZeroBin(TH1 *hist) {
    int bin = 0;
    if (!hist) {
        return 0;
    }

    for (bin = 1; hist->GetBinContent(bin) == 0 && bin <= hist->GetNbinsX(); bin++) {
        ;
    }
    return bin;
}

Int_t findFirstNonZeroBin(std::vector<TH1*> histList) {
    int minFirstNonZeroBin = std::numeric_limits<int>::max();
    for (TH1 *hist : histList) {
        Int_t firstNonZeroBin = findFirstNonZeroBin(hist);
        if (firstNonZeroBin < minFirstNonZeroBin)
            minFirstNonZeroBin = firstNonZeroBin;
    }
    return minFirstNonZeroBin;
}

Int_t findLastNonZeroBin(TH1 *hist) {
    int bin;
    for (bin = hist->GetNbinsX(); hist->GetBinContent(bin) == 0 && bin >= 1; bin--) {
        ;
    }
    return bin;
}

Int_t findLastNonZeroBin(std::vector<TH1*> histList) {
    Int_t maxLastNonZeroBin = 0;
    for (TH1 *hist : histList) {
        Int_t lastNonZeroBin = findLastNonZeroBin(hist);
        if (lastNonZeroBin > maxLastNonZeroBin)
            maxLastNonZeroBin = lastNonZeroBin;
    }
    return maxLastNonZeroBin;
}

TH2* makeHistQueue(std::vector<TH1*> histList, const char *name, Double_t crystalLength) {
    TH1 *firstHist = histList[0];
    if (!firstHist)
        return NULL;

    Int_t nUnitVolumes = (Int_t) histList.size();

    TH2 *histQueue = new TH2D(name, "", firstHist->GetXaxis()->GetNbins(), firstHist->GetXaxis()->GetBinLowEdge(1),
            firstHist->GetXaxis()->GetBinUpEdge(firstHist->GetXaxis()->GetNbins()), nUnitVolumes, 0, crystalLength);
    Int_t y = 1;
    for (TH1 *hist : histList) {
        for (int x = 1; x <= hist->GetXaxis()->GetNbins(); x++) {
            for (int c = 0; c < hist->GetBinContent(x); c++) {
                histQueue->Fill(hist->GetBinCenter(x), crystalLength / (2 * nUnitVolumes) + (y - 1) * crystalLength / nUnitVolumes);
            }
        }
        y++;
    }
    return histQueue;
}

// Interpolation for the TGraph
Double_t getTGraphValue(Double_t x, TGraph *graph) {
    // Test if x is within limits
    if (x < graph->GetPointX(0))
        return 0;
    if (x > graph->GetPointX(graph->GetN() - 1))
        return 0;

    int pointNumber = 0;
    for (; x >= graph->GetPointX(pointNumber); pointNumber++) {
        ;
    }

    Double_t ratio = (x - graph->GetPointX(pointNumber - 1)) / (graph->GetPointX(pointNumber) - graph->GetPointX(pointNumber - 1));
    Double_t y = graph->GetPointY(pointNumber - 1) + (graph->GetPointY(pointNumber) - graph->GetPointY(pointNumber - 1)) * ratio;

    /*
     std::cout << "Point A:      x=" << graph->GetPointX(pointNumber-1) << " y=" << graph->GetPointY(pointNumber-1) << std::endl;
     std::cout << "Point B:      x=" << graph->GetPointX(pointNumber) << " y=" << graph->GetPointY(pointNumber) << std::endl;
     std::cout << "ratio=" << ratio << std::endl;
     std::cout << "Interpolated: x=" << x << " y=" << y << std::endl;
     */

    return y;
}

TGraph* getEfficiencyGraph(const char *detector) {
    // Open correcponding material file
    TString detectorFileName = "detectors/";
    detectorFileName += detector;
    detectorFileName += ".root";
    TFile *detectorFile = openFile(detectorFileName);
    TGraph *efficiencyGraph = (TGraph*) detectorFile->Get("efficiency");
    return efficiencyGraph;
}

TH1* convertOpToPe(TH1 *histOp, const char *detector) {
    TGraph *efficiencyGraph = getEfficiencyGraph(detector);

    TString histPeName = histOp->GetName();
    histPeName += "-PE";
    TH1 *histPe = (TH1*) histOp->Clone(histPeName);
    histPe->Reset();

    std::cout << "Efficiency graph:" << std::endl;
    efficiencyGraph->Print();

    for (int bin = 1; bin <= histOp->GetXaxis()->GetNbins(); bin++) {
        Int_t op = histOp->GetBinContent(bin);
        Double_t efficiency = getTGraphValue(histOp->GetBinCenter(bin), efficiencyGraph);
        Double_t pe = op * efficiency / 100.;

        for (int p = 1; p <= pe; p++)
            histPe->Fill(histOp->GetBinCenter(bin));
    }

    return histPe;
}

TH1* cloneHist(TH1 *hist, const char *newName) {
    Int_t nBins = hist->GetNbinsX();
    TH1 *newHist = new TH1I(newName, "", nBins, hist->GetXaxis()->GetBinLowEdge(1), hist->GetXaxis()->GetBinUpEdge(nBins));
    newHist->Reset();
    for (int bin = 1; bin <= hist->GetNbinsX(); bin++) {
        Int_t value = hist->GetBinContent(bin);
        for (int i = 0; i < value; i++) {
            newHist->Fill(hist->GetBinCenter(bin));
        }
    }
    return newHist;
}

void setCanvasTitle(TVirtualPad *pad, TH1 *firstHist, const char *newTitle) {
    // Draw legend first. Then change title.
    // Histogram title is reflected on the canvas
    firstHist->SetTitle(newTitle);

    // Delete the "title" TPaveText object. It will be recreated with proper size.
    TList *primitives = pad->GetListOfPrimitives();
    if (!primitives)
        return;
    TListIter iter(primitives);
    TObject *object;
    while ((object = iter())) {
        if (object->InheritsFrom(TPaveText::Class()) && strcmp(object->GetName(), "title") == 0) {
            TPaveText *paveText = (TPaveText*) object;
            paveText->Delete();
        }
    }
    pad->Modified();
    pad->Update();
}

TString formatMaterialAndSize(TFile *file) {
    TString s = "";
    s += ((TObjString*) file->Get("crystalMaterial"))->GetString();
    s += " ";
    s += ((TVector3*) file->Get("crystalSize"))->X();
    s += "x";
    s += ((TVector3*) file->Get("crystalSize"))->Y();
    s += "x";
    s += ((TVector3*) file->Get("crystalSize"))->Z();
    s += " mm";
    return s;
}

Double_t findHistsMax(std::vector<TH1*> hists) {
    Double_t yMax = 0;
    for (TH1 *hist : hists) {
        Double_t histMax = hist->GetBinContent(hist->GetMaximumBin());
        if (histMax > yMax)
            yMax = histMax;
    }
    return yMax;
}

// template function to iterate over a tlist
// we select objects from TList that inherited from T
/* https://stackoverflow.com/questions/2298242/callback-functions-in-c\ */
// 3.3.2 Lambda expressions
template<class T>
void iterateTList(TList *list, std::function<void(T*)> fp) {
    TListIter iter(list);
    TObject *object;
    while ((object = iter())) {
        T *castedObject = dynamic_cast<T*>(object);
        if (castedObject != NULL) {
            fp(castedObject);
        }
    }
}

// Get Graph maximum
Double_t getGraphMax(TGraph *graph) {
    Double_t max = graph->GetPointY(0);
    for (int i = 1; i < graph->GetN(); i++) {
        if (graph->GetPointY(i) > max)
            max = graph->GetPointY(i);
    }
    return max;
}

// This function returns a list of filenames in directory with certain extension
TList* getFilesListInDrectory(const char *dirPath, const char *ext) {
    // Check if directory exist
    if (gSystem->AccessPathName(dirPath)) {
        std::cout << "Directory " << dirPath << " does not exist." << std::endl;
        return NULL;
    }

    // Create list of filenames to return
    // Grab spectra filenames from input directory
    TSystemDirectory *dir = new TSystemDirectory(".", dirPath);
    if (dir->IsDirectory()) {
        if (TList *files = dir->GetListOfFiles()) {
            files->Sort();
            TIter next(files);
            TList *fileNames = new TList();
            while (TSystemFile *file = (TSystemFile*) next()) {
                TString fileName = file->GetName();
                if (!file->IsDirectory() && fileName.EndsWith(ext)) {
                    std::cout << "* " << fileName << std::endl;
                    TObjString *objFileName = new TObjString(fileName.Data());
                    fileNames->Add(objFileName);
                } else {
                    std::cout << "  " << fileName << std::endl;
                }
            }
        }

        std::cout << "\"" << dirPath << "\" is not a directory";
        return NULL;
    }

    return new TList();
}

// Minimum  column width
int minAsciiColWidth = 10;

Int_t exportTreeToGnuplot(TTree *tree, TString filename, const char *delimeter, Bool_t append) {
    TObjArray *branches = tree->GetListOfBranches();

    // Fill colNames and zero colValues
    std::vector<TString> colNames;
    const Int_t nBranches = tree->GetNbranches();
    Double_t *colValues = new Double_t[nBranches];
    for (int i = 0; i < branches->GetEntries(); i++) {
        const char *branchName = branches->At(i)->GetName();
        const char *branchTitle = branches->At(i)->GetTitle();
        colNames.push_back(branchTitle);
        colValues[i] = 0;
        tree->SetBranchAddress(branchName, &colValues[i]);
    }

    std::ofstream myfile;
    if (append) {
        if (gSystem->AccessPathName(filename)) {
            myfile.open(filename);
        } else {
            myfile.open(filename, std::ios_base::app);
        }
    } else {
        myfile.open(filename);
    }

    // Check success open
    if (!myfile.is_open()) {
        std::cout << "Cannot create \"" << filename << "\" file.";
        return 1;
    }

    // Write column numbers
    myfile << "# ";
    for (int i = 0; i < (int) colNames.size(); i++) {
        TString colNumber = "$";
        colNumber += (i + 1);
        myfile << std::left << std::setw(std::max(minAsciiColWidth, (int) colNames[i].Length())) << colNumber;
        if (i != (int) colNames.size() - 1)
            myfile << delimeter;
    }
    myfile << std::endl;

    // Write header
    myfile << "# ";
    for (int i = 0; i < (int) colNames.size(); i++) {
        TString colName = colNames[i];
        myfile << std::left << std::setw(std::max(minAsciiColWidth, (int) colNames[i].Length())) << colName;
        if (i != (int) colNames.size() - 1)
            myfile << delimeter;
    }
    myfile << std::endl;

    // Write data
    for (int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        myfile << "  ";
        for (int j = 0; j < nBranches; j++) {
            myfile << std::left << std::setw(std::max(minAsciiColWidth, (int) colNames[j].Length())) << colValues[j];
            if (j != nBranches - 1)
                myfile << delimeter;
        }
        myfile << std::endl;
    }
    return 0;
}

Int_t exportPointToGnuplot(TString filename, std::vector<std::string> colNames, std::vector<double> values, const char *delimeter) {
    std::ofstream myfile;
    Bool_t fileExisted = kFALSE;
    if (gSystem->AccessPathName(filename, EAccessMode::kFileExists)) {
      // File not exists
      myfile.open(filename);
    } else {
      // File exists
        myfile.open(filename, std::ios_base::app);
        fileExisted = kTRUE;
    }
    // Check file opened successfully
    if (!myfile.is_open()) {
        Error("exportPointToGnuplot()", "Cannot create \"%s\"", filename.Data());
        return 1;
    }

    // Write header if file was not existing before
    if (!fileExisted){
      myfile << "# ";
      for (std::string colName : colNames) {
          myfile << std::setw(std::max(minAsciiColWidth, (int) colName.length())) << colName << delimeter;
      }
      myfile << std::endl;
    }

    // Write data
    myfile << "  ";
    int i = 0;
    for (double value : values) {
        myfile << std::setw(std::max(minAsciiColWidth, (int) colNames[i++].length())) << value << delimeter;
    }

    myfile.close();
    return 0;
}

Int_t exportGraphToGnuplot(TGraph *graph, const char *filename, const char *delimeter) {
    std::ofstream myfile;
    if (gSystem->AccessPathName(filename)) {
        myfile.open(filename);
    } else {
        myfile.open(filename, std::ios_base::app);
    }
    if (!myfile.is_open()) {
        std::cout << "Cannot create \"" << filename << "\" file.";
        return 1;
    }
    const char *xAxisTitle = strlen(graph->GetXaxis()->GetTitle()) > 0 ? graph->GetXaxis()->GetTitle() : "X";
    const char *yAxisTitle = strlen(graph->GetYaxis()->GetTitle()) > 0 ? graph->GetYaxis()->GetTitle() : "Y";
    const char *errColumnTitle = "Error";

    // Write header
    myfile << "# " << std::left << std::setw(std::max(minAsciiColWidth, (int) strlen(xAxisTitle))) << xAxisTitle << delimeter;
    if (graph->InheritsFrom(TGraphErrors::Class())) {
        myfile << std::setw(std::max(minAsciiColWidth, (int) strlen(errColumnTitle))) << errColumnTitle << delimeter;
    }
    myfile << std::setw(std::max(minAsciiColWidth, (int) strlen(yAxisTitle))) << yAxisTitle << delimeter;
    if (graph->InheritsFrom(TGraphErrors::Class())) {
        myfile << std::setw(std::max(minAsciiColWidth, (int) strlen(errColumnTitle))) << errColumnTitle;
    }
    myfile << std::endl;

    // Write data
    for (int i = 0; i < graph->GetN(); i++) {
        myfile << "  " << std::left << std::setw(std::max(minAsciiColWidth, (int) strlen(xAxisTitle))) << graph->GetPointX(i) << delimeter;
        if (graph->InheritsFrom(TGraphErrors::Class())) {
            myfile << std::setw(std::max(minAsciiColWidth, (int) strlen(errColumnTitle))) << graph->GetErrorX(i) << delimeter;
        }
        myfile << std::setw(std::max(minAsciiColWidth, (int) strlen(yAxisTitle))) << graph->GetPointY(i) << delimeter;
        if (graph->InheritsFrom(TGraphErrors::Class())) {
            myfile << std::setw(std::max(minAsciiColWidth, (int) strlen(errColumnTitle))) << graph->GetErrorY(i);
        }
        myfile << std::endl;
    }
    return 0;
}

void saveCanvasToDisk(TCanvas *canvas, const char *name, Bool_t savePNG, Bool_t saveC, Bool_t saveROOT) {
    if (savePNG) {
        TString pngName = name;
        pngName += ".png";
        canvas->SaveAs(pngName);
    }

    if (saveC) {
//    TString cName = name; cName += ".cpp";
//    canvas->SaveAs(cName);
    }

    if (saveROOT) {
        TString rootName = name;
        rootName += ".root";
        canvas->SaveAs(rootName);
    }
}

void saveGraphToDisk(TGraph *graph, const char *name, Bool_t saveGNUPLOT, Bool_t saveROOT) {
    // Export to Gnupot format
    if (saveGNUPLOT) {
        TString gnuplotName = name;
        gnuplotName += ".dat";
        exportGraphToGnuplot(graph, gnuplotName);
    }

    // Save as a ROOT file
    if (saveROOT) {
        TString rootName = name;
        rootName += ".root";
        TFile *graphFile = new TFile(rootName, "UPDATE");
        if (graphFile->IsZombie()) {
            std::cout << "Error opening file: " << rootName << std::endl;
        } else {
            graph->Write();
            graphFile->Close();
        }
    }
}

TString removeFileExtension(const char *fileNamePath) {
    TString s = fileNamePath;

    // Test here: https://regex101.com/r/7H6We8/1
    // Don't forget to escape c++ double slashes
    TObjArray *objArray = TPRegexp("^(.*)\\.\\w*$").MatchS(s);
    if (objArray->GetLast() + 1 != 2) {
        return TString();
    }
    const TString str = ((TObjString*) objArray->At(1))->GetString();
    return TString(str.Data());
}

TString getMaterialName(TFile *file) {
    TString string = ((TObjString*) file->Get("crystalMaterial"))->GetName();
    return string;
}

TString getMaterialName(const char *uri) {
    TFile *file = openFile(uri);
    TString string = getMaterialName(file);
    file->Close();
    return string;
}

TString getAssemblySize(TFile *file) {
    TVector2 *crystalsNumber = (TVector2*) file->Get("crystalsNumber");
    TString string = "";
    string += (int) crystalsNumber->X();
    string += "x";
    string += (int) crystalsNumber->Y();
    return string;
}

TString getAssemblySize(const char *uri) {
    TFile *file = openFile(uri);
    TString string = getAssemblySize(file);
    file->Close();
    return string;
}

TString getCrystalSize(TFile *file) {
    TVector3 *crystalSize = (TVector3*) file->Get("crystalSize");
    TString string = "";
    string += (int) crystalSize->X();
    string += "x";
    string += (int) crystalSize->Y();
    string += "x";
    string += (int) crystalSize->Z();
    return string;
}

TString getCrystalSize(const char *uri) {
    TFile *file = openFile(uri);
    TString string = getCrystalSize(file);
    file->Close();
    return string;
}

TString getIncidentParticleType(TFile *file) {
    TString string = ((TObjString*) file->Get("gpsParticleName"))->GetName();
    return string;
}

TString getIncidentParticleType(const char *uri) {
    TFile *file = openFile(uri);
    TString string = getIncidentParticleType(file);
    file->Close();
    return string;
}

Double_t getIncidentParticleEnergy(TFile *file) {
    return ((RooConstVar*) file->Get("gpsParticleEnergy"))->getVal();
}

Double_t getIncidentParticleEnergy(const char *uri) {
    TFile *file = openFile(uri);
    Double_t energy = getIncidentParticleEnergy(file);
    file->Close();
    return energy;
}

TString getIncidentParticlePosition(TFile *file) {
    RooConstVar *gpsPosition = (RooConstVar*) file->Get("gpsPosition");
    TString string = "";
    string += gpsPosition->getVal();
    return string;
}

TString getIncidentParticlePosition(const char *uri) {
    TFile *file = openFile(uri);
    TString string = getIncidentParticlePosition(file);
    file->Close();
    return string;
}

Int_t getNumberOfEvents(TFile *file) {
    RooConstVar *numberOfEvents = (RooConstVar*) file->Get("numberOfEvents");
    return (Int_t) numberOfEvents->getVal();
}

Int_t getNumberOfEvents(const char *uri) {
    TFile *file = openFile(uri);
    Int_t nEvents = getNumberOfEvents(file);
    file->Close();
    return nEvents;
}

TTree* sortTree(TTree *tree, const char *branchName) {
    Int_t nEntries = (Int_t) tree->GetEntries();
    tree->Draw(branchName, "", "goff");
    Int_t *index = new Int_t[nEntries];
    TMath::Sort(nEntries, tree->GetV1(), index, kFALSE);

    TTree *treeSorted = (TTree*) tree->CloneTree(0);
    for (Int_t i = 0; i < nEntries; i++) {
        tree->GetEntry(index[i]);
        treeSorted->Fill();
    }
    delete[] index;
    return treeSorted;
}

// Determine maximum emitted scintillation and cherenkov photons
// Here argument needs to be "branch/leaf". Get leaf name like this:
// tree->FindBranch("prod_op_chere")->GetListOfLeaves()->Print("V");
// P.S. all our branches have one leaf
Double_t getBranchMaximum(TTree *tree, const char *branchName) {
    const char *leafName = tree->FindBranch(branchName)->GetListOfLeaves()->At(0)->GetName();
    TString branchAndLeafName = TString::Format("%s/%s", branchName, leafName);
    return tree->GetMaximum(branchAndLeafName);
}

TText* makeLabelAtHistogramPeak(TH1 *hist, const char *text, Double_t ndcXOffset) {
    TString label = strlen(text) != 0 ? text : hist->GetTitle();
    Double_t x = hist->GetBinCenter(hist->GetMaximumBin()) + ndcXOffset * (hist->GetXaxis()->GetXmax() - hist->GetXaxis()->GetXmin());
    Double_t y = hist->GetBinContent(hist->GetMaximumBin());
    return new TText(x, y, label);
}

std::pair<Double_t, Double_t> getBranchDistributionPerMeV(const char *fileName, const char *branch, Bool_t draw) {
    TFile *file = openFile(fileName);
    TTree *tree = getTree(file, "tree_optical");

    // Write branch per/MeV
    if (draw)
        new TCanvas();
    TString option = TString::Format("%s/%f", branch, getIncidentParticleEnergy(fileName));
    tree->Draw(option, "", draw ? "" : "goff");
    Long64_t n = tree->GetBranch(branch)->GetEntries();
    return std::make_pair(TMath::Mean(n, tree->GetV1()), TMath::RMS(n, tree->GetV1()));
}

TString getFileNameFromPath(const char *fileNamePath) {
    // Get file path directory
    TString dirName = gSystem->DirName(fileNamePath);

    // Remove directory from path
    TString fileNamePathString = fileNamePath;
    fileNamePathString.ReplaceAll(dirName, "");

    // Remove leading '/' or '\'
    TString str = fileNamePathString(1, fileNamePathString.Length() - 1);

    return str;
}

CrystalBallFunctionObject::CrystalBallFunctionObject(Bool_t _isReversed) {
    isReversed = _isReversed;
}

Double_t CrystalBallFunctionObject::operator()(double *_x, double *par) {
    // https://en.wikipedia.org/wiki/Crystal_Ball_function
    LongDouble_t x = _x[0];
    LongDouble_t a = par[0];
    LongDouble_t n = par[1];
    LongDouble_t mean = par[2];
    LongDouble_t sigma = par[3];
    LongDouble_t norm = par[4];

    LongDouble_t abs_a = TMath::Abs(a);

    LongDouble_t A = TMath::Power(n / abs_a, n) * TMath::Exp(-abs_a*abs_a / 2);
    LongDouble_t B = n / abs_a - abs_a;
    LongDouble_t C = n / abs_a / (n - 1) * TMath::Exp(-abs_a*abs_a / 2);
    LongDouble_t D = TMath::Sqrt(TMath::PiOver2()) * (1 + TMath::Erf(abs_a / TMath::Sqrt2()));
    LongDouble_t N = 1 / sigma / (C + D);

    // > a not working

    if (x - mean > (isReversed ? 1:-1)* a * sigma) {
        return norm * N * TMath::Exp(-TMath::Power(x - mean,(LongDouble_t)2) / 2 / TMath::Power(sigma,(LongDouble_t)2));
    }
    return norm * N * A * TMath::Power(B - (x - mean) / sigma, -n);
}

GaussFunctionObject::GaussFunctionObject(TH1 *hist) {
    // if option "width" is specified, the integral is the sum of
    // the bin contents multiplied by the bin width in x.
    this->normalization = hist->Integral("width");
}

Double_t GaussFunctionObject::operator()(double *_x, double *par) {
    Double_t x = _x[0];
    Double_t mean = par[0];
    Double_t sigma = par[1];

    return normalization * 1 / sigma / TMath::Sqrt(2 * TMath::Pi()) * TMath::Exp(-(x - mean) * (x - mean) / 2 / sigma / sigma);
}

TF1* fitHistWithROOTCrystalBall(TH1 *hist, Bool_t inverted) {
    Double_t minX = hist->GetXaxis()->GetBinLowEdge(hist->GetXaxis()->GetFirst());
    Double_t maxX = hist->GetXaxis()->GetBinUpEdge(hist->GetXaxis()->GetLast());
    TF1 *f = new TF1("fBallRoot", "crystalballn", minX, maxX);

    f->SetParameters(inverted ? -1 : 1, 3, hist->GetBinCenter(hist->GetMaximumBin()), hist->GetRMS()/10);
    // hist->Fit(f);
    return f;
}

TF1* getCrystalBallFunction(TH1 *hist, Bool_t reversed) {
    TF1 *fBall = new TF1();
    CrystalBallFunctionObject *fFunctionObject = new CrystalBallFunctionObject(reversed);
    Double_t minX = hist->GetXaxis()->GetBinLowEdge(1);
    Double_t maxX = hist->GetXaxis()->GetBinUpEdge(hist->GetNbinsX());
//  Double_t intervalX = maxX-minX;
    fBall = new TF1("fBall", *fFunctionObject, minX, maxX, 5);    // create TF1 class.

    fBall->SetNpx(500);

    fBall->SetParName(0, "a");
    fBall->SetParameter(0, 1);
    fBall->SetParLimits(0, 1E-2, 1E2);

    fBall->SetParName(1, "n");
    fBall->SetParameter(1, 1);
    fBall->SetParLimits(1, 1, 20);

    fBall->SetParName(2, "mean");
    Double_t mean = hist->GetBinCenter(hist->GetMaximumBin());
    fBall->SetParameter(2, mean);
    fBall->SetParLimits(2, mean - 2 * hist->GetRMS(), mean + 2* hist->GetRMS());

    fBall->SetParName(3, "sigma");
    fBall->SetParameter(3, hist->GetRMS()/2);
    fBall->SetParLimits(3, hist->GetRMS()/4, hist->GetRMS());

    fBall->SetParName(4, "norm");
    Double_t integral = hist->Integral("width");
    fBall->SetParameter(4, integral);
    fBall->SetParLimits(4, integral*0.9, integral*1.1);

    // hist->Fit(fBall);
    // Abnormal termination of minimization.
    // Solution: increase the fitting range. Specify function range (-10000, 10000) and pass "R" parameter
    //           also use Long Double vs Double in the fitting function evaluate()

    return fBall;
//  m = edepHist->GetFunction("fBall")->GetParameter(2); // mean
//  Dm = edepHist->GetFunction("fBall")->GetParError(2); // mean error
//  s = edepHist->GetFunction("fBall")->GetParameter(3); // sigma
//  Ds = edepHist->GetFunction("fBall")->GetParError(3); // sigma error

}
