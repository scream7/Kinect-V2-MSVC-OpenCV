#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H
#include <windows.h>
#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <fstream>
using namespace std;
using namespace cv;
class VideoProcessor
{
private:
	bool bInit_;
	bool bRecording_;
	string strFileName_;
	VideoWriter vw_;
public:
	typedef enum VideoType_
	{
		COLOR, DEPTH, BODYINDEX, BODY
	}VideoType;
	VideoProcessor();
	bool Init(VideoType e, int nWidth, int nHeight);
	bool Record(const Mat& image);
	void EndRecord();

};
#endif