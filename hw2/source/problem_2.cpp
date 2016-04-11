#include <iostream>
#include <string>
#include "ImageUtil.h"
#include "ImageUtil.cpp"

void print_usage();

int main(int argc, char* argv[]) {

	// check arguments
	if(argc != 5) {
		cerr << "wrong arguments!" << endl;
		print_usage();
		exit(-1);
	}

	// prepare four 2d arrays for storing sample4.raw ~ sample7.raw images
	// (size = 324 x 324)
	unsigned char** im[4];
	for(int i=0; i<4; i++) {
		im[i] = (unsigned char**)malloc(324 * sizeof(unsigned char*));
		for(int j=0; j<324; j++) {
			im[i][j] = (unsigned char*)malloc(324 * sizeof(unsigned char));
		}
	}

	// ========== read in sample{4,5,6,7}.raw ========== //

	for(int i=0; i<4; i++) {
		if(!openImageAsMatrix(argv[i+1], im[i], 324, 324)) {
			cerr << "cannot open image at: " << argv[i+1] << endl;
		}
	}

	// ========== image registration ========== //

	pair<int, int> offset[4];
	offset[0] = getOffset(im[3], im[0], 324, 324);
	offset[1] = getOffset(im[0], im[1], 324, 324);
	offset[2] = getOffset(im[3], im[2], 324, 324);
	offset[3] = getOffset(im[2], im[1], 324, 324);
	// ultimate offset for each
	if((offset[0].first + offset[1].first) == (offset[2].first + offset[3].first)
		&& (offset[0].second + offset[1].second) == (offset[2].second + offset[3].second))
		cerr << "getOffset(): Perfectly Matched!" << endl;
	offset[1].first += offset[0].first;
	offset[1].second += offset[0].second;
	offset[3].first = 0;
	offset[3].second = 0;
	// offset[0]: offset for sample4 with respect to sample7
	// offset[1]: offset for sample5 with respect to sample7
	// offset[2]: offset for sample6 with respect to sample7
	// offset[3]: offset for sample7 with respect to sample7 (i.e., (0, 0))
	// calculate the size of the stitching result
	int offset_r_max = offset[0].first, offset_r_min = offset[0].first;
	int offset_c_max = offset[0].second, offset_c_min = offset[0].second;
	for(int i=0; i<4; i++) {
		offset_r_max = (offset[i].first > offset_r_max) ? offset[i].first : offset_r_max;
		offset_r_min = (offset[i].first < offset_r_min) ? offset[i].first : offset_r_min;
		offset_c_max = (offset[i].second > offset_c_max) ? offset[i].second : offset_c_max;
		offset_c_min = (offset[i].second < offset_c_min) ? offset[i].second : offset_c_min;
	}
	int stitching_size[2] = {(offset_r_max - offset_r_min + 324), (offset_c_max - offset_c_min + 324)};
	cerr << "after stitching, the total size = (" << stitching_size[0] << ", " << stitching_size[1] << ")" << endl;

	// ========== image stitching ========== //

	// initialize stitching_map: for marking pixels as covered / not-covered (i.e., residual)
	bool stitching_map[stitching_size[0]][stitching_size[1]];
	for(int i=0; i<stitching_size[0]; i++)
		for(int j=0; j<stitching_size[1]; j++)
			stitching_map[i][j] = false; // initialize as not-covered area (residual area)
	// initialize im_stitched: the resulted stitched image
	unsigned char** im_stitched = (unsigned char**)malloc(stitching_size[0] * sizeof(unsigned char*));
	for(int i=0; i<stitching_size[0]; i++) {
		im_stitched[i] = (unsigned char*)malloc(stitching_size[1] * sizeof(unsigned char));
		for(int j=0; j<stitching_size[1]; j++)
			im_stitched[i][j] = 0;
	}
	// for each of the original small images
	for(int i=0; i<4; i++) {
		for(int r=0; r<324; r++) {
			for(int c=0; c<324; c++) {
				// calculate the position to draw on stitched image
				int drawIndex[2] = {
					(offset[i].first - offset_r_min + r),
					(offset[i].second - offset_c_min + c)
				};
				im_stitched[drawIndex[0]][drawIndex[1]] = im[i][r][c];
				// mark as covered pixel
				stitching_map[drawIndex[0]][drawIndex[1]] = true;
			}
		}
	}
	// write stitched result to file
	// bool writeImageToFile(char* path, unsigned char** data, int height, int width);
	if(!writeImageToFile("p2_R.raw", im_stitched, stitching_size[0], stitching_size[1])) {
		cerr << "cannot write image to file (p2_R.raw)..." << endl;
		exit(-1);
	}

	// ========== cropping ========== //

	// find the start position for cropping
	// hint: the cropped result will be of size 512 x 512
	int cropped_start[2] = {-1, -1};
	for(int r=0; r <= (stitching_size[0] - 512); r++) {
		for(int c=0; c <= (stitching_size[1] - 512); c++) {
			// check if (r, c) is the start pos
			bool check = true;
			for(int r2=0; r2<512; r2++)
				for(int c2=0; c2<512; c2++)
					if(!stitching_map[r+r2][c+c2]) // encountered uncovered pixel in 512 x 512
						check = false;
			if(check) {
				// (r, c) is the start pos
				cropped_start[0] = r;
				cropped_start[1] = c;
				break;
			}
		}
	}
	if(cropped_start[0] == -1 || cropped_start[1] == -1) {
		cerr << "cannot find a 512 x 512 region to crop!..." << endl;
		exit(-1);
	}
	cerr << "cropping starts at: (" << cropped_start[0] << ", " << cropped_start[1] << ")" << endl;
	// do the cropping
	unsigned char** im_cropped = (unsigned char**)malloc(512 * sizeof(unsigned char*));
	for(int i=0; i<512; i++) {
		im_cropped[i] = (unsigned char*)malloc(512 * sizeof(unsigned char));
		for(int j=0; j<512; j++)
			im_cropped[i][j] = im_stitched[i + cropped_start[0]][j + cropped_start[1]];
	}
	// write result to file
	if(!writeImageToFile("p2_S.raw", im_cropped, 512, 512)) {
		cerr << "cannot write image to file (p2_S.raw)... " << endl;
		exit(-1);
	}


	// ========== warping ========== //

	// 1st part: recrangle => rectangle
	// 2nd part: rectangle => circle
	// 3rd part: rectangle => oval


	return 0;
}

void print_usage() {
	cerr << "Usage:" << endl;
	cerr << "\t./a.out <sample4> <sample5> <sample6> <sample7>" << endl;
}


