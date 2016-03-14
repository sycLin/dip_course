/**
 * This source file is for the problem 1 in DIP hw1.
 * In this problem, we have 5 sub-problems:
 *
 * (1) Plot the histogram of 2 images, I and D, i.e., "sample1.raw" and "sample2.raw" respectively.
 * (2) Perform histogram equalization on D and save the result as H.
 * (3) Perform local histogram equalization on D and save as L.
 * (4) Plot the histogram of H and L.
 * (5) Perform log transform, inverse log transform, and power-law transform to enhance D.
 * 	   and show the corresponding histogram.
 *
 */

#include <iostream>
#include "ImageUtil.h"
#include "ImageUtil.cpp"

using namespace std;

int main(int argc, char* argv[]) {

	// check the command-line arguments
	if(argc != 3) {
		printf("[fatal] need to provide file name in argument!\n");
		exit(1);
	}

	// (1) plot the histograms of I and D

	// (1-1) plot for I

	unsigned char** image_data = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		image_data[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!openImageAsMatrix(argv[1], image_data, 256, 256)) {
		cerr << "cannot open file at: " << argv[1] << endl;
		return -1;
	}

	unsigned char** H = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		H[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!drawHistogram(image_data, H, 256, 256)) {
		cerr << "cannot draw histogram..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_I_histo.raw", H, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}

	// (1-2) plot for D

	if(!openImageAsMatrix(argv[2], image_data, 256, 256)) {
		cerr << "cannot open file at: " << argv[2] << endl;
		return -1;
	}

	if(!drawHistogram(image_data, H, 256, 256)) {
		cerr << "cannot draw histogram..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_D_histo.raw", H, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}


	// (2) perform histogram equalization on D, save as H

	unsigned char** HE = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		HE[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!hist_eq(image_data, HE, 256, 256)) {
		cerr << "cannot do histogram equalization..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_D_he.raw", HE, 256, 256)) {
		cerr << "cannot write image to file" << endl;
		return -1;
	}

	// (3) perform local histogram equalization on D, save as L

	unsigned char** LHE = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		LHE[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	// change the last 2 parameters to determine the contextual region
	if(!local_hist_eq(image_data, LHE, 256, 256, 10, 10)) {
		cerr << "cannot do histogram equalization..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_D_lhe.raw", LHE, 256, 256)) {
		cerr << "cannot write image to file" << endl;
		return -1;
	}

	// (4) plot histograms for H and L

	// (4-1) plot for H
	if(!drawHistogram(HE, H, 256, 256)) {
		cerr << "cannot draw histogram..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_HE_histo.raw", H, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}

	// (4-2) plot for L
	if(!drawHistogram(LHE, H, 256, 256)) {
		cerr << "cannot draw histogram..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_LHE_histo.raw", H, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}

	// (5) perform 3 different kinds of transformations

	// (5-1) perform log transform / inverse log transform / power-law transform

	// log transform

	unsigned char** LT = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		LT[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!log_transform(image_data, LT, 256, 256)) {
		cerr << "cannot do log transform..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_D_lt.raw", LT, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}

	// inverse log transform

	unsigned char** ILT = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		ILT[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!inverse_log_transform(image_data, ILT, 256, 256)) {
		cerr << "cannot do log transform..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_D_ilt.raw", ILT, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}

	// power-law transform

	unsigned char** PLT = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		PLT[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!power_law_transform(image_data, PLT, 256, 256, 1.0/2.5)) {
		cerr << "cannot do log transform..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_D_plt.raw", PLT, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}

	// (5-2) show the corresponding histograms

	// show histogram of log transform

	if(!drawHistogram(LT, H, 256, 256)) {
		cerr << "cannot draw histogram..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_LT_histo.raw", H, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}

	// show histogram of inverse log transform

	if(!drawHistogram(ILT, H, 256, 256)) {
		cerr << "cannot draw histogram..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_ILT_histo.raw", H, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}

	// show histogram of power-law transform

	if(!drawHistogram(PLT, H, 256, 256)) {
		cerr << "cannot draw histogram..." << endl;
		return -1;
	}

	if(!writeImageToFile("p1_PLT_histo.raw", H, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}


	return 0;
}