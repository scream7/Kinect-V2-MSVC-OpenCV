#ifndef KINECTREADER_H
#define KINECTREADER_H
#include <windows.h>
#include <Kinect.h>
#include "opencv2/opencv.hpp"
#include "VideoProcessor.h"
#include "TextProcessor.h"
using namespace cv;
class KinectReader
{
private:
	//Singleton instance
	static KinectReader* instance_;

	bool bInit_;

	//Current Kinect pointer
	IKinectSensor * pKinectSensor_;
	//CoordinateMapper used in body position map
	ICoordinateMapper*      pCoordinateMapper_;

	//MultiSourceFrameReader
	IMultiSourceFrameReader * pMultiSourceFrameReader_;

	/*******  Color data variable  *******/
	//Default color image size
	static const int cColorWidth_ = 1920;
	static const int cColorHeight_ = 1080;
	//Image to show size
	static const int cShowImageWidth_ = 640;
	static const int cShowImageHeight_ = 360;
	//Color RGB
	RGBQUAD* pColorRGBX_;
	//Color image buffer
	UINT nColorImageBufferSize_;
	RGBQUAD *pColorImageBuffer_;
	//ColorFrame
	IColorFrame *pColorFrame_;
	//ColorFrameDescription
	IFrameDescription *pColorFrameDescription_;

	/*******  Depth data variable  *******/
	//Default depth image size
	static const int cDepthWidth_ = 512;
	static const int cDepthHeight_ = 424;
	//Depth RGB
	RGBQUAD* pDepthRGBX_;
	//Range of depth
	USHORT nDepthMinReliableDistance_;
	USHORT nDepthMaxReliableDistance_;
	//Depth image buffer
	UINT nDepthImageBUfferSize_;
	UINT16 *pDepthImageBuffer_;
	//DepthFrame
	IDepthFrame *pDepthFrame_;
	//DepthFrameDescription
	IFrameDescription *pDepthFrameDescription_;

	/*******  BodyIndex data variable  *******/
	//Default bodyindex image size
	static const int cBodyIndexWidth_ = 512;
	static const int cBodyIndexHeight_ = 424;
	//Bodyindex body color
	static const UINT BodyColor_[6];
	//BodyIndex UINT
	UINT* pBodyIndexUINT_;
	//BodyIndex frame
	IBodyIndexFrame *pBodyIndexFrame_;
	//BodyIndex frame description
	IFrameDescription *pBodyIndexFrameDescription_;
	//BodyIndex image buffer
	UINT nBodyIndexImageBUfferSize_;
	BYTE *pBodyIndexImageBuffer_;

	/*******  Body data variable  *******/
	//Body frame
	IBodyFrame *pBodyFrame_;
	//Bodies
	IBody *ppBodies_[BODY_COUNT];
	//Body type
	//default JointType_Count == 25
	static const int cJointType_Count_ = JointType_Count;
	static const string JointType_[cJointType_Count_];
	//INT64 nBodyTime_;

	KinectReader();

	KinectReader(const KinectReader&);

	~KinectReader();

	KinectReader& operator=(const KinectReader&);

	//For video writer
	VideoProcessor colorVideo_;
	VideoProcessor depthVideo_;
	VideoProcessor bodyIndexVideo_;

	//Body data writer
	TextProcessor bodyText_;

	//For process color/depth image steps:
	//step 1:Pre-Process frame
	bool PreProcessFrame(IMultiSourceFrame* pMultiSourceFrame);
	//step 2:Process color/depth image
	void ProcessColorImage(RGBQUAD* pBuffer,int nWidth = cColorWidth_,int nHeight = cColorHeight_);
	//step 2:Process depth image
	void ProcessDepthImage(const UINT16* pBuffer, int nWidth = cDepthWidth_, int nHeight = cDepthHeight_, USHORT nMinDepth = 0, USHORT nMaxDepth = 255);
	//step 2:Process BodyIndex image
	void ProcessBodyIndexImage(BYTE* pBuffer, int nWidth, int nHeight);
	//step 2:Process body data
	void ProcessBodyData(int nBodyCount);
	//step 3:Release frame
	void ReleaseFrame();


public:
	static int nCount;
	static KinectReader* GetInstance();
	bool DestroyInstance();

	//Kinect Device Init
	bool Init();

	//Update image,color,depth,etc.
	void Update();
};

#endif // KINECTREADER_H
