#include "pch.h"
#include "img2noteimg.h"
#include "cv_util.h"
using namespace cv;
using namespace std;
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

std::vector<pair<int, float>> img2noteimg::sum_scale_volume(const cv::Mat& m)
{
	if (m.type() != CV_8UC4) throw std::exception("sum_scale_volume needs CV_8UC4");
	vector<pair<int, float>> idx_sum;
	uchar* p = m.data;
	float sum[
	]
	for (int y = 0; y < m.rows; ++y) {
		float sum_x = 0;
		for (int x = 0; x < m.cols; ++x) {
			if (*(p + (y*m.cols + x) * 4 + 3) != 0) {
				sum_x += 1;
			}
		}
		idx_sum.push_back(make_pair((m.cols-y-1)%12, sum_x));
	}
	return idx_sum;
}

static int _sort_idx_sum(const pair<int, float>& a, const pair<int, float>& b)
{
	return (a.second - b.second) * 100000;
}

std::vector<int> img2noteimg::code_by_volume(const std::vector<int, float>& volume)
{
	vector<pair<int, float>> sorted = std::sort(volume.front, volume.end, _sort_idx_sum);
	vector<int> code;
	
	if (sorted.size() <= 3) {
		for (auto ite : sorted)
		{
			code.push_back(ite.first);
		}
		return code;
	}




}

cv::Mat img2noteimg::decimate_by_code(const cv::Mat& m, const std::vector<int>& code, float rate_outsider)
{

}