# include "utils.h"
#include "time.h"

int main(int argc, char* argv[]){
{

	time_t start, end;
	{}
	int imp = 0;
	//
	if (imp == 0) {
	}
	//
	else if (imp == 1) {
		
		//Mat src = imread("../../Img_set/DIP4E/iris-RGB.tif", IMREAD_GRAYSCALE);
		Mat src = imread("../../Img_set/DIP4E/building-1200by1200.tif", IMREAD_GRAYSCALE);
		//Mat src = imread("../../Img_set/textbook/CH03/Fig0316(3)(third_from_top).tif", IMREAD_GRAYSCALE);
		if (src.empty()) {
			cout << "Input images do not exist." << endl;
			exit(-1);
		}
		Mat gau_pyr2, gau_pyr1, gau_pyr0;
		Mat lap_pyr3, lap_pyr2, lap_pyr1;
		pyr_Down(src, gau_pyr2, lap_pyr3);
		pyr_Down(gau_pyr2, gau_pyr1, lap_pyr2);
		pyr_Down(gau_pyr1, gau_pyr0, lap_pyr1);

		imshow("original", src);
		imshow("gaussian_pyramid_level2", gau_pyr2);
		imshow("gaussian_pyramid_level1", gau_pyr1);
		imshow("gaussian_pyramid_level0", gau_pyr0);

		gau_pyr2 = gau_pyr2 * 255;
		gau_pyr2 = Mat_<uchar>(gau_pyr2);
		gau_pyr1 = gau_pyr1 * 255;
		gau_pyr1 = Mat_<uchar>(gau_pyr1);
		gau_pyr0 = gau_pyr0 * 255;
		gau_pyr0 = Mat_<uchar>(gau_pyr0);
		Mat edge_lev3 = Laplacian_filtering(src);
		Mat edge_lev2 = Laplacian_filtering(gau_pyr2);
		Mat edge_lev1 = Laplacian_filtering(gau_pyr1);
		Mat edge_lev0 = Laplacian_filtering(gau_pyr0);
		normalize(edge_lev3, edge_lev3, 255, 0, NORM_MINMAX);
		normalize(edge_lev2, edge_lev2, 255, 0, NORM_MINMAX);
		normalize(edge_lev1, edge_lev1, 255, 0, NORM_MINMAX);
		normalize(edge_lev0, edge_lev0, 255, 0, NORM_MINMAX);
		imshow("edge detect_LV3", edge_lev3);
		imshow("edge detect_LV2", edge_lev2);
		imshow("edge detect_LV1", edge_lev1);
		imshow("edge detect_LV0", edge_lev0);



		waitKey(0);
		
	}
}

{
	int pointer = 6;
	if (pointer == 99) {
	}
	/*
	use image vase, lena
	gen image pyramid(3 level)
	gaussian blur size = (5,5), sigma = 1*/
	else if (pointer == 1) {
		Mat src = imread("../../Img_set/DIP4E/vase.tif", IMREAD_GRAYSCALE);
		if (src.empty()) {
			cout << "Input images do not exist." << endl;
			exit(-1);
		}
		Mat gau_pyr2, gau_pyr1, gau_pyr0;
		Mat lap_pyr3, lap_pyr2, lap_pyr1;
		pyr_Down(src, gau_pyr2, lap_pyr3);
		pyr_Down(gau_pyr2, gau_pyr1, lap_pyr2);
		pyr_Down(gau_pyr1, gau_pyr0, lap_pyr1);

		imshow("original", src);
		imshow("gaussian_pyramid_level2", gau_pyr2);
		imshow("gaussian_pyramid_level1", gau_pyr1);
		imshow("gaussian_pyramid_level0", gau_pyr0);
		imshow("laplacian pyramid_level3", lap_pyr3);
		imshow("laplacian pyramid_level2", lap_pyr2);
		imshow("laplacian pyramid_level1", lap_pyr1);

		imwrite("original_level3.tif", src);
		imwrite("gaussian_pyramid_level2.tif", gau_pyr2);
		imwrite("gaussian_pyramid_level1.tif", gau_pyr1);
		imwrite("gaussian_pyramid_level0.tif", gau_pyr0);

		imwrite("laplacian pyramid_level3.tif", lap_pyr3);
		imwrite("laplacian pyramid_level2.tif", lap_pyr2);
		imwrite("laplacian pyramid_level1.tif", lap_pyr1);


		waitKey(0);
	}

	/*haar matrix*/
	else if (pointer == 2) {
		//Mat src = imread("../../Img_set/DIP4E/vase.tif", IMREAD_GRAYSCALE);
		Mat src  = Mat::ones(8,8,CV_32F)/2;

		if (src.empty()) {
			cout << "Input images do not exist." << endl;
			exit(-1);
		}
		//gen harr matrix
		//int H = src.rows;
		//int W = src.cols;
		int H = 8;
		int W = 8;
		float p, q;
		float z;
		Mat haar = Mat::zeros(H, W, CV_32F);
		haar.row(0) = Mat::ones(1, W, CV_32F);
		for (int i = 1; i < H; i++) {
			p = floor(log2f(i));
			q = i + 1 - pow(2, p);
			for (int j = 0; j < W; j++) {
				z = (float) j / W;
				if ((((q - 1) / pow(2, p)) <= z) && (((q - 0.5) / pow(2, p)) > z)) {
					haar.at<float>(i, j) = pow(2, p / 2);
				}
				else if ((((q - 0.5) / pow(2, p)) <= z) && (((q) / pow(2, p)) > z)) {
					haar.at<float>(i, j) = (-1) * pow(2, p / 2);
				}
			}
		}
		
		//src.convertTo(src, CV_32F);
		// = src / 255;
		haar = haar / sqrt(W);
		Mat dst = haar * src * haar.t();
		Mat haar_T = haar.t();
		Mat reconst = haar.inv() * dst * haar_T.inv();
		
		imshow("dst", dst);
		imshow("src", src);
		imshow("reconst", reconst);
		
		Mat dst_sample1D;
		dst(Rect(src.cols / 2, src.rows / 2, src.cols / 2, src.rows / 2)).copyTo(dst_sample1D);
		normalize(dst_sample1D, dst_sample1D, 1, 0, NORM_MINMAX);
		Mat dst_sample2D;
		dst(Rect(src.cols / 4, src.rows / 4, src.cols / 4, src.rows / 4)).copyTo(dst_sample2D);
		normalize(dst_sample2D, dst_sample2D, 1, 0, NORM_MINMAX);
		Mat dst_sample3D;
		dst(Rect(src.cols / 8, src.rows / 8, src.cols / 8, src.rows / 8)).copyTo(dst_sample3D);
		normalize(dst_sample3D, dst_sample3D, 1, 0, NORM_MINMAX);
		Mat dst_sample4D;
		dst(Rect(src.cols / 16, src.rows / 16, src.cols / 16, src.rows / 16)).copyTo(dst_sample4D);
		normalize(dst_sample4D, dst_sample4D, 1, 0, NORM_MINMAX);
		Mat dst_sampleA;
		dst(Rect(0, 0, src.cols / 16, src.rows / 16)).copyTo(dst_sampleA);
		normalize(dst_sampleA, dst_sampleA, 1, 0, NORM_MINMAX);
		Mat dst_sampleH;
		dst(Rect(src.cols/2, src.rows/8, src.cols / 2, src.rows / 2- src.rows / 8)).copyTo(dst_sampleH);
		normalize(dst_sampleH, dst_sampleH, 1, 0, NORM_MINMAX);

		imshow("dst_sam", dst);
		}
	

	/*2D-discrete wavelet transform*/
	else if (pointer == 3) {
	//Mat src = imread("../../Img_set/edge-test.tif", IMREAD_GRAYSCALE);
	//Mat src = imread("../../Img_set/DIP4E/testpattern512.tif", IMREAD_GRAYSCALE);
	Mat src = imread("../../Img_set/textbook/CH07/Fig0723(a).tif", IMREAD_GRAYSCALE);
	if (src.empty()) {
			cout << "Input images do not exist." << endl;
			exit(-1);
		}
		//Mat proto_type = Mat_<float>({ 1,2 }, { 0.707, 0.707 });
		//Mat proto_type = Mat_<float>({ 1,4 }, {0.483, 0.837, 0.224, -0.129});
		Mat proto_type = Mat_<float>({ 1,8 }, {0.0322, -0.0126, -0.0992, 0.2979, 0.8037, 0.4976, -0.0296, -0.0758});//symlet

		src.convertTo(src, CV_32F);
		src = src / 255;
		Mat dst = waveletTransform2D_visual(src, proto_type);
		//imwrite("vase_haar_LV1.tif", dst);
		//waitKey(0);
		Mat next = dst(Rect(0, 0, src.cols / 2, src.rows / 2));
		Mat dst2 = waveletTransform2D_visual(next, proto_type);
		dst2.copyTo(dst(Rect(0, 0, src.cols / 2, src.rows / 2)));
		imwrite("hidden_symbols_haar_LV2.tif", dst);
		Mat pad2 = Mat::zeros(src.rows / 4, src.cols / 4, CV_32F);
		pad2.copyTo(dst(Rect(0, 0, src.cols / 4, src.rows / 4)));
		imwrite("hidden_symbols_haar_LV2_pro.tif", dst);
		//next = dst(Rect(0, 0, src.cols / 4, src.rows / 4));
		//Mat dst3 = waveletTransform2D_visual(next, proto_type);
		//dst3.copyTo(dst(Rect(0, 0, src.cols / 4, src.rows / 4)));
		//imwrite("testpattern_sym_LV3.tif", dst);
		imshow("dst", dst);

	}
	else if (pointer == 4) {
	//Mat src = imread("../../Img_set/DIP4E/astronaut-interference.tif", IMREAD_GRAYSCALE);
	Mat src = imread("../../Img_set/DIP4E/test-pattern-gamma-noise.tif", IMREAD_GRAYSCALE);
	//Mat src = imread("../../Img_set/DIP4E/testpattern512.tif", IMREAD_GRAYSCALE);
	//Mat src = imread("../../Img_set/textbook/CH07/Fig0723(a).tif", IMREAD_GRAYSCALE);
	if (src.empty()) {
		cout << "Input images do not exist." << endl;
		exit(-1);
	}
	Mat proto_type = Mat_<float>({ 1,2 }, { 0.707, 0.707 });
	//Mat proto_type = Mat_<float>({ 1,4 }, {0.483, 0.837, 0.224, -0.129});
	//Mat proto_type = Mat_<float>({ 1,8 }, {0.0322, -0.0126, -0.0992, 0.2979, 0.8037, 0.4976, -0.0296, -0.0758});//symlet

	src.convertTo(src, CV_32F);
	src = src / 255;
	Mat dst = waveletTransform2D(src, proto_type);

	Mat next = dst(Rect(0, 0, src.cols / 2, src.rows / 2));
	Mat dst2 = waveletTransform2D(next, proto_type);
	dst2.copyTo(dst(Rect(0, 0, src.cols / 2, src.rows / 2)));

	next = dst(Rect(0, 0, src.cols / 4, src.rows / 4));
	Mat dst3 = waveletTransform2D(next, proto_type);
	dst3.copyTo(dst(Rect(0, 0, src.cols / 4, src.rows / 4)));

	//next = dst(Rect(0, 0, src.cols / 8, src.rows / 8));
	//Mat dst4 = waveletTransform2D(next, proto_type);
	//dst4.copyTo(dst(Rect(0, 0, src.cols / 8, src.rows / 8)));

	//next = dst(Rect(0, 0, src.cols / 16, src.rows / 16));
	//Mat dst5 = waveletTransform2D(next, proto_type);
	//dst5.copyTo(dst(Rect(0, 0, src.cols / 16, src.rows / 16)));

	//next = dst(Rect(0, 0, src.cols / 32, src.rows / 32));
	//Mat dst6 = waveletTransform2D(next, proto_type);
	//dst6.copyTo(dst(Rect(0, 0, src.cols / 32, src.rows / 32)));


	Mat pad1 = Mat::zeros(src.rows / 2, src.cols / 2, CV_32F);
	Mat pad2 = Mat::zeros(src.rows / 4, src.cols / 4, CV_32F);
	Mat pad3 = Mat::zeros(src.rows / 8, src.cols / 8, CV_32F);
	//Mat pad4 = Mat::zeros(src.rows / 16, src.cols / 16, CV_32F);
	//Mat pad5 = Mat::zeros(src.rows / 32, src.cols / 32, CV_32F);
	//Mat pad6 = Mat::zeros(src.rows / 64, src.cols / 64, CV_32F);

	//pad2.copyTo(dst(Rect(0, 0, src.cols / 4, src.rows / 4)));
	//pad6.copyTo(dst(Rect(src.cols / 64, 0, src.cols / 64, src.rows / 64)));
	//pad5.copyTo(dst(Rect(src.cols / 32, 0, src.cols / 32, src.rows / 32)));
	//pad4.copyTo(dst(Rect(src.cols / 16, 0, src.cols / 16, src.rows / 16)));

	//pad3.copyTo(dst(Rect(0, src.rows / 8, src.cols / 8, src.rows / 8)));
	//pad3.copyTo(dst(Rect(src.cols / 8, 0, src.cols / 8, src.rows / 8)));
	pad3.copyTo(dst(Rect(src.cols / 8, src.rows/8, src.cols / 8, src.rows / 8)));

	//pad2.copyTo(dst(Rect(0, src.rows / 4, src.cols / 4, src.rows / 4)));
	//pad2.copyTo(dst(Rect(src.cols / 4, 0, src.cols / 4, src.rows / 4)));
	pad2.copyTo(dst(Rect(src.cols / 4, src.rows / 4, src.cols / 4, src.rows / 4)));

	//pad1.copyTo(dst(Rect(0, src.rows / 2, src.cols / 2, src.rows / 2)));
	//pad1.copyTo(dst(Rect(src.cols / 2, 0, src.cols / 2, src.rows / 2)));
	pad1.copyTo(dst(Rect(src.cols / 2, src.rows / 4, src.cols / 2, src.rows / 2)));


	Mat wavelet;
	dst.copyTo(wavelet);


	Mat recon = dewaveletsyn(dst, proto_type, 3);
	normalize(wavelet, wavelet, 1, 0, NORM_MINMAX);
	//pad2.copyTo(wavelet(Rect(0, 0, src.cols / 4, src.rows / 4)));

	imshow("wavelet", wavelet);
	normalize(recon, recon, 1, 0, NORM_MINMAX);
	imwrite("test-pattern-gamma-noise_haar_remove_V.tif", recon);
	imshow("recon", recon);
	}

	else if (pointer == 5) {}

	else if (pointer == 6) {
	//Mat src = imread("../../Img_set/DIP4E/hurricane-Andrew.tif", IMREAD_GRAYSCALE);
	//Mat src = imread("../test.tif", IMREAD_GRAYSCALE);
	//Mat src = Mat_<uchar>({ 4,4 }, { 72,84,98,72,84,98,105,84,105,72,84,128,84,98,72,98 });
	Mat src = Mat_<uchar>({ 4,4 }, { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 });
	if (src.empty()) {
		cout << "Input images do not exist." << endl;
		exit(-1);
	}
	//src = src(Rect(0, 0, 4, 4));


	Mat dst = huffman(src);
	imshow("dst", dst);
	waitKey(0);
	}


	else if (pointer == 7) {
	Mat src = imread("../../Img_set/DIP4E/lena.tif", IMREAD_GRAYSCALE);
	//Mat src = imread("../test.tif", IMREAD_GRAYSCALE);
	//Mat src = Mat_<uchar>({ 4,4 }, { 72,84,98,72,84,98,105,84,105,72,84,128,84,98,72,98 });
	if (src.empty()) {
		cout << "Input images do not exist." << endl;
		exit(-1);
	}
	//src = src(Rect(0, 0, 4, 4));


	Mat dst = huffman(src);
	imshow("dst", dst);
	waitKey(0);
	}

	


}

	waitKey(0);
	destroyAllWindows();
}

