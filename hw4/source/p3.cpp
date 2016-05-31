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

	// ========== read Sample3 ========== //
	// create image
	unsigned char** sample = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		sample[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	// read the image in
	if(!openImageAsMatrix(argv[1], sample, 256, 256)) {
		cerr << "cannot open image at: " << argv[1] << endl;
		exit(-1);
	}

	// ========== Discrete Fourier Transform ========== //
	// compute the transform
	double** dft_map_real;
	double** dft_map_im;
	computeDFT(sample, 256, 256, dft_map_real, dft_map_im);
	// output the image
	// create an image
	unsigned char** dft_image = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		dft_image[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	// filling
	for(int r=0; r<256; r++) {
		for(int c=0; c<256; c++) {
			int value = (int)log(dft_map_real[r][c] * dft_map_real[r][c] + dft_map_im[r][c] * dft_map_im[r][c]);
			// centering
			if(r * 2 < 256 && c * 2 < 256 ) {
				// section A : upper-left => lower-right
				dft_image[r+128][c+128] = value;
			} else if(c * 2 < 256) {
				// section B : lower-left => upper-right
				dft_image[r-128][c+128] = value;
			} else if(r * 2 < 256) {
				// section D : upper-right => lower-left
				dft_image[r+128][c-128] = value;
			} else {
				// section C : lower-right => upper-left
				dft_image[r-128][c-128] = value;
			}
		}
	}
	if(!writeImageToFile("D.raw", dft_image, 256, 256)) {
		cerr << "cannot write image to file. (D.raw)" << endl;
		exit(-1);
	}
	if(!writeImageToFile("D_spatial.raw", inverseDFT(dft_map_real, dft_map_im, 256, 256), 256, 256)) {
		cerr << "cannot write image to file. (D_spatial.raw)" << endl;
		exit(-1);
	}

	// ========== Ideal Low-Pass Filters ========== //
	// create the image
	unsigned char** ILPF = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		ILPF[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	// ILPF with D0 = 5
	double** L5_real_part = (double**)malloc(256 * sizeof(double*));
	double** L5_im_part = (double**)malloc(256 * sizeof(double*));
	for(int i=0; i<256; i++) {
		L5_real_part[i] = (double*)malloc(256 * sizeof(double));
		L5_im_part[i] = (double*)malloc(256 * sizeof(double));
	}
	for(int u=0; u<256; u++) {
		for(int v=0; v<256; v++) {
			double distance = sqrt((u - 128) * (u - 128) + (v - 128) * (v - 128));
			int factor = (distance > 5.0) ? 0 : 1;
			ILPF[u][v] = dft_image[u][v] * factor;
			L5_real_part[u][v] = dft_map_real[u][v] * (double)factor;
			L5_im_part[u][v] = dft_map_im[u][v] * (double)factor;
		}
	}
	// write the result
	if(!writeImageToFile("L_5.raw", ILPF, 256, 256)) {
		cerr << "cannot write image to file. (L_5.raw)" << endl;
		exit(-1);
	}
	// ILPF with D0 = 30
	double** L30_real_part = (double**)malloc(256 * sizeof(double*));
	double** L30_im_part = (double**)malloc(256 * sizeof(double*));
	for(int i=0; i<256; i++) {
		L30_real_part[i] = (double*)malloc(256 * sizeof(double));
		L30_im_part[i] = (double*)malloc(256 * sizeof(double));
	}
	for(int u=0; u<256; u++) {
		for(int v=0; v<256; v++) {
			double distance = sqrt((u - 128) * (u - 128) + (v - 128) * (v - 128));
			int factor = (distance > 30.0) ? 0 : 1;
			ILPF[u][v] = dft_image[u][v] * factor;
			L30_real_part[u][v] = dft_map_real[u][v] * (double)factor;
			L30_im_part[u][v] = dft_map_im[u][v] * (double)factor;
		}
	}
	// write the result
	if(!writeImageToFile("L_30.raw", ILPF, 256, 256)) {
		cerr << "cannot write image to file. (L_30.raw)" << endl;
		exit(-1);
	}


	// ========== Gaussian Low-Pass Filters ========== //
	unsigned char** GLPF = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		GLPF[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	// D0 = 5
	double** G5_real_part = (double**)malloc(256 * sizeof(double*));
	double** G5_im_part = (double**)malloc(256 * sizeof(double*));
	for(int i=0; i<256; i++) {
		G5_real_part[i] = (double*)malloc(256 * sizeof(double));
		G5_im_part[i] = (double*)malloc(256 * sizeof(double));
	}
	for(int u=0; u<256; u++) {
		for(int v=0; v<256; v++) {
			double distance = sqrt((u - 128) * (u - 128) + (v - 128) * (v - 128));
			double factor = exp(-distance * distance / (2.0 * 5.0 * 5.0));
			GLPF[u][v] = (int)(dft_image[u][v] * factor);
			G5_real_part[u][v] = dft_map_real[u][v] * factor;
			G5_im_part[u][v] = dft_map_im[u][v] * factor;
		}
	}
	// write the result
	if(!writeImageToFile("G_5.raw", GLPF, 256, 256)) {
		cerr << "cannot write image to file. (G_5.raw)" << endl;
		exit(-1);
	}
	// D0 = 30
	double** G30_real_part = (double**)malloc(256 * sizeof(double*));
	double** G30_im_part = (double**)malloc(256 * sizeof(double*));
	for(int i=0; i<256; i++) {
		G30_real_part[i] = (double*)malloc(256 * sizeof(double));
		G30_im_part[i] = (double*)malloc(256 * sizeof(double));
	}
	for(int u=0; u<256; u++) {
		for(int v=0; v<256; v++) {
			double distance = sqrt((u - 128) * (u - 128) + (v - 128) * (v - 128));
			double factor = exp(-distance * distance / (2.0 * 30.0 * 30.0));
			GLPF[u][v] = (int)(dft_image[u][v] * factor);
			G30_real_part[u][v] = dft_map_real[u][v] * factor;
			G30_im_part[u][v] = dft_map_im[u][v] * factor;
		}
	}
	// write the result
	if(!writeImageToFile("G_30.raw", GLPF, 256, 256)) {
		cerr << "cannot write image to file. (G_30.raw)" << endl;
		exit(-1);
	}


	// ========== Transfer back to Spatial Domain ========== //
	unsigned char** tmp;
	// L_5
	tmp = inverseDFT(L5_real_part, L5_im_part, 256, 256);
	if(!writeImageToFile("L_5_spatial.raw", tmp, 256, 256)) {
		cerr << "cannot write image to file. (L_5_spatial.raw)" << endl;
	}
	// L_30
	tmp = inverseDFT(L30_real_part, L30_im_part, 256, 256);
	if(!writeImageToFile("L_30_spatial.raw", tmp, 256, 256)) {
		cerr << "cannot write image to file. (L_30_spatial.raw)" << endl;
	}
	// G_5
	tmp = inverseDFT(G5_real_part, G5_im_part, 256, 256);
	if(!writeImageToFile("G_5_spatial.raw", tmp, 256, 256)) {
		cerr << "cannot write image to file. (G_5_spatial.raw)" << endl;
	}
	// G_30
	tmp = inverseDFT(G30_real_part, G30_im_part, 256, 256);
	if(!writeImageToFile("G_30_spatial.raw", tmp, 256, 256)) {
		cerr << "cannot write image to file. (G_30_spatial.raw)" << endl;
	}


	return 0;
}

void print_usage() {
	cerr << "Usage: ./a.out <Sample3>" << endl;
}