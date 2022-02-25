# Modify default canvas size
set terminal qt 0 font "Sans,9" size 400,450

# Turn on enhanced text mode (for LaTeX labels)
set termopt enhanced

set xlabel "Electron energy, GeV"
set ylabel "Resolution, %"
set title "Energy Resolution (gaussian fit)"
set grid

plot [:][:35] './output-farm-3x3-20x20x200mm-BaSi2O5/resolution-BaSi2O5-3x3-20x20x200-gauss.txt' using 1:2:3 with errorbars pointtype 2 linetype 1 title "BaSi_2O_5 3x3 2x2x20 cm", \
              './output-farm-3x3-20x20x200mm-BaGdSiO/resolution-BaGdSiO-3x3-20x20x200-gauss.txt' using 1:2:3 with errorbars pointtype 2 linetype 2 title "BaGdSiO 3x3 2x2x20 cm", \
              './output-farm-1x1-20x20x400mm-BaGdSiO/resolution-BaGdSiO-1x1-20x20x400-gauss.txt' using 1:2:3 with errorbars pointtype 2 linetype 3 title "BaGdSiO 1x1 2x2x40 cm", \
              './output-farm-1x1-40x40x400mm-BaGdSiO/resolution-BaGdSiO-1x1-40x40x400-gauss.txt' using 1:2:3 with errorbars pointtype 2 linetype 4 title "BaGdSiO 1x1 4x4x40 cm", \
              './output-farm-3x3-20x20x400mm-BaGdSiO/resolution-BaGdSiO-3x3-20x20x400-gauss.txt' using 1:2:3 with errorbars pointtype 2 linetype 8 title "BaGdSiO 3x3 2x2x40 cm", \
              './output-farm-5x5-20x20x400mm-BaGdSiO/resolution-BaGdSiO-5x5-20x20x400-gauss.txt' using 1:2:3 with errorbars pointtype 2 linetype 6 title "BaGdSiO 5x5 2x2x40 cm", \
              './output-farm-3x3-20x20x200mm-PbWO4/resolution-PbWO4-3x3-20x20x200-gauss.txt'     using 1:2:3 with errorbars pointtype 2 linetype 7 title "PbWO_4 3x3 2x2x20 cm"

