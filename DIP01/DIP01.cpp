#include "utils.h"

int main(int argc, char* argv[])
{
	int implement = 1;
	if (implement == 1) {
		Mat src = imread("../images/letterT.tif", IMREAD_ANYCOLOR);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		int H = src.rows;
		int W = src.cols;

		string flags = "bilinear";
		src = resize(src, Size(500, 500), flags);//

		Mat affine = Mat::zeros(Size(3, 3), CV_32F);
		float* aff_data = (float*)affine.data;
		aff_data[0] = 1.5;	aff_data[1] = 0.0;	aff_data[2] = 0.0;
		aff_data[3] = 0.0;	aff_data[4] = 1.5;	aff_data[5] = 0.0;
		aff_data[6] = 0.0;	aff_data[7] = 0.0;	aff_data[8] = 1.0;

		imshow("Origin", src);
		Mat dst = warpAffine(src, affine, flags);
		imshow("Affine_1", dst);
		aff_data[0] = 1.0;	aff_data[1] = -0.1;	aff_data[2] = -20.0;
		aff_data[3] = 0.2;	aff_data[4] = 2.0;	aff_data[5] = 0.0;
		aff_data[6] = 0.0;	aff_data[7] = 0.0;	aff_data[8] = 1.0;
		dst = warpAffine(src, affine, flags);
		imshow("affine_2", dst);
	}
	else if (implement == 2) {
		Mat src = imread("../images/urban100_87.tif", IMREAD_ANYCOLOR);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		string flags = "bilinear";

		Mat dst = rotate(src, 0.7071, flags);
		dst = resize(dst, Size(500, 500), flags);
		imshow("Origin", src);
		imshow("Resize", dst);
		imshow("Rotate", dst);
		//Mat dst = resize(src, Size(500, 500), flags);
		//imshow("Origin", src);
		//imshow("Resize", dst);
		//dst = rotate(dst, 0.7071, flags);
		//imshow("Rotate", dst);
		imwrite("rev_Rotate.tif", dst);
	}
	waitKey(0);
	destroyAllWindows();
	return 0;
}