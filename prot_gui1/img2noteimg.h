#pragma once

#include "opencv2/opencv.hpp"
class img2noteimg
{
public:
	static cv::Mat edge2noteimg(const cv::Mat& m, int w, int h);
	static std::vector<pair<int, float>> sum_scale_volume(const cv::Mat& m);
	static std::vector<int> code_by_volume(const std::vector<int, float>& volume);
	static cv::Mat decimate_by_code(const cv::Mat& m, const std::vector<int>& code, float rate_outsider);
};

