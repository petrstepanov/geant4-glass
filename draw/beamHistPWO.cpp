#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TApplication.h>

#include <FitUtils.h>
#include <CanvasHelper.h>

int beamHistPWO() {
  TH1D *hist = new TH1D("hist-gauss", "PWO ProjectionY of binx=94 [x=92.5..93.5]", 1200, 0, 20000);
  hist->SetBinContent(0, 1);
  hist->SetBinContent(1, 72);
  hist->SetBinContent(48, 1);
  hist->SetBinContent(56, 1);
  hist->SetBinContent(138, 1);
  hist->SetBinContent(227, 1);
  hist->SetBinContent(272, 1);
  hist->SetBinContent(299, 1);
  hist->SetBinContent(324, 1);
  hist->SetBinContent(378, 1);
  hist->SetBinContent(383, 1);
  hist->SetBinContent(385, 1);
  hist->SetBinContent(401, 1);
  hist->SetBinContent(402, 1);
  hist->SetBinContent(406, 1);
  hist->SetBinContent(417, 1);
  hist->SetBinContent(419, 2);
  hist->SetBinContent(422, 1);
  hist->SetBinContent(424, 1);
  hist->SetBinContent(433, 1);
  hist->SetBinContent(436, 1);
  hist->SetBinContent(437, 1);
  hist->SetBinContent(441, 1);
  hist->SetBinContent(445, 1);
  hist->SetBinContent(446, 1);
  hist->SetBinContent(455, 2);
  hist->SetBinContent(456, 1);
  hist->SetBinContent(457, 1);
  hist->SetBinContent(465, 1);
  hist->SetBinContent(469, 2);
  hist->SetBinContent(471, 1);
  hist->SetBinContent(474, 2);
  hist->SetBinContent(478, 1);
  hist->SetBinContent(487, 1);
  hist->SetBinContent(489, 1);
  hist->SetBinContent(497, 1);
  hist->SetBinContent(498, 1);
  hist->SetBinContent(499, 2);
  hist->SetBinContent(501, 1);
  hist->SetBinContent(503, 1);
  hist->SetBinContent(504, 1);
  hist->SetBinContent(508, 1);
  hist->SetBinContent(516, 2);
  hist->SetBinContent(517, 1);
  hist->SetBinContent(519, 1);
  hist->SetBinContent(520, 1);
  hist->SetBinContent(522, 1);
  hist->SetBinContent(523, 1);
  hist->SetBinContent(524, 2);
  hist->SetBinContent(526, 1);
  hist->SetBinContent(527, 3);
  hist->SetBinContent(528, 1);
  hist->SetBinContent(531, 1);
  hist->SetBinContent(532, 2);
  hist->SetBinContent(533, 1);
  hist->SetBinContent(537, 4);
  hist->SetBinContent(541, 1);
  hist->SetBinContent(542, 1);
  hist->SetBinContent(543, 1);
  hist->SetBinContent(545, 1);
  hist->SetBinContent(546, 2);
  hist->SetBinContent(547, 1);
  hist->SetBinContent(548, 1);
  hist->SetBinContent(550, 1);
  hist->SetBinContent(551, 1);
  hist->SetBinContent(552, 2);
  hist->SetBinContent(555, 1);
  hist->SetBinContent(556, 2);
  hist->SetBinContent(558, 2);
  hist->SetBinContent(559, 2);
  hist->SetBinContent(560, 1);
  hist->SetBinContent(561, 3);
  hist->SetBinContent(563, 1);
  hist->SetBinContent(565, 3);
  hist->SetBinContent(566, 1);
  hist->SetBinContent(567, 2);
  hist->SetBinContent(568, 2);
  hist->SetBinContent(570, 2);
  hist->SetBinContent(571, 1);
  hist->SetBinContent(572, 1);
  hist->SetBinContent(573, 1);
  hist->SetBinContent(574, 2);
  hist->SetBinContent(575, 4);
  hist->SetBinContent(576, 4);
  hist->SetBinContent(577, 4);
  hist->SetBinContent(578, 3);
  hist->SetBinContent(579, 1);
  hist->SetBinContent(580, 2);
  hist->SetBinContent(581, 5);
  hist->SetBinContent(582, 1);
  hist->SetBinContent(583, 6);
  hist->SetBinContent(584, 3);
  hist->SetBinContent(586, 3);
  hist->SetBinContent(587, 1);
  hist->SetBinContent(588, 4);
  hist->SetBinContent(589, 3);
  hist->SetBinContent(590, 3);
  hist->SetBinContent(593, 3);
  hist->SetBinContent(594, 9);
  hist->SetBinContent(595, 3);
  hist->SetBinContent(596, 4);
  hist->SetBinContent(597, 8);
  hist->SetBinContent(598, 4);
  hist->SetBinContent(599, 3);
  hist->SetBinContent(600, 2);
  hist->SetBinContent(601, 4);
  hist->SetBinContent(602, 4);
  hist->SetBinContent(603, 6);
  hist->SetBinContent(604, 7);
  hist->SetBinContent(605, 5);
  hist->SetBinContent(606, 7);
  hist->SetBinContent(607, 6);
  hist->SetBinContent(608, 11);
  hist->SetBinContent(609, 7);
  hist->SetBinContent(610, 9);
  hist->SetBinContent(611, 7);
  hist->SetBinContent(612, 4);
  hist->SetBinContent(613, 7);
  hist->SetBinContent(614, 11);
  hist->SetBinContent(615, 9);
  hist->SetBinContent(616, 17);
  hist->SetBinContent(617, 10);
  hist->SetBinContent(618, 14);
  hist->SetBinContent(619, 13);
  hist->SetBinContent(620, 15);
  hist->SetBinContent(621, 16);
  hist->SetBinContent(622, 21);
  hist->SetBinContent(623, 18);
  hist->SetBinContent(624, 15);
  hist->SetBinContent(625, 19);
  hist->SetBinContent(626, 21);
  hist->SetBinContent(627, 22);
  hist->SetBinContent(628, 18);
  hist->SetBinContent(629, 27);
  hist->SetBinContent(630, 34);
  hist->SetBinContent(631, 42);
  hist->SetBinContent(632, 34);
  hist->SetBinContent(633, 30);
  hist->SetBinContent(634, 34);
  hist->SetBinContent(635, 31);
  hist->SetBinContent(636, 31);
  hist->SetBinContent(637, 36);
  hist->SetBinContent(638, 42);
  hist->SetBinContent(639, 53);
  hist->SetBinContent(640, 58);
  hist->SetBinContent(641, 58);
  hist->SetBinContent(642, 69);
  hist->SetBinContent(643, 60);
  hist->SetBinContent(644, 50);
  hist->SetBinContent(645, 54);
  hist->SetBinContent(646, 76);
  hist->SetBinContent(647, 81);
  hist->SetBinContent(648, 69);
  hist->SetBinContent(649, 84);
  hist->SetBinContent(650, 101);
  hist->SetBinContent(651, 90);
  hist->SetBinContent(652, 82);
  hist->SetBinContent(653, 98);
  hist->SetBinContent(654, 87);
  hist->SetBinContent(655, 111);
  hist->SetBinContent(656, 101);
  hist->SetBinContent(657, 106);
  hist->SetBinContent(658, 111);
  hist->SetBinContent(659, 114);
  hist->SetBinContent(660, 108);
  hist->SetBinContent(661, 125);
  hist->SetBinContent(662, 132);
  hist->SetBinContent(663, 143);
  hist->SetBinContent(664, 140);
  hist->SetBinContent(665, 121);
  hist->SetBinContent(666, 144);
  hist->SetBinContent(667, 142);
  hist->SetBinContent(668, 125);
  hist->SetBinContent(669, 119);
  hist->SetBinContent(670, 145);
  hist->SetBinContent(671, 133);
  hist->SetBinContent(672, 147);
  hist->SetBinContent(673, 146);
  hist->SetBinContent(674, 155);
  hist->SetBinContent(675, 130);
  hist->SetBinContent(676, 144);
  hist->SetBinContent(677, 132);
  hist->SetBinContent(678, 128);
  hist->SetBinContent(679, 143);
  hist->SetBinContent(680, 132);
  hist->SetBinContent(681, 116);
  hist->SetBinContent(682, 117);
  hist->SetBinContent(683, 124);
  hist->SetBinContent(684, 112);
  hist->SetBinContent(685, 104);
  hist->SetBinContent(686, 103);
  hist->SetBinContent(687, 101);
  hist->SetBinContent(688, 83);
  hist->SetBinContent(689, 96);
  hist->SetBinContent(690, 89);
  hist->SetBinContent(691, 79);
  hist->SetBinContent(692, 88);
  hist->SetBinContent(693, 79);
  hist->SetBinContent(694, 65);
  hist->SetBinContent(695, 64);
  hist->SetBinContent(696, 63);
  hist->SetBinContent(697, 54);
  hist->SetBinContent(698, 50);
  hist->SetBinContent(699, 62);
  hist->SetBinContent(700, 41);
  hist->SetBinContent(701, 33);
  hist->SetBinContent(702, 38);
  hist->SetBinContent(703, 38);
  hist->SetBinContent(704, 22);
  hist->SetBinContent(705, 31);
  hist->SetBinContent(706, 23);
  hist->SetBinContent(707, 10);
  hist->SetBinContent(708, 16);
  hist->SetBinContent(709, 22);
  hist->SetBinContent(710, 11);
  hist->SetBinContent(711, 13);
  hist->SetBinContent(712, 8);
  hist->SetBinContent(713, 3);
  hist->SetBinContent(714, 8);
  hist->SetBinContent(715, 5);
  hist->SetBinContent(716, 7);
  hist->SetBinContent(717, 4);
  hist->SetBinContent(718, 4);
  hist->SetBinContent(719, 3);
  hist->SetBinContent(720, 6);
  hist->SetBinContent(721, 5);
  hist->SetBinContent(723, 2);
  hist->SetBinContent(730, 1);
  hist->SetBinContent(736, 1);
  hist->SetBinContent(1201, 1);
  hist->SetEntries(7478);

  hist->SetAxisRange(7000., 13000., "X");

  hist->SetFillColor(kGray);
  hist->SetLineColor(kGray+2);
  TH1D *hist2 = (TH1D*) hist->Clone("hist-crystal-ball");

  // Enable most common fit statistics
  gStyle->SetOptFit(1);

  TCanvas *canvas = new TCanvas();
  canvas->SetWindowSize(1280, 500);
  canvas->Divide(2, 1, 1E-5, 1E-5);

  // LEFT PAD - Gaussian fit
  TVirtualPad* pad1 = canvas->cd(1);
  pad1->SetGrid();
  hist->Draw();

  TF1 *gauss = new TF1("gauss", "gaus", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
  gauss->SetNpx(200);
  gauss->SetLineColor(kPink);
  hist->Fit(gauss, "M");

  // Add values to statistics box
  CanvasHelper::addTextToStats("", pad1);

  TString str;
  // TVector2 resFit = FitUtils::evalResolution(gauss->GetParameter(1), gauss->GetParError(1), gauss->GetParameter(2), gauss->GetParError(2));
  // str = TString::Format("Resolution, %% = %f #pm %f", resFit.X(), resFit.Y());
  // CanvasHelper::addTextToStats(str.Data(), pad1);

  Double_t gaussMean = gauss->Mean(gauss->GetXmin(), gauss->GetXmax());
  str = TString::Format("1st momentum, %% = %f #pm %f", gaussMean, gauss->GetParError(1));
  CanvasHelper::addTextToStats(str.Data(), pad1);

  Double_t gaussVariance = gauss->Variance(gauss->GetXmin(), gauss->GetXmax());
  Double_t gaussDispersion = TMath::Sqrt(gaussVariance);
  str = TString::Format("2nd momentum, %% = %f #pm %f", gaussDispersion, gauss->GetParError(2));
  CanvasHelper::addTextToStats(str.Data(), pad1);

  TVector2 resTF1 = FitUtils::evalResolution(gaussMean, gauss->GetParError(1), gaussDispersion, gauss->GetParError(2));
  str = TString::Format("Resolution, %% = %f #pm %f", resTF1.X(), resTF1.Y());
  CanvasHelper::addTextToStats(str.Data(), pad1);

  // Align statistics box
  TPave *pave = CanvasHelper::getDefaultPaveStats(pad1);
  CanvasHelper::setPaveAlignment(pave, kPaveAlignLeft | kPaveAlignTop);

  // RIGHT PAD - Crystal Ball fit
  TVirtualPad* pad2 = canvas->cd(2);
  pad2->SetGrid();
  hist2->Draw();

  TF1 *cball = FitUtils::getCrystalBallFunction(hist2, kFALSE);
  cball->SetNpx(200);
  cball->SetLineColor(kBlue);
  hist2->Fit(cball, "M");

  // Add values to statistics box
  TVector2 mean = FitUtils::getCrystalBallMean(cball);
  TVector2 dispersion = FitUtils::getCrystalBallDispersion(cball);
  TVector2 resolution = FitUtils::getCrystalBallResolution(cball);

  CanvasHelper::addTextToStats("", pad2);
  str = TString::Format("1st momentum, MeV = %f #pm %f", mean.X(), mean.Y());
  CanvasHelper::addTextToStats(str.Data(), pad2);
  str = TString::Format("2nd momentum, MeV = %f #pm %f", dispersion.X(), dispersion.Y());
  CanvasHelper::addTextToStats(str.Data(), pad2);
  str = TString::Format("Resolution, %% = %f #pm %f", resolution.X(), resolution.Y());
  CanvasHelper::addTextToStats(str.Data(), pad2);

  // gauss->Draw("SAME");

  // Align statistics box
  TPave *pave2 = CanvasHelper::getDefaultPaveStats(pad2);
  CanvasHelper::setPaveAlignment(pave2, kPaveAlignLeft | kPaveAlignTop);
//  CanvasHelper::getInstance()->addCanvas(pad2);

  CanvasHelper::getInstance()->addCanvas(canvas);

  return 1;
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication *app = new TApplication("beamHistPWO", &argc, argv);

  beamHistPWO();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
