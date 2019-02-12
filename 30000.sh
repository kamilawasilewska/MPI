#!/bin/bash
#SBATCH --job-name="wasilewska"
#SBATCH -o plik3.txt
#SBATCH -p ibm_large

mpirun -np 4  ./main 30000 24000
