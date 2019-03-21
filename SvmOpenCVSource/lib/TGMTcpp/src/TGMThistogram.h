#pragma once
#include "stdafx.h"
class TGMThistogram
{
public:
	TGMThistogram();
	~TGMThistogram();

	enum HistogramType
	{
		
	};

	static cv::Mat GetHistogram(cv::Mat matInput);
	static void DrawHistogram(cv::Mat matInput);
	

};

