#!/bin/bash

#SBATCH --partition=priority
#SBATCH --account=eic
#SBATCH --mem-per-cpu=512
#SBATCH --time=12:00:00
#SBATCH --output=eres_%A_%a.out
#SBATCH --error=eres_%A_%a.err
#SBATCH --array=0-4

# Crystal size, mm
SIZEX=20
SIZEY=20
SIZEZ=200

# Crystal material - currently supported values are:
# PWO-CRYTUR, PWO-SICCAS, PWO-BOGO, PWO-SICCAS-YANG
# CSGlass-0-T, SciGlass-4-1-L, SciGlass-4-1-T, SciGlass-2-2-T, SciGlass-2-1-L, SciGlass-2-1-T, SciGlass-1-1-T 
MATERIAL=PWO-CRYTUR

# Wrap material. "C10H8O4" is vm2000. "G4_TEFLON" is teflon. "NONE" is no wrap material
WRAPMATER=G4_TEFLON

# TODO: check how much time this takes
# SCINTILLATIONYIELD = 300/MeV



NEVENTS=100

# Define energy array with float energy values  (slurm does not support float values)
# We will use slurm array as index for our array with float incident energies
GPS_POS_Z=(10 55 100 145 190)

# Template and output Geant4 macro names
TEMPLATE_MACROS=./macros/slurm-npe-vs-gps-pos-co60.mac
MACROS=${TEMPLATE_MACROS}-${MATERIAL}-${NX}x${NY}-${SIZEX}x${SIZEY}x${SIZEZ}mm-gpsposx-${GPS_POS_Z[$SLURM_ARRAY_TASK_ID]}mm-${NEVENTS}.mac

# Source the environment
source /site/12gev_phys/softenv.sh 2.5

# Navigate into the build directory
# Program should be compiled with same environment first: ./installGlass.sh
cd /u/home/petrs/Development/glass-prototype-build/

# Create new Geant4 macros from template
cp ${TEMPLATE_MACROS} ${MACROS}

# Substitute template values with actual parameters
sed -i "s;_SIZEX;${SIZEX};g" ${MACROS} 
sed -i "s;_SIZEY;${SIZEY};g" ${MACROS} 
sed -i "s;_SIZEZ;${SIZEZ};g" ${MACROS}
sed -i "s;_MATERIAL;${MATERIAL};g" ${MACROS}
sed -i "s;_WRAPMATER;${WRAPMATER};g" ${MACROS}
sed -i "s;_NEVENTS;${NEVENTS};g" ${MACROS}
sed -i "s;_GPS_POS_Z;${GPS_POS_Z[$SLURM_ARRAY_TASK_ID]};g" ${MACROS}

# Pass macros to executable
./glass ${MACROS} 
