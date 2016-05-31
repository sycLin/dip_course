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

void binarize(unsigned char** data, double threshold);

unsigned char** dilation(unsigned char** data, int height, int width, int foreground, int mask_size);

unsigned char** erosion(unsigned char** data, int height, int width, int foreground, int mask_size);

unsigned char** extractBoundary(unsigned char** data, int height, int width, int foreground);

unsigned char** imageUnion(unsigned char** data1, unsigned char** data2, int height, int width, int foreground);

unsigned char** imageDifference(unsigned char** data1, unsigned char** data2, int height, int width, int foreground);

void computeDFT(unsigned char** data, int height, int width, double** &real_part, double** &im_part);

unsigned char** inverseDFT(double** real_part, double** im_part, int height, int width);
