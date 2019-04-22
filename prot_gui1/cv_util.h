#pragma once
#include "opencv2/core.hpp"
#include <string>

class cv_util
{
public:
	static cv::Mat load_mat(const std::string& path);
	static void paste(cv::Mat dst, cv::Mat src, int x, int y, int width, int height);
	static void paste(cv::Mat dst, cv::Mat src, int x, int y);
};

