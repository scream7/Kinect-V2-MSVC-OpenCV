#include "../inc/TextProcessor.h"
TextProcessor::TextProcessor() :
								bInit_(false),
								strFileName_("")
{

}
bool TextProcessor::Init(TextType e)
{
	char tempStr[32] = "";
	int nindex = 1;
	fstream ffile;
	switch (e)
	{
	case TextProcessor::BODY:
		sprintf_s(tempStr, "Body_%d.txt", nindex);
		ffile.open(tempStr, ios::in);
		while (ffile)
		{
			ffile.close();
			nindex++;
			sprintf_s(tempStr, "Body_%d.txt", nindex);
			ffile.open(tempStr, ios::in);
		}
		break;
	default:
		break;
	}
	ffile.close();
	strFileName_ = tempStr;
	ofile_.open(strFileName_, ios::out);
	if (!ofile_)
	{
		bInit_ = false;
	}
	else
	{
		bInit_ = true;
	}
	return bInit_;
}
bool TextProcessor::Record(const string& strData)
{
	ofile_ << strData << endl;
	return true;
}
void TextProcessor::EndRecord()
{
	if (bInit_)
	{
		ofile_.close();
		bInit_ = false;
	}
}