/**
 * This is the implementation of the functions defined in imageUtil.h header
 */
#include "ImageUtil.h"

bool openImageAsMatrix(char* path, unsigned char** ptr, int height, int width) {

	// open file
	FILE* fp;
	fp = fopen(path, "rb");
	if(!fp)
		return false;

	// read it
	unsigned char tmp_data[height][width];
	if(fread(tmp_data, sizeof(unsigned char), height * width, fp) != (height * width))
		return false;

	// close file
	fclose(fp);

	// copy data to ptr
	for(int row=0; row < height; row++) {
		for(int col=0; col < width; col++)
			ptr[row][col] = tmp_data[row][col];
	}

	return true;
}

bool writeImageToFile(char* path, unsigned char** data, int height, int width) {
	
	// open file for writing
	FILE* fp;
	fp = fopen(path, "wb");
	if(!fp)
		return false;

	// write it
	unsigned char tmp_data[height][width];
	for(int row = 0; row < height; row++)
		for(int col = 0; col < width; col++)
			tmp_data[row][col] = data[row][col];
	if(fwrite(tmp_data, sizeof(unsigned char), height * width, fp) != (height * width))
		return false;

	// close file
	fclose(fp);

	return true;
}

bool median_filter(unsigned char** data, unsigned char** res, int height, int width, int bheight, int bwidth) {
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			// use vector to store neighbors and self
			vector<int> neighbor_values;
			// loop over neighbors (nr, nc)
			for(int nr = r - bheight / 2; nr <= r + bheight / 2; nr++) {
				for(int nc = c - bwidth / 2; nc <= c + bwidth / 2; nc++) {

					// this _tmp_ stores the value of current neighbor being explored
					int tmp = 0;
					
					// check if neighbor (nr, nc) is in range
					// if not: we use "extending solution"
					/*
						type 1 |   type 2    | type 3
						-------|-------------|-------
						       |             |
						type 4 |    Image    | type 5
						       |             |
						-------|-------------|-------
						type 6 |   type 7    | type 8
					*/
					if(nr < 0) {
						// type 1, 2, 3
						if(nc < 0) {
							// type 1
							tmp = data[0][0];
						} else if(nc >= width) {
							// type 3
							tmp = data[0][width-1];
						} else {
							// type 2
							tmp = data[0][nc];
						}
					} else if(nr >= height) {
						// type 6, 7, 8
						if(nc < 0) {
							// type 6
							tmp = data[height-1][0];
						} else if(nc >= width) {
							// type 8
							tmp = data[height-1][width-1];
						} else {
							// type 7
							tmp = data[height-1][nc];
						}
					} else {
						// type 4, 5
						if(nc < 0) {
							// type 4
							tmp = data[nr][0];
						} else if(nc >= width) {
							// type 5
							tmp = data[nr][width-1];
						} else {
							// (in range)
							tmp = data[nr][nc];
						}
					}

					// push into vector
					neighbor_values.push_back(tmp);
				}
			}
			// a little check
			if(neighbor_values.size() != (bheight * bwidth)) {
				cerr << "error occurred when getting neighbor_values..." << endl;
				return false;
			}
			// sort the vector
			sort(neighbor_values.begin(), neighbor_values.end());
			res[r][c] = neighbor_values.at(neighbor_values.size() / 2);
		}
	}
	return true;
}

bool low_pass_filter(unsigned char** data, unsigned char** res, int height, int width, double b) {
	// we use 3x3 low pass filter here
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			// set up some variables
			int counter = 0;
			int neighbors[9] = {0};
			double kernel[9] = {1, b, 1, b, b*b, b, 1, b, 1};
			// get neighbors
			for(int nr = r-1; nr <= r+1; nr++) {
				for(int nc = c-1; nc <= c+1; nc++) {
					int real_r, real_c;
					// check row in range
					if(nr < 0)
						real_r = 0;
					else if(nr >= height)
						real_r = height - 1;
					else
						real_r = nr;
					// check column in range
					if(nc < 0)
						real_c = 0;
					else if(nc >= width)
						real_c = width - 1;
					else
						real_c = nc;
					// put into _neighbors_ array
					neighbors[counter++] = data[real_r][real_c];
				}
			}
			// calculate
			double conv_sum = 0.0;
			for(int i=0; i<9; i++)
				conv_sum += ((double)neighbors[i]) * kernel[i];
			res[r][c] = floor(conv_sum / ((b + 2.0) * (b + 2.0)));
		}
	}
	return true;
}

double getSNR(unsigned char** im1, unsigned char** im2, int height, int width) {
	// 1) calculate MSE
	double MSE = 0.0; // mean squared error
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			MSE = MSE + (im1[r][c] - im2[r][c]) * (im1[r][c] - im2[r][c]);
		}
	}
	MSE = MSE / (double)(height * width);

	// 2) form PSNR, i.e., peak signal-to-noise ratio
	double PSNR = 10.0 * log(255.0 * 255.0 / MSE) / log(10.0);
	return PSNR;
}

bool doCannyEdgeDetector(unsigned char** im, unsigned char** edge_im, int height, int width, double TH, double TL) {
	
	// parameters:
	//     im: the original image
	//     edge_im: the resulted edge map (edge pixels = 255, non-edge pixels = 0)
	//     height: the height of im
	//     width: the width of im
	//     TH: the high threshold for Hysteretic thresholding
	//     TL: the low threshold for Hysteretic thresholding

	// 5 Steps of Canny Edge Detector:
	//     #1: Noise Reduction (with a Gaussian filter)
	//     #2: Compute Gradient Magnitude and Orientation
	//     #3: Non-maximal Suppression
	//     #4: Hysteretic Thresholding
	//     #5: Connected Component Labeling Method

	// step #1: Noise Reduction (with a Gaussian filter)
	unsigned char im_nr[height][width]; // store the result of noide reduction
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			// get neighboring values
			vector<int> neighbor_values; // store the intensity of neighboring pixels
			for(int nr = r - 2; nr <= r + 2; nr++) {
				for(int nc = c - 2; nc <= c + 2; nc++) {
					// check if this neighbor (nr, nc) in range
					// if not, we use "extending solution"
					int real_nr = (nr < 0) ? 0 : (nr >= height) ? (height-1) : nr;
					int real_nc = (nc < 0) ? 0 : (nc >= width) ? (width-1) : nc;
					neighbor_values.push_back(im[real_nr][real_nc]);
				}
			}
			// convolution
			int gaussian_mask[25] = {
				2,  4,  5,  4, 2,
				4,  9, 12,  9, 4,
				5, 12, 15, 12, 5,
				4,  9, 12,  9, 4,
				2,  4,  5,  4, 2
			};
			int result = 0;
			for(int i=0; i<25; i++) {
				result += (neighbor_values[i] * gaussian_mask[i]);
			}
			// put the result in im_nr[r][c]
			im_nr[r][c] = (result / 159);
		}
	}
	return true;
}

