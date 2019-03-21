#pragma once
#include "stdafx.h"

#include <opencv2/ml/ml.hpp>

#if CV_MAJOR_VERSION == 3
using namespace cv::ml;
#endif

#define GetTGMTsvm TGMTsvm::GetInstance

class TGMTsvm
{
	static TGMTsvm* m_instance;
	cv::Mat mMatData, mLabels;
	

#if CV_MAJOR_VERSION == 3
	cv::Ptr<SVM> svm;
#else
	CvSVM svm;
#endif
	bool mIsTrained = false;

	
public:
	TGMTsvm();
	~TGMTsvm();
	
	static TGMTsvm* GetInstance()
	{
		if (!m_instance)
			m_instance = new TGMTsvm();
		return m_instance;
	}

	void TrainData(std::vector<cv::Mat> matInputs, std::vector<int> labels);	
	float Predict(cv::Mat matInput);
	float Predict(std::string filePath);
	void SaveData(std::string filePath);
	void LoadData(std::string filePath);
};

