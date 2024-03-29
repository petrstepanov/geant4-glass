#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TApplication.h>

#include <FitUtils.h>
#include <CanvasHelper.h>

int beamHist(){
   TH1D *hist = new TH1D("hist-gauss","ProjectionY of binx=86 [x=84.5..85.5]",1024,0,4096);
   hist->SetBinContent(1,2);
   hist->SetBinContent(2,2);
   hist->SetBinContent(3,1);
   hist->SetBinContent(6,1);
   hist->SetBinContent(7,1);
   hist->SetBinContent(9,1);
   hist->SetBinContent(10,1);
   hist->SetBinContent(11,1);
   hist->SetBinContent(14,1);
   hist->SetBinContent(25,1);
   hist->SetBinContent(26,1);
   hist->SetBinContent(44,1);
   hist->SetBinContent(47,1);
   hist->SetBinContent(87,1);
   hist->SetBinContent(100,1);
   hist->SetBinContent(146,1);
   hist->SetBinContent(147,1);
   hist->SetBinContent(151,1);
   hist->SetBinContent(153,1);
   hist->SetBinContent(159,1);
   hist->SetBinContent(161,2);
   hist->SetBinContent(164,1);
   hist->SetBinContent(174,1);
   hist->SetBinContent(192,1);
   hist->SetBinContent(195,1);
   hist->SetBinContent(205,1);
   hist->SetBinContent(234,1);
   hist->SetBinContent(241,1);
   hist->SetBinContent(243,1);
   hist->SetBinContent(250,1);
   hist->SetBinContent(253,1);
   hist->SetBinContent(255,1);
   hist->SetBinContent(279,1);
   hist->SetBinContent(288,1);
   hist->SetBinContent(296,1);
   hist->SetBinContent(301,1);
   hist->SetBinContent(304,1);
   hist->SetBinContent(305,1);
   hist->SetBinContent(311,1);
   hist->SetBinContent(314,1);
   hist->SetBinContent(316,1);
   hist->SetBinContent(323,1);
   hist->SetBinContent(328,2);
   hist->SetBinContent(333,1);
   hist->SetBinContent(340,1);
   hist->SetBinContent(344,1);
   hist->SetBinContent(348,1);
   hist->SetBinContent(353,1);
   hist->SetBinContent(354,1);
   hist->SetBinContent(364,2);
   hist->SetBinContent(375,1);
   hist->SetBinContent(378,2);
   hist->SetBinContent(384,2);
   hist->SetBinContent(391,1);
   hist->SetBinContent(392,1);
   hist->SetBinContent(396,3);
   hist->SetBinContent(398,2);
   hist->SetBinContent(400,1);
   hist->SetBinContent(402,2);
   hist->SetBinContent(404,3);
   hist->SetBinContent(410,3);
   hist->SetBinContent(412,1);
   hist->SetBinContent(416,1);
   hist->SetBinContent(420,2);
   hist->SetBinContent(421,2);
   hist->SetBinContent(423,1);
   hist->SetBinContent(426,1);
   hist->SetBinContent(427,1);
   hist->SetBinContent(428,1);
   hist->SetBinContent(430,1);
   hist->SetBinContent(431,2);
   hist->SetBinContent(432,1);
   hist->SetBinContent(433,1);
   hist->SetBinContent(434,1);
   hist->SetBinContent(437,1);
   hist->SetBinContent(438,2);
   hist->SetBinContent(439,2);
   hist->SetBinContent(440,1);
   hist->SetBinContent(441,2);
   hist->SetBinContent(442,2);
   hist->SetBinContent(443,1);
   hist->SetBinContent(444,2);
   hist->SetBinContent(445,1);
   hist->SetBinContent(446,2);
   hist->SetBinContent(447,2);
   hist->SetBinContent(448,2);
   hist->SetBinContent(449,2);
   hist->SetBinContent(450,2);
   hist->SetBinContent(451,2);
   hist->SetBinContent(452,1);
   hist->SetBinContent(453,2);
   hist->SetBinContent(454,1);
   hist->SetBinContent(455,1);
   hist->SetBinContent(456,2);
   hist->SetBinContent(458,2);
   hist->SetBinContent(459,4);
   hist->SetBinContent(461,3);
   hist->SetBinContent(463,2);
   hist->SetBinContent(464,3);
   hist->SetBinContent(465,1);
   hist->SetBinContent(466,2);
   hist->SetBinContent(467,1);
   hist->SetBinContent(468,1);
   hist->SetBinContent(469,1);
   hist->SetBinContent(470,2);
   hist->SetBinContent(472,4);
   hist->SetBinContent(473,2);
   hist->SetBinContent(474,4);
   hist->SetBinContent(476,3);
   hist->SetBinContent(477,1);
   hist->SetBinContent(478,2);
   hist->SetBinContent(479,1);
   hist->SetBinContent(480,4);
   hist->SetBinContent(481,1);
   hist->SetBinContent(482,5);
   hist->SetBinContent(483,2);
   hist->SetBinContent(484,2);
   hist->SetBinContent(485,3);
   hist->SetBinContent(486,2);
   hist->SetBinContent(487,3);
   hist->SetBinContent(488,4);
   hist->SetBinContent(489,4);
   hist->SetBinContent(490,3);
   hist->SetBinContent(491,5);
   hist->SetBinContent(492,3);
   hist->SetBinContent(493,7);
   hist->SetBinContent(494,1);
   hist->SetBinContent(495,2);
   hist->SetBinContent(496,5);
   hist->SetBinContent(497,3);
   hist->SetBinContent(498,2);
   hist->SetBinContent(499,7);
   hist->SetBinContent(500,8);
   hist->SetBinContent(501,5);
   hist->SetBinContent(502,4);
   hist->SetBinContent(503,5);
   hist->SetBinContent(504,3);
   hist->SetBinContent(505,5);
   hist->SetBinContent(506,1);
   hist->SetBinContent(507,4);
   hist->SetBinContent(508,5);
   hist->SetBinContent(509,4);
   hist->SetBinContent(510,7);
   hist->SetBinContent(511,5);
   hist->SetBinContent(512,7);
   hist->SetBinContent(513,8);
   hist->SetBinContent(514,8);
   hist->SetBinContent(515,8);
   hist->SetBinContent(516,5);
   hist->SetBinContent(517,8);
   hist->SetBinContent(518,9);
   hist->SetBinContent(519,7);
   hist->SetBinContent(520,8);
   hist->SetBinContent(521,4);
   hist->SetBinContent(522,8);
   hist->SetBinContent(523,8);
   hist->SetBinContent(524,12);
   hist->SetBinContent(525,5);
   hist->SetBinContent(526,7);
   hist->SetBinContent(527,5);
   hist->SetBinContent(528,10);
   hist->SetBinContent(529,11);
   hist->SetBinContent(530,10);
   hist->SetBinContent(531,7);
   hist->SetBinContent(532,9);
   hist->SetBinContent(533,12);
   hist->SetBinContent(534,7);
   hist->SetBinContent(535,6);
   hist->SetBinContent(536,8);
   hist->SetBinContent(537,10);
   hist->SetBinContent(538,9);
   hist->SetBinContent(539,22);
   hist->SetBinContent(540,6);
   hist->SetBinContent(541,19);
   hist->SetBinContent(542,11);
   hist->SetBinContent(543,5);
   hist->SetBinContent(544,15);
   hist->SetBinContent(545,7);
   hist->SetBinContent(546,12);
   hist->SetBinContent(547,11);
   hist->SetBinContent(548,18);
   hist->SetBinContent(549,9);
   hist->SetBinContent(550,13);
   hist->SetBinContent(551,16);
   hist->SetBinContent(552,9);
   hist->SetBinContent(553,14);
   hist->SetBinContent(554,13);
   hist->SetBinContent(555,11);
   hist->SetBinContent(556,11);
   hist->SetBinContent(557,18);
   hist->SetBinContent(558,18);
   hist->SetBinContent(559,11);
   hist->SetBinContent(560,23);
   hist->SetBinContent(561,20);
   hist->SetBinContent(562,12);
   hist->SetBinContent(563,18);
   hist->SetBinContent(564,16);
   hist->SetBinContent(565,11);
   hist->SetBinContent(566,20);
   hist->SetBinContent(567,18);
   hist->SetBinContent(568,20);
   hist->SetBinContent(569,20);
   hist->SetBinContent(570,22);
   hist->SetBinContent(571,18);
   hist->SetBinContent(572,15);
   hist->SetBinContent(573,29);
   hist->SetBinContent(574,25);
   hist->SetBinContent(575,21);
   hist->SetBinContent(576,20);
   hist->SetBinContent(577,19);
   hist->SetBinContent(578,15);
   hist->SetBinContent(579,32);
   hist->SetBinContent(580,18);
   hist->SetBinContent(581,15);
   hist->SetBinContent(582,23);
   hist->SetBinContent(583,19);
   hist->SetBinContent(584,22);
   hist->SetBinContent(585,12);
   hist->SetBinContent(586,21);
   hist->SetBinContent(587,27);
   hist->SetBinContent(588,28);
   hist->SetBinContent(589,23);
   hist->SetBinContent(590,24);
   hist->SetBinContent(591,20);
   hist->SetBinContent(592,28);
   hist->SetBinContent(593,19);
   hist->SetBinContent(594,24);
   hist->SetBinContent(595,13);
   hist->SetBinContent(596,16);
   hist->SetBinContent(597,28);
   hist->SetBinContent(598,24);
   hist->SetBinContent(599,24);
   hist->SetBinContent(600,18);
   hist->SetBinContent(601,23);
   hist->SetBinContent(602,18);
   hist->SetBinContent(603,32);
   hist->SetBinContent(604,20);
   hist->SetBinContent(605,19);
   hist->SetBinContent(606,25);
   hist->SetBinContent(607,22);
   hist->SetBinContent(608,32);
   hist->SetBinContent(609,28);
   hist->SetBinContent(610,20);
   hist->SetBinContent(611,25);
   hist->SetBinContent(612,28);
   hist->SetBinContent(613,22);
   hist->SetBinContent(614,26);
   hist->SetBinContent(615,26);
   hist->SetBinContent(616,32);
   hist->SetBinContent(617,21);
   hist->SetBinContent(618,35);
   hist->SetBinContent(619,34);
   hist->SetBinContent(620,28);
   hist->SetBinContent(621,27);
   hist->SetBinContent(622,34);
   hist->SetBinContent(623,27);
   hist->SetBinContent(624,26);
   hist->SetBinContent(625,30);
   hist->SetBinContent(626,29);
   hist->SetBinContent(627,32);
   hist->SetBinContent(628,35);
   hist->SetBinContent(629,33);
   hist->SetBinContent(630,27);
   hist->SetBinContent(631,27);
   hist->SetBinContent(632,25);
   hist->SetBinContent(633,24);
   hist->SetBinContent(634,34);
   hist->SetBinContent(635,29);
   hist->SetBinContent(636,28);
   hist->SetBinContent(637,27);
   hist->SetBinContent(638,30);
   hist->SetBinContent(639,19);
   hist->SetBinContent(640,32);
   hist->SetBinContent(641,29);
   hist->SetBinContent(642,34);
   hist->SetBinContent(643,17);
   hist->SetBinContent(644,19);
   hist->SetBinContent(645,32);
   hist->SetBinContent(646,31);
   hist->SetBinContent(647,30);
   hist->SetBinContent(648,32);
   hist->SetBinContent(649,39);
   hist->SetBinContent(650,27);
   hist->SetBinContent(651,32);
   hist->SetBinContent(652,28);
   hist->SetBinContent(653,35);
   hist->SetBinContent(654,22);
   hist->SetBinContent(655,22);
   hist->SetBinContent(656,34);
   hist->SetBinContent(657,24);
   hist->SetBinContent(658,31);
   hist->SetBinContent(659,30);
   hist->SetBinContent(660,24);
   hist->SetBinContent(661,25);
   hist->SetBinContent(662,29);
   hist->SetBinContent(663,32);
   hist->SetBinContent(664,29);
   hist->SetBinContent(665,29);
   hist->SetBinContent(666,30);
   hist->SetBinContent(667,28);
   hist->SetBinContent(668,26);
   hist->SetBinContent(669,34);
   hist->SetBinContent(670,34);
   hist->SetBinContent(671,28);
   hist->SetBinContent(672,27);
   hist->SetBinContent(673,28);
   hist->SetBinContent(674,34);
   hist->SetBinContent(675,20);
   hist->SetBinContent(676,27);
   hist->SetBinContent(677,31);
   hist->SetBinContent(678,30);
   hist->SetBinContent(679,26);
   hist->SetBinContent(680,29);
   hist->SetBinContent(681,34);
   hist->SetBinContent(682,18);
   hist->SetBinContent(683,30);
   hist->SetBinContent(684,18);
   hist->SetBinContent(685,32);
   hist->SetBinContent(686,25);
   hist->SetBinContent(687,30);
   hist->SetBinContent(688,33);
   hist->SetBinContent(689,23);
   hist->SetBinContent(690,18);
   hist->SetBinContent(691,27);
   hist->SetBinContent(692,18);
   hist->SetBinContent(693,22);
   hist->SetBinContent(694,29);
   hist->SetBinContent(695,30);
   hist->SetBinContent(696,25);
   hist->SetBinContent(697,30);
   hist->SetBinContent(698,20);
   hist->SetBinContent(699,25);
   hist->SetBinContent(700,29);
   hist->SetBinContent(701,37);
   hist->SetBinContent(702,26);
   hist->SetBinContent(703,26);
   hist->SetBinContent(704,25);
   hist->SetBinContent(705,21);
   hist->SetBinContent(706,28);
   hist->SetBinContent(707,19);
   hist->SetBinContent(708,25);
   hist->SetBinContent(709,24);
   hist->SetBinContent(710,25);
   hist->SetBinContent(711,25);
   hist->SetBinContent(712,26);
   hist->SetBinContent(713,25);
   hist->SetBinContent(714,30);
   hist->SetBinContent(715,27);
   hist->SetBinContent(716,26);
   hist->SetBinContent(717,30);
   hist->SetBinContent(718,19);
   hist->SetBinContent(719,19);
   hist->SetBinContent(720,12);
   hist->SetBinContent(721,21);
   hist->SetBinContent(722,26);
   hist->SetBinContent(723,28);
   hist->SetBinContent(724,24);
   hist->SetBinContent(725,17);
   hist->SetBinContent(726,25);
   hist->SetBinContent(727,18);
   hist->SetBinContent(728,14);
   hist->SetBinContent(729,22);
   hist->SetBinContent(730,32);
   hist->SetBinContent(731,19);
   hist->SetBinContent(732,13);
   hist->SetBinContent(733,16);
   hist->SetBinContent(734,19);
   hist->SetBinContent(735,12);
   hist->SetBinContent(736,21);
   hist->SetBinContent(737,25);
   hist->SetBinContent(738,15);
   hist->SetBinContent(739,21);
   hist->SetBinContent(740,24);
   hist->SetBinContent(741,23);
   hist->SetBinContent(742,21);
   hist->SetBinContent(743,20);
   hist->SetBinContent(744,25);
   hist->SetBinContent(745,20);
   hist->SetBinContent(746,14);
   hist->SetBinContent(747,21);
   hist->SetBinContent(748,13);
   hist->SetBinContent(749,14);
   hist->SetBinContent(750,21);
   hist->SetBinContent(751,18);
   hist->SetBinContent(752,18);
   hist->SetBinContent(753,25);
   hist->SetBinContent(754,22);
   hist->SetBinContent(755,16);
   hist->SetBinContent(756,15);
   hist->SetBinContent(757,16);
   hist->SetBinContent(758,20);
   hist->SetBinContent(759,17);
   hist->SetBinContent(760,18);
   hist->SetBinContent(761,13);
   hist->SetBinContent(762,13);
   hist->SetBinContent(763,15);
   hist->SetBinContent(764,17);
   hist->SetBinContent(765,13);
   hist->SetBinContent(766,13);
   hist->SetBinContent(767,21);
   hist->SetBinContent(768,13);
   hist->SetBinContent(769,12);
   hist->SetBinContent(770,16);
   hist->SetBinContent(771,10);
   hist->SetBinContent(772,12);
   hist->SetBinContent(773,11);
   hist->SetBinContent(774,12);
   hist->SetBinContent(775,11);
   hist->SetBinContent(776,11);
   hist->SetBinContent(777,17);
   hist->SetBinContent(778,18);
   hist->SetBinContent(779,12);
   hist->SetBinContent(780,12);
   hist->SetBinContent(781,14);
   hist->SetBinContent(782,16);
   hist->SetBinContent(783,14);
   hist->SetBinContent(784,11);
   hist->SetBinContent(785,13);
   hist->SetBinContent(786,8);
   hist->SetBinContent(787,15);
   hist->SetBinContent(788,12);
   hist->SetBinContent(789,12);
   hist->SetBinContent(790,9);
   hist->SetBinContent(791,8);
   hist->SetBinContent(792,2);
   hist->SetBinContent(793,13);
   hist->SetBinContent(794,10);
   hist->SetBinContent(795,9);
   hist->SetBinContent(796,11);
   hist->SetBinContent(797,11);
   hist->SetBinContent(798,10);
   hist->SetBinContent(799,19);
   hist->SetBinContent(800,8);
   hist->SetBinContent(801,9);
   hist->SetBinContent(802,14);
   hist->SetBinContent(803,11);
   hist->SetBinContent(804,11);
   hist->SetBinContent(805,8);
   hist->SetBinContent(806,14);
   hist->SetBinContent(807,16);
   hist->SetBinContent(808,15);
   hist->SetBinContent(809,12);
   hist->SetBinContent(810,8);
   hist->SetBinContent(811,9);
   hist->SetBinContent(812,11);
   hist->SetBinContent(813,15);
   hist->SetBinContent(814,4);
   hist->SetBinContent(815,4);
   hist->SetBinContent(816,7);
   hist->SetBinContent(817,11);
   hist->SetBinContent(818,8);
   hist->SetBinContent(819,11);
   hist->SetBinContent(820,6);
   hist->SetBinContent(821,12);
   hist->SetBinContent(822,9);
   hist->SetBinContent(823,11);
   hist->SetBinContent(824,13);
   hist->SetBinContent(825,6);
   hist->SetBinContent(826,6);
   hist->SetBinContent(827,9);
   hist->SetBinContent(828,4);
   hist->SetBinContent(829,6);
   hist->SetBinContent(830,7);
   hist->SetBinContent(831,8);
   hist->SetBinContent(832,11);
   hist->SetBinContent(833,11);
   hist->SetBinContent(834,9);
   hist->SetBinContent(835,7);
   hist->SetBinContent(836,4);
   hist->SetBinContent(837,9);
   hist->SetBinContent(838,13);
   hist->SetBinContent(839,5);
   hist->SetBinContent(840,6);
   hist->SetBinContent(841,3);
   hist->SetBinContent(842,5);
   hist->SetBinContent(843,5);
   hist->SetBinContent(844,8);
   hist->SetBinContent(845,12);
   hist->SetBinContent(846,4);
   hist->SetBinContent(847,5);
   hist->SetBinContent(848,5);
   hist->SetBinContent(849,2);
   hist->SetBinContent(850,2);
   hist->SetBinContent(851,5);
   hist->SetBinContent(852,6);
   hist->SetBinContent(853,7);
   hist->SetBinContent(854,1);
   hist->SetBinContent(855,8);
   hist->SetBinContent(856,8);
   hist->SetBinContent(857,6);
   hist->SetBinContent(858,5);
   hist->SetBinContent(859,4);
   hist->SetBinContent(860,2);
   hist->SetBinContent(861,8);
   hist->SetBinContent(862,5);
   hist->SetBinContent(863,3);
   hist->SetBinContent(864,3);
   hist->SetBinContent(865,7);
   hist->SetBinContent(866,9);
   hist->SetBinContent(867,3);
   hist->SetBinContent(868,3);
   hist->SetBinContent(869,5);
   hist->SetBinContent(870,5);
   hist->SetBinContent(871,2);
   hist->SetBinContent(872,4);
   hist->SetBinContent(873,7);
   hist->SetBinContent(874,5);
   hist->SetBinContent(875,2);
   hist->SetBinContent(876,4);
   hist->SetBinContent(877,7);
   hist->SetBinContent(878,2);
   hist->SetBinContent(879,4);
   hist->SetBinContent(880,7);
   hist->SetBinContent(881,7);
   hist->SetBinContent(882,2);
   hist->SetBinContent(883,6);
   hist->SetBinContent(884,5);
   hist->SetBinContent(885,1);
   hist->SetBinContent(886,4);
   hist->SetBinContent(887,2);
   hist->SetBinContent(888,6);
   hist->SetBinContent(889,8);
   hist->SetBinContent(890,4);
   hist->SetBinContent(891,4);
   hist->SetBinContent(892,6);
   hist->SetBinContent(893,2);
   hist->SetBinContent(894,2);
   hist->SetBinContent(895,8);
   hist->SetBinContent(896,8);
   hist->SetBinContent(897,2);
   hist->SetBinContent(898,4);
   hist->SetBinContent(899,1);
   hist->SetBinContent(900,4);
   hist->SetBinContent(901,2);
   hist->SetBinContent(903,3);
   hist->SetBinContent(904,3);
   hist->SetBinContent(905,5);
   hist->SetBinContent(906,1);
   hist->SetBinContent(907,3);
   hist->SetBinContent(908,3);
   hist->SetBinContent(909,2);
   hist->SetBinContent(910,1);
   hist->SetBinContent(911,4);
   hist->SetBinContent(912,4);
   hist->SetBinContent(913,2);
   hist->SetBinContent(914,1);
   hist->SetBinContent(915,3);
   hist->SetBinContent(916,3);
   hist->SetBinContent(917,1);
   hist->SetBinContent(918,3);
   hist->SetBinContent(919,3);
   hist->SetBinContent(920,3);
   hist->SetBinContent(921,1);
   hist->SetBinContent(922,2);
   hist->SetBinContent(923,3);
   hist->SetBinContent(924,1);
   hist->SetBinContent(926,2);
   hist->SetBinContent(927,2);
   hist->SetBinContent(928,3);
   hist->SetBinContent(929,1);
   hist->SetBinContent(931,2);
   hist->SetBinContent(932,4);
   hist->SetBinContent(933,2);
   hist->SetBinContent(934,2);
   hist->SetBinContent(935,1);
   hist->SetBinContent(936,4);
   hist->SetBinContent(938,2);
   hist->SetBinContent(939,2);
   hist->SetBinContent(940,1);
   hist->SetBinContent(941,1);
   hist->SetBinContent(942,1);
   hist->SetBinContent(943,3);
   hist->SetBinContent(944,3);
   hist->SetBinContent(945,1);
   hist->SetBinContent(947,1);
   hist->SetBinContent(948,1);
   hist->SetBinContent(949,3);
   hist->SetBinContent(950,1);
   hist->SetBinContent(951,2);
   hist->SetBinContent(952,2);
   hist->SetBinContent(953,1);
   hist->SetBinContent(954,1);
   hist->SetBinContent(955,1);
   hist->SetBinContent(957,3);
   hist->SetBinContent(959,2);
   hist->SetBinContent(960,1);
   hist->SetBinContent(961,2);
   hist->SetBinContent(962,1);
   hist->SetBinContent(967,2);
   hist->SetBinContent(968,1);
   hist->SetBinContent(969,1);
   hist->SetBinContent(970,1);
   hist->SetBinContent(972,1);
   hist->SetBinContent(973,1);
   hist->SetBinContent(974,2);
   hist->SetBinContent(975,1);
   hist->SetBinContent(976,1);
   hist->SetBinContent(977,1);
   hist->SetBinContent(980,1);
   hist->SetBinContent(981,2);
   hist->SetBinContent(982,1);
   hist->SetBinContent(983,1);
   hist->SetBinContent(984,1);
   hist->SetBinContent(985,1);
   hist->SetBinContent(986,1);
   hist->SetBinContent(987,3);
   hist->SetBinContent(994,2);
   hist->SetBinContent(996,1);
   hist->SetBinContent(998,1);
   hist->SetBinContent(999,1);
   hist->SetBinContent(1000,1);
   hist->SetBinContent(1002,1);
   hist->SetEntries(7081);

   // hist->SetAxisRange(7000., 13000., "X");

   hist->SetFillColor(kGray);
   hist->SetLineColor(kGray+2);
   TH1D *hist2 = (TH1D*) hist->Clone("hist-crystal-ball-reversed");

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

   TF1 *cball = FitUtils::getCrystalBallFunction(hist2, kTRUE);
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
  TApplication* app = new TApplication("beamHist", &argc, argv);

  beamHist();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
