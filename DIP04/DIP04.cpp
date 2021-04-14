#include "utils.h"


int main(int argc, char* argv[])
{
	int implement = 3;
	if (implement == 1) {
		Mat src = imread("../images/circuitboard.tif", IMREAD_GRAYSCALE);
		if (src.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		imshow("original", src);
		Mat noise;

		noise = addSaltPepper(src, 0.25, 0.25);
		imshow("noise", noise);
		waitKey(0);

		Mat dst_contra, dst_meidan, dst_adamed;
		dst_contra = contraharmonic(noise, Size(3, 3), -1.5);
		imshow("Contraharmonic mean", dst_contra);
		waitKey(0);
		dst_contra = contraharmonic(noise, Size(3, 3), 0.0);
		imshow("Contraharmonic mean", dst_contra);
		waitKey(0);
		dst_contra = contraharmonic(noise, Size(3, 3), 1.5);
		imshow("Contraharmonic mean", dst_contra);
		waitKey(0);


			
		dst_meidan = median(noise, Size(3, 3));
		imshow("Median", dst_meidan);
		waitKey(0);
		dst_meidan = median(noise, Size(5, 5));
		imshow("Median", dst_meidan);
		waitKey(0);


		dst_adamed = adaptiveMedian(noise, Size(7, 7));
		imshow("Adaptive median", dst_adamed);
		waitKey(0);
		dst_adamed = adaptiveMedian(noise, Size(15, 15));
		imshow("Adaptive median", dst_adamed);
		waitKey(0);

	}

	else if (implement == 2) {
	Mat src = imread("../images/linearnoise.tif", IMREAD_GRAYSCALE);
	Mat notch = imread("../images/notch.tif", IMREAD_GRAYSCALE);
	if (src.empty() || notch.empty()) {
		cout << "Input image does not exist." << endl;
		exit(-1);
	}
	Mat tmp[2] = { notch, notch };
	merge(tmp, 2, notch);
	notch.convertTo(notch, CV_32F);
	normalize(notch, notch, 0, 1, NORM_MINMAX);

	Mat src_fft, dst_fft;
	fft2d(src, src_fft);
	fftshift2d(src_fft, src_fft);
	dst_fft = src_fft.mul(notch);

	ifftshift2d(dst_fft, dst_fft);
	Mat dst = ifft2d(dst_fft);
	normalize(dst, dst, 0, 255, NORM_MINMAX);
	dst = Mat_<uchar>(dst);

	imshow("Original", src);
	imshow("Notch", dst);
	}

	else if (implement == 0) {
		Mat src = imread("../images/linearnoise.tif", IMREAD_GRAYSCALE);
		Mat notch = imread("../images/notch.tif", IMREAD_GRAYSCALE);
		if (src.empty() || notch.empty()) {
			cout << "Input image does not exist." << endl;
			exit(-1);
		}
		imshow("Notch", notch);
		
		Mat tmp[2] = { notch, notch };
		merge(tmp, 2, notch);
		notch.convertTo(notch, CV_32F);
		normalize(notch, notch, 0, 1, NORM_MINMAX);
		Mat src_fft, dst_fft;
		fft2d(src, src_fft);
		fftshift2d(src_fft, src_fft);
		Mat temp[2];
		Mat img_mag;
		split(src_fft, temp);
		magnitude(temp[0], temp[1], img_mag);
		log(1 + img_mag, img_mag);
		normalize(img_mag, img_mag, 0, 1, NORM_MINMAX);
		imshow("img_mag1", img_mag);
		waitKey(0);
		dst_fft = src_fft.mul(notch);


		split(dst_fft, temp);
		magnitude(temp[0], temp[1], img_mag);
		log(1 + img_mag, img_mag);
		normalize(img_mag, img_mag, 0, 1, NORM_MINMAX);
		imshow("img_mag2", img_mag);
		waitKey(0);


		ifftshift2d(dst_fft, dst_fft);
		Mat dst = ifft2d(dst_fft);
		//cout << "M = " << endl << " " << dst << endl << endl;
		//cout << "M = " << endl << " " << dst << endl << endl;
		normalize(dst, dst, 0, 255, NORM_MINMAX);
		dst = Mat_<uchar>(dst);
		Mat dif = src - dst;

		imshow("Original", src);
		imshow("Notch", dst);
		imshow("diff", dif);
	}	

	else if (implement == 3) {
		Mat src1 = imread("../images/boy.tif", IMREAD_GRAYSCALE);
		//Mat src2 = imread("../images/text.tif", IMREAgD_GRAYSCALE);
		Mat src2 = imread("../images/text.tif", IMREAD_GRAYSCALE);
		if (src1.empty() || src2.empty()) {
			cout << "Input images do not exist." << endl;
			exit(-1);
		}
		float a = 0.1;
		float b = 0.1;
		float T = 1.0;

		Mat src1_blur = linearMotionBlur(src1, a, b, T);
		Mat src2_blur = linearMotionBlur(src2, a, b, T);

	/*
			Estimate system transfer function, H.
	*/
		Mat H1(src1.size(), CV_32FC2, Scalar::all(0));
				int H = src1.rows;
				int W = src1.cols;
				const float pi = 3.141592;

				float Temp;
				Mat spl_Tf[2];
				spl_Tf[0] = Mat::zeros(src1.size(), CV_32F);
				spl_Tf[1] = Mat::zeros(src1.size(), CV_32F);
				for (int i = 0; i < H; i++) {
					for (int j = 0; j < W; j++) {
						Temp = pi * (a * i + b * j);
						if (Temp == 0) {
							spl_Tf[0].at<float>(i, j) = 1;
							spl_Tf[1].at<float>(i, j) = 0;
						}
						else {
							spl_Tf[0].at<float>(i, j) = (T * sin(Temp) * cos(Temp)) / Temp;
							spl_Tf[1].at<float>(i, j) = -(T * sin(Temp) * sin(Temp)) / Temp;
						}
					}
				}
				merge(spl_Tf,2, H1);
		Mat H2(src2.size(), CV_32FC2, Scalar::all(0));
				H = src2.rows;
				W = src2.cols;
				spl_Tf[0] = Mat::zeros(src2.size(), CV_32F);
				spl_Tf[1] = Mat::zeros(src2.size(), CV_32F);
				for (int i = 0; i < H; i++) {
					for (int j = 0; j < W; j++) {
						Temp = pi * (a * i + b * j);
						if (Temp == 0) {
							spl_Tf[0].at<float>(i, j) = 1;
							spl_Tf[1].at<float>(i, j) = 0;
						}
						else {
							spl_Tf[0].at<float>(i, j) = (T * sin(Temp) * cos(Temp)) / Temp;
							spl_Tf[1].at<float>(i, j) = -(T * sin(Temp) * sin(Temp)) / Temp;
						}
					}
				}
				merge(spl_Tf, 2, H2);
		//Mat dst1 = wienerFilter(src1_blur, H1, 0.00110);
				Mat dst1 = wienerFilter(src1_blur, H1, 0.0002);

		Mat dst2 = wienerFilter(src2_blur, H2, 0.00007);
		float MSE1, MSE2;
		float PSNR1, PSNR2;
		MSE1 = 0;
		MSE2 = 0;
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				MSE1 += pow((src1.at<uchar>(i, j) - src1_blur.at<uchar>(i,j)), 2);
			}
		}
		MSE1 = MSE1 / (H*W);

		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				MSE2 += pow((src1.at<uchar>(i, j) - dst1.at<uchar>(i, j)), 2);
			}
		}
		MSE2 = MSE2 / (H*W);
		PSNR1 = 10 * log10((255 *255) / MSE1);
		PSNR2 = 10 * log10((255 *255) / MSE2);

		cout << "MSE1 = " << PSNR1 << endl;
		cout << "MSE2 = " << PSNR2 << endl;


		imshow("Boy_origin", src1);
		waitKey(0);
		imshow("Text_origin", src2);
		waitKey(0);
		 
		imshow("Boy_blur", src1_blur);
		waitKey(0);
		imshow("Text_blur", src2_blur);
		waitKey(0);

		imshow("Boy_restore", dst1);
		waitKey(0); 
		imshow("Text_restore", dst2);
		waitKey(0);

	}

	else if (implement == 4) {
		Mat src = imread("../images/book.tif", IMREAD_GRAYSCALE);
		imshow("Text_origin", src);
		Mat src2_blur = linearMotionBlur(src, 0.05, 0.1, 1.0);
		imshow("Text_blur", src2_blur);
		waitKey(0);

	}
	else if (implement == 9) {
		Mat src = imread("../images/book.tif", IMREAD_GRAYSCALE);
		imshow("Text_origin", src);
		float a = 0.1;
		float b = 0.1;
		float T = 1.0;

		Size padding_size = Size(2 * src.rows, 2 * src.cols);
		src.convertTo(src, CV_32F);//format transform
		Mat pad_src = Mat::zeros(padding_size, CV_32F);//padding
		imshow("img_mag0", pad_src);
		waitKey(0);
		for (int i = 0; i < src.rows; i++) {
			for (int j = 0; j < src.cols; j++) {
				pad_src.at<float>(i, j) = src.at<float>(i, j);
			}
		}
		pad_src.convertTo(pad_src, CV_8UC1);//format transform
		imshow("img_mag1", pad_src);
		waitKey(0);
		pad_src.convertTo(pad_src, CV_32F);//format transform
		for (int i = 0; i < pad_src.rows; i++) {	//move origin to center
			for (int j = 0; j < pad_src.cols; j++) {
				pad_src.at<float>(i, j) = pow(-1, i + j) * pad_src.at<float>(i, j);
			}
		}
		Mat img_mag;
		Mat temp[2];
		int H = pad_src.rows;
		int W = pad_src.cols;
		const float pi = 3.141592;
		float Temp;
		Mat Tmp[2];
		Tmp[0] = Mat::zeros(pad_src.size(), CV_32F);
		Tmp[1] = Mat::zeros(pad_src.size(), CV_32F);
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				Temp = pi * (a * (i+H/2) + b * (j+W/2));
				//Temp = pi * (a * i + b * j);
				if (Temp == 0) {
					Tmp[0].at<float>(i, j) = 1;
					Tmp[1].at<float>(i, j) = 0;
				}
				else {
					Tmp[0].at<float>(i, j) = (T * sin(Temp) * cos(Temp)) / Temp;
					Tmp[1].at<float>(i, j) = -(T * sin(Temp) * sin(Temp)) / Temp;
				}
			}
		}

		Mat Tf;// = { Tmp[0],Tmp[0] };
		merge(Tmp, 2, Tf);
		fftshift2d(Tf, Tf);
		normalize(Tf, Tf, 0, 1, NORM_MINMAX);



					split(Tf, temp);
					magnitude(temp[0], temp[1], img_mag);
					log(1 + img_mag, img_mag);
					normalize(img_mag, img_mag, 0, 1, NORM_MINMAX);
					imshow("img_mag2", img_mag);
					waitKey(0);
		Mat src_fft, dst_fft;
		fft2d(pad_src, src_fft);
		Mat spl_pad[2], spl_ker[2], spl_dst[2];
		spl_dst[0] = Mat::zeros(pad_src.size(), CV_32F);
		spl_dst[1] = Mat::zeros(pad_src.size(), CV_32F);
		float A, B, C, D;
		split(src_fft, spl_pad);
		split(Tf, spl_ker);
		for (int i = 0; i < pad_src.rows; i++) {
			for (int j = 0; j < pad_src.cols; j++) {
				A = spl_pad[0].at<float>(i, j);
				B = spl_pad[1].at<float>(i, j);
				C = spl_ker[0].at<float>(i, j);
				D = spl_ker[1].at<float>(i, j);
				spl_dst[0].at<float>(i, j) = A * C - B * D;
				spl_dst[1].at<float>(i, j) = A * D + B * C;
			}
		}
		Mat dst_tmp;
		merge(spl_dst, 2, dst_tmp);

					split(dst_tmp, temp);
					magnitude(temp[0], temp[1], img_mag);
					log(1 + img_mag, img_mag);
					normalize(img_mag, img_mag, 0, 1, NORM_MINMAX);
					imshow("img_mag3", img_mag);
					waitKey(0);

		ifft2d(dst_tmp, dst_tmp);
		split(dst_tmp, Tmp);
		Mat pad_dst = Tmp[0];
		Mat dst(Size(pad_src.rows / 2, pad_src.cols / 2), CV_32F);
		for (int i = 0; i < dst.rows; i++) {
			for (int j = 0; j < dst.cols; j++) {
				dst.at<float>(i, j) = pow(-1, i + j) * pad_dst.at<float>(i, j);
			}
		}

		//cout << "M = " << endl << " " << dst << endl << endl;

		normalize(dst, dst, 0, 255, NORM_MINMAX);
		dst = Mat_<uchar>(dst);

		//cout << "M = " << endl << " " << dst << endl << endl;

			imshow("Text_blur", dst);
			waitKey(0);

	}
	else if (implement == 5) {
	Mat src = imread("../images/book.tif", IMREAD_GRAYSCALE);
	imshow("Text_origin", src);
	float a = 0.5;
	float b = 0.0;
	float T = 1.0;

				int H = src.rows;
				int W = src.cols;
				const float pi = 3.141592;
				float Temp;
				//Generate frequency domain Transfer funcion
				Mat Tmp[2];
				Tmp[0] = Mat::ones(src.size(), CV_32F);
				Tmp[1] = Mat::ones(src.size(), CV_32F);
				for (int i = 0; i < H; i++) {
					for (int j = 0; j < W; j++) {
						//Tf.at<float>(i, j) = (T * sin(pi * (a * i + b * j)) * exp(-sqrt(-1)* pi * (a * i + b * j)))/( pi * (a * i + b * j));
						Temp = pi * (a * i + b * j);
						//Tmp[0].at<float>(i, j) = (T * sin(Temp) * cos(Temp)) / Temp;
						//Tmp[1].at<float>(i, j) = -(T * sin(Temp) * sin(Temp)) / Temp;
						Tmp[0].at<float>(i, j) = (T * sin(pi * (a * i + b * j)) * cos(pi * (a * i + b * j))) / pi * ((a * i + b * j));
						Tmp[1].at<float>(i, j) = -(T * sin(pi * (a * i + b * j)) * sin(pi * (a * i + b * j))) / pi * ((a * i + b * j));
					}
				}
				Mat Tf;
				merge(Tmp, 2, Tf);
				fftshift2d(Tf, Tf);
				split(Tf, Tmp);

				Mat src_fft, dst_fft;
				Mat src_fft_sp[2], dst_fft_sp[2];
				dst_fft_sp[0] = Mat::zeros(src.size(), CV_32F);
				dst_fft_sp[1] = Mat::zeros(src.size(), CV_32F);
				float r1, r2, i1, i2;
				fft2d(src, src_fft);
				fftshift2d(src_fft, src_fft);

				split(src_fft, src_fft_sp);
				for (int i = 0; i < src.rows; i++) {
					for (int j = 0; j < src.cols; j++) {
						r1 = src_fft_sp[0].at<float>(i, j);
						i1 = src_fft_sp[1].at<float>(i, j);
						r2 = Tmp[0].at<float>(i, j);
						i2 = Tmp[1].at<float>(i, j);
						dst_fft_sp[0].at<float>(i, j) = r1 * r2 - i1 * i2;
						dst_fft_sp[1].at<float>(i, j) = r1 * i2 + r2 * i1;
					}
				}

				Mat img_mag;
				magnitude(dst_fft_sp[0], dst_fft_sp[1], img_mag);
				log(1 + img_mag, img_mag);
				normalize(img_mag, img_mag, 0, 1, NORM_MINMAX);
				imshow("img_mag", img_mag);
				waitKey(0);


				merge(dst_fft_sp, 2, dst_fft);
				ifftshift2d(dst_fft, dst_fft);
				Mat dst = ifft2d(dst_fft);







	imshow("Text_blur", dst);
	waitKey(0);

	}
	else if (implement == 6) {
	Mat tmp[2];
	Mat img_mag;
	Mat src1 = imread("../images/book-cover.tif", IMREAD_GRAYSCALE);
	Mat src2 = imread("../images/book-cover-blurred.tif", IMREAD_GRAYSCALE);
	Mat src_fft, src_fft1, src_fft2, dst_fft;
	fft2d(src1, src_fft1);
	fftshift2d(src_fft1, src_fft1);
	fft2d(src2, src_fft2);
	fftshift2d(src_fft2, src_fft2);
	dst_fft = src_fft2/ src_fft1;


	//ifftshift2d(dst_fft, dst_fft);
	split(dst_fft, tmp);
	magnitude(tmp[0], tmp[1], img_mag);
	log(1 + img_mag, img_mag);
	normalize(img_mag, img_mag, 0, 1, NORM_MINMAX);
	imshow("img_mag", img_mag);
	waitKey(0);


	}
	else if (implement == 8) {
	Mat src = imread("../images/Tmp.tif", IMREAD_GRAYSCALE);
	Mat src_fft;
	fft2d(src, src_fft);
	Mat tmp[2];
	split(src_fft, tmp);




	cout << "M = " << endl << " " << tmp[1] << endl << endl;

	}

	waitKey(0);
	destroyAllWindows();
	return 0;
}