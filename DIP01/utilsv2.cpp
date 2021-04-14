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

Mat warpAffine(Mat src, Mat affmat, string interp_type)
{
	float* aff_data = (float*)affmat.data;

	if (affmat.size() != Size(3, 3)) {
		exit(-1);
	}
	int H = src.rows;
	int W = src.cols;
	int C = src.channels();

	vector<int> x = { 0, int(aff_data[0] * (H - 1)), int(aff_data[1] * (W - 1)), int(aff_data[0] * (H - 1) + aff_data[1] * (W - 1)) };
	vector<int> y = { 0, int(aff_data[3] * (H - 1)), int(aff_data[4] * (W - 1)), int(aff_data[3] * (H - 1) + aff_data[4] * (W - 1)) };

	auto xmin = *min_element(x.begin(), x.end());
	auto xmax = *max_element(x.begin(), x.end());
	auto ymin = *min_element(y.begin(), y.end());
	auto ymax = *max_element(y.begin(), y.end());

	int H_new = xmax - xmin + 1;
	int W_new = ymax - ymin + 1;

	Mat dst = Mat::ones(Size(W_new, H_new), src.type()) * 128;

	Mat affmat_inv = affmat.inv();
	float* affmat_inv_data = (float*)affmat_inv.data;
	for (int i = xmin; i <= xmax; i++) {
		for (int j = ymin; j <= ymax; j++) {
			Mat coord_new(3, 1, CV_32F, Scalar(1));
			float* coord_new_data = (float*)coord_new.data;
			coord_new_data[0] = (float)i;
			coord_new_data[1] = (float)j;

			Mat coord = affmat_inv * coord_new;
			float* coord_data = (float*)coord.data;

			if (coord_data[0] < -1e-6 || coord_data[0] >= H - 1)
				continue;
			else if (coord_data[1] < -1e-6 || coord_data[1] >= W - 1)
				continue;

			float delta[2] = { coord_data[0] - floor(coord_data[0]), coord_data[1] - floor(coord_data[1]) };

			int x0 = (int)floor(coord_data[0]);
			int x1 = (int)ceil(coord_data[0]);
			int y0 = (int)floor(coord_data[1]);
			int y1 = (int)ceil(coord_data[1]);

			for (int c = 0; c < C; c++) {
				float f[4] = {
					float(src.data[W * x0 * C + y0 * C + c]), float(src.data[W * x0 * C + y1 * C + c]),
					float(src.data[W * x1 * C + y0 * C + c]), float(src.data[W * x1 * C + y1 * C + c])
				};

				dst.data[W_new * (i - xmin) * C + (j - ymin) * C + c] = uchar(interp2D(delta, f, interp_type));
			}
			coord_new.release();
			coord.release();
		}
	}
	affmat_inv.release();

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
	Mat affine = Mat::zeros(Size(3, 3), CV_32F);
	float* aff_data = (float*)affine.data;
	aff_data[0] = cosf(radian);	aff_data[1] = -sinf(radian); aff_data[2] = 0.0;
	aff_data[3] = sinf(radian);	aff_data[4] = cosf(radian); aff_data[6] = 0.0;
	aff_data[6] = 0.0;	aff_data[7] = 0.0;	aff_data[8] = 1.0;

	Mat dst = warpAffine(src, affmat, interp_type);
	return dst;
}

