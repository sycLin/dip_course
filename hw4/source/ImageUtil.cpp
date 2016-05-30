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

unsigned char** dilation(unsigned char** data, int height, int width, int foreground) {
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
				for(int i=r-1; i<=r+1; i++) {
					for(int j=c-1; j<=c+1; j++) {
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


