#include "TGMTknn.h"
#include "TGMTdebugger.h"

TGMTknn* TGMTknn::m_instance = nullptr;

TGMTknn::TGMTknn()
{
#if CV_MAJOR_VERSION == 3
	knn = KNearest::create();
	knn->setIsClassifier(true);
	knn->setAlgorithmType(KNearest::Types::BRUTE_FORCE);
#else
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGMTknn::~TGMTknn()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTknn::TrainData(std::vector<cv::Mat> matInputs, std::vector<int> labels)
{
	ASSERT(matInputs.size() > 0 && labels.size() > 0 && matInputs.size() == matInputs.size(), "KNN train data is not valid");
	int numMats = matInputs.size();

	int width = matInputs[0].cols;
	int height = matInputs[0].rows;
	int matArea = width * height;
	cv::Mat matData = cv::Mat(numMats, matArea, CV_32FC1);
	cv::Mat matLabels = cv::Mat(numMats, 1, CV_32FC1);

	//prepare train set
	for (int fileIndex = 0; fileIndex < numMats; fileIndex++)
	{

		//set label
		matLabels.at<float>(fileIndex, 0) = labels[fileIndex];

		cv::Mat matCurrent = matInputs[fileIndex];
		ASSERT(matCurrent.cols == width && matCurrent.rows == height, "Array of Mat input not same size: %d", fileIndex);

		int matDataIndex = 0; // Current column in training_mat
		for (int rowIndex = 0; rowIndex<matCurrent.rows; rowIndex++)
		{
			for (int colIndex = 0; colIndex < matCurrent.cols; colIndex++)
			{
				matData.at<float>(fileIndex, matDataIndex++) = matCurrent.at<uchar>(rowIndex, colIndex);
			}
		}
	}
	knn->train(matData, ROW_SAMPLE, matLabels);
	mIsTrained = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float TGMTknn::Predict(cv::Mat matInput)
{
	ASSERT(matInput.data, "Mat input is null");
	ASSERT(mIsTrained, "You must train KNN before use");

	cv::Mat matData;
	if (matInput.type() != CV_32FC1)
	{
		int width = matInput.cols;
		int height = matInput.rows;
		int matArea = width * height;
		matData = cv::Mat(1, matArea, CV_32FC1);

		int matDataIndex = 0; // Current column in training_mat
		for (int rowIndex = 0; rowIndex<matInput.rows; rowIndex++)
		{
			for (int colIndex = 0; colIndex < matInput.cols; colIndex++)
			{
				uchar temp = matInput.at<uchar>(rowIndex, colIndex);
				matData.at<float>(0, matDataIndex++) = matInput.at<uchar>(rowIndex, colIndex);
			}
		}
	}
	else
	{
		matData = matInput;
	}
	cv::Mat matResults(0, 0, CV_32FC1);
	return knn->findNearest(matData, knn->getDefaultK() , matResults);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTknn::SaveData(std::string fileName)
{
	knn->save(fileName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTknn::LoadData(std::string fileName)
{
	knn = cv::Algorithm::load<KNearest>(fileName);
	ASSERT(knn != nullptr, "Can not load file: %s", fileName.c_str());
	mIsTrained = true;
}