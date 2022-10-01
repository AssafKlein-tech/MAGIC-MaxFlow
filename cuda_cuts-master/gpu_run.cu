/********************************************************************************************
* Implementing Graph Cuts on CUDA using algorithm given in CVGPU '08                       **
* paper "CUDA Cuts: Fast Graph Cuts on GPUs"                                               **
*                                                                                          **
* Copyright (c) 2008 International Institute of Information Technology.                    **
* All rights reserved.                                                                     **
*                                                                                          **
* Permission to use, copy, modify and distribute this software and its documentation for   **
* educational purpose is hereby granted without fee, provided that the above copyright     **
* notice and this permission notice appear in all copies of this software and that you do  **
* not sell the software.                                                                   **
*                                                                                          **
* THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,EXPRESS, IMPLIED OR    **
* OTHERWISE.                                                                               **
*                                                                                          **
* Created By Vibhav Vineet.                                                                **
********************************************************************************************/

#include "CudaCuts.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

using namespace std;

const string DSTPATH = "./algs.csv";
const string SRCFILES = "./data/";

const int REPEATS = 20;

CudaCuts cuts[REPEATS];

void loadFile(string filepath, int idx);
int writePGM(char *filename);
void writeTime(string path, float time, int size, char*type);

int main(int argc, char ** argv)
{
	if (argc < 1)
	{
		std::cout << " missing arguments" << std::endl;
	}
	int initCheck [REPEATS];
	int dataCheck, smoothCheck, hcueCheck, vcueCheck, graphCheck, resultCheck;
	string path = SRCFILES + argv[1] + ".txt";
	for(int i = 0; i < REPEATS ; i++)
	{
		loadFile(path, i);


		initCheck[i] = cuts[i].cudaCutsInit(cuts[i].width, cuts[i].height, cuts[i].num_Labels);

		printf("Compute Capability %d\n", initCheck);

		if (initCheck[i] > 0)
		{
			printf("The grid is initialized successfully\n");
		}
		else
		if (initCheck[i] == -1)
		{
			printf("Error: Please check the device present on the system\n");
		}



		dataCheck = cuts[i].cudaCutsSetupDataTerm();

		if (dataCheck == 0)
		{
			printf("The dataterm is set properly\n");

		}
		else
		if (dataCheck == -1)
		{
			printf("Error: Please check the device present on the system\n");
		}


		smoothCheck = cuts[i].cudaCutsSetupSmoothTerm();


		if (smoothCheck == 0)
		{
			printf("The smoothnessterm is set properly\n");
		}
		else
		if (smoothCheck == -1)
		{
			printf("Error: Please check the device present on the system\n");
		}


		hcueCheck = cuts[i].cudaCutsSetupHCue();

		if (hcueCheck == 0)
		{
			printf("The HCue is set properly\n");
		}
		else
		if (hcueCheck == -1)
		{
			printf("Error: Please check the device present on the system\n");
		}

		vcueCheck = cuts[i].cudaCutsSetupVCue();


		if (vcueCheck == 0)
		{
			printf("The VCue is set properly\n");
		}
		else
		if (vcueCheck == -1)
		{
			printf("Error: Please check the device present on the system\n");
		}


		graphCheck = cuts[i].cudaCutsSetupGraph();

		if (graphCheck == 0)
		{
			printf("The graph is constructed successfully\n");
		}
		else
		if (graphCheck == -1)
		{
			printf("Error: Please check the device present on the system\n");
		}
	}
    float time[REPEATS];
	int optimizeCheck = -1;
	for (int i = 0; i < REPEATS; i++)
	{
		if (initCheck[i] == 1)
		{
			//CudaCuts involving atomic operations are called
			//optimizeCheck = cuts.cudaCutsNonAtomicOptimize();
			//CudaCuts involving stochastic operations are called
			optimizeCheck = cuts[i].cudaCutsStochasticOptimize(&time[i]);
		}

		if (optimizeCheck == 0)
		{
			printf("The algorithm successfully converged\n");
		}
		else
		if (optimizeCheck == -1)
		{
			printf("Error: Please check the device present on the system\n");
		}
	}

    for ( int i = 0; i < REPEATS; i++)
	{
		resultCheck = cuts[i].cudaCutsGetResult();
		if (resultCheck == 0)
		{
			printf("The pixel labels are successfully stored\n");
		}
		else
		if (resultCheck == -1)
		{
			printf("Error: Please check the device present on the system\n");
		}

		//int max_flow = writePGM("result_sponge/flower_cuda_test.pgm");
		//printf("yay");
		writeTime(DSTPATH, time[i], cuts[i].graph_size, argv[1]);

		cuts[i].cudaCutsFreeMem();
	}
	return 0;
}

void writeTime(string path, float time, int size, char * type)
{
    std::ofstream outfile;
    outfile.open(path, std::ios_base::app);
    outfile << size << "," << time << "," <<type << "\n"; 
    outfile.close();
}

int writePGM(char* filename, int idx)
{
	int** out_pixel_values = (int**)malloc(sizeof(int*)*cuts[idx].height);

	for (int i = 0; i < cuts[idx].height; i++)
	{
		out_pixel_values[i] = (int*)malloc(sizeof(int)* cuts[idx].width);
		for (int j = 0; j < cuts[idx].width; j++) {
			out_pixel_values[i][j] = 0;
		}
	}
	for (int i = 0; i < cuts[idx].graph_size1; i++)
	{

		int row = i / cuts[idx].width1, col = i % cuts[idx].width1;

		if (row >= 0 && col >= 0 && row <= cuts[idx].height - 1 && col <= cuts[idx].width - 1)
			out_pixel_values[row][col] = cuts[idx].pixelLabel[i] * 255;
	}
	FILE* fp = fopen(filename, "w");

	fprintf(fp, "%c", 'P');
	fprintf(fp, "%c", '2');
	fprintf(fp, "%c", '\n');
	fprintf(fp, "%d %c %d %c ", cuts[idx].width, ' ', cuts[idx].height, '\n');
	fprintf(fp, "%d %c", 255, '\n');

	for (int i = 0; i<cuts[idx].height; i++)
	{
		for (int j = 0; j<cuts[idx].width; j++)
		{
			fprintf(fp, "%d\n", out_pixel_values[i][j]);
		}
	}
	fclose(fp);
	for (int i = 0; i < cuts[idx].height; i++)
		free(out_pixel_values[i]);
	free(out_pixel_values);
    return 0;
}


void loadFile(string filepath, int idx)
{
	int &width = cuts[idx].width;
	int &height = cuts[idx].height;
	int &nLabels = cuts[idx].num_Labels;
	
	int *&dataCostArray = cuts[idx].dataTerm;
	int *&smoothCostArray = cuts[idx].smoothTerm;
	int *&hCue = cuts[idx].hCue;
	int *&vCue = cuts[idx].vCue;

	int s_len = filepath.length();
 
    // declaring character array
    char char_filepath[s_len + 1];
 
    // copying the contents of the
    // string to char array
    strcpy(char_filepath, filepath.c_str());
	FILE *fp = fopen(char_filepath, "r");

	fscanf(fp, "%d %d %d", &width, &height, &nLabels);

	int i, n, x, y;
	int gt;
	for (i = 0; i < width * height; i++)
		fscanf(fp, "%d", &gt);

	dataCostArray = (int*)malloc(sizeof(int)* width * height * nLabels);

	for (int c = 0; c < nLabels; c++) {
		n = c;
		for (i = 0; i < width * height; i++) {
			fscanf(fp, "%d", &dataCostArray[n]);
			n += nLabels;
		}
	}

	hCue = (int*)malloc(sizeof(int)* width * height);
	vCue = (int*)malloc(sizeof(int)* width * height);

	n = 0;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width - 1; x++) {
			fscanf(fp, "%d", &hCue[n++]);
		}
		hCue[n++] = 0;
	}

	n = 0;
	for (y = 0; y < height - 1; y++) {
		for (x = 0; x < width; x++) {
			fscanf(fp, "%d", &vCue[n++]);
		}
	}
	for (x = 0; x < width; x++) {
		vCue[n++] = 0;
	}

	fclose(fp);
	printf("done loading");
	smoothCostArray = (int*)malloc(sizeof(int)*nLabels * nLabels);
	smoothCostArray[0] = 0;
	smoothCostArray[1] = 1;
	smoothCostArray[2] = 1;
	smoothCostArray[3] = 0;
}