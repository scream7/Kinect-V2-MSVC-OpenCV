#include <iostream>
#include <opencv2/opencv.hpp>
#include "../inc/KinectReader.h"
using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	KinectReader *pKinect = KinectReader::GetInstance();
	bool bResult = pKinect->Init();
	if (bResult)
	{
		cout << "Kinect init succeeded." << endl;
	}
	else
	{
		cout << "Kinect init failed." << endl;
	}
#ifdef _DEBUG
	cout << "debug mode" << endl;
#endif
	while (TRUE)
	{
		pKinect->Update();
		if (waitKey(1) >= 0)
		{
			cout << KinectReader::nCount;
			break;
		}
	}
	pKinect->DestroyInstance();
	getchar();
	return 0;
}