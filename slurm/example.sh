#!/bin/bash

#SBATCH --partition=priority
#SBATCH --account=eic
#SBATCH --mem-per-cpu=512
#SBATCH --time=12:00:00

POS=10
MATERIAL=PWO-CRYTUR

source /site/12gev_phys/softenv.sh 2.5

cd /u/home/petrs/Development/glass-prototype-build/
cp ./macros/farm-lumi-unit-volumes-output-${MATERIAL}.mac ./macros/farm-lumi-unit-volumes-output-${MATERIAL}-${POS}mm.mac
sed -i "s;/gps/pos/centre 0 0 [0-9]\+ mm;/gps/pos/centre 0 0 ${POS} mm;" ./macros/farm-lumi-unit-volumes-output-${MATERIAL}-${POS}mm.mac
./glass ./macros/farm-lumi-unit-volumes-output-${MATERIAL}-${POS}mm.mac
