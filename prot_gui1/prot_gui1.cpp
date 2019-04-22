#include "pch.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define CVUI_IMPLEMENTATION
#include "cvui.h"
#define WINDOW_NAME	"Interaction area"

#include "cv_util.h"

int main(int argc, const char *argv[])
{
	cv::Mat frame = cv::Mat(300, 600, CV_8UC3);

	// Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
	cvui::init(WINDOW_NAME);


	cv::Mat m0 = cv_util::load_mat("./data/a.png");

	while (true) {
		// Fill the frame with a nice color
		frame = cv::Scalar(49, 52, 49);

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

		cv_util::paste(frame, m0, 10, 10);

		// Show everything on the screen
		cv::imshow(WINDOW_NAME, frame);

		

		// Check if ESC key was pressed
		if (cv::waitKey(20) == 27) {
			break;
		}
	}

	return 0;
}