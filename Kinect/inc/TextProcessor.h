#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
class TextProcessor
{
private:
	bool bInit_;
	string strFileName_;
	ofstream ofile_;
public:
	typedef enum TextType_
	{
		BODY
	}TextType;
	TextProcessor();
	bool Init(TextType e);
	inline bool Record(const string& strData);
	void EndRecord();
};
#endif