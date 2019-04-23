#include "pch.h"
#include "cv_util.h"
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

cv::Mat cv_util::load_mat(const std::string& path)
{
	cv::Mat mat = cv::imread(path);
	return mat;
}

void cv_util::paste(cv::Mat dst, cv::Mat src, int x, int y, int width, int height) {
	cv::Mat resized_img;
	cv::resize(src, resized_img, cv::Size(width, height));

	if (x >= dst.cols || y >= dst.rows) return;
	int w = (x >= 0) ? std::min(dst.cols - x, resized_img.cols) : std::min(std::max(resized_img.cols + x, 0), dst.cols);
	int h = (y >= 0) ? std::min(dst.rows - y, resized_img.rows) : std::min(std::max(resized_img.rows + y, 0), dst.rows);
	int u = (x >= 0) ? 0 : std::min(-x, resized_img.cols - 1);
	int v = (y >= 0) ? 0 : std::min(-y, resized_img.rows - 1);
	int px = std::max(x, 0);
	int py = std::max(y, 0);

	cv::Mat roi_dst = dst(cv::Rect(px, py, w, h));
	cv::Mat roi_resized = resized_img(cv::Rect(u, v, w, h));
	roi_resized.copyTo(roi_dst);
}

// 画像を画像に貼り付ける関数（サイズ指定を省略したバージョン）
void cv_util::paste(cv::Mat dst, cv::Mat src, int x, int y) {
	paste(dst, src, x, y, src.cols, src.rows);
}

cv::Mat cv_util::kmean(const cv::Mat& m, int num)
{
	cv::Mat m0(m.size(), m.type());

	// 画像配列を1次元に変換
	cv::Mat points;
	m.convertTo(points, CV_32FC3);
	points = points.reshape(3, m.rows*m.cols);

	// RGB空間でk-meansクラスタリングを実行
	cv::Mat_<int> clusters(points.size(), CV_32SC1);
	cv::Mat centers;

	// クラスタ数
	const int cluster = num;
	// k-meansnクラスタリングの実行
	kmeans(points, cluster, clusters, cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 10, 1.0), 1, cv::KmeansFlags::KMEANS_PP_CENTERS, centers);

	// 画素値をクラスタ中心値に置換
	cv::MatIterator_<cv::Vec3b> itd = m0.begin<cv::Vec3b>(), itd_end = m0.end<cv::Vec3b>();
	for (int i = 0; itd != itd_end; ++itd, ++i) {
		cv::Vec3f &color = centers.at<cv::Vec3f>(clusters(i), 0);
		(*itd)[0] = cv::saturate_cast<uchar>(color[0]);
		(*itd)[1] = cv::saturate_cast<uchar>(color[1]);
		(*itd)[2] = cv::saturate_cast<uchar>(color[2]);
	}
	return m0;
}

cv::Mat cv_util::contrast(const cv::Mat& m, float a)
{
	std::function< float(float) > f = [=](float i) { return 255.0f / (1 + exp(-a * (i - 128) / 255)); };
	return cv_util::lut(m, f);
}

cv::Mat cv_util::gamma(const cv::Mat& m, float a)
{
	std::function< float(float) > f = [=](float i) { return 255.0f * pow(i/255.0f, 1.0f/a); };
	return cv_util::lut(m, f);
}

cv::Mat cv_util::lut(const cv::Mat& m, std::function<float(float)> f_lut)
{
	cv::Mat m_clone = m.clone();
	std::vector<cv::Mat> channels;
	if (m_clone.channels() == 1)
	{
		channels.push_back(m_clone);
	}
	else if (m_clone.channels() >= 3)
	{
		cv::split(m_clone, channels);
	}

	cv::Mat lut(256, 1, CV_8UC1);
	for (int i = 0; i < 256; ++i)
	{
		lut.at<uchar>(i, 0) = f_lut(i);
	}
	int c = m_clone.channels() == 1 ? 1 : 3;

	for (int i = 0; i < c; ++i)
	{
		cv::Mat channel;
		cv::LUT(channels[i], lut, channels[i]);
	}

	cv::Mat ret;
	cv::merge(channels, ret);
	return ret;
}

std::string cv_util::get_mat_depth(const cv::Mat& mat)
{
	const int depth = mat.depth();

	switch (depth)
	{
	case CV_8U:  return "CV_8U";
	case CV_8S:  return "CV_8S";
	case CV_16U: return "CV_16U";
	case CV_16S: return "CV_16S";
	case CV_32S: return "CV_32S";
	case CV_32F: return "CV_32F";
	case CV_64F: return "CV_64F";
	default:
		return "Invalid depth type of matrix!";
	}
}

std::string cv_util::get_mat_type(const cv::Mat& mat)
{
	const int mtype = mat.type();

	switch (mtype)
	{
	case CV_8UC1:  return "CV_8UC1";
	case CV_8UC2:  return "CV_8UC2";
	case CV_8UC3:  return "CV_8UC3";
	case CV_8UC4:  return "CV_8UC4";

	case CV_8SC1:  return "CV_8SC1";
	case CV_8SC2:  return "CV_8SC2";
	case CV_8SC3:  return "CV_8SC3";
	case CV_8SC4:  return "CV_8SC4";

	case CV_16UC1: return "CV_16UC1";
	case CV_16UC2: return "CV_16UC2";
	case CV_16UC3: return "CV_16UC3";
	case CV_16UC4: return "CV_16UC4";

	case CV_16SC1: return "CV_16SC1";
	case CV_16SC2: return "CV_16SC2";
	case CV_16SC3: return "CV_16SC3";
	case CV_16SC4: return "CV_16SC4";

	case CV_32SC1: return "CV_32SC1";
	case CV_32SC2: return "CV_32SC2";
	case CV_32SC3: return "CV_32SC3";
	case CV_32SC4: return "CV_32SC4";

	case CV_32FC1: return "CV_32FC1";
	case CV_32FC2: return "CV_32FC2";
	case CV_32FC3: return "CV_32FC3";
	case CV_32FC4: return "CV_32FC4";

	case CV_64FC1: return "CV_64FC1";
	case CV_64FC2: return "CV_64FC2";
	case CV_64FC3: return "CV_64FC3";
	case CV_64FC4: return "CV_64FC4";

	default:
		return "Invalid type of matrix!";
	}
}

cv::Mat cv_util::to_r(const cv::Mat& m)
{
	std::vector<Mat> channels;
	cv::split(m, channels);
	return channels[0];
}

cv::Mat cv_util::to_rgba(const cv::Mat& m)
{
	if (m.depth() != CV_8U) throw std::exception("to_rgba needs CV_8U");

	int num = m.channels();
	if (num == 4) return m;
	if (num == 2) throw std::exception("to_rgba invalid channel num 2");
	std::vector<Mat> channels;
	cv::split(m, channels);
	if (num == 3) {
		Mat alpha = channels[0].clone();
		alpha.setTo(255);
		channels.push_back(alpha);
	}
	if (num == 1) {
		channels.push_back(m.clone());
		channels.push_back(m.clone());
		Mat alpha = channels[0].clone();
		alpha.setTo(255);
		channels.push_back(alpha);
	}
	Mat ret;
	cv::merge(channels, ret);
	return ret;
}

cv::Mat cv_util::to_rgb(const cv::Mat& m)
{
	if (m.depth() != CV_8U) throw std::exception("to_rgba needs CV_8U");

	int num = m.channels();
	if (num == 3) return m;
	if (num == 2) throw std::exception("to_rgba invalid channel num 2");
	std::vector<Mat> channels;
	cv::split(m, channels);
	if (num == 4) {
		channels.pop_back();
	}
	if (num == 1) {
		channels.push_back(m.clone());
		channels.push_back(m.clone());
	}
	Mat ret;
	cv::merge(channels, ret);
	return ret;
}