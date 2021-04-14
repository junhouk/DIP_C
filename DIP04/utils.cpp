#include "utils.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>

/*
Define Random number gendertor function
*/
static int strm = 1;	//index of current stream
#define A 16807L		//multiplier (7**5) for ranf function
#define M 2147483647L	//modulus (2**31 -1) for ranf function
static long SeedTable[16] = {
			 0L, 1973272912L,  747177549L,   20464843L,
	 640830765L, 1098742207L,   78126602L,   84743774L,
	 831312807L,  124667236L, 1172177002L, 1124933064L,
	1223960546L, 1878892440L, 1449793615L,  553303732L
};


double ranf(void) {
	short* p, * q, k;
	long Hi, Lo;
	p = (short*)&SeedTable[strm];
	Hi = *(p + 1) * A;
	*(p + 1) = 0;
	Lo = SeedTable[strm] * A;
	p = (short*)&Lo;
	Hi += *(p + 1);
	q = (short*)&Hi;
	*(p + 1) = *q & 0X7FFF;
	k = *(q + 1) << 1;
	if (*q & 0X8000) k++;
	Lo -= M;
	Lo += k;
	if (Lo < 0) Lo += M;
	SeedTable[strm] = Lo;
	return ((double)Lo * 4.656612875E-10);
}

Mat addSaltPepper(Mat src, float ps , float pp, float salt_val, float pepp_val) {
	int H = src.rows;
	int W = src.cols;
	double R_tmp;
	src.convertTo(src, CV_32F);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			R_tmp = ranf();
			if (R_tmp < ps) {
				src.at<float>(i, j) = salt_val;
			}
			else if(R_tmp > (1 - pp)) {
				src.at<float>(i, j) = pepp_val;
			}
		}
	}
	src.convertTo(src, CV_8UC1);
	return src;
}

Mat contraharmonic(Mat src, Size kernel_size, float Q) {
	int H = src.rows;
	int W = src.cols;
	int K_H = (kernel_size.height - 1) / 2;
	int K_W = (kernel_size.width - 1) / 2;
	float Temp_num, Temp_den, Temp;
	Mat src_pad(H + 2 * K_H, W + 2 * K_W, CV_8UC1, Scalar(0));//padding Matrix
	//zero padding
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			src_pad.at<uchar>(i + K_H, j + K_W) = src.at<uchar>(i, j);
		}
	}
	Mat dst(H, W, CV_32F, Scalar(0));
	src_pad.convertTo(src_pad, CV_32F);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Temp_num = 0;
			Temp_den = 0;
			for (int m = 0; m < 2 * K_H + 1; m++) {
				for (int n = 0; n < 2 * K_W + 1; n++) {
					Temp = src_pad.at<float>(i + m, j + n);
					Temp_num += pow(Temp, (Q + 1));
					Temp_den += pow(Temp, Q);
				}
			}
			dst.at<float>(i, j) = Temp_num / Temp_den;
		}
	}
	dst.convertTo(dst, CV_8UC1);
	return dst;
}


Mat median(Mat src, Size kernel_size) {
	int H = src.rows;
	int W = src.cols;
	int K_H = (kernel_size.height - 1) / 2;
	int K_W = (kernel_size.width - 1) / 2;
	Mat dst(H, W, CV_8UC1, Scalar(0));
	vector<int> tmp;
	
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			for (int m = -K_H; m < K_H + 1; m++) {
				for (int n = -K_W; n < K_W + 1; n++) {
					if (((i + m) < 0) || ((i + m) >= H)) {
						continue;
					}
					else if(((j + n) < 0) || ((j + n) >= W)) {
						continue;
					}
					else {
						tmp.push_back(src.at<uchar>(i + m, j + n));
					}
				}
			}
			sort(tmp.begin(), tmp.end());
			dst.at<uchar>(i, j) = tmp.at(floor(tmp.size() / 2));
			tmp.clear();
		}
	}
	return dst;
}

Mat adaptiveMedian(Mat src, Size kernel_size) {
	int H = src.rows;
	int W = src.cols;
	int M_K_size = (kernel_size.height - 1) / 2;
	int K_size = 1;
	Mat dst(H, W, CV_8UC1, Scalar(0));
	vector<int> tmp;
	int Med;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			for (int K_size = 1; K_size < M_K_size + 1; K_size++) {
				tmp.clear();
				for (int m = -K_size; m < K_size + 1; m++) {
					for (int n = -K_size; n < K_size + 1; n++) {
						if (((i + m) < 0) || ((i + m) >= H)) {
							continue;
						}
						else if(((j + n) < 0) || ((j + n) >= W)){
							continue;
						}
						else {
							tmp.push_back(src.at<uchar>(i + m, j + n));
						}
					}
				}
				sort(tmp.begin(), tmp.end());
				Med = tmp.at(floor(tmp.size() / 2));
				if ((tmp.at(0) < Med) && (Med < tmp.back())) {
					if ((tmp.at(0) < src.at<uchar>(i, j)) && ((src.at<uchar>(i, j) < tmp.back()))) {
						dst.at<uchar>(i, j) = src.at<uchar>(i, j);
					}
					else {
						dst.at<uchar>(i, j) = Med;
					}
				}
				else{
					continue;
				}
			}
			if (K_size = M_K_size + 1){
				dst.at<uchar>(i, j) = Med;
			}
		}
	}
	return dst;
}


Mat linearMotionBlur(Mat src, float a, float b, float T) {
	int H = src.rows;
	int W = src.cols;
	const float pi = 3.141592;

	float Temp;
	Mat spl_Tf[2];
	spl_Tf[0] = Mat::zeros(src.size(), CV_32F);
	spl_Tf[1] = Mat::zeros(src.size(), CV_32F);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Temp = pi * (a * i + b * j);
			if (Temp == 0) {
				spl_Tf[0].at<float>(i, j) = 1;
				spl_Tf[1].at<float>(i, j) = 0;
			}
			else {
				spl_Tf[0].at<float>(i, j) = (T * sin(Temp) * cos(Temp)) / Temp;
				spl_Tf[1].at<float>(i, j) = -(T * sin(Temp) * sin(Temp)) / Temp;
			}
		}
	}
	src.convertTo(src, CV_32F);//format transform
	Mat src_fft, dst_fft;
	fft2d(src, src_fft);

	Mat spl_src[2], spl_dst[2];
	split(src_fft, spl_src);
	float r1, r2, i1, i2;
	spl_dst[0] = Mat::zeros(src.size(), CV_32F);
	spl_dst[1] = Mat::zeros(src.size(), CV_32F);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			r1 = spl_src[0].at<float>(i, j);
			i1 = spl_src[1].at<float>(i, j);
			r2 = spl_Tf[0].at<float>(i, j);
			i2 = spl_Tf[1].at<float>(i, j);
			spl_dst[0].at<float>(i, j) = r1 * r2 - i1 * i2;
			spl_dst[1].at<float>(i, j) = r1 * i2 + i1 * r2;
		}
	}

	Mat dst_tmp;
	merge(spl_dst, 2, dst_fft);
	ifft2d(dst_fft, dst_tmp);
	Mat Tmp[2];
	split(dst_tmp, Tmp);
	Mat dst = Tmp[0];
	normalize(dst, dst, 0, 255, NORM_MINMAX);
	dst = Mat_<uchar>(dst);
	return dst;
}

Mat wienerFilter(Mat src, Mat H, float K) {
	const float eps = 1E-20;
	Mat Hf[2], wiener[2];
	Mat src_fft, dst_fft;
	Mat spl_src[2], spl_dst[2];
	fft2d(src, src_fft);
	split(src_fft, spl_src);
	split(H, Hf);
	Mat mag_Hf;
	magnitude(Hf[0], Hf[1], mag_Hf);
	float Temp;
	//calculate inverse filter
	wiener[0] = Mat::zeros(src.size(), CV_32F);
	wiener[1] = Mat::zeros(src.size(), CV_32F);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {

			Temp = pow(mag_Hf.at<float>(i, j), 2) + K;
			if (Temp == 0) {
				Temp = eps;
			}
			wiener[0].at<float>(i, j) = Hf[0].at<float>(i, j) / Temp;
			wiener[1].at<float>(i, j) = -Hf[1].at<float>(i, j) / Temp;
		}
	}

	//filtering
	float r1, r2, i1, i2;
	spl_dst[0] = Mat::zeros(src.size(), CV_32F);
	spl_dst[1] = Mat::zeros(src.size(), CV_32F);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			r1 = spl_src[0].at<float>(i, j);
			i1 = spl_src[1].at<float>(i, j);
			r2 = wiener[0].at<float>(i, j);
			i2 = wiener[1].at<float>(i, j);
			spl_dst[0].at<float>(i, j) = r1 * r2 - i1 * i2;
			spl_dst[1].at<float>(i, j) = r1 * i2 + i1 * r2;
		}
	}
	Mat dst_tmp;
	merge(spl_dst, 2, dst_fft);
	ifft2d(dst_fft, dst_tmp);
	Mat Tmp[2];
	split(dst_tmp, Tmp);
	Mat dst = Tmp[0];
	normalize(dst, dst, 0, 255, NORM_MINMAX);
	dst = Mat_<uchar>(dst);
	return dst;
}
