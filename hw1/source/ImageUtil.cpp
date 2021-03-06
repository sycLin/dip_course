/**
 * This is the implementation of the functions defined in imageUtil.h header.w
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

bool drawHistogram(unsigned char** data, unsigned char** res, int height, int width) {

	// check width (should be at least 256)
	if(width < 256)
		return false;

	// initialize the res variable
	for(int row = 0; row < height; row++)
		for(int col = 0; col < width; col++)
			res[row][col] = (unsigned char)0;

	// store intensity count in array: count
	int count[256] = {0};
	int MAX_COUNT = 0;

	// do the counting
	for(int row = 0; row < height; row++) {
		for(int col = 0; col < width; col++) {
			int intensity = (int)(data[row][col]);
			count[intensity] += 1;
			if(count[intensity] > MAX_COUNT)
				MAX_COUNT = count[intensity];
		}
	}

	// scale and draw
	int width_unit = width / 256;
	for(int i=0; i<256; i++) {
		int bar_height = (int)floor((double)count[i] / (double)MAX_COUNT * (double)height); // scaling
		for(int col = i * width_unit; col < (i+1) * width_unit; col++) {
			for(int row = height - 1; row >= height - bar_height; row--) {
				res[row][col] = (unsigned char)255;
			}
		}
	}

	return true;
}

bool hist_eq(unsigned char** data, unsigned char** res, int height, int width) {
	
	// count the intensity
	
	int count[256] = {0};
	
	for(int row = 0; row < height; row++) {
		for(int col = 0; col < width; col++) {
			int intensity = (int)(data[row][col]); // cast to (int) from (unsigned char)
			count[intensity] += 1;
		}
	}

	// compute the cdf (cumulative distribution function)
	
	int cdf[256] = {0};
	int MIN_CDF = 0;

	cdf[0] = count[0];
	for(int i=1; i<256; i++) {
		cdf[i] = cdf[i-1] + count[i];
		if(MIN_CDF == 0 && cdf[i] != 0)
			MIN_CDF = cdf[i]; // this line executes for only once
	}

	// create the h function (a 1-D array for table lookup)
	
	int h[256] = {0};
	int pixel_count = height * width;
	
	for(int i=0; i<256; i++) {
		if(cdf[i] == 0)
			h[i] = 0;
		else
			h[i] = floor((float)(cdf[i] - MIN_CDF) / (float)(pixel_count - MIN_CDF) * 255.0f); // THE FORMULA!!
	}

	// produce the result
	for(int row = 0; row < height; row++) {
		for(int col = 0; col < width; col++) {
			res[row][col] = (unsigned char)h[(int)data[row][col]];
		}
	}

	return true;

}

bool local_hist_eq(unsigned char** data, unsigned char** res, int height, int width, int window_h, int window_w) {

	// loop over all pixels in original image
	for(int row = 0; row < height; row++) {
		for(int col = 0; col < width; col++) {
			// record the rank, i.e., how many neighbors are smaller than myself
			int rank = 0;
			int neighbor_count = 0;

			// go through neighbors
			for(int r = row - window_h/2; r <= row + window_h/2; r++) {
				for(int c = col - window_w/2; c <= col + window_w/2; c++) {
					// check if inRange and isNeighbor
					if(r >= 0 && r < height && r != row && c >= 0 && c < width && c != col) {
						neighbor_count += 1;
						// check if smaller than myself
						if(data[r][c] < data[row][col])
							rank += 1;
					}
				}
			}

			// assign new intensity
			res[row][col] = floor(((float)rank / (float)neighbor_count) * 255.0f);
		}
	}
	return true;
}

bool log_transform(unsigned char** data, unsigned char** res, int height, int width) {

	if(height <= 0 || width <= 0)
		return false;
	
	// store the transformation table
	int tr[256] = {0};
	
	// get the MAX magnitude
	int MAX_MAG = 0;
	for(int r = 0; r < height; r++)
		for(int c = 0; c < width; c++)
			if(data[r][c] > MAX_MAG)
				MAX_MAG = data[r][c];

	// calculate and fill in the table
	double a = 1.0; // can be modified
	double tmp;
	for(int i=0; i<256; i++) {
		tmp  = log(1.0 + a * ((double)i / 255.0)) / log(2.0);
		tr[i] = (int)floor(tmp*255.0);
	}

	// do the transformation
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			res[r][c] = tr[data[r][c]];
		}
	}
	return true;
}

bool inverse_log_transform(unsigned char** data, unsigned char** res, int height, int width) {
	if(height <= 0 || width <= 0)
		return false;
	
	// store the transformation table
	int tr[256] = {0};
	
	// get the MAX magnitude
	int MAX_MAG = 0;
	for(int r = 0; r < height; r++)
		for(int c = 0; c < width; c++)
			if(data[r][c] > MAX_MAG)
				MAX_MAG = data[r][c];

	// calculate and fill in the table
	double tmp;
	for(int i=0; i<256; i++) {
		// 1) normalize
		tmp = (double)i / (double)MAX_MAG; // tmp: 0~1 (inclusive)
		// 2) exp transform
		tmp = exp(tmp);
		// 3) re-scaling
		
		// exp(0) => 0
		// exp(1) => 255

		//   exp(0)        tmp      exp(1)
		// ----|------------|---------|
		//     0            x        255

		// so, x:255 = (tmp - exp(0)):(exp(1) - exp(0))
		// so, x = 255 * (tmp - exp(0)) / (exp(1) - exp(0))
		tmp = 255.0 * (tmp - 1.0) / (exp(1) - 1.0);
		tr[i] = (int)floor(tmp);
	}

	// do the transformation
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			res[r][c] = tr[data[r][c]];
		}
	}
	return true;
}

bool power_law_transform(unsigned char** data, unsigned char** res, int height, int width, double gamma) {
	if(height <= 0 || width <= 0)
		return false;
	
	// store the transformation table
	int tr[256] = {0};
	
	// get the MAX magnitude and MIN magnitude
	int MAX_MAG = 0;
	int MIN_MAG = 255;
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			if(data[r][c] > MAX_MAG)
				MAX_MAG = data[r][c];
			if(data[r][c] < MIN_MAG)
				MIN_MAG = data[r][c];
		}
	}

	// calculate and fill in the table
	
	double pMAX = pow(MAX_MAG, gamma);
	double pMIN = pow(MIN_MAG, gamma);

	// pow(MIN, gamma)   pow(tmp, gamma)   pow(MAX, gamma)
	//    ---|-----------------|------------------|----
	//       0                 x                 255
	// => x : 255 = (pow(tmp, gamma) - pow(MIN, gamma)) : (pow(MAX, gamma) - pow(MIN, gamma))
	// => x = 255 * (pow(tmp, gamma) - pow(MIN, gamma)) / (pow(MAX, gamma) - pow(MIN, gamma))
	// => x = 255 * (pow(tmp, gamma) - pMIN) / (pMAX - pMIN)
	double tmp;
	for(int i=0; i<256; i++) {
		tmp = pow((double)i, gamma);
		tmp = 255.0 * (tmp - pMIN) / (pMAX - pMIN);
		tr[i] = (int)floor(tmp);
	}

	// do the transformation
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			res[r][c] = tr[data[r][c]];
		}
	}
	return true;
}

bool add_impulse_noise(unsigned char** data, unsigned char** res, int height, int width, double prob) {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0.0, 1.0);
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			double ran = static_cast<double>(dis(gen));
			if(ran < prob)
				res[r][c] = 0;
			else if(ran > (1-prob))
				res[r][c] = 255;
			else
				res[r][c] = data[r][c];
		}
	}
	return true;
}

bool add_uniform_noise(unsigned char** data, unsigned char** res, int height, int width, double amp) {
	random_device rd;
	mt19937 gen(rd());
	normal_distribution<double> dis(0.0, 1.0);
	// double ran = distribution(generator);
	for(int r = 0; r < height; r++) {
		for(int c = 0; c < width; c++) {
			// get Gaussian random number
			double ran = static_cast<double>(dis(gen));
			// get new value
			ran = (double)data[r][c] + amp * ran;
			// check in range
			if(ran >= 255.0)
				res[r][c] = 255;
			else if(ran <= 0.0)
				res[r][c] = 0;
			else
				res[r][c] = floor(ran);
		}
	}
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



