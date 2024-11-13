#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <opencv2/video/tracking.hpp>

using namespace std;
using namespace cv;

#define HEIGHT 360
#define WIDTH 640

namespace module1
{
	// Dimensiunea ferestrei de vizualizare
	const int WINDOW_WIDTH = 1280;
	const int WINDOW_HEIGHT = 720;

	// Funcție pentru a afișa o parte din canvas, în funcție de poziția de scroll
	void showCanvasRegion(const Mat& canvas, int x, int y);

	Mat filterFrame(Mat frame);

	void PrintFrames(string, int);

	void PrintWindow(Mat, int);

	void addFrame(Mat canvas, Mat frame, int i);
}
