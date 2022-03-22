#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

std::vector<int> LUT(256,0);

void countLUT(std::vector<int>& LUT) {
	for (int i = 0; i < 256; i++)
	{
		LUT[i] = 255 * std::sin(double(i) / 200.0);
	}
}

cv::Mat changeBrightnessGS(cv::Mat grayscalePic) {
	cv::Mat result = grayscalePic.clone();
	for (int i = 0; i < result.rows; i++) {
		for (int j = 0; j < result.cols; j++) {
			result.at<uint8_t>(i, j) = LUT[result.at<uint8_t>(i, j)];
		}
	}
	return result;
}

cv::Mat changeBrightness(cv::Mat defaultPic) {
	cv::Mat channels[3], result;
	cv::split(defaultPic, channels);
	channels[0] = changeBrightnessGS(channels[0]);
	channels[1] = changeBrightnessGS(channels[1]);
	channels[2] = changeBrightnessGS(channels[2]);
	cv::merge(channels, 3, result);
	return result;
}

int main() {
	cv::Mat originalPic = cv::imread("../../../data/cross_0256x0256.png");
	cv::imwrite("lab03_rgb.png", originalPic);
	countLUT(LUT);
	//LUT graphic
	cv::Mat functionGraph(512, 512, CV_8UC1, cv::Scalar(255, 255, 255));
	for (int i = 0; i < functionGraph.cols - 1; i++) {
		cv::line(functionGraph,
			cv::Point(i, functionGraph.rows - LUT[i / 2] * 2),
			cv::Point(i + 1, functionGraph.rows - LUT[(i + 1) / 2] * 2),
			cv::Scalar(0, 0, 0), 1, 8, 0);
	}
	cv::line(functionGraph,
		cv::Point(0, 0),
		cv::Point(0, functionGraph.rows),
		cv::Scalar(0, 0, 0), 4, 8, 0);
	cv::line(functionGraph,
		cv::Point(0, functionGraph.cols),
		cv::Point(functionGraph.cols, functionGraph.cols),
		cv::Scalar(0, 0, 0), 4, 8, 0);
	cv::imwrite("lab03_viz_func.png", functionGraph);
	//generating grayscale picture
	cv::Mat grayscalePic;
	cv::cvtColor(originalPic, grayscalePic, cv::COLOR_BGR2GRAY);
	cv::imwrite("lab03_gre.png", grayscalePic);
	changeBrightnessGS(grayscalePic);
	//converting brigthness for gs pic
	cv::imwrite("lab03_gre_res.png", changeBrightnessGS(grayscalePic));
	//converting brigthness for original pic
	cv::imwrite("lab03_rgb_res.png", changeBrightness(originalPic));

}