#include <iostream>
#include <string>
#include "ImageUtil.h"
#include "ImageUtil.cpp"

void print_usage();

int main(int argc, char* argv[]) {
	srand(time(NULL));

	// check arguments
	cerr << "// check arguments" << endl;
	if(argc != 2) {
		cerr << "wrong arguments!" << endl;
		print_usage();
		exit(-1);
	}

	// initialize a 2d array to store the original image
	unsigned char** im = (unsigned char**)malloc(512 * sizeof(unsigned char*));
	for(int i=0; i<512; i++) {
		im[i] = (unsigned char*)malloc(512 * sizeof(unsigned char));
		for(int j=0; j<512; j++)
			im[i][j] = 0;
	}

	// ========== Read Image Specified in Argument ========== //
	// bool openImageAsMatrix(char* path, unsigned char** ptr, int height, int width);
	if(!openImageAsMatrix(argv[1], im, 512, 512)) {
		cerr << "cannot open image at: " << argv[1] << endl;
		exit(-1);
	}

	// ========== Apply Impulse Response Arrays ========== //
	cerr << "// ========== Apply Impulse Response Arrays ========== //" << endl;

	int laws_coeff[9] = {36, 12, 12, 12, 4, 4, 12, 4, 4};
	int laws_masks[9][9] = {
		{
			1, 2, 1,
			2, 4, 2,
			1, 2, 1
		},
		{
			1, 0, -1,
			2, 0, -2,
			1, 0, -1
		},
		{
			-1, 2, -1,
			-2, 4, -2,
			-1, 2, -1
		},
		{
			-1, -2, -1,
			0, 0, 0,
			1, 2, 1
		},
		{
			1, 0, -1,
			0, 0, 0,
			-1, 0, 1
		},
		{
			-1, 2, -1,
			0, 0, 0,
			1, -2, 1
		},
		{
			-1, -2, -1,
			2, 4, 2,
			-1, -2, -1
		},
		{
			-1, 0, 1,
			2, 0, -2,
			-1, 0, 1
		},
		{
			1, -2, 1,
			-2, 4, -2,
			1, -2, 1
		}
	};
	double M[9][512][512];
	for(int i=0; i<9; i++) {
		// convolution with laws_masks[i]
		// store the result in M[i]
		for(int r=0; r<512; r++) {
			for(int c=0; c<512; c++) {
				// get neighboring values
				vector<int> neighbor_values;
				for(int nr = r - 1; nr <= r + 1; nr++) {
					for(int nc = c - 1; nc <= c + 1; nc++) {
						int real_nr = (nr < 0) ? 0 : (nr >= 512) ? 511 : nr;
						int real_nc = (nc < 0) ? 0 : (nc >= 512) ? 511 : nc;
						neighbor_values.push_back(im[real_nr][real_nc]);
					}
				}
				// convolve
				double result = 0.0;
				for(int j=0; j<9; j++) {
					result += (double)laws_masks[i][j] * (double)neighbor_values[j];
				}
				result = result / (double)laws_coeff[i];
				// store in M[i]
				M[i][r][c] = result;
			}
		}
	}

	// ========== Calculate Feature Vectors ========== //
	cerr << "// ========== Calculate Feature Vectors ========== //" << endl;

	// calculate feature vector with a moving window
	// the window size here is set to 35 x 35
	int window_size = 13;
	double T[9][512][512];
	double feature_vec_max[9]; // to store maximum in each dimension
	for(int i=0; i<9; i++)
		feature_vec_max[i] = 0.0;
	for(int i=0; i<9; i++) {
		// perform standard deviation measurement within moving window
		// store in T[i]
		for(int r=0; r<512; r++) {
			for(int c=0; c<512; c++) {
				// loop through window
				double result = 0.0;
				for(int nr = r - (window_size/2); nr <= r + (window_size/2); nr++) {
					for(int nc = c - (window_size/2); nc <= c + (window_size/2); nc++) {
						int real_nr = (nr < 0) ? 0 : (nr >= 512) ? 511 : nr;
						int real_nc = (nc < 0) ? 0 : (nc >= 512) ? 511 : nc;
						double tmp = (double)(im[real_nr][real_nc] - M[i][real_nr][real_nc]);
						result += tmp * tmp;
					}
				}
				// standard deviation
				result = sqrt(result) / (double)window_size;
				T[i][r][c] = result;
				// update feature_vec_max
				if(result > feature_vec_max[i])
					feature_vec_max[i] = result;
			}
		}
	}

	// ========== Apply K-means Algorithm ========== //
	cerr << "// ========== Apply K-means Algorithm ========== //" << endl;
	// int K = 4; // domain knowledge
	double centroid[4][9];
	unsigned char** classified = (unsigned char**)malloc(512 * sizeof(unsigned char*));
	for(int i=0; i<512; i++) {
		classified[i] = (unsigned char*)malloc(512 * sizeof(unsigned char));
		for(int j=0; j<512; j++)
			classified[i][j] = 0;
	}
	// randomly assign all the centroids first
	for(int i=0; i<4; i++) {
		cerr << "centroid[" << i << "]: ";
		for(int j=0; j<9; j++) {
			centroid[i][j] = (double)(rand() % (int)feature_vec_max[j]);
			cerr << centroid[i][j] << " ";
		}
		cerr << endl;
	}
	int iteration_count = 0;
	// debug: run 10 times
	while(iteration_count < 3000) {
		// tmp_centroid for pre-calculating the next centroids
		double tmp_centroid[4][9];
		for(int i=0; i<4; i++) {
			for(int j=0; j<9; j++)
				tmp_centroid[i][j] = 0.0;
		}
		int cluster_counter[4] = {0, 0, 0, 0}; // to store how many pixels in a cluster
		// classify each pixel
		for(int r=0; r<512; r++) {
			for(int c=0; c<512; c++) {
				// calculate the distance
				double dist[4];
				for(int i=0; i<4; i++) {
					// dist[i]: the distance from T[][r][c] to centroid[i][]
					dist[i] = 0.0;
					for(int j=0; j<9; j++) {
						dist[i] += (T[j][r][c] - centroid[i][j]) * (T[j][r][c] - centroid[i][j]);
					}
				}
				// determine the cluster 
				// also pre-calculate the next centroid
				if(dist[0] <= dist[1] && dist[0] <= dist[2] && dist[0] <= dist[3]) {
					// cluster 0
					// mark cluster
					classified[r][c] = 255 * 0 / 3;
					// update next centroid calculation
					for(int i=0; i<9; i++)
						tmp_centroid[0][i] += T[i][r][c];
					cluster_counter[0] += 1;
				} else if(dist[1] <= dist[0] && dist[1] <= dist[2] && dist[1] <= dist[3]) {
					// cluster 1
					// mark cluster
					classified[r][c] = 255 * 1 / 3;
					// update next centroid calculation
					for(int i=0; i<9; i++)
						tmp_centroid[1][i] += T[i][r][c];
					cluster_counter[1] += 1;
				} else if(dist[2] <= dist[0] && dist[2] <= dist[1] && dist[2] <= dist[3]) {
					// cluster 2
					// mark cluster
					classified[r][c] = 255 * 2 / 3;
					// update next centroid calculation
					for(int i=0; i<9; i++)
						tmp_centroid[2][i] += T[i][r][c];
					cluster_counter[2] += 1;
				} else {
					// cluster 3
					// mark cluster
					classified[r][c] = 255 * 3 / 3;
					// update next centroid calculation
					for(int i=0; i<9; i++)
						tmp_centroid[3][i] += T[i][r][c];
					cluster_counter[3] += 1;
				}
			}
		}
		// update centroid
		for(int i=0; i<4; i++) {
			for(int j=0; j<9; j++) {
				if(cluster_counter[i] == 0)
					centroid[i][j] = (double)(rand() % (int)feature_vec_max[j]);
				else
					centroid[i][j] = tmp_centroid[i][j] / (double)cluster_counter[i];
			}
		}
		// debug
		// for(int i=0; i<4; i++) {
		// 	cerr << "cluster " << i << " has " << cluster_counter[i] << " pixels." << endl;
		// }
		if(iteration_count % 100 == 99) {
			cerr << "\rAlready " << iteration_count+1 << " iterations.";
		}
		iteration_count += 1;
	}
	cerr << "\rAlready " << iteration_count+1 << " iterations." << endl;

	// debug write to image
	if(!writeImageToFile("p3_L.raw", classified, 512, 512)) {
		cerr << "cannot write image to file! (p3_L.raw)" << endl;
		exit(-1);
	}

	
	// ========== Attach Texture to Animals ========== //
	cerr << "// ========== Attach Texture to Animals ========== //" << endl;

	// initialize the textured image
	unsigned char** textured_im = (unsigned char**)malloc(512 * sizeof(unsigned char*));
	for(int i=0; i<512; i++) {
		textured_im[i] = (unsigned char*)malloc(512 * sizeof(unsigned char));
		for(int j=0; j<512; j++) {
			textured_im[i][j] = 0;
		}
	}
	// get the textures (heuristic)
	// [0]: zebra
	// [1]: jaguar
	// [2]: giraffe
	int texture_start[3][2] = {
		{350, 225},
		{195, 435},
		{10, 225}
	};
	int texture_size[3][2] = {
		{70, 50},
		{105, 65},
		{75, 50}
	};
	// fill in
	int zebra_where;
	int jaguar_where;
	int giraffe_where;
	cout << "Where is zebra? [0] black [1] dark-gray [2] light-gray [3] white. > ";
	cin >> zebra_where;
	cout << "Where is jaguar? [0] black [1] dark-gray [2] light-gray [3] white. > ";
	cin >> jaguar_where;
	cout << "Where is giraffe? [0] black [1] dark-gray [2] light-gray [3] white. > ";
	cin >> giraffe_where;
	for(int r=0; r<512; r++) {
		for(int c=0; c<512; c++) {
			if(classified[r][c] == zebra_where * 255 / 3) {
				textured_im[r][c] = im[texture_start[0][0] + r % texture_size[0][0]][texture_start[0][1] + c % texture_size[0][1]];
			} else if(classified[r][c] == jaguar_where * 255 / 3) {
				textured_im[r][c] = im[texture_start[1][0] + r % texture_size[1][0]][texture_start[1][1] + c % texture_size[1][1]];
			} else if(classified[r][c] == giraffe_where * 255 / 3) {
				textured_im[r][c] = im[texture_start[2][0] + r % texture_size[2][0]][texture_start[2][1] + c % texture_size[2][1]];
			} else {
				// background
			}
		}
	}
	// write image to file
	if(!writeImageToFile("p3_C.raw", textured_im, 512, 512)) {
		cerr << "cannot write image to file! (p3_C.raw)" << endl;
		exit(-1);
	}



	return 0;
}

void print_usage() {
	cerr << "Usage:" << endl;
	cerr << "\t./a.out <sample8>" << endl;
}


