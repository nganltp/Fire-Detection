#pragma once
#include "stdafx.h"

//----------------------------------------
// TGMT mean: ThiGiacMayTinh
//----------------------------------------
class TGMTcolor
{
public:

	static std::string GetColorName(cv::Mat, cv::Point);
	static std::string GetColorName(cv::Mat, int x, int y);	

	static cv::Scalar GetPixelColorType(int H, int S, int V);
	static cv::Scalar GetPixelColorType(cv::Vec3b pixel);
	//static std::vector<std::string> ColorNames;
	static cv::Scalar GetPixelValue(cv::Mat matInput, cv::Point p);
	static cv::Scalar GetRandomColor();

	static cv::Mat FilterColor(cv::Mat matInput, uchar minH, uchar maxH, uchar minS, uchar maxS, uchar minV, uchar maxV, bool isHSV = false);
	static cv::Mat FilterColor(cv::Mat matInput, cv::Scalar lower, cv::Scalar higher, bool isHSV = false);
};

