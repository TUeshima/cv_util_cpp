#pragma once

#include "opencv2/opencv.hpp"
class img2noteimg
{
public:
	static cv::Mat edge2noteimg(const cv::Mat& m, int w, int h);
};

