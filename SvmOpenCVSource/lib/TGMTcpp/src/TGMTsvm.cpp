#include "TGMTsvm.h"
#include "TGMTdebugger.h"
#include "TGMTfile.h"

TGMTsvm* TGMTsvm::m_instance = nullptr;

TGMTsvm::TGMTsvm()
{
#if CV_MAJOR_VERSION == 3
	svm = SVM::create();
#else
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGMTsvm::~TGMTsvm()
{
#if CV_MAJOR_VERSION == 3
#else
	svm.~CvSVM();
#endif
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTsvm::TrainData(std::vector<cv::Mat> matInputs, std::vector<int> labels)
{
	ASSERT(matInputs.size() > 0 && labels.size() > 0 && matInputs.size() == matInputs.size(), "SVM train data is not valid");
	int numMats = matInputs.size();
	
	int width = matInputs[0].cols;
	int height = matInputs[0].rows;
	int matArea = width * height;
	mMatData = cv::Mat(numMats, matArea, CV_32FC1);
	mLabels = cv::Mat(numMats, 1, CV_32S);

	//prepare train set
	for (int fileIndex = 0; fileIndex < numMats; fileIndex++)
	{
		
		//set label
		mLabels.at<int>(fileIndex, 0) = labels[fileIndex];

		cv::Mat matCurrent = matInputs[fileIndex];
		ASSERT(matCurrent.cols == width && matCurrent.rows == height, "Array of Mat input not same size: %d", fileIndex);

		int matDataIndex = 0; // Current column in training_mat
		for (int rowIndex = 0; rowIndex<matCurrent.rows; rowIndex++)
		{
			for (int colIndex = 0; colIndex < matCurrent.cols; colIndex++)
			{
				mMatData.at<float>(fileIndex, matDataIndex++) = matCurrent.at<uchar>(rowIndex, colIndex);
			}
		}
	}

#if CV_MAJOR_VERSION == 3
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
	svm->train(mMatData, ROW_SAMPLE, mLabels);
#else
	CvSVMParams params;
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type = CvSVM::LINEAR;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	svm.train(mMatData, mLabels, cv::Mat(), cv::Mat(), params);
#endif

	
	mIsTrained = true;
	PrintSuccess("Trained %d images", numMats);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float TGMTsvm::Predict(cv::Mat matInput)
{
	ASSERT(matInput.data, "Mat input is null");	
	ASSERT(mIsTrained, "You must train SVM before use");
	
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

#if CV_MAJOR_VERSION == 3
	return svm->predict(matData);
#else
	return svm.predict(matData);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTsvm::SaveData(std::string filePath)
{
#if CV_MAJOR_VERSION == 3
	svm->save(filePath.c_str());
#else
	svm.save(filePath.c_str());
#endif
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTsvm::LoadData(std::string filePath)
{
	ASSERT(TGMTfile::FileExist(filePath), "File svm \'%s\' does not exist", filePath.c_str());
#if CV_MAJOR_VERSION == 3
	svm = StatModel::load<SVM>(filePath);
#else
	svm.load(filePath.c_str());
#endif
	
	mIsTrained = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float TGMTsvm::Predict(std::string filePath)
{
	cv::Mat mat = cv::imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);
	return Predict(mat);
}