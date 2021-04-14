# include "utils.h"

Mat Ideal_box_filter(Mat src, string filter_type, float cuts_off) {
	Mat H (src.size(), CV_32F, Scalar(0));
    float C_x = src.rows / 2;
	float C_y = src.cols / 2;
	float D;
	for (int i = 0; i < src.rows;i++){
		for (int j = 0; j < src.cols;j++){
		D = sqrt((i-C_x)*(i-C_x)+(j-C_y)*(j-C_y));
		if (D > cuts_off) {
			H.at<float>(i,j) = 0;
		}
		else {
			H.at<float>(i,j) = 1;
		}
		}
	}
	if (filter_type == "High"){
		H = 1 - H;
	}
	return H;
}
Mat Gaussian_filter(Mat src, string filter_type, float cuts_off) {
	Mat H (src.size(), CV_32F, Scalar(0));
    float C_x = src.rows / 2;
	float C_y = src.cols / 2;
	float D;
	for (int i = 0; i < src.rows;i++){
		for (int j = 0; j < src.cols;j++){
		D = sqrt((i-C_x)*(i-C_x)+(j-C_y)*(j-C_y));
			H.at<float>(i,j) = exp(-(D*D)/(2*cuts_off*cuts_off));
		}
	}
	normalize(H, H, 1, 0, NORM_INF);
	if (filter_type == "High"){
		H = 1 - H;
	}
	return H;
}
Mat Butterworth_filter(Mat src, string filter_type, float n, float cuts_off) {
	Mat H (src.size(), CV_32F, Scalar(0));
    float C_x = src.rows / 2;
	float C_y = src.cols / 2;
	float D;
	if (filter_type == "Low"){
		for (int i = 0; i < src.rows;i++){
			for (int j = 0; j < src.cols;j++){
			D = sqrt((i-C_x)*(i-C_x)+(j-C_y)*(j-C_y));
			H.at<float>(i,j) = 1/(1 + pow((D/cuts_off),2*n));
			}
		}
	}
	if (filter_type == "High"){
		for (int i = 0; i < src.rows;i++){
			for (int j = 0; j < src.cols;j++){
			D = sqrt((i-C_x)*(i-C_x)+(j-C_y)*(j-C_y));
			H.at<float>(i,j) = 1/(1 + pow((cuts_off/D),2*n));
			}
		}
	}
	normalize(H, H, 1, 0, NORM_INF);
	return H;
}

Mat Freq_filter(Mat src, Mat kernel) {//input padded src, kernel
	Mat src_dft, dst_dft;
	Mat dst_pad;
	fft2d(src, src_dft);//dft at paded image(obtain F(u,v)
	fftshift2d(src_dft, src_dft);
	Mat tmp[2] = { kernel, kernel };
	merge(tmp, 2, kernel);
	dst_dft = src_dft.mul(kernel);
	//////
	Mat mag, temp[2];
	split(dst_dft, temp);
	Mat magni;
	magnitude(temp[0], temp[1], mag);
	log(1 + mag, magni);
	normalize(magni, magni, 0, 1, NORM_MINMAX);
	imwrite("test.tif", magni);

	//////
	ifftshift2d(dst_dft, dst_dft);
	ifft2d(dst_dft, dst_pad);
	normalize(dst_pad, dst_pad, 1, 0, NORM_INF);

	return dst_pad;
}
Mat Freq_filter_2(Mat src, Mat kernel) {//input padded src, kernel
	Mat src_dft, dst_dft;
	Mat dst_pad;
	fft2d(src, src_dft);//dft at paded image(obtain F(u,v)
	Mat tmp[2] = { kernel, kernel };
	merge(tmp, 2, kernel);
	dst_dft = src_dft.mul(kernel);
	ifft2d(dst_dft, dst_pad);
	normalize(dst_pad, dst_pad, 1, 0, NORM_INF);
	return dst_pad;
}
Mat Notch_filter(Mat src, string filter_type, int row, int col, float cuts_off, float n) {
	Mat pre_Ha(src.rows * 2, src.cols * 2, CV_32F, Scalar(1));
	Mat pre_Hb, H;
	int r = 2 * row;
	int c = 2 * col;
	float C_x = src.rows;
	float C_y = src.cols;
	float D = 0;
	if (filter_type == "Box") {
		for (int i = 0; i < src.rows * 2; i++) {//
			for (int j = 0; j < src.cols * 2; j++) {
				D = sqrt((i - C_x - r) * (i - C_x - r) + (j - C_y - c) * (j - C_y - c));
				if (D <= cuts_off) {
					pre_Ha.at<float>(i, j) = 0;
				}
			}
		}
		rotate(pre_Ha, pre_Hb, ROTATE_180);
		H = pre_Ha.mul(pre_Hb);
	}
	else if (filter_type == "Gaussian") {
		for (int i = 0; i < src.rows * 2; i++) {//
			for (int j = 0; j < src.cols * 2; j++) {
				D = sqrt((i - C_x - r) * (i - C_x - r) + (j - C_y - c) * (j - C_y - c));
				pre_Ha.at<float>(i, j) = exp(-(D * D) / (2 * cuts_off * cuts_off));
			}
		}
		normalize(pre_Ha, pre_Ha, 1, 0, NORM_INF);
		pre_Ha = 1 - pre_Ha;
		rotate(pre_Ha, pre_Hb, ROTATE_180);
		H = pre_Ha.mul(pre_Hb);

	}
	else if (filter_type == "Butterworth") {
		for (int i = 0; i < src.rows * 2; i++) {//
			for (int j = 0; j < src.cols * 2; j++) {
				D = sqrt((i - C_x - r) * (i - C_x - r) + (j - C_y - c) * (j - C_y - c));
				pre_Ha.at<float>(i, j) = 1 / (1 + pow((cuts_off / D), 2 * n));
			}
		}
		normalize(pre_Ha, pre_Ha, 1, 0, NORM_INF);
		rotate(pre_Ha, pre_Hb, ROTATE_180);
		H = pre_Ha.mul(pre_Hb);
	}
	return H;
}

Mat show_mag(Mat src) {
	src.convertTo(src, CV_32F);
	normalize(src, src, 1, 0, NORM_MINMAX);
	Mat src_dft;
	fft2d(src, src_dft);
	fftshift2d(src_dft, src_dft);
	Mat tmp[2];
	split(src_dft, tmp);
	Mat img_mag;
	magnitude(tmp[0], tmp[1], img_mag);
	log(1 + img_mag, img_mag);
	normalize(img_mag, img_mag, 1, 0, NORM_INF);
	imshow("FT", img_mag);
	return img_mag;
}
