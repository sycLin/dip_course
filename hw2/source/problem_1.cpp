#include <iostream>
#include <string>
#include "ImageUtil.h"
#include "ImageUtil.cpp"

void print_usage();

int main(int argc, char* argv[]) {

	// check arguments
	if(argc != 4) {
		cerr << "wrong arguments!" << endl;
		print_usage();
		exit(-1);
	}

	// read in sample1.raw
	unsigned char** sample1 = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		sample1[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));

	if(!openImageAsMatrix(argv[1], sample1, 256, 256)) {
		cerr << "cannot open image at: " << argv[1] << endl;
		exit(-1);
	}

	// do Sobel edge detection on sample1
	unsigned char** sample1_sobel = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		sample1_sobel[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	if(!doSobelEdgeDetector(sample1, sample1_sobel, 256, 256, 200.0)) {
		cerr << "cannot perform sobel edge detection on sample1..." << endl;
		exit(-1);
	}

	// do Canny edge detection on sample1
	unsigned char** sample1_canny = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		sample1_canny[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	if(!doCannyEdgeDetector(sample1, sample1_canny, 256, 256, 250.0, 25.0)) {
		cerr << "cannot perform canny edge detection on sample1..." << endl;
		exit(-1);
	}

	// write results to file
	if(!writeImageToFile("sample1_canny.raw", sample1_canny, 256, 256)) {
		cerr << "cannot write image to file (sample1_canny.raw)..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample1_sobel.raw", sample1_sobel, 256, 256)) {
		cerr << "cannot write image to file (sample1_sobel.raw)..." << endl;
		exit(-1);
	}


	return 0;
}

void print_usage() {
	cerr << "Usage:" << endl;
	cerr << "\t./a.out <sample1> <sample2> <sample3>" << endl;
}


