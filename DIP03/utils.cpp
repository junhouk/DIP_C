#include "utils.h"
void swapPhase(Mat src1, Mat src2, Mat& dst1, Mat& dst2) {
	Mat tmp1[2], tmp2[2];
	Mat mag_dst1, pha_dst1;
	Mat mag_dst2, pha_dst2;
	//obtain mag/phase response of src1
	fft2d(src1, dst1);
	split(dst1, tmp1);
	magnitude(tmp1[0], tmp1[1], mag_dst1);
	phase(tmp1[0], tmp1[1], pha_dst1);
	//obtain mag/phase response of src2
	fft2d(src2, dst2);
	split(dst2, tmp2);
	magnitude(tmp2[0], tmp2[1], mag_dst2);
	phase(tmp2[0], tmp2[1], pha_dst2);
	//polarToCart with swap phase
	float A, B, C, D;
	for (int i = 0; i < dst1.rows; i++) {
		for (int j = 0; j < dst1.cols; j++) {
			A = mag_dst1.at<float>(i, j);
			B = mag_dst2.at<float>(i, j);
			C = pha_dst1.at<float>(i, j);
			D = pha_dst2.at<float>(i, j);
			tmp1[0].at<float>(i, j) = A * cos(D);
			tmp1[1].at<float>(i, j) = A * sin(D);
			tmp2[0].at<float>(i, j) = B * cos(C);
			tmp2[1].at<float>(i, j) = B * sin(C);
		}
	}
	//merge real and imaginary part
	merge(tmp1, 2, dst1);
	merge(tmp2, 2, dst2);
	//idft
	ifft2d(dst1, dst1);
	ifft2d(dst2, dst2);
	//change format for imshow
	dst1.convertTo(dst1, CV_8U);
	dst2.convertTo(dst2, CV_8U);
}
Mat boxDFT(Size kernel_size, Size output_size) {
	Mat kernel(output_size, CV_32F);
	Mat pad_filter = Mat::zeros(output_size, CV_32F);
	for (int i = 0; i < kernel_size.height; i++) {
		pad_filter.at<float>(i, i) = 1/ (float)(kernel_size.height);
	}
	fft2d(pad_filter, kernel);
	return kernel;
}
Mat gaussDFT(Size kernel_size, Size output_size) {
	Mat kernel(output_size, CV_32F);
	int s = 8;//sigma value, 입력변수에는 존재하지 않아 조건에따라 코드를 수정해줘야함.
				//(구현2 s=1(7X7), s=5(31X31),구현3 s = 4(25X25), s=8(49X49))
	int Tx, Ty;
	Tx = (kernel_size.height - 1) / 2;
	Ty = (kernel_size.width - 1) / 2;
	float T;
	float sum = 0;
	Mat GLPF = Mat::zeros(kernel_size, CV_32F);
	Mat pad_filter = Mat::zeros(output_size, CV_32F);
	for (int i = -Tx; i <= Tx; i++) {
		for (int j = -Ty; j <= Ty; j++) {
			T = sqrt(i * i + j * j);
			GLPF.at<float>(i + Tx, j + Ty) = (exp(-(T * T) / (2 * s * s)));
			sum += GLPF.at<float>(i + Tx, j + Ty);
		}
	}
	GLPF = GLPF / sum;
	//padding
	for (int i = 0; i < kernel_size.height; i++) {
		for (int j = 0; j < kernel_size.width; j++) {
			pad_filter.at<float>(i, j) = GLPF.at<float>(i, j);
		}
	}
	fft2d(pad_filter, kernel);
	return kernel;
}
Mat freqFilter(Mat src, Mat kernel) {
	fft2d(src, src);				//dft at paded image(obtain F(u,v)
	fftshift2d(kernel, kernel);		//change filter's center (0,0) to (P/2,Q/2)
	//define temp matrix for calclulate G(u,v) - element wise multiplication
	Mat spl_pad[2], spl_ker[2], spl_dst[2];
	spl_dst[0] = Mat::zeros(src.size(), CV_32F);
	spl_dst[1] = Mat::zeros(src.size(), CV_32F);
	float A, B, C, D;
	//split real and imaginary part
	split(src, spl_pad);
	split(kernel, spl_ker);
	//element wise multiplication
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			A = spl_pad[0].at<float>(i, j);
			B = spl_pad[1].at<float>(i, j);
			C = spl_ker[0].at<float>(i, j);
			D = spl_ker[1].at<float>(i, j);
			spl_dst[0].at<float>(i, j) = A * C - B * D;//real part G(u,v)
			spl_dst[1].at<float>(i, j) = A * D + B * C;//imaginary part of G(u,v)
		}
	}
	Mat dst_tmp;
	merge(spl_dst, 2, dst_tmp);//merge real and imaginary part of G(u,v)
	ifft2d(dst_tmp, dst_tmp);// idft
	//obtain real part of IDFT(G(u,v))
	Mat tmp[2];
	split(dst_tmp, tmp);
	Mat pad_dst = tmp[0];
	Mat dst(Size(src.rows/2, src.cols/2), CV_32F);
	//change image's center (P/2,Q/2) to (0,0)
	//obtain g(x,y) by extracting M*N region from the g_p(x,y)
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			dst.at<float>(i, j) = pow(-1, i + j) * pad_dst.at<float>(i, j);
		}
	}
	return dst;
}
Mat freqBoxFilter(Mat src, Size kernel_size) {
	Size padding_size = Size(2 * src.rows, 2 * src.cols);
	src.convertTo(src, CV_32F);//format transform
	Mat pad_src = Mat::zeros(padding_size, CV_32F);//padding
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			pad_src.at<float>(i, j) = src.at<float>(i, j);
		}
	}
	for (int i = 0; i < pad_src.rows; i++) {	//move origin to center
		for (int j = 0; j < pad_src.cols; j++) {
			pad_src.at<float>(i, j) = pow(-1, i + j) * pad_src.at<float>(i, j);
		}
	}
	Mat kernel = boxDFT(kernel_size, padding_size);//obtain Box filter

	Mat dst = freqFilter(pad_src, kernel);//apply filter
	return dst;
}
Mat freqGaussFilter(Mat src, Size kernel_size) {
	Size padding_size = Size(2 * src.rows, 2 * src.cols);
	src.convertTo(src, CV_32F);//format transform
	Mat pad_src = Mat::zeros(padding_size, CV_32F);//padding
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			pad_src.at<float>(i, j) = src.at<float>(i, j);
		}
	}
	for (int i = 0; i < pad_src.rows; i++) {	//move origin to center
		for (int j = 0; j < pad_src.cols; j++) {
			pad_src.at<float>(i, j) = pow(-1, i + j) * pad_src.at<float>(i, j);
		}
	}
	Mat kernel = gaussDFT(kernel_size, padding_size);//obtain Box filter

	Mat dst = freqFilter(pad_src, kernel);//apply filter
	return dst;
}
