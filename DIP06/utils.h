#ifndef __UTILS_H_
#define __UTILS_H_

#include <iostream>
#include <math.h>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


// tool_dft.cpp
Mat fft2d(Mat src, int flags = DFT_COMPLEX_OUTPUT);
void fft2d(Mat src, Mat& dst, int flags = DFT_COMPLEX_OUTPUT);
Mat ifft2d(Mat src, int flags = DFT_INVERSE | DFT_SCALE | DFT_REAL_OUTPUT);
void ifft2d(Mat src, Mat& dst, int flags = DFT_INVERSE | DFT_SCALE | DFT_REAL_OUTPUT);

Mat fftshift2d(Mat src);
void fftshift2d(Mat src, Mat& dst);
Mat ifftshift2d(Mat src);
void ifftshift2d(Mat src, Mat& dst);

// tool_spatial_transform.cpp
Mat IntensityTransform_LUT(Mat src, float LUT[256]);
Mat ImageNagative(Mat src);
Mat LogTrnaform(Mat src, float c = 1.0);
Mat GammaTransform(Mat src, float g, float c = 1.0);
Mat affineTansform(Mat src, Mat affine, int inter_mode = 1);
/*
inter_mode = 0 --> nearest interpolation
inter_mode = 1 --> bilinear interpolation
*/
float interpolation2D(float delta_x, float delta_y, float data_lo_lo, float data_lo_hi, float data_hi_lo, float data_hi_hi, int interp_mode = 1);
Mat Rotation_Aff(Mat src, float radian);
Mat Resize_aff(Mat src, Size size);

//tool_spat_domain_filter.cpp
Mat Box_filtering(Mat src, Size size, int pad_flag = 0);
Mat Gaussian_filtering(Mat src, Size size, float sigma = 1.0, int pad_flag = 0);
Mat Laplacian_filtering(Mat src, int sel_ker = 0, int pad_flag = 0);
//sel_ker = 0  ==> 4 direction edge detection
//sel_ker = 1  ==> 8 direction edge detection
Mat sobel_filtering(Mat src, int sel_ker = 2, int pad_flag = 0);
//sel_ker = 0  ==> x axis
//sel_ker = 1  ==> y axis
//sel_ker = 2  ==> xy axis(sum)
Mat Lin_spat_filter(Mat src, Mat kernel, int pad_tpye_flag);
Mat Adap_med_filter(Mat src, Size max_ker);
Mat unsharp(Mat src, string filter, Size size);
Mat high_boost(Mat src, int k, string filter, Size size);


//tool_freq_domain_filter.cpp
Mat Ideal_box_filter(Mat src, string filter_type, float cuts_off);
Mat Gaussian_filter(Mat src, string filter_type, float cuts_off);
Mat Butterworth_filter(Mat src, string filter_type, float n, float cuts_off);
Mat Freq_filter(Mat src, Mat kernel);
Mat Freq_filter_2(Mat src, Mat kernel);
Mat Notch_filter(Mat src, string filter_type, int u, int k, float cuts_off, float n = 2.0);
Mat show_mag(Mat src);

 
// wavelet.cpp
void pyr_Down(Mat src, Mat& appr, Mat& residu);
Mat waveletTransform2D(Mat src, Mat proto_type);
Mat waveletTransform2D_visual(Mat src, Mat proto_type);


Mat dewaveletsyn_1(Mat dst, Mat proto_type);
Mat dewaveletsyn(Mat dst, Mat proto_type, int LV);

//tool_image_compress.cpp
Mat huffman(Mat src);

#endif