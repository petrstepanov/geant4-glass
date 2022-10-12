#!/bin/bash

#SBATCH --partition=priority
#SBATCH --account=eic
#SBATCH --mem-per-cpu=512
#SBATCH --time=12:00:00
#SBATCH --output=eres-optical-40_%A_%a.out
#SBATCH --error=eres-optical-40_%A_%a.err
#SBATCH --array=0-12

# Define key simulation parameters
# Number of crystals in assembly
NX=1
NY=1

# Crystal size, mm
SIZEX=20.29
SIZEY=20.31
SIZEZ=402.1

# Crystal material - currently supported values are:
# PWO-CRYTUR, PWO-SICCAS, PWO-BOGO, PWO-SICCAS-YANG
# CSGlass-0-T, SciGlass-4-1-L, SciGlass-4-1-T, SciGlass-2-2-T, SciGlass-2-1-L, SciGlass-2-1-T, SciGlass-1-1-T 
MATERIAL=SciGlass-4-1-L

# Wrap material. "C10H8O4" is vm2000. "G4_TEFLON" is teflon. "NONE" is no wrap material
WRAPMATER=C10H8O4

# Incident particle
PARTICLE=e-

# Define energy array with float energy values  (slurm does not support float values)
# We will use slurm array as index for our array with float incident energies
ENERGIES=(3      3.25   3.5    3.75   4      4.25   4.5    4.75   5      5.25   5.5    5.75   6)

# Index   0      1      2      3      4      5      6      7      8      9      10     11     12 
EVENTS=(  5000   5000   5000   5000   5000   5000   5000   5000   5000   5000   5000   5000   5000)
# Estima  10min  10min  10min  10min  10min  10min  10min  10min  10min  10min  10min  10min  10min

# Template and output Geant4 macro names
# Make sure unit volumes are turned on in the template below: /detector/useUnitVolumeScoringMeshes true
TEMPLATE_MACROS=./macros/slurm-eres-optical-template.mac
MACROS=./macros/eres-optical-${MATERIAL}-${NX}x${NY}-${SIZEX}x${SIZEY}x${SIZEZ}mm-${ENERGIES[$SLURM_ARRAY_TASK_ID]}GeV-${EVENTS[$SLURM_ARRAY_TASK_ID]}events.mac

# Source the environment
source /site/12gev_phys/softenv.sh 2.6

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
sed -i "s;_EVENTS;${EVENTS[$SLURM_ARRAY_TASK_ID]};g" ${MACROS}

# Substitute energy value with energy array element with Slurm array index
sed -i "s;_ENERGY;${ENERGIES[$SLURM_ARRAY_TASK_ID]};g" ${MACROS} 

# Pass macros to executable
./glass ${MACROS} 
