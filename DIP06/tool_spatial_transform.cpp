# include "utils.h"

Mat IntensityTransform_LUT(Mat src, float LUT[256]) {
	int H = src.rows;
	int W = src.cols;
	Mat dst(src.size(), src.type(), Scalar(0));
	for (int i = 1; i < H; i++) {
		for (int j = 1; j < W; j++) {
			dst.at<uchar>(i, j) = LUT[src.at<uchar>(i, j)];
		}
	}
	return dst;
}
Mat ImageNagative(Mat src) {
	//generate LUT
	float LUT[256];
	for (int i = 0; i < 256; i++) {
		LUT[i] = 255 - i;
	}
	//Transform
	Mat dst = IntensityTransform_LUT(src, LUT);
	return dst;
}
Mat LogTrnaform(Mat src, float c) {
	float LUT[256];
	float Temp;
	for (int i = 0; i < 256; i++) {
		Temp = c * log(1 + i);
		LUT[i] = (int)Temp;
	}
	//Transform
	Mat dst = IntensityTransform_LUT(src, LUT);
	return dst;
}

Mat GammaTransform(Mat src, float g, float c) {
	float LUT[256];
	float Temp;
	for (int i = 0; i < 256; i++) {
		Temp = c * 255 * pow(i / 255, g);
		LUT[i] = (int)Temp;
	}
	//Transform
	Mat dst = IntensityTransform_LUT(src, LUT);
	return dst;
}
Mat affineTansform(Mat src, Mat affine, int inter_mode ) {
	int H = src.rows;
	int W = src.cols;
	int C = src.channels();
	float* aff_data = (float*)affine.data;
	//결과행렬 크기 계산
	int x_max = max({ 0, (int)(aff_data[0] * (H - 1)),(int)(aff_data[1] * (W - 1)), (int)(aff_data[0] * (H - 1) + aff_data[1] * (W - 1)) });
	int x_min = min({ 0, (int)(aff_data[0] * (H - 1)),(int)(aff_data[1] * (W - 1)), (int)(aff_data[0] * (H - 1) + aff_data[1] * (W - 1)) });
	int y_max = max({ 0, (int)(aff_data[3] * (H - 1)),(int)(aff_data[4] * (W - 1)), (int)(aff_data[3] * (H - 1) + aff_data[4] * (W - 1)) });
	int y_min = min({ 0, (int)(aff_data[3] * (H - 1)),(int)(aff_data[4] * (W - 1)), (int)(aff_data[3] * (H - 1) + aff_data[4] * (W - 1)) });
	int H_dst = x_max - x_min + 1;
	int W_dst = y_max - y_min + 1;
	Mat dst = Mat::ones(H_dst, W_dst, src.type()) * 128;
	Mat affine_inv = affine.inv();
	float* aff_inv_data = (float*)affine_inv.data;
	float x_ori, y_ori;
	int x_hi, x_lo, y_hi, y_lo;
	for (int i = x_min; i < x_max; i++) {
		for (int j = y_min; j < y_max; j++) {
			x_ori = aff_inv_data[0] * i + aff_inv_data[1] * j + aff_inv_data[2];
			y_ori = aff_inv_data[3] * i + aff_inv_data[4] * j + aff_inv_data[5];
			if (x_ori <= 0 || x_ori >= H) {
				continue;
			}
			if (y_ori <= 0 || y_ori >= W) {
				continue;
			}
			float dx = x_ori - floor(x_ori);
			float dy = y_ori - floor(y_ori);
			x_hi = (int)ceil(x_ori);
			x_lo = (int)floor(x_ori);
			y_hi = (int)ceil(y_ori);
			y_lo = (int)floor(y_ori);
			for (int c = 0; c < C; c++) {
				float da1 = src.data[W * x_lo * C + y_lo * C + c];
				float da2 = src.data[W * x_lo * C + y_hi * C + c];
				float da3 = src.data[W * x_hi * C + y_lo * C + c];
				float da4 = src.data[W * x_hi * C + y_hi * C + c];

				dst.data[(i-x_min) * W_dst * C + (j-y_min) * C + c] = (uchar)(interpolation2D(dx, dy, da1, da2, da3, da4, inter_mode));
			}
		}
	}
		return dst;
}
float interpolation2D(float delta_x, float delta_y, float data_lo_lo, float data_lo_hi, float data_hi_lo, float data_hi_hi, int interp_mode) {
	float a = 0;
	if (interp_mode == 0){//nearest
		if (delta_x < 0.5) {
			if (delta_y < 0.5) {
				a = data_lo_lo;
			}
			else {
				a = data_lo_hi;
			}
		}
		else {
			if (delta_y < 0.5) {
				a = data_hi_lo;
			}
			else {
				a = data_hi_hi;
			}
		}
	}
	if (interp_mode == 1) {//bilinear
		a = data_hi_hi * delta_x * delta_y + data_hi_lo * (1 - delta_x) * delta_y + data_lo_hi * delta_x * (1 - delta_y) + data_lo_lo * (1 - delta_x) * (1 - delta_y);
	}
	return a;
}
Mat Rotation_Aff(Mat src, float radian) {
	Mat affine = Mat::zeros(Size(3, 3), CV_32F);
	float* aff_data = (float*)affine.data;
	aff_data[0] = cosf(radian);	aff_data[1] = -sinf(radian);
	aff_data[3] = sinf(radian);	aff_data[4] = cosf(radian);
	aff_data[8] = 1.0;
	Mat dst = affineTansform(src, affine, 1);
	return dst;
}
Mat Resize_aff(Mat src, Size size) {
	float Mag_H = (float)size.height/src.rows;
	float Mag_W = (float)size.width / src.cols;
	Mat affine = Mat::zeros(Size(3, 3), CV_32F);
	float* aff_data = (float*)affine.data;
	aff_data[0] = Mag_H;
	aff_data[4] = Mag_W;
	aff_data[8] = 1.0;
	Mat dst = affineTansform(src, affine, 1);
	return dst;

}