#include "utils.h"

int main(int argc, char* argv[])
{
	int implement = 3;
	if (implement == 1) {
		Mat src1 = imread("../images/astronaut.tif", IMREAD_GRAYSCALE);
		Mat src2 = imread("../images/pollen.tif", IMREAD_GRAYSCALE);
		if (src1.empty() || src2.empty()) {
			cout << "Input images do not exist." << endl;
			exit(-1);
		}

		Mat dst1 = gammaTransform(src1, 0.6);
		Mat dst2 = gammaTransform(src2, 0.6);
		imshow("astronaut", src1);
		imshow("pollen", src2);
		imshow("Gamma_astronuat", dst1);
		imshow("Gamma_pollen", dst2);
	}
	else if (implement == 2) {
		Mat src1 = imread("../images/astronaut.tif", IMREAD_GRAYSCALE);
		Mat src2 = imread("../images/pollen.tif", IMREAD_GRAYSCALE);
		if (src1.empty() || src2.empty()) {
			cout << "Input images do not exist." << endl;
			exit(-1);
		}

		Mat dst1 = histEqualization(src1);
		Mat dst2 = histEqualization(src2);
		imshow("astronaut", src1);
		imshow("pollen", src2);
		imshow("Hist_Equalization_astronuat", dst1);
		imshow("Hist_Equalization_pollen", dst2);
	}
	else if (implement == 3) {
		Mat src = imread("../images/rose.tif", IMREAD_GRAYSCALE);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		Mat kernel = boxFilter(Size(5, 5));
		Mat blur_img = imFilter(src, kernel);

		Mat src_f;
		src.convertTo(src_f, CV_32FC1);
		Mat unsharp_mask = src_f - blur_img;

		Mat dst = src_f + 3.0 * unsharp_mask;
		dst.convertTo(dst, CV_8UC1);

		imshow("Original", src);
		imshow("Highboost Filtering", dst);
	}
	waitKey(0);
	destroyAllWindows();
	return 0;
}