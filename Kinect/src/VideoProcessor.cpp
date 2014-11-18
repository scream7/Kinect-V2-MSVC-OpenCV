#include "../inc/VideoProcessor.h"
VideoProcessor::VideoProcessor() :
								bInit_(false),
								bRecording_(false),
								strFileName_("")
{

}

bool VideoProcessor::Init(VideoType e, int nWidth, int nHeight)
{
	char tempStr[32] = "";
	int nindex = 1;
	fstream ffile;
	switch (e)
	{
	case VideoType::COLOR:
		sprintf_s(tempStr, "ColorVideo_%d.avi", nindex);
		ffile.open(tempStr, ios::in);
		while (ffile)
		{
			ffile.close();
			nindex++;
			sprintf_s(tempStr, "ColorVideo_%d.avi", nindex);
			ffile.open(tempStr, ios::in);
		}
		break;
	case VideoType::DEPTH:
		ffile.close();
		sprintf_s(tempStr, "DepthVideo_%d.avi", nindex);
		ffile.open(tempStr, ios::in);
		while (ffile)
		{
			nindex++;
			sprintf_s(tempStr, "DepthVideo_%d.avi", nindex);
			ffile.open(tempStr, ios::in);
		}
		break;
	case VideoType::BODYINDEX:
		ffile.close();
		sprintf_s(tempStr, "BodyIndex_%d.avi", nindex);
		ffile.open(tempStr, ios::in);
		while (ffile)
		{
			nindex++;
			sprintf_s(tempStr, "BodyIndex_%d.avi", nindex);
			ffile.open(tempStr, ios::in);
		}
		break;
	case VideoType::BODY:
		//not used
		return false;
		break;
	default:
		break;
	}
	ffile.close();
	strFileName_ = tempStr;
	bool bResult = 
		vw_.open(strFileName_, -1, 20.0, Size(nWidth, nHeight));
	if (bResult)
	{
		bInit_ = true;
	}
	else
	{
		bInit_ = false;
	}
	return bResult;
}

bool VideoProcessor::Record(const Mat& image)
{
	if (!bInit_)
	{
		return false;
	}
	else
	{
		vw_.write(image);
		bRecording_ = true;
		return true;
	}
}
void VideoProcessor::EndRecord()
{
	if (bInit_)
	{
		vw_.release();
		bRecording_ = false;
	}
	else
	{
		fstream ffile;
		ffile.open(strFileName_, ios::in);
		if (ffile)
		{
			//delete file
			/*remove(strFileName_.c_str());*/
		}
		ffile.close();
	}
}