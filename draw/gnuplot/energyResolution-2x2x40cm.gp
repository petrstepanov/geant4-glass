# Modify default canvas size
set terminal qt 0 font "Sans,9" size 400,450

# Turn on enhanced text mode (for LaTeX labels)
set termopt enhanced

set xlabel "Electron energy, GeV"
set ylabel "Resolution, %"
set title "Energy Resolution of 2x2x400cm (crystal ball fit)"
set grid

plot [:][:35] './output-farm-1x1-20x20x400mm-BaGdSiO/resolution-BaGdSiO-1x1-20x20x400-cball.txt' using 1:2:3 with errorbars pointtype 2 linetype 3 title "BaGdSiO 1x1 2x2x40 cm", \
              './output-farm-3x3-20x20x400mm-BaGdSiO/resolution-BaGdSiO-3x3-20x20x400-cball.txt' using 1:2:3 with errorbars pointtype 2 linetype 8 title "BaGdSiO 3x3 2x2x40 cm", \
              './output-farm-5x5-20x20x400mm-BaGdSiO/resolution-BaGdSiO-5x5-20x20x400-cball.txt' using 1:2:3 with errorbars pointtype 2 linetype 6 title "BaGdSiO 5x5 2x2x40 cm"
