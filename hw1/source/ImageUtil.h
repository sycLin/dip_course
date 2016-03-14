/**
 * This is an utility file for funtions for image processing.
 * Author: Steven YuChih Lin.
 * Created: 8th March, 2016.
 */

#include <iostream>
#include <math.h> // for logarithm / exponential / floor / power

using namespace std;

bool openImageAsMatrix(char* path, unsigned char** ptr, int height, int width);

bool writeImageToFile(char* path, unsigned char** data, int height, int width);

bool drawHistogram(unsigned char** data, unsigned char** res, int height, int width);

bool hist_eq(unsigned char** data, unsigned char** res, int height, int width);

bool local_hist_eq(unsigned char** data, unsigned char** res, int height, int width, int window_h, int window_w);

bool log_transform(unsigned char** data, unsigned char** res, int height, int width);

bool inverse_log_transform(unsigned char** data, unsigned char** res, int height, int width);

bool power_law_transform(unsigned char** data, unsigned char** res, int height, int width, double gamma);

