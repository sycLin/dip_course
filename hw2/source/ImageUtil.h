/**
 * This is an utility file for funtions for image processing.
 * Author: Steven YuChih Lin.
 * Created: 10th April, 2016.
 */

#include <iostream>
#include <math.h> // for logarithm / exponential / floor / power
#include <random> // for random
#include <time.h> // for time()
#include <queue>

using namespace std;

bool openImageAsMatrix(char* path, unsigned char** ptr, int height, int width);

bool writeImageToFile(char* path, unsigned char** data, int height, int width);

bool median_filter(unsigned char** data, unsigned char** res, int height, int width, int bheight, int bwidth);

bool low_pass_filter(unsigned char** data, unsigned char** res, int height, int width, double b);

double getSNR(unsigned char** im1, unsigned char** im2, int height, int width);

bool doSobelEdgeDetector(unsigned char** im, unsigned char** edge_im, int height, int width, double threshold);

bool doCannyEdgeDetector(unsigned char** im, unsigned char** edge_im, int height, int width, double TH, double TL);

bool doDifferenceOfGaussian(unsigned char** im, unsigned char** edge_im, int height, int width, int threshold);