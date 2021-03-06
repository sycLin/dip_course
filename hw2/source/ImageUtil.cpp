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

bool doSobelEdgeDetector(unsigned char** im, unsigned char** edge_im, int height, int width, double threshold) {
	// prepare sobel kernels
	int sobel_mask_row[9] = {
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1
	};
	int sobel_mask_col[9] = {
		-1, -2, -1,
		0, 0, 0,
		1, 2, 1
	};
	// compute gradient magnitude
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			// get neighboring values
			vector<int> neighbor_values; // store the intensity of neighboring pixels
			for(int nr = r - 1; nr <= r + 1; nr++) {
				for(int nc = c - 1; nc <= c + 1; nc++) {
					// check if this neighbor (nr, nc) in range
					// if not, we use "extending solution"
					int real_nr = (nr < 0) ? 0 : (nr >= height) ? (height-1) : nr;
					int real_nc = (nc < 0) ? 0 : (nc >= width) ? (width-1) : nc;
					neighbor_values.push_back(im[real_nr][real_nc]);
				}
			}
			// convolution
			double Gr = 0.0;
			double Gc = 0.0;
			for(int i=0; i<9; i++) {
				Gr += (double)(neighbor_values[i] * sobel_mask_row[i]);
				Gc += (double)(neighbor_values[i] * sobel_mask_col[i]);
			}
			double G = sqrt(Gr * Gr + Gc * Gc);
			if(G > threshold)
				edge_im[r][c] = 255;
			else
				edge_im[r][c] = 0;
		}
	}
	return true;
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

	// step #2: Compute Gradient Magnitude and Orientation
	double gradient_map[height][width]; // store the gradient magnitude of every pixel
	int gradient_direction[height][width]; // one of {1, 2, 3, 4}, which stands for {0, 45, 90, 135} respectively. 
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			// get neighboring values
			vector<int> neighbor_values; // store the intensity of neighboring pixels
			for(int nr = r - 1; nr <= r + 1; nr++) {
				for(int nc = c - 1; nc <= c + 1; nc++) {
					// check if this neighbor (nr, nc) in range
					// if not, we use "extending solution"
					int real_nr = (nr < 0) ? 0 : (nr >= height) ? (height-1) : nr;
					int real_nc = (nc < 0) ? 0 : (nc >= width) ? (width-1) : nc;
					neighbor_values.push_back(im[real_nr][real_nc]);
				}
			}
			// convolution with sobel filter (row & col)
			int sobel_mask_row[9] = {
				-1, 0, 1,
				-2, 0, 2,
				-1, 0, 1
			};
			int sobel_mask_col[9] = {
				-1, -2, -1,
				0, 0, 0,
				1, 2, 1
			};
			int row_gradient = 0;
			int col_gradient = 0;
			for(int i=0; i<9; i++) {
				row_gradient += (neighbor_values[i] * sobel_mask_row[i]);
				col_gradient += (neighbor_values[i] * sobel_mask_col[i]);
			}
			// get gradient from row_gradient and col_gradient
			gradient_map[r][c] = sqrt(row_gradient * row_gradient + col_gradient * col_gradient);
			// determine the gradient direction
			double theta = atan((double)col_gradient / (double)row_gradient) * 180.0 / M_PI; // will be -90 ~ 90
			if(theta >= -90.0 && theta < -67.5)
				gradient_direction[r][c] = 3; // 90 degrees
			else if(theta >= -67.5 && theta < -22.5)
				gradient_direction[r][c] = 4; // 135 degrees
			else if(theta >= -22.5 && theta < 22.5)
				gradient_direction[r][c] = 1; // 0 degrees
			else if(theta >= 22.5 && theta < 67.5)
				gradient_direction[r][c] = 2; // 45 degrees
			else if(theta >= 67.5 && theta <= 90)
				gradient_direction[r][c] = 3; // 90 degrees again
		}
	}

	// step #3: Non-maximal Suppression
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			// get the two nearest neighbors along the edge normal
			double n1_gradient, n2_gradient; // store the gradient magnitude of the 2 neighbors
			switch(gradient_direction[r][c]) {
				case 1:
					// n1: up, n2: down
					n1_gradient = ((r - 1) < 0) ? 0.0 : gradient_map[r-1][c];
					n2_gradient = ((r + 1) >= height) ? 0.0 : gradient_map[r+1][c];
					break;
				case 2:
					// n1: up-left, n2: down-right
					n1_gradient = ((r - 1) < 0) ? 0.0 : ((c - 1) < 0) ? 0.0 : gradient_map[r-1][c-1];
					n2_gradient = ((r + 1) >= height) ? 0.0 : ((c + 1) >= width) ? 0.0 : gradient_map[r+1][c+1];
					break;
				case 3:
					// n1: left, n2: right
					n1_gradient = ((c - 1) < 0) ? 0.0 : gradient_map[r][c-1];
					n2_gradient = ((c + 1) >= width) ? 0.0 : gradient_map[r][c+1];
					break;
				case 4:
					// n1: up-right, n2: down-left
					n1_gradient = ((r - 1) < 0) ? 0.0 : ((c + 1) >= width) ? 0.0 : gradient_map[r-1][c+1];
					n2_gradient = ((r + 1) >= height) ? 0.0 : ((c - 1) < 0) ? 0.0 : gradient_map[r+1][c-1];
					break;
				default: // shouldn't be here actually
					n1_gradient = 0.0;
					n2_gradient = 0.0;
					break;
			}
			if(gradient_map[r][c] <= n1_gradient || gradient_map[r][c] <= n2_gradient)
				// suppressed
				gradient_map[r][c] = 0.0;
		}
	}

	// step #4: Hysteretic Thresholding
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(gradient_map[r][c] >= TH)
				edge_im[r][c] = 255; // edge pixel
			else if(gradient_map[r][c] >= TL)
				edge_im[r][c] = 128; // candidate pixel
			else
				edge_im[r][c] = 0; // non-edge pixel
		}
	}
	int count_edge = 0;
	int count_candidate = 0;
	int count_non_edge = 0;
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(edge_im[r][c] == 255)
				count_edge += 1;
			else if(edge_im[r][c] == 128)
				count_candidate += 1;
			else if(edge_im[r][c] == 0)
				count_non_edge += 1;
			else
				cerr << "cannot categorize pixel at: " << r << ", " << c << endl;
		}
	}
	// cerr << "(edge, candidate, non-edge) = (" << count_edge << ", " << count_candidate << ", " << count_non_edge << ")" << endl;

	// step #5: Connected Component Labeling Method
	// initialize visited[i][j] to false for all i, j
	bool visited[height][width];
	for(int i=0; i<height; i++) 
		for(int j=0; j<width; j++)
			visited[i][j] = false;
	// go through the image
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(edge_im[r][c] == 255 && !visited[r][c]) {
				visited[r][c] = true;
				// diffuse from "not-visited edge pixel"
				// (using 8-connected, BFS, and a queue)
				queue< pair<int, int> > pixel_queue; // store the x, y coordinates
				pixel_queue.push(make_pair(r, c)); // push self
				while(!pixel_queue.empty()) {
					// pop one out
					pair<int, int> coord = pixel_queue.front();
					pixel_queue.pop();
					// explore
					for(int nr = coord.first-1; nr <= coord.first+1; nr++) {
						for(int nc = coord.second-1; nc <= coord.second+1; nc++) {
							if(nr >= 0 && nr < height && nc >= 0 && nc < width) {
								// neighbor (nr, nc) in range
								if(edge_im[nr][nc] >= 128 && !visited[nr][nc]) {
									// set as edge
									edge_im[nr][nc] = 255;
									visited[nr][nc] = true;
									// push into queue
									pixel_queue.push(make_pair(nr, nc));
								}
							}
						}
					}
				}
			}
		}
	}
	// eliminate the rest of the candidate pixels
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++)
			if(edge_im[r][c] != 255)
				edge_im[r][c] = 0;
	}
	return true;
}

bool doDifferenceOfGaussian(unsigned char** im, unsigned char** edge_im, int height, int width, int threshold) {
	// prepare the DOG kernel
	int DOG_MASK[121] = {
		-1, -3, -4, -6, -7, -8, -7, -6, -4, -3, -1,
		-3, -5, -8, -11, -13, -13, -13, -11, -8, -5, -3,
		-4, -8, -12, -16, -17, -17, -17, -16, -12, -8, -4,
		-6, -11, -16, -16, 0, 15, 0, -16, -16, -11, -6,
		-7, -13, -17, 0, 85, 160, 85, 0, -17, -13, -7,
		-8, -13, -17, 15, 160, 283, 160, 15, -17, -13, -8,
		-7, -13, -17, 0, 85, 160, 85, 0, -17, -13, -7,
		-6, -11, -16, -16, 0, 15, 0, -16, -16, -11, -6,
		-4, -8, -12, -16, -17, -17, -17, -16, -12, -8, -4,
		-3, -5, -8, -11, -13, -13, -13, -11, -8, -5, -3,
		-1, -3, -4, -6, -7, -8, -7, -6, -4, -3, -1
	};
	// convolve with DOG mask, and generate thresholded map
	int thresholded_map[height][width];
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			// get neighbor values
			vector<int> neighbor_values;
			for(int nr = r - 5; nr <= r + 5; nr++) {
				for(int nc = c - 5; nc <= c + 5; nc++) {
					// check if this neighbor (nr, nc) in range
					// if not, we use "extending solution"
					int real_nr = (nr < 0) ? 0 : (nr >= height) ? (height-1) : nr;
					int real_nc = (nc < 0) ? 0 : (nc >= width) ? (width-1) : nc;
					neighbor_values.push_back(im[real_nr][real_nc]);
				}
			}
			// convolve
			int tmp = 0;
			for(int i=0; i<121; i++) {
				tmp += (neighbor_values[i] * DOG_MASK[i]);
				if(tmp > threshold)
					thresholded_map[r][c] = 1;
				else if(tmp < -threshold)
					thresholded_map[r][c] = -1;
				else
					thresholded_map[r][c] = 0;
			}
		}
	}
	// determine zero-crossing with thresholded map
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(thresholded_map[r][c] == 1) {
				// check if any of the neighbors is -1
				bool check = false;
				for(int nr = r - 1; nr <= r + 1; nr++) {
					for(int nc = c - 1; nc <= c + 1; nc++) {
						// if in range && is -1
						if(nr >= 0 && nr < height && nc >= 0 && nc < width && thresholded_map[nr][nc] == -1)
							check = true;
					}
				}
				edge_im[r][c] = (check) ? 255 : 0;
			} else if(thresholded_map[r][c] == -1) {
				// check if any of the neighbors is 1
				bool check = false;
				for(int nr = r - 1; nr <= r + 1; nr++) {
					for(int nc = c - 1; nc <= c + 1; nc++) {
						// if in range && is 1
						if(nr >= 0 && nr < height && nc >= 0 && nc < width && thresholded_map[nr][nc] == 1)
							check = true;
					}
				}
				edge_im[r][c] = (check) ? 255 : 0;
			} else {
				edge_im[r][c] = 0;
			}
		}
	}
	return true;
}

pair<int, int> getOffset(unsigned char** im1, unsigned char** im2, int height, int width) {
	// try to move im2 to align to im1
	vector< pair< pair<int, int>, double > > diff_vector; // store the offset and diff value
	// enumerate all possible translation
	int debug_counter = 0;
	for(int off_x = -height+1; off_x < height; off_x++) {
		for(int off_y = 0; off_y < width; off_y++, debug_counter++) {
			// im2 with translation (off_x, off_y)
			// check overlap region area (should be > 200 pixels)
			int overlap_pixel_count = (int)((height - fabs(off_x)) * (width - fabs(off_y)));
			if(overlap_pixel_count <= 200)
				continue;
			double diff_value = 0.0;
			// loop over pixels in im1
			for(int r=0; r<height; r++) {
				for(int c=0; c<width; c++) {
					// im1[r][c]
					// im2[r - off_x][c - off_y]
					int r2 = r - off_x;
					int c2 = c - off_y;
					// check if in range
					if(r2 >= 0 && r2 < height && c2 >= 0 && c2 < width) {
						overlap_pixel_count += 1;
						diff_value += (double)((im1[r][c] - im2[r2][c2]) * (im1[r][c] - im2[r2][c2]));
					}
				}
			}
			diff_vector.push_back(make_pair(make_pair(off_x, off_y), diff_value/(double)overlap_pixel_count));
			if(debug_counter % 10000 == 9999)
				cerr << "\rProcessed " << debug_counter+1 << " offset possibilities!";
		}
	}
	cerr << "\rProcessed " << debug_counter+1 << " offset possibilities!" << endl;
	// sort the vector with std::sort()
	sort(diff_vector.begin(), diff_vector.end(), getOffset_compare);
	return diff_vector[0].first;
}

bool getOffset_compare(const pair<pair<int, int>, double>&i, const pair<pair<int, int>, double>&j) {
	// ascending order
	return i.second < j.second;
}

