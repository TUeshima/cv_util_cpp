#pragma once
#include "opencv2/core.hpp"
#include <string>

class cv_util
{
public:
	static cv::Mat load_mat(const std::string& path);
	static void paste(cv::Mat dst, cv::Mat src, int x, int y, int width, int height);
	static void paste(cv::Mat dst, cv::Mat src, int x, int y);
	static cv::Mat kmean(const cv::Mat& m, int num);

	static cv::Mat contrast(const cv::Mat& m, float a);
	static cv::Mat gamma(const cv::Mat& m, float a);
	static cv::Mat lut(const cv::Mat& m, std::function<float(float)> f_lut);

};
