#include "pch.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME	"Interaction area"

#include "cv_util.h"
#include "img2noteimg.h"

float v_bar0 = 100;
float v_bar1 = 140;
int v_bar2 = 60;

using namespace cv;
using namespace std;

cv::Mat p1(const cv::Mat& m)
{
	cv::Mat out = m.clone();
	int size_blur = 12;
	cv::blur(m, out, cv::Size(size_blur, size_blur));
	
	out = cv_util::kmean(out, 20);
	out = m.mul(out) / 255;
	out = cv_util::contrast(out, 12.0f);
	out = cv_util::gamma(out, 4);
	return out;
}

cv::Mat p2(const cv::Mat& m)
{
	cv::Mat out = m.clone();
	cv::cvtColor(m, out, cv::COLOR_RGB2GRAY);
	out.clone().convertTo(out, CV_16SC1);
	int size_blur = 12;
	//cv::blur(out, out, cv::Size(size_blur, size_blur));
	cv::Mat edge = out;
	
	//cv::Canny(out, edge, v_bar0, v_bar1);
	cv::Mat dx(edge.rows, edge.cols, CV_16SC1);
	cv::Mat dy(edge.rows, edge.cols, CV_16SC1);
	dx.setTo(v_bar2);
	short old_v = 0;
	for (int x = 0; x < dx.cols; ++x) {
		old_v = out.at<short>(0, x);
		for (int y = 0; y < dx.rows; ++y) {
		
			dy.at<short>(y, x) = out.at<short>(y, x) - old_v;
			old_v = out.at<short>(y, x);
		}
	}
	cv::Canny(dx, dy, edge, v_bar0, v_bar1);

	std::vector<cv::Mat> channels;
	channels.push_back(edge);
	channels.push_back(edge);
	channels.push_back(edge);

	cv::Mat ret;
	cv::merge(channels, ret);

	return ret;
}

int main(int argc, const char *argv[])
{
	cv::Mat frame = cv::Mat(1600, 900, CV_8UC3);

	// Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
	cvui::init(WINDOW_NAME);

	cv::VideoCapture cap(0);//デバイスのオープン
	if (!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
	{
		//読み込みに失敗したときの処理
		return -1;
	}


	cv::Mat m0 = cv_util::load_mat("./data/b.png");
	
	while (true) {


		cap.read(m0);
		cv::resize(m0, m0, cv::Size(m0.cols / 2, m0.rows / 2));
		
		// Fill the frame with a nice color
		frame = cv::Scalar(49, 92, 49);

		// Render a rectangle on the screen.
		cv::Rect rectangle(50, 50, 100, 100);
		cvui::rect(frame, rectangle.x, rectangle.y, rectangle.width, rectangle.height, 0xff0000);

		// Check what is the current status of the mouse cursor
		// regarding the previously rendered rectangle.
		int status = cvui::iarea(rectangle.x, rectangle.y, rectangle.width, rectangle.height);

		// cvui::iarea() will return the current mouse status:
		//  CLICK: mouse just clicked the interaction are
		//	DOWN: mouse button was pressed on the interaction area, but not released yet.
		//	OVER: mouse cursor is over the interaction area
		//	OUT: mouse cursor is outside the interaction area
		switch (status) {
		case cvui::CLICK:	std::cout << "Rectangle was clicked!" << std::endl; break;
		case cvui::DOWN:	cvui::printf(frame, 240, 70, "Mouse is: DOWN"); break;
		case cvui::OVER:	cvui::printf(frame, 240, 70, "Mouse is: OVER"); break;
		case cvui::OUT:		cvui::printf(frame, 240, 70, "Mouse is: OUT"); break;
		}

		// Show the coordinates of the mouse pointer on the screen
		cvui::printf(frame, 240, 50, "Mouse pointer is at (%d,%d)", cvui::mouse().x, cvui::mouse().y);

		// This function must be called *AFTER* all UI components. It does
		// all the behind the scenes magic to handle mouse clicks, etc.
		cvui::update();

		cv_util::paste(frame, m0, 10, 100);
		Mat edge = p2(m0);
		cv_util::paste(frame, edge, 10, 500);
		Mat edge8;
		cv_util::to_r(edge).convertTo(edge8, CV_8UC1);
		Mat noteimg0 = img2noteimg::edge2noteimg(edge8, 60, 36);
		cv_util::paste(frame, cv_util::to_rgb(noteimg0), 500, 10);

		// Show everything on the screen

		cvui::trackbar(frame, 10, 10, 200, &v_bar0, 0.0f, 1000.0f);
		cvui::trackbar(frame, 10, 60, 200, &v_bar1, 0.0f, 1000.0f);
		cvui::trackbar(frame, 10, 110, 200, &v_bar2, 0, 256);

		cv::imshow(WINDOW_NAME, frame);

		

		// Check if ESC key was pressed
		if (cv::waitKey(20) == 27)
		{
			break;
		}
	}

	return 0;
}