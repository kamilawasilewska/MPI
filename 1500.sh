#!/bin/bash
#SBATCH --job-name="wasilewska"
#SBATCH -o plik1.txt
#SBATCH -p ibm_large

mpirun -np 4  ./main 1500 1200
