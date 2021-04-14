#include "utils.h"
#include <math.h>

void mag(Mat img) {
	Mat tmp[2];
	split(img, tmp);
	Mat img_mag;
	magnitude(tmp[0], tmp[1], img_mag);
	log(1 + img_mag, img_mag);
	normalize(img_mag, img_mag, 0, 1, NORM_MINMAX);
	imshow("img_mag", img_mag);
	waitKey(0);
}
int main(int argc, char* argv[])
{
	int implement = 3;
	if (implement == 4) {	
	}
	/*
	else if (implement == 5) {
		Mat src = imread("../images/test_pattern.tif", IMREAD_GRAYSCALE);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}

		Mat dst = freqBoxFilter(src, Size(25, 25));
		dst.convertTo(dst, CV_8U);

		imshow("Original", src);
		imshow("Box filtering", dst);

		dst = freqGaussFilter(src, Size(49, 49));
		dst.convertTo(dst, CV_8U);

		imshow("Gaussian filtering", dst);	
	}
	*/
	//구현1
	else if (implement == 1) {
		Mat src1 = imread("../images/building.tif", IMREAD_GRAYSCALE);
		Mat src2 = imread("../images/Chronometer.tif", IMREAD_GRAYSCALE);
		if (src1.empty() || src2.empty()) {
			cout << "Input images do not exist." << endl;
			exit(-1);
		}

		Mat dst1, dst2;
		swapPhase(src1, src2, dst1, dst2);

		imshow("Magnitude(building) + Phase(Chronometer)", dst1);
		imshow("Magnitude(Chronometer) + Phase(building)", dst2);
		imwrite("Magnitude(building) + Phase(Chronometer).tif", dst1);
		imwrite("Magnitude(Chronometer) + Phase(building).tif", dst2);

	}

	//구현2
	else if (implement == 2) {
		Mat tmp[2];
		Mat kernel_mag, kernel_pha;
		int imp = 4;
		if (imp == 1) {
		//----------------------------------------------------------
			Mat kernel = boxDFT(Size(5, 5), Size(1000, 1000));
			fftshift2d(kernel, kernel);
			split(kernel, tmp);
			magnitude(tmp[0], tmp[1], kernel_mag);
			phase(tmp[0], tmp[1], kernel_pha);
			log(1 + kernel_mag, kernel_mag);
			normalize(kernel_mag, kernel_mag, 0, 1, NORM_MINMAX);
			imshow("Box kernel_mag(5 X 5)", kernel_mag);
			imshow("Box kernel_pha(5 X 5)", kernel_pha);

			kernel_mag.convertTo(kernel_mag, CV_8U);
			kernel_pha.convertTo(kernel_pha, CV_8U);

			imwrite("Box kernel_mag(5 X 5).tif", kernel_mag);
			imwrite("Box kernel_pha(5 X 5).tif", kernel_pha);
		}
		else if (imp == 2) {
		//----------------------------------------------------------
			Mat kernel = boxDFT(Size(21, 21), Size(1000, 1000));
			fftshift2d(kernel, kernel);
			split(kernel, tmp);
			magnitude(tmp[0], tmp[1], kernel_mag);
			phase(tmp[0], tmp[1], kernel_pha);
			log(1 + kernel_mag, kernel_mag);
			normalize(kernel_mag, kernel_mag, 0, 1, NORM_MINMAX);
			imshow("Box kernel_mag(21 X 21)", kernel_mag);
			imshow("Box kernel_pha(21 X 21)", kernel_pha);

			kernel_mag.convertTo(kernel_mag, CV_8U);
			kernel_pha.convertTo(kernel_pha, CV_8U);

			imwrite("Box kernel_mag(21 X 21).tif", kernel_mag);
			imwrite("Box kernel_pha(21 X 21).tif", kernel_pha);
		}
		else if (imp == 3) {
			//----------------------------------------------------------
			Mat kernel = gaussDFT(Size(7, 7), Size(1000, 1000));//s = 1
			fftshift2d(kernel, kernel);
			split(kernel, tmp);
			magnitude(tmp[0], tmp[1], kernel_mag);
			phase(tmp[0], tmp[1], kernel_pha);
			log(1 + kernel_mag, kernel_mag);
			normalize(kernel_mag, kernel_mag, 0, 1, NORM_MINMAX);
			imshow("Gaussian kernel_mag(7 X 7)", kernel_mag);
			imshow("Gaussian kernel_pha(7 X 7)", kernel_pha);

			kernel_mag.convertTo(kernel_mag, CV_8U);
			kernel_pha.convertTo(kernel_pha, CV_8U);

			imwrite("Gaussian kernel_mag(7 X 7).tif", kernel_mag);
			imwrite("Gaussian kernel_pha(7 X 7).tif", kernel_pha);
		}
		else if (imp == 4) {
			//----------------------------------------------------------
			Mat kernel = gaussDFT(Size(31, 31), Size(1000, 1000));//s = 5
			fftshift2d(kernel, kernel);
			split(kernel, tmp);
			magnitude(tmp[0], tmp[1], kernel_mag);
			phase(tmp[0], tmp[1], kernel_pha);
			log(1 + kernel_mag, kernel_mag);
			normalize(kernel_mag, kernel_mag, 0, 1, NORM_MINMAX);
			imshow("Gaussian kernel_mag(31 X 31)", kernel_mag);
			imshow("Gaussian kernel_pha(31 X 31)", kernel_pha);

			kernel_mag.convertTo(kernel_mag, CV_8U);
			kernel_pha.convertTo(kernel_pha, CV_8U);

			imwrite("Gaussian kernel_mag(31 X 31).tif", kernel_mag);
			imwrite("Gaussian kernel_pha(31 X 31).tif", kernel_pha);
		}
	}
	//구현3
	else if (implement == 3) {
		Mat src = imread("../images/book.tif", IMREAD_GRAYSCALE);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		Mat dst1 = freqBoxFilter(src, Size(69, 69));
		dst1.convertTo(dst1, CV_8U);

		//imshow("Original", src);
		imshow("Box filtering(13 X 13)", dst1);

	//freq gauss filtering
		//Mat dst3 = freqGaussFilter(src, Size(25, 25));
		//dst3.convertTo(dst3, CV_8U);

		//imshow("Gaussian filtering(25 X 25)", dst3);
	}
	
	waitKey(0);
	destroyAllWindows();
	return 0;
}