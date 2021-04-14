# include "utils.h"

Mat Box_filtering(Mat src, Size size, int pad_flag) {
	Mat Box = Mat::ones(size, CV_32F);
	Box = Box / Box.total();
	Mat dst = Lin_spat_filter(src, Box, pad_flag);
	dst.convertTo(dst, CV_8UC1);
	return dst;
}
Mat Gaussian_filtering(Mat src, Size size, float sigma, int pad_flag) {
	int H = size.height;
	Mat Gaussian1D = Mat::zeros(H, 1, CV_32F);
	int K_H = (H-1)/2;
	int Tmp;
	float sum = 0.0;
	for (int i = 0; i < H; i++) {
		Tmp = i - K_H;
		Gaussian1D.at<float>(i, 0) = (exp(-(Tmp * Tmp) / (2 * sigma * sigma)));
		sum += Gaussian1D.at<float>(i, 0);
	}
	Gaussian1D = Gaussian1D / sum;
	Mat Gaussian2D = Gaussian1D * Gaussian1D.t();
	Mat dst = Lin_spat_filter(src, Gaussian2D, pad_flag);
	//dst.convertTo(dst, CV_8UC1);
	return dst;
}
Mat Laplacian_filtering(Mat src, int sel_ker, int pad_flag) {
	//sel_ker = 0  ==> 4 direction edge detection
	//sel_ker = 1  ==> 8 direction edge detection
	Mat L_kernal4 = Mat_<float>({ 3,3 }, { 0,1,0,1,-4,1,0,1,0 });//4direction
	Mat L_kernal8 = Mat_<float>({ 3,3 }, { 1,1,1,1,-8,1,1,1,1 });//8direction
	Mat dst;
	if (sel_ker == 0) {
		dst = Lin_spat_filter(src, L_kernal4, 1);
	}
	else if (sel_ker == 1) {
		dst = Lin_spat_filter(src, L_kernal8, 1);
	}
	dst.convertTo(dst, CV_8UC1);
	return dst;
}
Mat sobel_filtering(Mat src, int sel_ker, int pad_flag) {
	//sel_ker = 0  ==> x axis
	//sel_ker = 1  ==> y axis
	//sel_ker = 2  ==> xy axis(sum)
	Mat sobel_X = Mat_<float>({ 3,3 }, { -1,0,1,-2,0,2,-1,0,1 });//x axis
	Mat sobel_Y = Mat_<float>({ 3,3 }, { -1,-2,-1,0,0,0,1,2,1 });//y axis
	Mat dst;
	if (sel_ker == 0) {
		dst = abs(Lin_spat_filter(src, sobel_X, 1));
	}
	else if (sel_ker == 1) {
		dst = abs(Lin_spat_filter(src, sobel_Y, 1));
	}
	else if (sel_ker == 2) {
		dst = abs(Lin_spat_filter(src, sobel_X, 1))/2;
		dst = dst + abs(Lin_spat_filter(src, sobel_Y, 1))/2;
	}
	dst.convertTo(dst, CV_8UC1);
	return dst;
}
Mat Lin_spat_filter(Mat src, Mat kernel, int pad_type_flag) {
	/*
	pad_tpye_flag = 0 => zero padding
	pad_type_flag = 1 => mirror padding
	*/
	int H = src.rows;
	int W = src.cols;
	int K_H = (kernel.rows - 1) / 2;
	int K_W = (kernel.cols - 1) / 2;
	float Tmp;
	src.convertTo(src, CV_32F);
	Mat src_pad(H + 2 * K_H, W + 2 * K_W, CV_32F, Scalar(0));//padding Matrix
	//padding
	if (pad_type_flag == 0) {			//zero padding
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				src_pad.at<float>(i + K_H, j + K_W) = src.at<float>(i, j);
			}
		}
	}
	else if (pad_type_flag == 1) {	//mirror padding
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
	Tmp = 0;
	//convolution
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			Tmp = 0;
			for (int m = 0; m < 2 * K_H + 1; m++) {
				for (int n = 0; n < 2 * K_W + 1; n++) {
					Tmp += kernel.at<float>(m, n) * src_pad.at<float>(i + m, j + n);
				}
			}
			dst.at<float>(i, j) = Tmp;
		}
	}
	return dst;
}
Mat Adap_med_filter(Mat src, Size max_ker) {
	int H = src.rows;
	int W = src.cols;
	int M_K_size = (max_ker.height - 1) / 2;
	int K_size = 1;
	Mat dst(H, W, CV_8UC1, Scalar(0));
	vector<int> tmp;
	int Med = 0;

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			for (int K_size = 1; K_size < M_K_size + 1; K_size++) {
				tmp.clear();
				for (int m = -K_size; m < K_size + 1; m++) {
					for (int n = -K_size; n < K_size + 1; n++) {
						//edge,cornor에서 image 외부의 값은 고려하지 않음
						if (((i + m) < 0) || ((i + m) >= H)) {
							continue;
						}
						else if (((j + n) < 0) || ((j + n) >= W)) {
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
				else {
					continue;
				}
			}
			if (K_size = M_K_size + 1) {
				dst.at<uchar>(i, j) = Med;
			}
		}
	}
	return dst;
}
Mat unsharp(Mat src, string filter, Size size) {
	Mat blur;
	if (filter == "Box") {
		blur = Box_filtering(src, size, 1);
	} 
	else if (filter == "Gaussian") {
		blur = Gaussian_filtering(src, size, 1,1);
	}
	src.convertTo(src, CV_32F);
	blur.convertTo(blur, CV_32F);
	Mat dst = src - blur;
	return dst;
}
Mat high_boost(Mat src, int k, string filter, Size size) {
	Mat mask = unsharp(src, filter, size);
	src.convertTo(src, CV_32F);
	Mat dst = src + k * mask;
	dst.convertTo(dst, CV_8UC1);
	return dst;
}