#include "utils.h"
#include <math.h>
#include <algorithm>

int main(int argc, char* argv[])
{
	int implement = 3;
	if (implement == 1) {
		Mat src = imread("../images/iris.tif", IMREAD_COLOR);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		Mat src_float = Mat_<Vec3f>(src);
		src_float = src_float / 255;

		Mat dst = cvtBGR2HSI(src_float);
		Mat hsi[3];
		split(dst, hsi);

		imshow("Color Cube", src);
		waitKey(0);
		imshow("H", hsi[0]);
		waitKey(0);
		imshow("S", hsi[1]);
		waitKey(0);
		imshow("I", hsi[2]);
		waitKey(0);
	}
	else if (implement == 2) {
		Mat src = imread("../images/iris.tif", IMREAD_COLOR);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		Mat src_float = Mat_<Vec3f>(src);
		src_float = src_float / 255;

		Mat src_hsi = cvtBGR2HSI(src_float);
		Mat hsi[3];
		split(src_hsi, hsi);

		int H = src.rows;
		int W = src.cols;
		Mat hue_mask;
		inRange(hsi[0], 0.65, 0.75, hue_mask);
		Mat tmp[3] = { hue_mask, hue_mask, hue_mask };
		merge(tmp, 3, hue_mask);

		Mat flower = src.mul(hue_mask / 255);
		imshow("original", src);
		waitKey(0);
		imshow("mask", hue_mask);
		waitKey(0);
		imshow("flower", flower);
		waitKey(0);
	}
	else if (implement == 3) {
		Mat src = imread("../images/lenna.tif", IMREAD_COLOR);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		Mat src_float = Mat_<Vec3f>(src);
		src_float = src_float / 255;

		Mat bgr[3];
		split(src_float, bgr);
		bgr[0] = abs(sobelFilter(bgr[0], 0)) + abs(sobelFilter(bgr[0], 1));
		bgr[1] = abs(sobelFilter(bgr[1], 0)) + abs(sobelFilter(bgr[1], 1));
		bgr[2] = abs(sobelFilter(bgr[2], 0)) + abs(sobelFilter(bgr[2], 1));

		imshow("B", bgr[0]);
		waitKey(0);
		imshow("G", bgr[1]);
		waitKey(0);
		imshow("R", bgr[2]);
		waitKey(0);

		Mat src_hsi = cvtBGR2HSI(src_float);
		Mat hsi[3];
		split(src_hsi, hsi);
		hsi[2] = abs(sobelFilter(hsi[2], 0)) + abs(sobelFilter(hsi[2], 1));

		Mat dst_rgb;
		merge(bgr, 3, dst_rgb);
		dst_rgb = Mat_<Vec3b>(dst_rgb * 255);

		Mat sobel_hsi;
		merge(hsi, 3, sobel_hsi);
		Mat dst_hsi = cvtHSI2BGR(sobel_hsi);
		dst_hsi = Mat_<Vec3b>(dst_hsi * 255);

		imshow("Origin", src);
		waitKey(0);
		imshow("Sobel in RGB", dst_rgb);
		waitKey(0);
		imshow("Sobel in HSI", dst_hsi);
		waitKey(0);
	}
	else if (implement == 0) {
		Mat src = imread("../images/lena.tif", IMREAD_GRAYSCALE);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		Mat src_float = Mat_<float>(src);
		src_float = src_float / 255;
		src_float = abs(sobelFilter(src_float, 0)) + abs(sobelFilter(src_float, 1));
		Mat bgr[3];
		/*split(src_float, bgr);
		bgr[0] = abs(sobelFilter(bgr[0], 0)) + abs(sobelFilter(bgr[0], 1));
		bgr[1] = abs(sobelFilter(bgr[1], 0)) + abs(sobelFilter(bgr[1], 1));
		bgr[2] = abs(sobelFilter(bgr[2], 0)) + abs(sobelFilter(bgr[2], 1));

		imshow("B", bgr[0]);
		imshow("G", bgr[1]);
		imshow("R", bgr[2]);

		Mat src_hsi = cvtBGR2HSI(src_float);
		Mat hsi[3];
		split(src_hsi, hsi);
		hsi[2] = abs(sobelFilter(hsi[2], 0)) + abs(sobelFilter(hsi[2], 1));*/

		/*Mat dst_rgb;
		merge(bgr, 3, dst_rgb);
		dst_rgb = Mat_<Vec3b>(dst_rgb * 255);

		Mat sobel_hsi;
		merge(hsi, 3, sobel_hsi);
		Mat dst_hsi = cvtHSI2BGR(sobel_hsi);
		dst_hsi = Mat_<Vec3b>(dst_hsi * 255);*/
		src_float = Mat_<uchar>(src_float * 255);
		imshow("Origin", src);
		imshow("Sobel in RGB", src_float);
		//imshow("Sobel in HSI", dst_hsi);
	}

	waitKey(0);
	destroyAllWindows();
	return 0;
}