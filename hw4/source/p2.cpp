#include <iostream>
#include "ImageUtil.h"
#include "ImageUtil.cpp"

using namespace std;


void print_usage();

int main(int argc, char* argv[]) {

	// ========== check argument ========== //

	if(argc != 2) {
		cerr << "wrong arguments!" << endl;
		print_usage();
		exit(-1);
	}

	// ========== read Sample2 ========== //
	// create image
	unsigned char** sample = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		sample[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	// read the image in
	if(!openImageAsMatrix(argv[1], sample, 256, 256)) {
		cerr << "cannot open image at: %s" << argv[1] << endl;
		exit(-1);
	}

	// ========== extract the boundary ========== //
	unsigned char** boundary = extractBoundary(sample, 256, 256, 255);
	// write the result
	if(!writeImageToFile("boundary.raw", boundary, 256, 256)) {
		cerr << "cannot write image to file. (boundary.raw)" << endl;
		exit(-1);
	}

	// ========== smoothing the boundary ========== //
	// actually: opening = erosion-then-dilation
	unsigned char** smooth = dilation(erosion(sample, 256, 256, 255, 11), 256, 256, 255, 11);
	// write the result
	if(!writeImageToFile("smooth.raw", smooth, 256, 256)) {
		cerr << "cannot write image to file. (smooth.raw)" << endl;
		exit(-1);
	}

	// ========== skeletonizeing ========== //
	// Sk(X) = [(X ⊖ kB)] − [(X ⊖ kB) ◦ B]
	unsigned char** eroded = erosion(sample, 256, 256, 255, 3);
	unsigned char** skeleton = imageDifference(eroded, dilation(erosion(eroded, 256, 256, 255, 3), 256, 256, 255, 3), 256, 256, 255);
	for(int i=0; i<20; i++) {
		eroded = erosion(eroded, 256, 256, 255, 3);
		skeleton = imageUnion(skeleton, imageDifference(eroded, dilation(erosion(eroded, 256, 256, 255, 3), 256, 256, 255, 3), 256, 256, 255), 256, 256, 255);
	}
	// write the result
	if(!writeImageToFile("skeleton.raw", skeleton, 256, 256)) {
		cerr << "cannot write image to file. (skeleton.raw)" << endl;
		exit(-1);
	}

	return 0;
}

void print_usage() {
	cerr << "Usage: ./a.out <Sample2>" << endl;
}