#include "../inc/KinectReader.h"
#include <iostream>
using namespace std;
KinectReader* KinectReader::instance_ = NULL;
int KinectReader::nCount = 0;
bool bRecordVideo = true;
bool bRecordText = true;
const UINT KinectReader::BodyColor_[6] = 
{
	//RBGA
	0x0000FF00,	//green
	0x00FF0000,	//blue
	0xFFFF4000, //purple
	0x40FFFF00, //cyan
	0xFF40FF00, //yellow
	0xFF808000, //pink
};
const string KinectReader::JointType_[KinectReader::cJointType_Count_] =
{
	"SpineBase",
	"pineMid",
	"Neck",
	"Head",
	"ShoulderLeft",
	"ElbowLeft",
	"WristLeft",
	"HandLeft",
	"ShoulderRight",
	"ElbowRight",
	"WristRight",
	"HandRight",
	"HipLeft",
	"KneeLeft",
	"AnkleLeft",
	"FootLeft",
	"HipRight",
	"KneeRight",
	"AnkleRight",
	"FootRight",
	"SpineShoulder",
	"HandTipLeft",
	"ThumbLeft",
	"HandTipRight",
	"ThumbRight",
};
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

KinectReader::KinectReader() :
		pKinectSensor_(NULL),
		pMultiSourceFrameReader_(NULL),
		bInit_(false)
{
	pColorRGBX_ = new RGBQUAD[cColorHeight_ * cColorWidth_];
	pDepthRGBX_ = new RGBQUAD[cDepthHeight_ * cDepthWidth_];
	pBodyIndexUINT_ = new UINT[cBodyIndexHeight_ * cBodyIndexWidth_];
	
	bool bResult =
		colorVideo_.Init(VideoProcessor::COLOR, cShowImageWidth_, cShowImageHeight_);
	bResult &=
		depthVideo_.Init(VideoProcessor::DEPTH, cDepthWidth_, cDepthHeight_);
	bResult &=
		bodyIndexVideo_.Init(VideoProcessor::BODYINDEX, cBodyIndexWidth_, cBodyIndexHeight_);
	bResult &= 
		bodyText_.Init(TextProcessor::BODY);
	if (!bResult)
	{
		cout << "Video/Text init failed" << endl;
	}
}

KinectReader::~KinectReader()
{
	if (pColorRGBX_)
	{
		delete[] pColorRGBX_;
		pColorRGBX_ = NULL;
	}
	if (pDepthRGBX_)
	{
		delete[] pDepthRGBX_;
		pDepthRGBX_ = NULL;
	}
	if (pKinectSensor_)
	{
		pKinectSensor_->Close();
		pKinectSensor_->Release();
		pKinectSensor_ = NULL;
	}
	colorVideo_.EndRecord();
	depthVideo_.EndRecord();
	bodyIndexVideo_.EndRecord();
	bodyText_.EndRecord();
}
bool KinectReader::PreProcessFrame(IMultiSourceFrame* pMultiSourceFrame)
{
	//ColorFrame init
	pColorFrame_ = NULL;

	//ColorFrameDescription init
	pColorFrameDescription_ = NULL;

	//ColorFrameReference
	IColorFrameReference *pColorFrameRef = NULL;

	//Color image size
	int nColorWidth = 0;
	int nColorHeight = 0;

	//Color image format
	ColorImageFormat CurrentColorImageFormat = ColorImageFormat_None;

	//Color image buffer
	nColorImageBufferSize_ = 0;
	pColorImageBuffer_ = NULL;

	//Depth frame init
	pColorFrame_ = NULL;

	//Depth frame description init
	pDepthFrameDescription_ = NULL;

	//Depth frame reference init
	IDepthFrameReference *pDepthFrameRef = NULL;

	//Depth image size
	int nDepthWidth = 0;
	int nDepthHeight = 0;


	//BodyIndex image size
	int nBodyIndexWidth = 0;
	int nBodyIndexHeight = 0;
	//BodyIndex frame init
	pBodyIndexFrame_ = NULL;

	//BodyIndex frame description init
	pBodyIndexFrameDescription_ = NULL;

	//BodyIndex frame reference init
	IBodyIndexFrameReference *pBodyIndexFrameRef = NULL;

	//Body frame init
	pBodyFrame_ = NULL;

	//Body frame reference init
	IBodyFrameReference *pBodyFrameRef = NULL;

	HRESULT hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameRef);
	
	if (FAILED(hr))
	{
		cout << "Color frame:get_ColorFrameReference() failed." << endl;
		return false;
	}

	hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameRef);
	if (FAILED(hr))
	{
		cout << "Depth frame:get_DepthFrameReference() failed." << endl;
		return false;
	}

	hr = pMultiSourceFrame->get_BodyIndexFrameReference(&pBodyIndexFrameRef);
	if (FAILED(hr))
	{
		cout << "BodyIndex frame:get_BodyIndexFrameReference() failed." << endl;
		return false;
	}

	hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameRef);
	if (FAILED(hr))
	{
		cout << "Body frame:get_BodyFrameReference() failed." << endl;
		return false;
	}

	hr = pColorFrameRef->AcquireFrame(&pColorFrame_);
	if (FAILED(hr))
	{
		cout << "Color frame:AcquireFrame() failed." << endl;
		return false;
	}

	hr = pDepthFrameRef->AcquireFrame(&pDepthFrame_);
	if (FAILED(hr))
	{
		cout << "Depth frame:AcquireFrame() failed." << endl;
		return false;
	}

	hr = pBodyIndexFrameRef->AcquireFrame(&pBodyIndexFrame_);
	if (FAILED(hr))
	{
		cout << "BodyIndex frame:AcquireFrame() failed." << endl;
		return false;
	}

	hr = pBodyFrameRef->AcquireFrame(&pBodyFrame_);
	if (FAILED(hr))
	{
		cout << "Body frame:AcquireFrame() failed." << endl;
		return false;
	}
	
	SafeRelease(pColorFrameRef);
	SafeRelease(pDepthFrameRef);
	SafeRelease(pBodyIndexFrameRef);
	SafeRelease(pBodyFrameRef);

	hr = pColorFrame_->get_FrameDescription(&pColorFrameDescription_);
	if (FAILED(hr))
	{
		cout << "Color frame:get_FrameDescription() failed." << endl;
		return false;
	}

	hr = pDepthFrame_->get_FrameDescription(&pDepthFrameDescription_);
	if (FAILED(hr))
	{
		cout << "Depth frame:get_FrameDescription() failed." << endl;
		return false;
	}

	hr = pBodyIndexFrame_->get_FrameDescription(&pBodyIndexFrameDescription_);
	if (FAILED(hr))
	{
		cout << "BodyIndex frame:get_FrameDescription() failed." << endl;
		return false;
	}

	hr = pColorFrameDescription_->get_Width(&nColorWidth);
	if (FAILED(hr) || nColorWidth != cColorWidth_)
	{
		cout << "Color frame:get_Width() failed." << endl;
		return false;
	}

	hr = pDepthFrameDescription_->get_Width(&nDepthWidth);
	if (FAILED(hr) || nDepthWidth != cDepthWidth_)
	{
		cout << "Depth frame:get_Width() failed." << endl;
		return false;
	}

	hr = pBodyIndexFrameDescription_->get_Width(&nBodyIndexWidth);
	if (FAILED(hr) || nBodyIndexWidth != cBodyIndexWidth_)
	{
		cout << "BodyIndex frame:get_Width() failed." << endl;
		return false;
	}

	hr =  pColorFrameDescription_->get_Height(&nColorHeight);
	if (FAILED(hr) || nColorHeight != cColorHeight_)
	{
		cout << "Color frame:get_Height() failed." << endl;
		return false;
	}

	hr = pDepthFrameDescription_->get_Height(&nDepthHeight);
	if (FAILED(hr) || nDepthHeight != cDepthHeight_)
	{
		cout << "Depth frame:get_Height() failed." << endl;
		return false;
	}

	hr = pBodyIndexFrameDescription_->get_Height(&nBodyIndexHeight);
	if (FAILED(hr) || nBodyIndexHeight != cBodyIndexHeight_)
	{
		cout << "BodyIndex frame:get_Height() failed." << endl;
		return false;
	}

	hr = pColorFrame_->get_RawColorImageFormat(&CurrentColorImageFormat);
	if (FAILED(hr))
	{
		cout << "Color frame:get_RawColorImageFormat() failed." << endl;
		return false;
	}

	hr = pDepthFrame_->get_DepthMinReliableDistance(&nDepthMinReliableDistance_);
	if (FAILED(hr))
	{
		cout << "Depth frame:get_DepthMinReliableDistance() failed." << endl;
		return false;
	}

	hr = pDepthFrame_->get_DepthMaxReliableDistance(&nDepthMaxReliableDistance_);
	if (FAILED(hr))
	{
		cout << "Depth frame:get_DepthMaxReliableDistance() failed." << endl;
		return false;
	}

	if (CurrentColorImageFormat == ColorImageFormat_Bgra)
	{
		hr = pColorFrame_->AccessRawUnderlyingBuffer(&nColorImageBufferSize_, reinterpret_cast<BYTE**>(&pColorImageBuffer_));
	}
	else if (pColorRGBX_)
	{
		pColorImageBuffer_ = pColorRGBX_;
		nColorImageBufferSize_ = cColorWidth_ * cColorHeight_ * sizeof(RGBQUAD);
		hr = pColorFrame_->CopyConvertedFrameDataToArray(nColorImageBufferSize_, reinterpret_cast<BYTE*>(pColorImageBuffer_), ColorImageFormat_Bgra);
	}
	else
	{
		cout << "Color frame:AccessRawUnderlyingBuffer() or CopyConvertedFrameDataToArray() failed." << endl;
		return false;
	}

	hr = pDepthFrame_->AccessUnderlyingBuffer(&nDepthImageBUfferSize_, &pDepthImageBuffer_);
	if (FAILED(hr))
	{
		cout << "Depth frame:AccessUnderlyingBuffer() failed." << endl;
		return false;
	}

	pBodyIndexFrame_->AccessUnderlyingBuffer(&nBodyIndexImageBUfferSize_, &pBodyIndexImageBuffer_);
	if (FAILED(hr))
	{
		cout << "BodyIndex frame:AccessUnderlyingBuffer() failed." << endl;
		return false;
	}

	//for body 
	//nBodyTime_ = 0;
	//hr = pBodyFrame_->get_RelativeTime(&nBodyTime_);
	//if (FAILED(hr))
	//{
	//	cout << "Body frame:get_RelativeTime() failed." << endl;
	//	return false;
	//}

	hr = pBodyFrame_->GetAndRefreshBodyData(_countof(ppBodies_), ppBodies_);
	if (FAILED(hr))
	{
		cout << "Body frame:GetAndRefreshBodyData() failed." << endl;
		return false;
	}

	return true;
}
void KinectReader::ReleaseFrame()
{
	SafeRelease(pColorFrameDescription_);
	SafeRelease(pColorFrame_);
	SafeRelease(pDepthFrameDescription_);
	SafeRelease(pDepthFrame_);
	SafeRelease(pBodyIndexFrameDescription_);
	SafeRelease(pBodyIndexFrame_);
	SafeRelease(pBodyFrame_);
	//SafeRelease(pCoordinateMapper_);
}
void KinectReader::ProcessColorImage(RGBQUAD *pBuffer, int nWidth, int nHeight)
{
	Mat ColorImage(nHeight, nWidth, CV_8UC4, pBuffer);
	Mat ShowImage;
	resize(ColorImage, ShowImage, Size(KinectReader::cShowImageWidth_, KinectReader::cShowImageHeight_));
	if (bRecordVideo)
	{
		colorVideo_.Record(ShowImage);
	}
	imshow("ColorImage", ShowImage);
}
void KinectReader::ProcessDepthImage(const UINT16* pBuffer, int nWidth , int nHeight , USHORT nMinDepth , USHORT nMaxDepth)
{
	RGBQUAD* pRGBX = pDepthRGBX_;

	const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

	while (pBuffer < pBufferEnd)
	{
		USHORT depth = *pBuffer;

		BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);

		pRGBX->rgbRed = intensity;
		pRGBX->rgbGreen = intensity;
		pRGBX->rgbBlue = intensity;

		++pRGBX;
		++pBuffer;
	}
	Mat resultImage(nHeight, nWidth, CV_8UC4, pDepthRGBX_);
	if (bRecordVideo)
	{
		depthVideo_.Record(resultImage);
	}

	imshow("DepthImage", resultImage);
}
void KinectReader::ProcessBodyIndexImage(BYTE* pBuffer, int nWidth, int nHeight)
{
	int nbodyIndexFrameDataSize = nWidth * nHeight;
	byte *frameData = pBuffer;
	int nColorNum = sizeof(KinectReader::BodyColor_) / sizeof(UINT);
	for (int i = 0; i < nbodyIndexFrameDataSize; i++)
	{
		if (frameData[i] < nColorNum)
		{
			pBodyIndexUINT_[i] = KinectReader::BodyColor_[frameData[i]];
		}
		else
		{
			pBodyIndexUINT_[i] = 0x00000000;
		}
	}
	Mat BodyIndexImage(nHeight, nWidth, CV_8UC4, pBodyIndexUINT_);
	if (bRecordVideo)
	{
		bodyIndexVideo_.Record(BodyIndexImage);
	}
	imshow("BodyIndexImage", BodyIndexImage);
}
void KinectReader::ProcessBodyData(int nBodyCount)
{
	Mat skeletonImage;
	skeletonImage.create(1080, 1920, CV_8UC3);
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody *pBody = ppBodies_[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			HRESULT hr = pBody->get_IsTracked(&bTracked);
			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count];
				HandState leftHandState = HandState_Unknown;
				HandState rightHandState = HandState_Unknown;

				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					ColorSpacePoint colorPoints[KinectReader::cJointType_Count_] = { 0 };
					CameraSpacePoint cameraSpacePoint[KinectReader::cJointType_Count_] = {0};
					Point pt;
					for (int j = 0; j < _countof(joints); ++j)
					{
						//cout << "Player id:" << i + 1 << " "
						//	<< KinectReader::JointType_[joints[j].JointType] << " "
						//	<< joints[j].Position.X << " "
						//	<< joints[j].Position.Y << " "
						//	<< joints[j].Position.Z << " " << endl;
						cameraSpacePoint[j] = joints[j].Position;
						ColorSpacePoint colorPoint;
						pCoordinateMapper_->MapCameraPointToColorSpace(cameraSpacePoint[j], &colorPoint);
						Point pt;
						pt.x = int(colorPoint.X  + 0.5);
						pt.y = int(colorPoint.Y  + 0.5);
						if (bRecordText)
						{ 
							char temp[32];
							sprintf_s(temp, "%d %d %d", j, pt.x, pt.y);
							bodyText_.Record(temp);
						}
						circle(skeletonImage, pt, 5, cvScalar(0, 0, 0), 1, 8, 0);
					}
					//nCount++;
				}
			}
		}
	}
	Mat ShowImage;
	resize(skeletonImage, ShowImage, Size(KinectReader::cShowImageWidth_, KinectReader::cShowImageHeight_));
	imshow("skeletonImage", ShowImage);
}
KinectReader* KinectReader::GetInstance()
{
	if (instance_ == NULL)
	{
		instance_ = new KinectReader();
	}
	return instance_;
}
bool KinectReader::DestroyInstance()
{
	if (pKinectSensor_)
	{
		pKinectSensor_->Close();
		pKinectSensor_->Release();
		pKinectSensor_ = NULL;
		instance_->~KinectReader();
		instance_ = NULL;
		return true;
	}
	else
	{
		return false;
	}
}
bool KinectReader::Init()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&pKinectSensor_);
	if (FAILED(hr))
	{
		return false;
	}

	if (pKinectSensor_)
	{
		IMultiSourceFrame  *pMultiSourceFrame = NULL;

		hr = pKinectSensor_->Open();

		if (SUCCEEDED(hr))
		{
			hr = pKinectSensor_->OpenMultiSourceFrameReader(FrameSourceTypes_Color | FrameSourceTypes_Depth | 
															FrameSourceTypes_BodyIndex | FrameSourceTypes_Body,
															&pMultiSourceFrameReader_);
		}

		if (SUCCEEDED(hr))
		{
			pKinectSensor_->get_CoordinateMapper(&pCoordinateMapper_);
		}
		SafeRelease(pMultiSourceFrame);

		if (!pKinectSensor_ || FAILED(hr))
		{
			return false;
		}
	}
	bInit_ = true;
	return true;
}
void KinectReader::Update()
{
	if (!pMultiSourceFrameReader_)
	{
		return;
	}
	IMultiSourceFrame  *pMultiSourceFrame = NULL;
	HRESULT hr = pMultiSourceFrameReader_->AcquireLatestFrame(&pMultiSourceFrame);
	if (FAILED(hr))
	{
		//cout << "AcquireLatestFrame failed." << endl;
		return;
	}
	
	//For process
	if (PreProcessFrame(pMultiSourceFrame))
	{
		ProcessColorImage(pColorImageBuffer_);
		ProcessDepthImage(pDepthImageBuffer_, cDepthWidth_, cDepthHeight_, nDepthMinReliableDistance_, nDepthMaxReliableDistance_);
		ProcessBodyIndexImage(pBodyIndexImageBuffer_, cBodyIndexWidth_, cBodyIndexHeight_);
		ProcessBodyData(BODY_COUNT);
	}
	ReleaseFrame();
}

