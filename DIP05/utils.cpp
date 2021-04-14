#include "utils.h"
#include <math.h>

//±∏«ˆ1 BGR-> HSI
Mat cvtBGR2HSI(Mat src) {
	float eta = 1E-8;
	const float pi = 3.141592;
	float B, R, G;
	float theta, Temp;
	Mat hsi[3];
	hsi[0] = Mat::zeros(src.size(), CV_32F);
	hsi[1] = Mat::zeros(src.size(), CV_32F);
	hsi[2] = Mat::zeros(src.size(), CV_32F);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			B = src.at<Vec3f>(i, j)[0];
			G = src.at<Vec3f>(i, j)[1];
			R = src.at<Vec3f>(i, j)[2];
			Temp = sqrt((pow((R - G), 2) + (R - B) * (G - B)));
			if (Temp == 0) {
				Temp = eta;
			}
			Temp = ((R - G) + (R - B)) / (2*Temp);
			theta =	acos(Temp)/(2*pi);
			if (B > G) {
				theta = 1- theta;
			}
			hsi[0].at<float>(i, j) = theta;
			hsi[1].at<float>(i, j) = 1 - (3 * min({ B, G, R }) / (B + G + R));
			hsi[2].at<float>(i, j) = (B + G + R) / 3;
		}
	}
	Mat dst;
	merge(hsi, 3, dst);
	return dst;

}


Mat cvtHSI2BGR(Mat src) {
	float eta = 1E-8;
	const float pi = 3.141592;
	float H,S,I, B, G, R;
	float theta, Temp;
	Mat BGR[3];
	BGR[0] = Mat::zeros(src.size(), CV_32F);//B
	BGR[1] = Mat::zeros(src.size(), CV_32F);//G
	BGR[2] = Mat::zeros(src.size(), CV_32F);//R
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {

			H = src.at<Vec3f>(i, j)[0];
			S = src.at<Vec3f>(i, j)[1];
			I = src.at<Vec3f>(i, j)[2];
			H = 2 * pi * H;
			if ((H >= 0) && (H < (2 * pi / 3))) {
				B = I * (1 - S);
				R = I * (1 + ((S * cos(H)) / cos((pi / 3) - H)));
				G = 3 * I - (B + R);

			}
			else if ((H >= (2 * pi / 3)) && (H < (4 * pi / 3))) {
				H = H - (2 * pi / 3);
				R = I * (1 - S);
				G = I * (1 + ((S * cos(H)) / cos((pi / 3) - H)));
				B = 3 * I - (R+G);
			}
			else if (H >= (4 * pi / 3)) {
				H = H - (4 * pi / 3);
				G = I * (1 - S);
				B = I * (1 + ((S * cos(H)) / cos((pi / 3) - H)));
				R = 3 * I - (G+B);
			}
			BGR[0].at<float>(i, j) = B;
			BGR[1].at<float>(i, j) = G;
			BGR[2].at<float>(i, j) = R;
		}
	}
	Mat dst;
	merge(BGR, 3, dst);
	return dst;
}

Mat imFilter(Mat src, Mat kernel, string pad_type) {//3¿Â imfilter
	int H = src.rows;
	int W = src.cols;
	int K_H = (kernel.rows - 1) / 2;
	int K_W = (kernel.cols - 1) / 2;
	float Temp;
	Mat src_pad(H + 2 * K_H, W + 2 * K_W, CV_32F, Scalar(0));//padding Matrix
	//padding
	if (pad_type == "zero") {			//zero padding
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				src_pad.at<float>(i + K_H, j + K_W) = src.at<float>(i, j);
			}
		}
	}
	else if (pad_type == "mirror") {	//mirror padding
		//original image location
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				src_pad.at<float>(i + K_H, j + K_W) = src.at<float>(i, j);
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
	Mat dst(H, W, CV_32F, Scalar(0));
	Temp = 0;
	//convolution
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			for (int m = 0; m < 2 * K_H + 1; m++) {
				for (int n = 0; n < 2 * K_W + 1; n++) {
					Temp += kernel.at<float>(m, n) * src_pad.at<float>(i + m, j + n);
				}
			}
			dst.at<float>(i, j) = Temp;
			Temp = 0;
		}
	}
	return dst;
}
Mat sobelFilter(Mat src, float direction) {
	Mat sobel_X = Mat_<float>({ 3,3 }, { -1,0,1,-2,0,2,-1,0,1 });
	Mat sobel_Y = Mat_<float>({ 3,3 }, { -1,-2,-1,0,0,0,1,2,1 });
	Mat dst;
	if (direction == 0) {
		dst = imFilter(src, sobel_X, "mirror");
	}
	else if (direction == 1) {
		dst = imFilter(src, sobel_Y, "mirror");
	}
	return dst;
}