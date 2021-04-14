#include "utils.h"
#include <math.h>
Mat intensityTransform(Mat src, float LUT[256]) {
	int H = src.rows;
	int W = src.cols;
	int temp;
	Mat dst(H, W, CV_8UC1, Scalar(0));
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			temp = src.at<uchar>(i, j);
			dst.at<uchar>(i, j) = (int)LUT[temp];
		}
	}
	return dst;
}

Mat gammaTransform(Mat src, float gamma){
	float LUT[256];
	for (int i = 0; i < 256; i++) {
		LUT[i] = powf((float)i/255, gamma) * 255;
	}
	Mat dst = intensityTransform(src, LUT);
	return dst;
}
float* getHist(Mat src){
	float Hist[256] = {0};
	int temp;
	int H = src.rows;
	int W = src.cols;
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			temp = src.at<uchar>(i, j);
			Hist[temp] += 1;
		}
	}
	for (int i = 0; i < 256; i++) {
		Hist[i] = Hist[i] / (H * W);
	}
	return Hist;
}
Mat histEqualization(Mat src){
	float LUT[256];
	float Temp = 0;
	float* Hist = getHist(src);
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			Temp += 255 * Hist[j];
		}
		LUT[i] = (int)Temp;
		Temp = 0;
	}
	Mat dst = intensityTransform(src, LUT);
	return dst;
}
Mat boxFilter(Size size){
	Mat Box = Mat::ones(size, CV_32F);
	Box = Box / (float)(Box.rows * Box.cols);
	return Box;
}
Mat imFilter(Mat src, Mat kernel, string pad_type){
	int H = src.rows;
	int W = src.cols;
	int K_H = (kernel.rows-1)/2;
	int K_W = (kernel.cols-1)/2;
	float Temp;
	Mat src_pad(H + 2*K_H, W + 2*K_W, CV_8UC1, Scalar(0));//padding Matrix
	//padding
	if (pad_type == "zero"){			//zero padding
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				src_pad.at<uchar>(i + K_H, j + K_W) = src.at<uchar>(i, j);
			}
		}
	}
	else if (pad_type == "mirror") {	//mirror padding
		//original image location
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				src_pad.at<uchar>(i + K_H, j + K_W) = src.at<uchar>(i, j);
			}
		}
		//mirroring
		for (int i = 0; i < K_H; i++) {
			src_pad.row(2 * K_H - i).copyTo(src_pad.row(i));
		}
		for (int i = 0; i < K_H; i++) {
			src_pad.row(src_pad.rows - 1 - 2 * K_H + i).copyTo(src_pad.row(src_pad.rows - 1 - i));
		}
		for (int j = 0; j < K_W; j++) {
			src_pad.col(2 * K_W - j).copyTo(src_pad.col(j));
		}
		for (int j = 0; j < K_W; j++) {
			src_pad.col(src_pad.cols - 1 - 2 * K_W + j).copyTo(src_pad.col(src_pad.cols - 1 - j));
		}
	}
	src_pad.convertTo(src_pad, CV_32F);
	Mat dst(H, W, CV_8UC1, Scalar(0));
	Temp = 0;
	//convolution
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			for (int m = 0; m < 2 * K_H + 1; m++) {
				for (int n = 0; n < 2 * K_W + 1; n++) {
					Temp += kernel.at<float>(m, n) * src_pad.at<float>(i + m, j + n);
				}
			}
			dst.at<uchar>(i, j) = Temp;
			Temp = 0;
		}
	}
	dst.convertTo(dst, CV_32F);
	return dst;
}