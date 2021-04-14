#ifndef __UTILS_H_
#define __UTILS_H_

constexpr auto M_PI = 3.14159265358979323846;

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


// Affine transform
float interp2D(float delta[2], float f[4], string interp_type="bilinear");
Mat warpAffine(Mat src, Mat affmat, string interp_type="bilinear");
Mat resize(Mat src, Size size, string interp_type="bilinear");
Mat rotate(Mat src, float radian, string interp_type="bilinear");
#endif