/**
 * This source file is for the problem 2 in DIP hw2.
 * In this prolem, we have 4 sub-problems:
 *
 * (1) Add impulse noise to I, save as N1.
 * (2) Add uniform noise to I, save as N2.
 * (3) De-noise N1 and N2 with proper filters and parameters,
 *     and save them as R1, R2 respectively.
 * (4) Compute PSNR values of R1 and R2.
 *
 */

#include <iostream>
#include "ImageUtil.h"
#include "ImageUtil.cpp"

using namespace std;

int main(int argc, char* argv[]) {
	
	// check the argument
	if(argc != 2) {
		cerr << "[fatal] need to provide file name in argument!\n" << endl;
		exit(-1);
	}

	// read the original image first

	unsigned char** oImage = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		oImage[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!openImageAsMatrix(argv[1], oImage, 256, 256)) {
		cerr << "cannot open file at: " << argv[1] << endl;
		return -1;
	}

	// (1) add impulse noise to I, and save as N1.

	unsigned char** N1 = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		N1[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!add_impulse_noise(oImage, N1, 256, 256, 0.025)) {
		cerr << "cannot add impulse noise..." << endl;
		return -1;
	}

	if(!writeImageToFile("p2_N1.raw", N1, 256, 256)) {
		cerr << "cannot write image to file..." << endl;
		return -1;
	}


	// (2) add uniform noise to I, and save as N2.

	// (3) de-noise with proper filters and parameters, save as R1, R2.

	// (4) compute PSNR values of R1 and R2

	return 0;
}
