#!/bin/bash
#SBATCH --job-name="wasilewska"
#SBATCH -o plik4.txt
#SBATCH -p ibm_large

mpirun -np 4  ./main 45000 36000
