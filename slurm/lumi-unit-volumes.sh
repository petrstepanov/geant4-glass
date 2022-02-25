#!/bin/bash

#SBATCH --partition=priority
#SBATCH --account=eic
#SBATCH --mem-per-cpu=512
#SBATCH --time=12:00:00
#SBATCH --output=lumi_%A_%a.out
#SBATCH --error=lumi_%A_%a.err

# Make sure maximum slurm array index matches size of the energy array
#SBATCH --array=0-9

# Define key simulation parameters
# Number of crystals in assembly - always 1x1 for luminescence unit volumes studies
# NX=3
# NY=3

# Crystal size, mm
SIZEX=20
SIZEY=20
SIZEZ=200

# Crystal material - currently supported values are:
# PWO-CRYTUR, PWO-SICCAS, PWO-BOGO, PWO-SICCAS-YANG
# CSGlass-0-T, SciGlass-4-1-L, SciGlass-4-1-T, SciGlass-2-2-T, SciGlass-2-1-L, SciGlass-2-1-T, SciGlass-1-1-T 
MATERIAL=PWO-CRYTUR

# Wrap material. "C10H8O4" is vm2000. "G4_TEFLON" is teflon. "NONE" is no wrap material
WRAPMATER=C10H8O4

# Number of events in run (300000 events take 20 mins, 1M events should take about 1 hour)
EVENTS=1000000

# Define energy array with float energy values  (slurm does not support float values)
# We will use slurm array as index for our array with float incident energies
POSITIONS=(10 30 50 70 90 110 130 150 170 190)

# Template and output Geant4 macro names
TEMPLATE_MACROS=./macros/slurm-lumi-unit-volumes-template.mac
MACROS=./macros/farm-lumi-unit-volumes-${MATERIAL}-${SIZEX}x${SIZEY}x${SIZEZ}mm-gps-at-${POSITIONS[$SLURM_ARRAY_TASK_ID]}mm.mac

# Source the environment
source /site/12gev_phys/softenv.sh 2.5

# Navigate into the build directory
# Program should be compiled with same environment first: ./installGlass.sh
cd /u/home/petrs/Development/glass-prototype-build/

# Create new Geant4 macros from template
cp ${TEMPLATE_MACROS} ${MACROS}

# Substitute template values with actual parameters
# sed -i "s;_NX;${NX};g" ${MACROS} 
# sed -i "s;_NY;${NY};g" ${MACROS} 
sed -i "s;_SIZEX;${SIZEX};g" ${MACROS} 
sed -i "s;_SIZEY;${SIZEY};g" ${MACROS} 
sed -i "s;_SIZEZ;${SIZEZ};g" ${MACROS}
sed -i "s;_MATERIAL;${MATERIAL};g" ${MACROS}
sed -i "s;_WRAPMATER;${WRAPMATER};g" ${MACROS}
sed -i "s;_EVENTS;${EVENTS};g" ${MACROS}

# Substitute energy value with energy array element with Slurm array index
sed -i "s;_POSITION;${POSITIONS[$SLURM_ARRAY_TASK_ID]};g" ${MACROS} 

# Pass macros to executable
./glass ${MACROS}
