#!/bin/bash

#SBATCH --partition=priority
#SBATCH --account=eic
#SBATCH --mem-per-cpu=512
#SBATCH --time=12:00:00
#SBATCH --output=eres_%A_%a.out
#SBATCH --error=eres_%A_%a.err
#SBATCH --array=0-22

# Define key simulation parameters
# Number of crystals in assembly
NX=3
NY=3

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

# Incident particle
PARTICLE=e-

# Define energy array with float energy values  (slurm does not support float values)
# We will use slurm array as index for our array with float incident energies
ENERGIES=(0.5    1.0    1.5    2.0    2.5    3.0    3.5    4.0    4.5    5.0    5.5    6.0    6.5    7      8      9      10     15     20     25     50     75     100)

# Index   0      1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22
# Events  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000  50000
# Time    5m     10m    12m    29m    22m    30m    55m    1h     1h     1h     1.5h   1.5h   1.5h   1.75h  2h     2.3h   2.5h   3.5h   3h     3.5h   7.5h   11h    15h

EVENTS=(  300000 250000 250000 100000 100000 75000  50000  50000  50000  50000  45000  35000  35000  25000  25000  20000  20000  15000  15000  15000  10000  4500   3500)
# Estima  1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h     1h


# Template and output Geant4 macro names
TEMPLATE_MACROS=./macros/slurm-eres-template.mac
MACROS=./macros/farm-eres-${MATERIAL}-${NX}x${NY}-${SIZEX}x${SIZEY}x${SIZEZ}mm-${ENERGIES[$SLURM_ARRAY_TASK_ID]}GeV-${EVENTS[$SLURM_ARRAY_TASK_ID]}events.mac

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
sed -i "s;_EVENTS;${EVENTS[$SLURM_ARRAY_TASK_ID]};g" ${MACROS}

# Substitute energy value with energy array element with Slurm array index
sed -i "s;_ENERGY;${ENERGIES[$SLURM_ARRAY_TASK_ID]};g" ${MACROS} 

# Pass macros to executable
./glass ${MACROS} 
