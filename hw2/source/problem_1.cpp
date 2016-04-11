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

	// prepare two 2d arrays, one for original image, the other for edge image
	unsigned char** im = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	unsigned char** im_e = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++) {
		im[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
		im_e[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	}


	// ========== sample1.raw ========== //

	// read in sample1.raw
	if(!openImageAsMatrix(argv[1], im, 256, 256)) {
		cerr << "cannot open image at: " << argv[1] << endl;
		exit(-1);
	}

	// do Sobel edge detection on sample1
	if(!doSobelEdgeDetector(im, im_e, 256, 256, 200.0)) {
		cerr << "cannot perform sobel edge detection on sample1..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample1_sobel.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample1_sobel.raw)..." << endl;
		exit(-1);
	}

	// do 2nd order edge detection (DOG) on sample1
	if(!doDifferenceOfGaussian(im, im_e, 256, 256, 400)) {
		cerr << "cannot perform 2nd order edge detection on sample1..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample1_2nd.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample1_2nd.raw)..." << endl;
		exit(-1);
	}

	// do Canny edge detection on sample1
	if(!doCannyEdgeDetector(im, im_e, 256, 256, 250.0, 25.0)) {
		cerr << "cannot perform canny edge detection on sample1..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample1_canny.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample1_canny.raw)..." << endl;
		exit(-1);
	}

	// ========== sample2.raw ========== //

	// read in sample2.raw
	if(!openImageAsMatrix(argv[2], im, 256, 256)) {
		cerr << "cannot open image at: " << argv[2] << endl;
		exit(-1);
	}

	// do Sobel edge detection on sample2
	if(!doSobelEdgeDetector(im, im_e, 256, 256, 200.0)) {
		cerr << "cannot perform sobel edge detection on sample2..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample2_sobel.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample2_sobel.raw)..." << endl;
		exit(-1);
	}

	// do 2nd order edge detection (DOG) on sample2
	if(!doDifferenceOfGaussian(im, im_e, 256, 256, 400)) {
		cerr << "cannot perform 2nd order edge detection on sample2..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample2_2nd.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample2_2nd.raw)..." << endl;
		exit(-1);
	}

	// do Canny edge detection on sample2
	if(!doCannyEdgeDetector(im, im_e, 256, 256, 250.0, 25.0)) {
		cerr << "cannot perform canny edge detection on sample2..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample2_canny.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample2_canny.raw)..." << endl;
		exit(-1);
	}

	// ========== sample3.raw ========== //

	// read in sample3.raw
	if(!openImageAsMatrix(argv[3], im, 256, 256)) {
		cerr << "cannot open image at: " << argv[3] << endl;
		exit(-1);
	}

	// do Sobel edge detection on sample3
	if(!doSobelEdgeDetector(im, im_e, 256, 256, 200.0)) {
		cerr << "cannot perform sobel edge detection on sample3..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample3_sobel.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample3_sobel.raw)..." << endl;
		exit(-1);
	}

	// do 2nd order edge detection (DOG) on sample3
	if(!doDifferenceOfGaussian(im, im_e, 256, 256, 400)) {
		cerr << "cannot perform 2nd order edge detection on sample3..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample3_2nd.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample3_2nd.raw)..." << endl;
		exit(-1);
	}

	// do Canny edge detection on sample3
	if(!doCannyEdgeDetector(im, im_e, 256, 256, 250.0, 25.0)) {
		cerr << "cannot perform canny edge detection on sample3..." << endl;
		exit(-1);
	} else if(!writeImageToFile("sample3_canny.raw", im_e, 256, 256)) {
		cerr << "cannot write image to file (sample3_canny.raw)..." << endl;
		exit(-1);
	}

	return 0;
}

void print_usage() {
	cerr << "Usage:" << endl;
	cerr << "\t./a.out <sample1> <sample2> <sample3>" << endl;
}


