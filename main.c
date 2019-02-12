#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <omp.h>
#include <mpi.h>

unsigned char * readFile(char* inFile[], unsigned char * image);
void save(char* inFile[], unsigned char* image, long fileLength);
unsigned char normalize(double value);
double convolution(int i, int j, unsigned char *image, int height, int width, int dim, const double mask[][7]);

int main(int argc, char * argv[])
{
	int col, row;
	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	char * dir = "../labMPI/infile";
	char * bin = "result";
	char * fileExtension = ".bin";
	char outFile[64], inFile[64];
	unsigned char *image;
	unsigned char *part;
	long size;
	int worldSize, worldRank, dataPiece;
	double start, end, time;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

	size = width * height;
	dataPiece = size / worldSize;
	image = (unsigned char *)malloc(size * sizeof(unsigned char));
	part = (unsigned char *)malloc(dataPiece * sizeof(unsigned char));

	if (worldRank == 0)
	{
		strcpy(inFile, dir);
		strcat(inFile, argv[1]);
		strcat(inFile, "_");
		strcat(inFile, argv[2]);
		strcat(inFile, fileExtension);

		strcpy(outFile, bin);
		strcat(outFile, argv[1]);
		strcat(outFile, "_");
		strcat(outFile, argv[2]);
		strcat(outFile, fileExtension);

		image = readFile(inFile, image);
	}

	MPI_Bcast(image, size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	if (worldRank == 0)
		start = MPI_Wtime();

	const double mask[][7] ={{0,0,-1,-1,-1,0,0 },
								{0,-1,-3,-3,-3,-1,0 },
								{ -1,-3,0,7,0,-3,-1},
								{ -1,-3,7,24,7,-3,-1},
								{-1,-3,0,7,0,-3,-1},
								{ 0,-1,-3,-3,-3,-1,0},
								{0,0,-1,-1,-1,0,0 }};
#pragma omp parallel for private(col, row) schedule(dynamic, 100)
	for (int i = dataPiece * worldRank; i < dataPiece * (worldRank + 1); i++)
	{
		col = i % width;
		row = i / width;
		part[i - dataPiece * worldRank] = normalize(convolution(col, row, image, height, width, 5, mask));
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(part, dataPiece, MPI_UNSIGNED_CHAR, image, dataPiece, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	if (worldRank == 0)
	{
		end = MPI_Wtime();
		time = (double)end - start;
		printf("Time: %f \n", time);

		save(outFile, image, size);
		printf("Saved file: %s\n", outFile);
	}

	free(image);
	free(part);
	MPI_Finalize();
	return 0;
}

unsigned char * readFile(char* inFile[], unsigned char * image)
{
	FILE *file = fopen(inFile, "rb");
	fseek(file, 0, SEEK_END);
	long fileLength = ftell(file);
	fseek(file, 0, SEEK_SET);
	image = (unsigned char *)malloc(fileLength * sizeof(unsigned char));
	fread(image, sizeof(unsigned char), fileLength, file);
	fclose(file);
	return image;
}

void save(char* inFile[], unsigned char* image, long fileLength)
{
	FILE *write = fopen(inFile, "wb");
	fwrite(image, sizeof(unsigned char), fileLength * sizeof(unsigned char), write);
	fclose(write);
}

double convolution(int i, int j, unsigned char *image, int height, int width, int dim, const double mask[][7])
{
	int x, y, middle, ii, jj;
	x = y = dim;
	middle = x / 2;
	double tmp = 0;
	for (long m = 0; m < x; ++m) {
		for (long n = 0; n < y; ++n) {
			ii = i + (middle - m);
			jj = j + (middle - n);
			if (ii >= 0 && ii < width && jj >= 0 && jj < height)
				tmp += image[jj * width + ii] * mask[m][n];
		}
	}
	return tmp;
}

unsigned char normalize(double value)
{
	if (value > 255)
		value = 255;
	else if (value < 0)
		value = 0;
	return (unsigned char)value;
}
