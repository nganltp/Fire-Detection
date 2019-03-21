#pragma once
#include "stdafx.h"

using namespace cv::ml;

#define GetTGMTknn TGMTknn::GetInstance

class TGMTknn
{
	bool mIsTrained = false;

#if CV_MAJOR_VERSION == 3
	cv::Ptr<KNearest> knn;
#else
	cv::KNearest    *knn;
#endif

	static TGMTknn* m_instance;
public:
	TGMTknn();
	~TGMTknn();

	static TGMTknn* GetInstance()
	{
		if (!m_instance)
			m_instance = new TGMTknn();
		return m_instance;
	}

	void TrainData(std::vector<cv::Mat> matInputs, std::vector<int> labels);
	float Predict(cv::Mat matInput);

	void SaveData(std::string fileName);
	void LoadData(std::string fileName);
};

