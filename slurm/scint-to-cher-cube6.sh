#!/bin/bash

#SBATCH --partition=priority
#SBATCH --account=eic
#SBATCH --mem-per-cpu=512
#SBATCH --time=24:00:00
#SBATCH --output=s-to-c_%A_%a.out
#SBATCH --error=s-to-c_%A_%a.err
#SBATCH --array=0-4

# Define key simulation parameters
# Number of crystals in assembly
NX=1
NY=1

# Crystal size, mm
SIZEX=45.5
SIZEY=44.7
SIZEZ=46.15

# Pmt type
DETECTOR=R2059

# Reflective shield between crysrtal and PMT/MPPC
REFSHIELD=false

# Crystal material - currently supported values are:
# PWO-CRYTUR, PWO-SICCAS, PWO-BOGO, PWO-SICCAS-YANG
# CSGlass-0-T, SciGlass-4-1-L, SciGlass-4-1-T, SciGlass-2-2-T, SciGlass-2-1-L, SciGlass-2-1-T, SciGlass-1-1-T 
MATERIAL=Cube-6-1

# Wrap material. "C10H8O4" is vm2000. "G4_TEFLON" is teflon. "NONE" is no wrap material
WRAPMATER=NONE

# Incident particle
PARTICLE=mu-

# Define energy array with float energy values  (slurm does not support float values)
# We will use slurm array as index for our array with float incident energies [GeV]
ENERGIES=(0.1    1.0    2.0    3.0    4.0)
# Index   0      1      2      3      4

EVENTS=(  50000  50000  50000  50000  50000)
# Estima  6h     6h     6h     6h     6h

# Template and output Geant4 macro names
TEMPLATE_MACROS=./macros/slurm-scint-to-cher-no-activators.mac
MACROS=./macros/farm-scint-to-cher-${MATERIAL}-${NX}x${NY}-${SIZEX}x${SIZEY}x${SIZEZ}mm-${ENERGIES[$SLURM_ARRAY_TASK_ID]}GeV-${EVENTS[$SLURM_ARRAY_TASK_ID]}events.mac

# Source the environment
source /site/12gev_phys/softenv.sh 2.5

# Navigate into the build directory
# Program should be compiled with same environment first: ./installGlass.sh
cd /u/home/petrs/Development/glass-prototype-build/

# Create new Geant4 macros from template
cp ${TEMPLATE_MACROS} ${MACROS}

# Substitute template values with actual parameters
sed -i "s;_NX;${NX};g" ${MACROS} 
sed -i "s;_NY;${NY};g" ${MACROS} 
sed -i "s;_SIZEX;${SIZEX};g" ${MACROS} 
sed -i "s;_SIZEY;${SIZEY};g" ${MACROS} 
sed -i "s;_SIZEZ;${SIZEZ};g" ${MACROS}
sed -i "s;_MATERIAL;${MATERIAL};g" ${MACROS}
sed -i "s;_WRAPMATER;${WRAPMATER};g" ${MACROS}
sed -i "s;_PARTICLE;${PARTICLE};g" ${MACROS}
sed -i "s;_DETECTOR;${DETECTOR};g" ${MACROS}
sed -i "s;_REFSHIELD;${REFSHIELD};g" ${MACROS}
sed -i "s;_EVENTS;${EVENTS[$SLURM_ARRAY_TASK_ID]};g" ${MACROS}

# Substitute energy value with energy array element with Slurm array index
sed -i "s;_ENERGY;${ENERGIES[$SLURM_ARRAY_TASK_ID]};g" ${MACROS} 

# Pass macros to executable
./glass ${MACROS} 
