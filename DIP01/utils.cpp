#include "utils.h"
#include <cmath>



float interp2D(float delta[2], float f[4], string interp_type) {
	float a = 0;
	if (interp_type == "bilinear") {
		a = f[3] * delta[0] * delta[1] + f[2] * (1 - delta[0]) * delta[1] + f[1] * delta[0] * (1 - delta[1]) + f[0] * (1 - delta[0]) * (1 - delta[1]);
	}
	else if (interp_type == "nearest") {
		if (delta[0] < 0.5) {
			if (delta[1] < 0.5) {
				a = f[0];
			}
			else {
				a = f[2];
			}
		}
		else {
			if (delta[1] < 0.5) {
				a = f[1];
			}
			else {
				a = f[3];
			}
		}
	}
	return a;
}

Mat warpAffine(Mat src, Mat affine, string interp_type) {
	float* aff_data = (float*)affine.data;
	float Size_X, Size_Y;
	float X_temp, Y_temp;
	float delta[2], f[4];
	float vertex_x1, vertex_x2, vertex_x3, vertex_x4, vertex_y1, vertex_y2, vertex_y3, vertex_y4;
	float det;
	float rev_aff_data[9] = { 1,1,1,1,1,1,1,1,1 };
	int H = src.rows;
	int W = src.cols;
	float temp;


	vertex_x1 = ((aff_data[0] * 0) + (aff_data[1] * 0) + aff_data[2]) / ((aff_data[6] * 0) + (aff_data[7] * 0) + aff_data[8]);
	vertex_x2 = ((aff_data[0] * H) + (aff_data[1] * 0) + aff_data[2]) / ((aff_data[6] * H) + (aff_data[7] * 0) + aff_data[8]);
	vertex_x3 = ((aff_data[0] * 0) + (aff_data[1] * W) + aff_data[2]) / ((aff_data[6] * 0) + (aff_data[7] * W) + aff_data[8]);
	vertex_x4 = ((aff_data[0] * H) + (aff_data[1] * W) + aff_data[2]) / ((aff_data[6] * H) + (aff_data[7] * W) + aff_data[8]);
	vertex_y1 = ((aff_data[3] * 0) + (aff_data[4] * 0) + aff_data[5]) / ((aff_data[6] * 0) + (aff_data[7] * 0) + aff_data[8]);
	vertex_y2 = ((aff_data[3] * H) + (aff_data[4] * 0) + aff_data[5]) / ((aff_data[6] * H) + (aff_data[7] * 0) + aff_data[8]);
	vertex_y3 = ((aff_data[3] * 0) + (aff_data[4] * W) + aff_data[5]) / ((aff_data[6] * 0) + (aff_data[7] * W) + aff_data[8]);
	vertex_y4 = ((aff_data[3] * H) + (aff_data[4] * W) + aff_data[5]) / ((aff_data[6] * H) + (aff_data[7] * W) + aff_data[8]);
	Size_X = (int)MAX(MAX(vertex_x1, vertex_x2), MAX(vertex_x3, vertex_x4));
	Size_Y = (int)MAX(MAX(vertex_y1, vertex_y2), MAX(vertex_y3, vertex_y4));
	Mat dst(Size_X, Size_Y, CV_8UC1, Scalar(0));
	det = aff_data[0] * aff_data[4] * aff_data[8] + aff_data[1] * aff_data[5] * aff_data[6] + aff_data[2] * aff_data[3] * aff_data[7] - aff_data[7] * aff_data[0] * aff_data[5] - aff_data[8] * aff_data[1] * aff_data[3] - aff_data[6] * aff_data[2] * aff_data[4];
	rev_aff_data[0] = (aff_data[4] * aff_data[8] - aff_data[5] * aff_data[7]) / det;
	rev_aff_data[1] = (aff_data[2] * aff_data[7] - aff_data[1] * aff_data[8]) / det;
	rev_aff_data[2] = (aff_data[1] * aff_data[5] - aff_data[2] * aff_data[4]) / det;
	rev_aff_data[3] = (aff_data[5] * aff_data[6] - aff_data[3] * aff_data[8]) / det;
	rev_aff_data[4] = (aff_data[0] * aff_data[8] - aff_data[2] * aff_data[6]) / det;
	rev_aff_data[5] = (aff_data[2] * aff_data[3] - aff_data[0] * aff_data[5]) / det;
	rev_aff_data[6] = (aff_data[3] * aff_data[7] - aff_data[4] * aff_data[6]) / det;
	rev_aff_data[7] = (aff_data[1] * aff_data[6] - aff_data[0] * aff_data[7]) / det;
	rev_aff_data[8] = (aff_data[0] * aff_data[4] - aff_data[1] * aff_data[3]) / det;
	for (int i = 0; i < Size_X - 1; i++) {
		for (int j = 0; j < Size_Y - 1; j++) {
			X_temp = ((rev_aff_data[0] * i) + (rev_aff_data[1] * j) + rev_aff_data[2]) / ((rev_aff_data[6] * i) + (rev_aff_data[7] * j) + rev_aff_data[8]);
			if (X_temp < 0) {
				continue;
			}
			if (X_temp > H - 1) {
				continue;
			}
			Y_temp = ((rev_aff_data[3] * i) + (rev_aff_data[4] * j) + rev_aff_data[5]) / ((rev_aff_data[6] * i) + (rev_aff_data[7] * j) + rev_aff_data[8]);
			if (Y_temp < 0) {
				continue;
			}
			if (Y_temp > W - 1) {
				continue;
			}
			delta[0] = X_temp - trunc(X_temp);
			delta[1] = Y_temp - trunc(Y_temp);
			f[0] = src.at<uchar>(trunc(X_temp), trunc(Y_temp));
			f[1] = src.at<uchar>(trunc(X_temp) + 1, trunc(Y_temp));
			f[2] = src.at<uchar>(trunc(X_temp), trunc(Y_temp) + 1);
			f[3] = src.at<uchar>(trunc(X_temp) + 1, trunc(Y_temp) + 1);
			temp = interp2D(delta, f, interp_type);
			dst.at<uchar>(i, j) = temp;
		}
	}
	return dst;
}

Mat resize(Mat src, Size size, string interp_type) {
	float x_ratio, y_ratio;
	int H = src.rows;
	int W = src.cols;
	x_ratio = ((float)size.height) / H;
	y_ratio = ((float)size.width) / W;
	Mat affine = Mat::zeros(Size(3, 3), CV_32F);
	float* aff_data = (float*)affine.data;
	aff_data[0] = x_ratio;	aff_data[1] = 0.0;	aff_data[2] = 0.0;
	aff_data[3] = 0.0;	aff_data[4] = y_ratio;	aff_data[5] = 0.0;
	aff_data[6] = 0.0;	aff_data[7] = 0.0;	aff_data[8] = 1.0;
	Mat dst = warpAffine(src, affine, interp_type);
	return dst;
}

Mat rotate(Mat src, float radian, string interp_type) {//HW01.cpp 에서 입력값이 radian 대신 cos(radian)값을 제공하여 이를 기준으로함
	float cosine = radian;
	float sine = sqrt(1 - pow(cosine, 2));
	float Size_X, Size_Y, MIN_X, MIN_Y;
	float vertex_x1, vertex_x2, vertex_x3, vertex_x4, vertex_y1, vertex_y2, vertex_y3, vertex_y4;
	int H = src.rows;
	int W = src.cols;
	//float H = src.rows;
	//float W = src.cols;

	Mat affine = Mat::zeros(Size(3, 3), CV_32F);
	float* aff_data = (float*)affine.data;
	aff_data[0] = cosine;	aff_data[1] = -sine;	aff_data[2] = 0.0;
	aff_data[3] = sine;	aff_data[4] = cosine;	aff_data[5] = 0.0;
	aff_data[6] = 0.0;	aff_data[7] = 0.0;	aff_data[8] = 1.0;

	vertex_x1 = ((aff_data[0] * 0) + (aff_data[1] * 0) + aff_data[2]) / ((aff_data[6] * 0) + (aff_data[7] * 0) + aff_data[8]);
	vertex_x2 = ((aff_data[0] * H) + (aff_data[1] * 0) + aff_data[2]) / ((aff_data[6] * H) + (aff_data[7] * 0) + aff_data[8]);
	vertex_x3 = ((aff_data[0] * 0) + (aff_data[1] * W) + aff_data[2]) / ((aff_data[6] * 0) + (aff_data[7] * W) + aff_data[8]);
	vertex_x4 = ((aff_data[0] * H) + (aff_data[1] * W) + aff_data[2]) / ((aff_data[6] * H) + (aff_data[7] * W) + aff_data[8]);
	vertex_y1 = ((aff_data[3] * 0) + (aff_data[4] * 0) + aff_data[5]) / ((aff_data[6] * 0) + (aff_data[7] * 0) + aff_data[8]);
	vertex_y2 = ((aff_data[3] * H) + (aff_data[4] * 0) + aff_data[5]) / ((aff_data[6] * H) + (aff_data[7] * 0) + aff_data[8]);
	vertex_y3 = ((aff_data[3] * 0) + (aff_data[4] * W) + aff_data[5]) / ((aff_data[6] * 0) + (aff_data[7] * W) + aff_data[8]);
	vertex_y4 = ((aff_data[3] * H) + (aff_data[4] * W) + aff_data[5]) / ((aff_data[6] * H) + (aff_data[7] * W) + aff_data[8]);
	Size_X = (int)MAX(MAX(vertex_x1, vertex_x2), MAX(vertex_x3, vertex_x4));
	Size_Y = (int)MAX(MAX(vertex_y1, vertex_y2), MAX(vertex_y3, vertex_y4));
	MIN_X = (int)MIN(MIN(vertex_x1, vertex_x2), MIN(vertex_x3, vertex_x4));
	if (MIN_X < 0) {
		aff_data[2] = aff_data[2] - MIN_X;
	}
	MIN_Y = (int)MIN(MIN(vertex_y1, vertex_y2), MIN(vertex_y3, vertex_y4));
	if (MIN_Y < 0) {
		aff_data[5] = aff_data[5] - MIN_Y;
	}
	Mat dst = warpAffine(src, affine, interp_type);
	return dst;
}

