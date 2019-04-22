#include "pch.h"
#include "cv_util.h"
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

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
