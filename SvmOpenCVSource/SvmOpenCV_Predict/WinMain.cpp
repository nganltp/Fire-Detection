// TGMTtemplate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "TGMTfile.h"
#include "TGMTdebugger.h"
#include "TGMTsvm.h"
#include "TGMTutil.h"

void PrintHelp()
{
	std::cout << "Using with syntax: \n";
	debug_out(3, "SvmOpenCV_Predict.exe -file <svm_file> -dir <directory> -w <width> -h <height>\n");
	std::cout << "with <svm_file> is SVM trained file\n";
	std::cout << "and <directory> is directory contain images to predict\n";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Predict(std::string filePath)
{	
	cv::Mat mat = cv::imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);
	cv::threshold(mat, mat, 10, 255, CV_THRESH_BINARY);

	cv::resize(mat, mat, cv::Size(12, 28));
	PrintMessage("Resutl: %c", (char) GetTGMTsvm()->Predict(mat));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1)
	{
		PrintHelp();
		getchar();
	}
	else if (argc < 9)
	{
		//missing parameter
		TGMTutil::CheckParameterExist(argc, argv, "-file");
		TGMTutil::CheckParameterExist(argc, argv, "-dir");	
		TGMTutil::CheckParameterExist(argc, argv, "-w");
		TGMTutil::CheckParameterExist(argc, argv, "-h");

		getchar();
		return 0;
	}

	std::string svmFile = TGMTutil::GetParameter(argc, argv, "-file");
	std::string dir = TGMTutil::GetParameter(argc, argv, "-dir");
	int w = atoi(TGMTutil::GetParameter(argc, argv, "-w").c_str());
	int h = atoi(TGMTutil::GetParameter(argc, argv, "-h").c_str());

	PrintMessage("Loading data...");
	GetTGMTsvm()->LoadData(svmFile);

	PrintMessage("Loading image...");
	std::vector<std::string> files = TGMTfile::GetImageFilesInDir(dir);
	std::string results;
	for (int i = 0; i < files.size(); i++)
	{
		SET_CONSOLE_TITLE("%d / %d", i, files.size());
		cv::Mat mat = cv::imread(files[i], cv::IMREAD_GRAYSCALE);
		if (mat.cols != w || mat.rows != h)
		{
			cv::resize(mat, mat, cv::Size(w, h));
		}
		float result =  GetTGMTsvm()->Predict(mat);
		std::string line = TGMTutil::FormatString("%s: %c", TGMTfile::GetFileName(files[i]).c_str(), (char)result);
		PrintMessage(line.c_str());
		results += line + "\n";
	}

	TGMTfile::WriteToFile("result.txt", results);
	PrintMessage("Write result into result.txt");

	getchar();
	return 0;
}