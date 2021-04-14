# include "utils.h"

void pyr_Down(Mat src, Mat& appr, Mat& residu);
void pyr_Down(Mat src, Mat & appr, Mat & residu) {
	Size size_blur(5, 5);
	Size size1(src.cols / 2, src.rows / 2);
	src.convertTo(src, CV_32F);
	normalize(src, src, 1, 0, NORM_INF);
	Mat blur_src = Gaussian_filtering(src, size_blur, 1, 1);
	resize(blur_src, appr, size1);
	Mat predict; resize(appr, predict, src.size());
	residu = src - predict;
}

Mat dewaveletsyn(Mat dst, Mat proto_type, int LV) {
	int r;
	for (int i = LV; i > 0; i--) {
		r = pow(2, i-1);
		Mat target = dst(Rect(0, 0, dst.cols / r, dst.rows / r));
		Mat temp = dewaveletsyn_1(target, proto_type);
		temp.copyTo(dst(Rect(0, 0, dst.cols / r, dst.rows / r)));
	}
	return dst;
}

Mat dewaveletsyn_1(Mat dst, Mat proto_type) {
	int H = dst.rows;
	int W = dst.cols;
	int L1 = proto_type.cols;
	int L2 = floor(L1 / 2);
	float Tmp;
	Mat type1_filter(1, L1, CV_32F);
	for (int i = 0; i < L1; i++) {
		type1_filter.at<float>(0, i) = pow((-1), i) * proto_type.at<float>(0, i);
	}

	//syn
	Mat syn_filter0(1, L1, CV_32F);
	for (int i = 0; i < L1; i++) {
		syn_filter0.at<float>(0, i) = proto_type.at<float>(0, L1 - i - 1);
	}
	Mat syn_filter1(1, L1, CV_32F);
	for (int i = 0; i < L1; i++) {
		syn_filter1.at<float>(0, i) = type1_filter.at<float>(0, L1 - i - 1);
	}
	Mat A = dst(Rect(0, 0, W / 2, H / 2));
	Mat Hd = dst(Rect(0, H / 2, W / 2, H / 2));
	Mat Vd = dst(Rect(W / 2, 0, W / 2, H / 2));
	Mat Dd = dst(Rect(W / 2, H / 2, W / 2, H / 2));
	Mat tmp0(H + L1, W + L1, CV_32F, Scalar(0));
	Mat tmp1(H + L1, W + L1, CV_32F, Scalar(0));
	Mat pre_recon0(H, W, CV_32F);
	Mat pre_recon1(H, W, CV_32F);

	for (int i = 0; i < H / 2; i++) {
		for (int j = 0; j < W / 2; j++) {
			tmp0.at<float>(2 * i + L2, j) = A.at<float>(i, j);
		}
	}
	for (int i = 0; i < H / 2; i++) {
		for (int j = 0; j < W / 2; j++) {
			tmp1.at<float>(2 * i + L2, j) = Vd.at<float>(i, j);
		}
	}
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += syn_filter0.at<float>(0, k) * tmp0.at<float>(i + k, j);
				Tmp += syn_filter1.at<float>(0, k) * tmp1.at<float>(i + k, j);
			}
			pre_recon0.at<float>(i, j) = Tmp;
		}
	}

	//--------------------------------------------------------------------------//
	for (int i = 0; i < H / 2; i++) {
		for (int j = 0; j < W / 2; j++) {
			tmp0.at<float>(2 * i + L2, j) = Hd.at<float>(i, j);
		}
	}
	for (int i = 0; i < H / 2; i++) {
		for (int j = 0; j < W / 2; j++) {
			tmp1.at<float>(2 * i + L2, j) = Dd.at<float>(i, j);
		}
	}
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += syn_filter0.at<float>(0, k) * tmp0.at<float>(i + k, j);
				Tmp += syn_filter1.at<float>(0, k) * tmp1.at<float>(i + k, j);
			}
			pre_recon1.at<float>(i, j) = Tmp;
		}
	}
	//--------------------------------------------------------------------------//
	copyMakeBorder(pre_recon0, pre_recon0, L2, L1-L2, L2, L1 - L2, 0, 0);
	copyMakeBorder(pre_recon1, pre_recon1, L2, L1-L2, L2, L1 - L2, 0, 0);
	tmp0 = Mat::zeros(H + L1, W + L1, CV_32F);
	tmp1 = Mat::zeros(H + L1, W + L1, CV_32F);
	Mat recon_pad(H + L1, W + L1, CV_32F, Scalar(0));
	//--------------------------------------------------------------------------//
	for (int i = L2; i < H+L2; i++) {
		for (int j = L2; j < W / 2 + L2; j++) {
			tmp0.at<float>(i, 2 * j-L2) = pre_recon0.at<float>(i, j);
		}
	}
	for (int i = L2; i < H+L2; i++) {
		for (int j = L2; j < W / 2 + L2; j++) {
			tmp1.at<float>(i, 2 * j-L2) = pre_recon1.at<float>(i, j);
		}
	}
	for (int i = L2; i < H+L2; i++) {
		for (int j = 0; j < W; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += syn_filter0.at<float>(0, k) * tmp0.at<float>(i, j + k);
				Tmp += syn_filter1.at<float>(0, k) * tmp1.at<float>(i, j + k);
			}
			recon_pad.at<float>(i, j+L2) = Tmp;
		}
	}
	Mat recon = recon_pad(Rect(L2, L2, W, H));


	return recon;
	//return dst;
}



Mat waveletTransform2D(Mat src, Mat proto_type) {
	int H = src.rows;
	int W = src.cols;
	int L1 = proto_type.cols;
	int L2 = floor(L1 / 2)-1;
	float Tmp;
	Mat type1_filter(1, L1, CV_32F);
	for (int i = 0; i < L1; i++) {
		type1_filter.at<float>(0, i) = pow((-1), i) * proto_type.at<float>(0, i);
	}
	Mat src_pad;
	copyMakeBorder(src, src_pad, L2, L1-L2, L2, L1 - L2, 0, 0);
	Mat pre_dst(src_pad.size(), CV_32F, Scalar(0));
	Mat dst(H, W, CV_32F, Scalar(0));
	Tmp = 0;
	//appr
	for (int i = L2; i < H+ L2; i++) {
		for (int j = 0; j < W; j += 2) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += proto_type.at<float>(0, k) * src_pad.at<float>(i + 0, j + k);
			}
			pre_dst.at<float>(i, j / 2) = Tmp;
		}
	}
	for (int i = 0; i < H; i += 2) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += proto_type.at<float>(0, k) * pre_dst.at<float>(i + k, j + 0);
			}
			dst.at<float>(i / 2, j) = Tmp;
		}
	}
	//H detail
	for (int i = 0; i < H; i += 2) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += type1_filter.at<float>(0, k) * pre_dst.at<float>(i + k, j + 0);
			}
			dst.at<float>(i / 2, W / 2 + j) = Tmp;
		}
	}
	//----------------------------------------------------------------------------------------//
	//V detail
	for (int i = L2; i < H+ L2; i++) {
		for (int j = 0; j < W; j += 2) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += type1_filter.at<float>(0, k) * src_pad.at<float>(i + 0, j + k);
			}
			pre_dst.at<float>(i, j / 2) = Tmp;
		}
	}
	for (int i = 0; i < H; i += 2) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += proto_type.at<float>(0, k) * pre_dst.at<float>(i + k, j + 0);
			}
			dst.at<float>(H / 2 + i / 2, j) = Tmp;
		}
	}
	//D detail
	for (int i = 0; i < H; i += 2) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += type1_filter.at<float>(0, k) * pre_dst.at<float>(i + k, j + 0);
			}
			dst.at<float>(H / 2 + i / 2, W / 2 + j) = Tmp;
		}
	}
	return dst;
}

Mat waveletTransform2D_visual(Mat src, Mat proto_type) {
	int H = src.rows;
	int W = src.cols;
	int L1 = proto_type.cols;
	int L2 = floor(L1 / 2) - 1;
	float Tmp;
	Mat type1_filter(1, L1, CV_32F);
	for (int i = 0; i < L1; i++) {
		type1_filter.at<float>(0, i) = pow((-1), i) * proto_type.at<float>(0, i);
	}
	Mat src_pad;
	copyMakeBorder(src, src_pad, L2, L1 - L2, L2, L1 - L2, 0, 0);
	Mat pre_dst(src_pad.size(), CV_32F, Scalar(0));
	Mat dst(H, W, CV_32F, Scalar(0));
	Tmp = 0;
	//appr
	for (int i = L2; i < H + L2; i++) {
		for (int j = 0; j < W; j += 2) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += proto_type.at<float>(0, k) * src_pad.at<float>(i + 0, j + k);
			}
			pre_dst.at<float>(i, j / 2) = Tmp;
		}
	}
	for (int i = 0; i < H; i += 2) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += proto_type.at<float>(0, k) * pre_dst.at<float>(i + k, j + 0);
			}
			dst.at<float>(i / 2, j) = Tmp;
		}
	}
	//H detail
	for (int i = 0; i < H; i += 2) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += type1_filter.at<float>(0, k) * pre_dst.at<float>(i + k, j + 0);
			}
			dst.at<float>(i / 2, W / 2 + j) = Tmp;
		}
	}
	//----------------------------------------------------------------------------------------//
	//V detail
	for (int i = L2; i < H + L2; i++) {
		for (int j = 0; j < W; j += 2) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += type1_filter.at<float>(0, k) * src_pad.at<float>(i + 0, j + k);
			}
			pre_dst.at<float>(i, j / 2) = Tmp;
		}
	}
	for (int i = 0; i < H; i += 2) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += proto_type.at<float>(0, k) * pre_dst.at<float>(i + k, j + 0);
			}
			dst.at<float>(H / 2 + i / 2, j) = Tmp;
		}
	}
	//D detail
	for (int i = 0; i < H; i += 2) {
		for (int j = 0; j < W / 2; j++) {
			Tmp = 0;
			for (int k = 0; k < L1; k++) {
				Tmp += type1_filter.at<float>(0, k) * pre_dst.at<float>(i + k, j + 0);
			}
			dst.at<float>(H / 2 + i / 2, W / 2 + j) = Tmp;
		}
	}
	int C = dst.cols/2;
	int R = dst.rows/2;
	//Mat norm = dst(Rect(0, 0, C - 1, R - 1));
	//normalize(norm, norm, 1, 0, NORM_MINMAX);
	//norm = dst(Rect(C, 0, C - 1, R - 1));
	//normalize(norm, norm, 1, 0, NORM_MINMAX);
	//norm = dst(Rect(0, R, C - 1, R - 1));
	//normalize(norm, norm, 1, 0, NORM_MINMAX);
	//norm = dst(Rect(C, R, C - 1, R - 1));
	//normalize(norm, norm, 1, 0, NORM_MINMAX);
	Mat norm = dst(Rect(0, 0, C, R ));
	normalize(norm, norm, 1, 0, NORM_MINMAX);
	norm = dst(Rect(C, 0, C , R ));
	normalize(norm, norm, 1, 0, NORM_MINMAX);
	norm = dst(Rect(0, R, C , R));
	normalize(norm, norm, 1, 0, NORM_MINMAX);
	norm = dst(Rect(C, R, C , R ));
	normalize(norm, norm, 1, 0, NORM_MINMAX);

	return dst;
}
