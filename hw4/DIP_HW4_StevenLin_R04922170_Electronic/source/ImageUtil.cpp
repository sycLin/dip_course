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

void binarize(unsigned char** data, int height, int width, double threshold) {
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(data[r][c] > threshold)
				data[r][c] = 255;
			else
				data[r][c] = 0;
		}
	}
}

unsigned char** dilation(unsigned char** data, int height, int width, int foreground, int mask_size) {
	// create new image and initialize
	unsigned char** new_data = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for(int r=0; r<height; r++) {
		new_data[r] = (unsigned char*)malloc(width * sizeof(unsigned char));
		// initialize as background
		for(int c=0; c<width; c++) {
			new_data[r][c] = (foreground == 0) ? 255 : 0;
		}
	}
	// do the dilation
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			// check if dilate
			if(data[r][c] == foreground) {
				// set new data foreground
				new_data[r][c] = foreground;
				// iterate through the neighbors
				for(int i = r - mask_size/2; i <= r + mask_size/2; i++) {
					for(int j = c - mask_size/2; j <= c + mask_size/2; j++) {
						// check if in range
						if(i >= 0 && i < height && j >= 0 && j < width)
							new_data[i][j] = foreground;
					}
				}
			}
		}
	}
	return new_data;
}

unsigned char** erosion(unsigned char** data, int height, int width, int foreground, int mask_size) {
	// create new image and initialize
	unsigned char** new_data = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for(int i=0; i<height; i++) {
		new_data[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
		for(int j=0; j<width; j++) {
			new_data[i][j] = (foreground == 0) ? 255 : 0 ; // set to background
		}
	}
	// do the erosion
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			// check if in range
			if(r - mask_size/2 >= 0 && c - mask_size/2 >= 0 && r + mask_size/2 < height && c + mask_size/2 < width) {
				// check if validate
				bool valid = true;
				for(int i = r - mask_size/2; i <= r + mask_size/2; i++)
					for(int j = c - mask_size/2; j <= c + mask_size/2; j++)
						if(data[i][j] != foreground)
							valid = false;
				if(valid)
					new_data[r][c] = foreground;
			}
		}
	}
	return new_data;
}

unsigned char** extractBoundary(unsigned char** data, int height, int width, int foreground) {
	// allocate boundary image
	unsigned char** boundary = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for(int i=0; i<height; i++) {
		boundary[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
		for(int j=0; j<width; j++)
			boundary[i][j] = (foreground == 0)? 255 : 0; // set to background
	}

	// do the erosion: eroded = erosion(data)
	unsigned char** eroded = erosion(data, height, width, foreground, 3);

	// do the difference: boundary = data - eroded
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(data[r][c] == foreground && eroded[r][c] != foreground)
				boundary[r][c] = foreground;
		}
	}
	return boundary;
}

unsigned char** imageUnion(unsigned char** data1, unsigned char** data2, int height, int width, int foreground) {
	// create the new image
	unsigned char** ret = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for(int i=0; i<height; i++) {
		ret[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
		for(int j=0; j<width; j++)
			ret[i][j] = (foreground == 0) ? 255 : 0; // set to background
	}
	// do the union
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(data1[r][c] == foreground || data2[r][c] == foreground)
				ret[r][c] = foreground;
		}
	}
	return ret;
}

unsigned char** imageDifference(unsigned char** data1, unsigned char** data2, int height, int width, int foreground) {
	// create the new image
	unsigned char** ret = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for(int i=0; i<height; i++) {
		ret[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
		for(int j=0; j<width; j++)
			ret[i][j] = (foreground == 0) ? 255 : 0; // set to background
	}
	// do the difference
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(data1[r][c] == foreground && data2[r][c] != foreground)
				ret[r][c] = foreground;
		}
	}
	return ret;
}

void computeDFT(unsigned char** data, int height, int width, double** &real_part, double** &im_part) {
	// allocating
	real_part = (double**)malloc(height * sizeof(double*));
	im_part = (double**)malloc(height * sizeof(double*));
	for(int i=0; i<height; i++) {
		real_part[i] = (double*)malloc(width * sizeof(double));
		im_part[i] = (double*)malloc(width * sizeof(double));
	}
	// computing DFT
	for(int u=0; u<height; u++) {
		for(int v=0; v<width; v++) {
			// computing F(u, v)
			double real = 0.0; // real part
			double im = 0.0; // imaginary part
			// summing over f(x, y) => to get F(u, v)
			for(int x=0; x<height; x++) {
				for(int y=0; y<width; y++) {
					// f(x, y) * exp(...)
					double tmp = ((double)(x * u) / (double)height) + ((double)(y * v) / (double)width);
					real += (data[x][y] * cos(-2.0 * M_PI * tmp));
					im += (data[x][y] * sin(-2.0 * M_PI * tmp));
				}
			}
			// store into real_part and im_part
			real_part[u][v] = real;
			im_part[u][v] = im;
		}
	}
}

unsigned char** inverseDFT(double** real_part, double** im_part, int height, int width) {
	// create new image
	unsigned char** new_data = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for(int i=0; i<height; i++)
		new_data[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
	// computing the inverse DFT
	for(int x=0; x<height; x++) {
		for(int y=0; y<width; y++) {
			// computing f(x, y)
			double real = 0.0;
			double im = 0.0;
			// summing over F(u, v)
			for(int u=0; u<height; u++) {
				for(int v=0; v<width; v++) {
					// F(u, v) * exp(...)
					double tmp = ((double)(x * u) / (double)height) + ((double)(y * v) / (double)width);
					real += (real_part[u][v] * cos(2.0 * M_PI * tmp) - im_part[u][v] * sin(2.0 * M_PI * tmp));
					im += (real_part[u][v] * sin(2.0 * M_PI * tmp) + im_part[u][v] * cos(2.0 * M_PI * tmp));
				}
			}
			// divided by (M * N), i.e., (height * width), and store into f(x, y)
			new_data[x][y] = real / (height * width);
		}
	}
	return new_data;
}



