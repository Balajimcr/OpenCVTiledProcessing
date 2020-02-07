// TiledAlgorithm.cpp : Defines the entry point for the console application.

#include <iostream>
#include "opencv2/opencv.hpp"
#include <opencv2\ximgproc.hpp>
#include "ProcessTileBased.h"
#include "Timer.h"
#include <iostream>

using namespace cv;
using namespace std;


int main(int argc, const char** argv)
{
	namedWindow("Output Image", WINDOW_AUTOSIZE);

	cv::Mat mSource = cv::imread("D:\\vslam_input\\Image_Frame_0.png", IMREAD_GRAYSCALE);
	cv::Mat mSmooth, mDx, mDy, mOutput, mOutputTiled;

	int Iterations = 50;
	Timer t_Def;
	t_Def.start();
	for (size_t i = 0; i < Iterations; i++)
	{
		GaussianBlur(mSource, mSmooth, Size(3, 3),0);
		cv::Sobel(mSmooth, mDx, CV_8UC1, 1, 0);
		cv::Sobel(mSmooth, mDy, CV_8UC1, 0, 1);
		cv::addWeighted(mDx, 1.0, mDy, 1.0, 1.0, mOutput);

	}
	t_Def.stop();

	printf("Time Taken for Opencv : %f ms\n", t_Def.getElapsedTimeInMilliSec() / Iterations);
	cv::imshow("Output Image", mOutput);

	mOutputTiled = Mat(mSource.size(), CV_8UC1);

	int TileSize = 64,			//Tile Size of the Algorithm should be less than L2 Cache Size
		BorderPaddingSize=7,	//Border Size of the Algorithm
		IntermidiateBuffers=5;	//No of Intermediate Buffers used inside the algorithm

	TiledAlgorithm TiledAlgo(mSource.size(), TileSize, BorderPaddingSize, BORDER_DEFAULT, IntermidiateBuffers);

	Timer t_Tiled;
	t_Tiled.start();

	for (size_t i = 0; i < Iterations; i++)
	{
		TiledAlgo.process(mSource, mOutputTiled);
	}

	t_Tiled.stop();
	printf("Time Taken for Tiled : %f ms", t_Tiled.getElapsedTimeInMilliSec() / Iterations);

	cv::imshow("Output Image Tiled", mOutputTiled);

	/*Mat mDiff;
	absdiff(mOutputTiled, mOutput, mDiff);
	cv::imshow("Diff Image Tiled", mDiff);
	imwrite("mOutput.png", mOutput);
	imwrite("mOutputTiled.png", mOutputTiled);*/


	cv::waitKey();
	return 0;
}


