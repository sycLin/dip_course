#include <iostream>
#include "ImageUtil.h"
#include "ImageUtil.cpp"
#include <unistd.h>

using namespace std;

struct Component {
	// constructor
	Component() {
		upperLeftCorner = make_pair(-1, -1);
		lowerRightCorner = make_pair(-1, -1);
	}
	// data
	pair<int, int> upperLeftCorner;
	pair<int, int> lowerRightCorner;
	// methods
	int getWidth() {
		return lowerRightCorner.second - upperLeftCorner.second;
	}
	int getHeight() {
		return lowerRightCorner.first - upperLeftCorner.first;
	}
};

struct FeatureVector {
	// constructors
	FeatureVector() {
		;
	}
	FeatureVector(double hr, double wr, double rr[9]) {
		heightRatio = hr;
		widthRatio = wr;
		for(int i=0; i<9; i++)
			regionRatio[i] = rr[i];
	}
	// methods
	double getLength() {
		double len = 0.0;
		len += (heightRatio * heightRatio);
		len += (widthRatio * widthRatio);
		for(int i=0; i<9; i++)
			len += (regionRatio[i] * regionRatio[i]);
		return sqrt(len);
	}
	double getSimilarity(FeatureVector fv) {
		double dist = 0.0;
		dist += (heightRatio - fv.heightRatio) * (heightRatio - fv.heightRatio);
		dist += (widthRatio - fv.widthRatio) * (widthRatio - fv.widthRatio);
		for(int i=0; i<9; i++)
			dist += (regionRatio[i] - fv.regionRatio[i]) * (regionRatio[i] - fv.regionRatio[i]);
		return 100-dist;
	}
	double getSimilarity2(FeatureVector fv) {
		double dot_product = 0.0;
		dot_product += (heightRatio * fv.heightRatio);
		dot_product += (widthRatio * fv.widthRatio);
		for(int i=0; i<9; i++)
			dot_product += (regionRatio[i] * fv.regionRatio[i]);
		return dot_product / (getLength() * fv.getLength());
	}
	// data
	double heightRatio;
	double widthRatio;
	double regionRatio[9];
};

char errmsg[100]; // global error message for ErrorExit() function
string training_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";

void print_usage();
void ErrorExit();
vector<Component> findConnectedComponents(unsigned char** &im, int height, int width);
FeatureVector* computeFeatureVector(Component component, unsigned char** &im);
double getMean(double arr[], int count);
double getSigma(double arr[], int count);

int main(int argc, char* argv[]) {


	// ========== check argument ========== //

	if(argc != 3) {
		cerr << "wrong arguments!" << endl;
		print_usage();
		exit(-1);
	}


	// ========== Sample1 & TrainingSet: declaration and initialization ========== //

	unsigned char** sample = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	unsigned char** train = (unsigned char**)malloc(248 * sizeof(unsigned char*));
	for(int i=0; i<256; i++)
		sample[i] = (unsigned char*)malloc(256 * sizeof(unsigned char));
	for(int i=0; i<248; i++)
		train[i] = (unsigned char*)malloc(450 * sizeof(unsigned char));
	// Sample1 & TrainingSet: read in
	if(!openImageAsMatrix(argv[1], sample, 256, 256)) {
		sprintf(errmsg, "cannot open image at: %s", argv[1]);
		ErrorExit();
	}
	if(!openImageAsMatrix(argv[2], train, 248, 450)) {
		sprintf(errmsg, "cannot open image at: %s", argv[2]);
		ErrorExit();
	}


	// ========== binarizing Sample1 & TrainingSet ========== //

	binarize(sample, 256, 256, 30); // 30 is a magic number
	binarize(train, 248, 450, 50); // 50 is also a magic number


	// ========== closing Sample1 & TrainingSet (dilation-then-erosion) ========== //

	sample = dilation(sample, 256, 256, 0, 3);
	train = dilation(train, 248, 450, 0, 3);


	// ========== Sample1 & TrainingSet: get the connected componenets ========== //

	// Sample1
	vector<Component> connected_components = findConnectedComponents(sample, 256, 256);
	// TrainingSet
	vector<Component> training_components(70);
	// TrainingSet step 1: split into 5 x 14 = 70 regions (vector of components)
	for(int i=0; i<5; i++) {
		for(int j=0; j<14; j++) {
			training_components[i * 14 + j].upperLeftCorner = make_pair(i*50, j*32);
			if(((i+1) * 50 - 1) >= 248)
				training_components[i * 14 + j].lowerRightCorner = make_pair(247, (j+1)*32-1);
			else
				training_components[i * 14 + j].lowerRightCorner = make_pair((i+1)*50-1, (j+1)*32-1);
		}
	}
	// TrainingSet step 2: refine the training_components to make their bounding boxes tighter
	for(vector<Component>::iterator it = training_components.begin(); it != training_components.end(); it++) {
		// to store the real bounding box
		pair<int, int> bboxUpperLeft = make_pair(-1, -1);
		pair<int, int> bboxLowerRight = make_pair(-1, -1);
		// iterate through all pixels in the region
		for(int r = it->upperLeftCorner.first; r <= it->lowerRightCorner.first; r++) {
			for(int c = it->upperLeftCorner.second; c <= it->lowerRightCorner.second; c++) {
				// ignore subtle pixels
				if(train[r][c] < 50) { // 50 is a magic threshold
					// update upper left
					if(bboxUpperLeft.first == -1 || bboxUpperLeft.first > r)
						bboxUpperLeft.first = r;
					if(bboxUpperLeft.second == -1 || bboxUpperLeft.second > c)
						bboxUpperLeft.second = c;
					// update lower right
					if(bboxLowerRight.first == -1 || bboxLowerRight.first < r)
						bboxLowerRight.first = r;
					if(bboxLowerRight.second == -1 || bboxLowerRight.second < c)
						bboxLowerRight.second = c;
				}
			}
		}
		it->upperLeftCorner = bboxUpperLeft;
		it->lowerRightCorner = bboxLowerRight;
	}


	// ========== write to debug ========== //

	// if(!writeImageToFile("new_sample.raw", sample, 256, 256)) {
	// 	sprintf(errmsg, "cannot write image to file (%s).", "new_sample.raw");
	// 	ErrorExit();
	// }
	// if(!writeImageToFile("new_train.raw", train, 248, 450)) {
	// 	sprintf(errmsg, "cannot write image to file (%s).", "new_train.raw");
	// 	ErrorExit();
	// }
	// exit(0);


	// ========== get the training feature vectors ========== //

	vector<FeatureVector> training_feature_vectors;
	
	for(vector<Component>::iterator it = training_components.begin(); it != training_components.end(); it++) {
		FeatureVector* fv = computeFeatureVector(*it, train);
		training_feature_vectors.push_back(*fv);
	}


	// ========== detemine the connected components in Sample1 ========== //

	for(vector<Component>::iterator it = connected_components.begin(); it != connected_components.end(); it++) {
		FeatureVector* fv = computeFeatureVector(*it, sample);
		// calculate similarity
		int result = 0; // which position in train is this component, will be 0~69
		double similarity = (*fv).getSimilarity2(training_feature_vectors[0]); // cosine similarity will be -1~1, so -2 works as -INF
		for(int i=0; i<training_feature_vectors.size(); i++) {
			double sim = (*fv).getSimilarity2(training_feature_vectors[i]);
			// check if better
			if(sim > similarity) {
				result = i;
				similarity = sim;
			}
		}
		// cerr << "best match for this component: " << training_string[result] << endl;
		// cerr << "(similarity = " << similarity << ")" << endl;
		cerr << training_string[result] << " ";
	}
	cerr << endl;

	// print truth
	// cerr << "Y 9 4 P 9 V 3 7 T 3 H C T 3 B C P V T H 5 B" << endl;


	return 0;
}

void print_usage() {
	cerr << "Usage: ./a.out <Sample1> <TrainingSet>" << endl;
}

void ErrorExit() {
	cerr << "ERROR: " << errmsg << endl;
	exit(-1);
}

// to find connected components in an image
vector<Component> findConnectedComponents(unsigned char** &im, int height, int width) {
	int** component_map = (int**)malloc(height * sizeof(int*)); // to store the connected component number map
	for(int i=0; i<height; i++) {
		component_map[i] = (int*)malloc(width * sizeof(int));
		for(int j=0; j<width; j++)
			component_map[i][j] = 0;
	}
	int component_count = 1; // starts from 1, b/c 0 for background pixels
	while(1) {
		bool change = false;
		// 1st pass: from upper-left to lower-right
		for(int r=0; r<height; r++) {
			for(int c=0; c<width; c++) {
				if(im[r][c] == 255) // not interested (background pixels)
					continue; // same as "component_map[r][c] = 0;" here
				// get up 
				int up = (r-1 < 0) ? 0 : component_map[r-1][c];
				// get left
				int left = (c-1 < 0) ? 0 : component_map[r][c-1];
				// get upLeft
				int upLeft = (r-1 < 0 || c-1 < 0) ? 0 : component_map[r-1][c-1];
				// check if needs setting to "up"
				if(up != 0 && (up < component_map[r][c] || component_map[r][c] == 0)) {
					component_map[r][c] = up;
					change = true;
				}
				// check if needs setting to "left"
				if(left != 0 && (left < component_map[r][c] || component_map[r][c] == 0)) {
					component_map[r][c] = left;
					change = true;
				}
				// check if needs setting to "upLeft"
				if(upLeft != 0 && (upLeft < component_map[r][c] || component_map[r][c] == 0)) {
					component_map[r][c] = upLeft;
					change = true;
				}
				// check if needs setting as new component
				if(component_map[r][c] == 0) {
					component_map[r][c] = component_count++;
					change = true;
				}
			}
		}
		// 2nd pass: from lower-right to upper-left
		for(int r = height - 1; r >= 0; r--) {
			for(int c = width - 1; c >= 0; c--) {
				if(im[r][c] == 255) // not interested (background pixels)
					continue;
				// get down
				int down = (r+1 >= height) ? 0 : component_map[r+1][c];
				// get right
				int right = (c+1 >= width) ? 0 : component_map[r][c+1];
				// get downRight
				int downRight = (r+1 >= height || c+1 >= width) ? 0 : component_map[r+1][c+1];
				// check if needs setting to "down"
				if(down != 0 && (down < component_map[r][c] || component_map[r][c] == 0)) {
					component_map[r][c] = down;
					change = true;
				}
				// check if needs setting to "right"
				if(right != 0 && (right < component_map[r][c] || component_map[r][c] == 0)) {
					component_map[r][c] = right;
					change = true;
				}
				// check if needs setting as new component
				if(component_map[r][c] == 0) {
					component_map[r][c] = component_count++;
					change = true;
				}
			}
		}
		// break condition: if nothing is changed
		if(!change)
			break;
	}
	// suppress the component count
	int real_count = 1;
	int* mapping = (int*)malloc(component_count * sizeof(int));
	for(int i=0; i<component_count; i++)
		mapping[i] = 0; // initialize the mapping to all 0's
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			// check if it's foreground
			if(component_map[r][c] != 0) {
				// check if it's not mapped
				if(mapping[component_map[r][c]] == 0)
					mapping[component_map[r][c]] = real_count++;
			}
		}
	}
	// debug output
	// for(int i=0; i<component_count; i++) {
	// 	cerr << "map[" << i << "]: " << mapping[i] << endl;
	// }
	// construct the vector to return
	vector<Component> ret(real_count);
	for(int r=0; r<height; r++) {
		for(int c=0; c<width; c++) {
			if(component_map[r][c] != 0) {
				// update upperLeftCorner if necessary
				if(ret[mapping[component_map[r][c]]].upperLeftCorner.first == -1 || r < ret[mapping[component_map[r][c]]].upperLeftCorner.first)
					ret[mapping[component_map[r][c]]].upperLeftCorner.first = r;
				if(ret[mapping[component_map[r][c]]].upperLeftCorner.second == -1 || c < ret[mapping[component_map[r][c]]].upperLeftCorner.second)
					ret[mapping[component_map[r][c]]].upperLeftCorner.second = c;
				// update lowerRightCorner if necessary
				if(ret[mapping[component_map[r][c]]].lowerRightCorner.first == -1 || r > ret[mapping[component_map[r][c]]].lowerRightCorner.first)
					ret[mapping[component_map[r][c]]].lowerRightCorner.first = r;
				if(ret[mapping[component_map[r][c]]].lowerRightCorner.second == -1 || c > ret[mapping[component_map[r][c]]].lowerRightCorner.second)
					ret[mapping[component_map[r][c]]].lowerRightCorner.second = c;
			}
		}
	}
	ret.erase(ret.begin());
	return ret;
}

// to find feature vector for a Component instance
FeatureVector* computeFeatureVector(Component component, unsigned char** &im) {
	// height ratio / width ratio
	double height = (double)component.getHeight();
	double width = (double)component.getWidth();
	double h_ratio = height / (height + width);
	double w_ratio = width / (height + width);

	// euler number: applying Gray's Algorithm (bit quad patterns)
	// (a little bit brute-force hardcore coding here...)
	int countQuads[6] = {0, 0, 0, 0, 0, 0}; // Q0, Q1, Q2, Q3, Q4, QD
	// iterate through every pixel in this component
	for(int i=component.upperLeftCorner.first; i <= component.lowerRightCorner.first; i++) {
		for(int j=component.upperLeftCorner.second; j <= component.lowerRightCorner.second; j++) {
			// the quad will be (i, j) & (i+1, j) & (i, j+1) & (i+1, j+1)
			// check if i+1 and j+1 are both in range
			if(i + 1 <= component.lowerRightCorner.first && j + 1 <= component.lowerRightCorner.second) {
				// count the number of foreground, i.e., not white (< 255)
				int total_count = 0;
				total_count += (im[i][j] < 255) ? 1 : 0; // += 1 if foreground
				total_count += (im[i+1][j] < 255) ? 1 : 0;
				total_count += (im[i][j+1] < 255) ? 1 : 0;
				total_count += (im[i+1][j+1] < 255) ? 1 : 0;
				// determine which kinds of quad
				if(total_count == 4) {
					// it's Q4
					countQuads[4] += 1;
				} else if(total_count == 3) {
					// it's Q3
					countQuads[3] += 1;
				} else if(total_count == 2) {
					// it's QD or Q2
					if((im[i][j] < 255 && im[i+1][j+1] < 255)
						|| (im[i+1][j] < 255 && im[i][j+1] < 255)) {
						// it's QD
						countQuads[5] += 1;
					} else {
						// it's Q2
						countQuads[2] += 1;
					}
				} else if(total_count == 1) {
					// it's Q1
					countQuads[1] += 1;
				} else if(total_count == 0) {
					// it's Q0
					countQuads[0] += 1;
				} else {
					// shouldn't be here!
					sprintf(errmsg, "computeFeatureVector(): unknown total_count = %d", total_count);
					ErrorExit();
				}
			}
		}
	}
	double euler = (countQuads[1] - countQuads[3] - 2.0 * countQuads[5]) / 4.0; // 8-connected
	// if 4-connected: double euler = (countQuads[1] - countQuads[3] + 2.0 * countQuads[5]) / 4.0;
	// cerr << "bitQuad values = " << countQuads[0] << " " << countQuads[1] << " " << countQuads[2] << " ";
	// cerr << countQuads[3] << " " << countQuads[4] << " " << countQuads[5] << endl;
	// cerr << "euler number = " << euler << endl;

	// spatial moments
	// compute raw moments first: M00, M01, M10, M11, M20, M02, M21, M12, M30, M03
	double raw_moments[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	for(int i=component.upperLeftCorner.first; i <= component.lowerRightCorner.first; i++) {
		for(int j=component.upperLeftCorner.second; j <= component.lowerRightCorner.second; j++) {
			// check foreground
			if(im[i][j] < 255) {
				raw_moments[0] += 1.0; // M00
				raw_moments[1] += (double)(j); // M01
				raw_moments[2] += (double)(i); // M10
				raw_moments[3] += (double)(i * j); // M11
				raw_moments[4] += (double)(i*i); // M20
				raw_moments[5] += (double)(j*j); // M02
				raw_moments[6] += (double)(i*i * j); // M21
				raw_moments[7] += (double)(i * j*j); // M12
				raw_moments[8] += (double)(i*i*i); // M30
				raw_moments[9] += (double)(j*j*j); // M03
			}
		}
	}
	// compute centroid: c = (M10 / M00, M01 / M00)
	double centroid[2] = {raw_moments[2] / raw_moments[0], raw_moments[1] / raw_moments[0]};
	// copmute central moments: m00, m01, m10, m11, m20, m02, m21, m12, m30, m03
	double central_moments[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	central_moments[0] = raw_moments[0]; // m00
	central_moments[1] = 0.0; // m01
	central_moments[2] = 0.0; // m10
	central_moments[3] = raw_moments[3] - centroid[0] * raw_moments[1]; // m11
	central_moments[4] = raw_moments[4] - centroid[0] * raw_moments[2]; // m20
	central_moments[5] = raw_moments[5] - centroid[1] * raw_moments[1]; // m02
	central_moments[6] = raw_moments[6] - 2.0 * centroid[0] * raw_moments[3] - centroid[1] * raw_moments[4] + 2.0 * centroid[0] * centroid[0] * raw_moments[1]; // m21
	central_moments[7] = raw_moments[7] - 2.0 * centroid[1] * raw_moments[3] - centroid[0] * raw_moments[5] + 2.0 * centroid[1] * centroid[1] * raw_moments[2]; // m12
	central_moments[8] = raw_moments[8] - 3.0 * centroid[0] * raw_moments[4] + 2.0 * centroid[0] * centroid[0] * raw_moments[2]; // m30
	central_moments[9] = raw_moments[9] - 3.0 * centroid[1] * raw_moments[5] + 2.0 * centroid[1] * centroid[1] * raw_moments[1]; // m03
	char debug_buf[100];
	sprintf(debug_buf, "%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf %lf", central_moments[0],
		central_moments[1],
		central_moments[2],
		central_moments[3],
		central_moments[4],
		central_moments[5],
		central_moments[6],
		central_moments[7],
		central_moments[8],
		central_moments[9]);
	// cerr << "central_moments complete: " << debug_buf << endl;

	// cerr << "start computing rr..." << endl;
	// region ratio: rr1 ~ rr25
	double rr[9];
	for(int i=0; i<9; i++) rr[i] = 0.0;
	for(int i=component.upperLeftCorner.first; i <= component.lowerRightCorner.first; i++) {
		for(int j=component.upperLeftCorner.second; j <= component.lowerRightCorner.second; j++) {
			// detemine which_rr is the coordinate (i, j)
			/*
			0  1  2  3  4
			5  6  7  8  9
			10 11 12 13 14
			15 16 17 18 19
			20 21 22 23 24
			*/
			int which_rr = ((i - component.upperLeftCorner.first) / (component.getHeight()/3)) * 3 + ((j - component.upperLeftCorner.second) / (component.getWidth() / 3));
			// accumulate the count if foreground
			if(im[i][j] < 255) {
				rr[which_rr] += 1.0;
			}
		}
	}
	// normalize rr, i.e., divided by number of pixels
	for(int i=0; i<9; i++)
		rr[i] = rr[i] / (component.getHeight() * component.getWidth() / 9);
	// double mean = getMean(rr, 25);
	// double sigma = getSigma(rr, 25);
	// for(int i=0; i<25; i++)
	// 	rr[i] = (rr[i] - mean) / sigma;
	// cerr << "finish computing rr..." << endl;
	// sprintf(debug_buf, "%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf", rr[0], rr[1], rr[2], rr[3], rr[4], rr[5], rr[6], rr[7], rr[8]);
	// cerr << "region ratio: " << debug_buf << endl;
	// create new FeatureVector and return
	return new FeatureVector(h_ratio, w_ratio, rr);
}

double getMean(double arr[], int count) {
	double sum = 0.0;
	for(int i=0; i<count; i++)
		sum += arr[i];
	return (sum / count);
}

double getSigma(double arr[], int count) {
	double mean = getMean(arr, count);
	double sum = 0.0; // sum of squared value of diff from mean
	for(int i=0; i<count; i++)
		sum += (arr[i] - mean) * (arr[i] - mean);
	return sqrt(sum / count);
}

