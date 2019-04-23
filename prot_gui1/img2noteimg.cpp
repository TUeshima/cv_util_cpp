#include "pch.h"
#include "img2noteimg.h"
#include "cv_util.h"
using namespace cv;

cv::Mat img2noteimg::edge2noteimg(const cv::Mat& m, int w, int h)
{
	if (m.type() != CV_8UC1) throw std::exception("edge2noteimg needs CV_8UC1.");

	int h_unit = m.rows / h;
	int w_unit = m.cols / w;
	Mat ret(h, w, CV_8UC4);
	ret.setTo(Scalar(0, 0, 0, 0));
	uchar* p = m.data;

	for (int y = 0; y < h; ++y) {
		int hh_unit = (y + 1) * (h_unit) >= m.rows ? m.rows % h_unit : h_unit;
		for (int x = 0; x < w; ++x) {
			int ww_unit = (x + 1) * (w_unit) >= m.cols ? m.cols% w_unit : w_unit;
			bool flag_break = false;
			for (int yy = 0; yy < hh_unit; ++yy) {
				for (int xx = 0; xx < ww_unit; ++xx) {
					if (*(p + (y*h_unit + yy)*m.cols + x * w_unit + xx) != 0) {
						flag_break = true;
						ret.at<Vec4b>(y, x) = Scalar(255, 255, 255, 255);
						break;
					}
					if (flag_break) break;
				}
			}
		}
	}

	return ret;
}