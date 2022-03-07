#include <opencv2/opencv.hpp>
#include <vector>

cv::Mat makeMonochromeMosaic(cv::Mat img) {
	cv::Mat channels[3];
	cv::split(img, channels);
	cv::Mat zeroChannel(256, 256, CV_8UC1);
	zeroChannel = 0;
	cv::Mat monochromeBlue, monochromeGreen, monochromeRed;
	cv::Mat channelBlue[3] = { channels[0], zeroChannel, zeroChannel };
	cv::merge(channelBlue, 3, monochromeBlue);
	cv::Mat channelGreen[3] = { zeroChannel, channels[1], zeroChannel };
	cv::merge(channelGreen, 3, monochromeGreen);
	cv::Mat channelRed[3] = { zeroChannel, zeroChannel, channels[2] };
	cv::merge(channelRed, 3, monochromeRed);
	cv::Mat matLeft[] = { img, monochromeGreen };
	cv::Mat matRight[] = { monochromeRed, monochromeBlue };
	cv::Mat outLeft, outRight, out;
	cv::vconcat(matLeft, 2, outLeft);
	cv::vconcat(matRight, 2, outRight);
	cv::Mat matOut[] = { outLeft, outRight };
	cv::hconcat(matOut, 2, out);
	return out;
}

void getBrightness(cv::Mat img, std::vector<int>& pixel_array) {
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			pixel_array[img.at<uchar>(i, j)]++;
		}
	}
}

cv::Mat createHist(cv::Mat img) {
	cv::Mat histogram(256, 256, CV_8UC3, cv::Scalar(240, 240, 240));
	std::vector<int> blue_pixels_count(256, 0);
	std::vector<int> green_pixels_count(256, 0);
	std::vector<int> red_pixels_count(256, 0);
	cv::Mat channels[3];
	cv::split(img, channels);
	getBrightness(channels[0], blue_pixels_count);
	getBrightness(channels[1], green_pixels_count);
	getBrightness(channels[2], red_pixels_count);
	//getting max height
	int max = 0;
	for (int i = 0; i < 256; i++) {
		if (max < blue_pixels_count[i]) {
			max = blue_pixels_count[i];
		}
	}
	for (int i = 0; i < 256; i++) {
		if (max < green_pixels_count[i]) {
			max = green_pixels_count[i];
		}
	}
	for (int i = 0; i < 256; i++) {
		if (max < red_pixels_count[i]) {
			max = red_pixels_count[i];
		}
	}
	//downscaling considering max height
	for (int i = 0; i < 256; i++) {
		blue_pixels_count[i] = ((double)blue_pixels_count[i] / max) * histogram.rows;
	}
	for (int i = 0; i < 256; i++) {
		green_pixels_count[i] = ((double)green_pixels_count[i] / max) * histogram.rows;
	}
	for (int i = 0; i < 256; i++) {
		red_pixels_count[i] = ((double)red_pixels_count[i] / max) * histogram.rows;
	}

	//drawing lines in histogram
	for (int i = 0; i < 255; i++)
	{
		cv::line(histogram, cv::Point(i, histogram.rows - blue_pixels_count[i]),
			cv::Point(i + 1, histogram.rows - blue_pixels_count[i + 1]),
			cv::Scalar(255, 0, 0), 1, 8, 0);
	}
	for (int i = 0; i < 255; i++)
	{
		cv::line(histogram, cv::Point(i, histogram.rows - green_pixels_count[i]),
			cv::Point(i + 1, histogram.rows - green_pixels_count[i + 1]),
			cv::Scalar(0, 255, 0), 1, 8, 0);
	}
	for (int i = 0; i < 255; i++)
	{
		cv::line(histogram, cv::Point(i, histogram.rows - red_pixels_count[i]),
			cv::Point(i + 1, histogram.rows - red_pixels_count[i + 1]),
			cv::Scalar(0, 0, 255), 1, 8, 0);
	}
	return histogram;
}

int main() {
	cv::Mat imgPNG;
	imgPNG = cv::imread("../../../data/cross_0256x0256.png");
	cv::imwrite("cross_0256x0256_025.jpg", imgPNG, { cv::IMWRITE_JPEG_QUALITY, 25});
	cv::Mat imgJPG = cv::imread("cross_0256x0256_025.jpg");
	cv::imwrite("cross_0256x0256_png_channels.png", makeMonochromeMosaic(imgPNG));
	cv::imwrite("cross_0256x0256_jpg_channels.png", makeMonochromeMosaic(imgJPG));
	cv::Mat histPNG = createHist(imgPNG);
	cv::Mat histJPG = createHist(imgJPG);
	cv::Mat histOut, pictures, res;
	cv::hconcat(imgPNG, imgJPG, pictures);
	cv::hconcat(histPNG, histJPG, histOut);
	cv::vconcat(pictures, histOut, res);
	cv::imwrite("cross_0256x0256_hists.png", res);
}