#!/bin/bash
#SBATCH --job-name="wasilewska"
#SBATCH -o plik2.txt
#SBATCH -p ibm_large

mpirun -np 4  ./main 15000 12000
