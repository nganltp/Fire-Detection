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
	PrintMessage("This program auto classify and move file to each folder");
	std::cout << "Using with syntax: \n";
	debug_out(3, "SvmOpenCV_Classifier.exe -file <svm_file> -in <directory> -out <directory> -w <width> -h <height>\n");
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

void CreateEmptyFolder(std::string dir)
{
	TGMTfile::CreateDir(dir);
	for (int i = 0; i < 10; i++)
	{
		TGMTfile::CreateDir(TGMTutil::FormatString("%s%d", dir.c_str(), i));
	}

	for (char i = 'A'; i <= 'Z'; i++)
	{
		TGMTfile::CreateDir(TGMTutil::FormatString("%s%c", dir.c_str(), i));
	}
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
		TGMTutil::CheckParameterExist(argc, argv, "-in");	
		TGMTutil::CheckParameterExist(argc, argv, "-out");
		TGMTutil::CheckParameterExist(argc, argv, "-w");
		TGMTutil::CheckParameterExist(argc, argv, "-h");

		getchar();
		return 0;
	}

	std::string svmFile = TGMTutil::GetParameter(argc, argv, "-file");
	std::string inDir = TGMTutil::GetParameter(argc, argv, "-in");

	std::string outDir = TGMTutil::GetParameter(argc, argv, "-out");
	if (outDir.substr(outDir.length() - 2) != "\\")
		outDir += "\\";	
	CreateEmptyFolder(outDir);

	int w = atoi(TGMTutil::GetParameter(argc, argv, "-w").c_str());
	int h = atoi(TGMTutil::GetParameter(argc, argv, "-h").c_str());

	START_COUNT_TIME("Classify");

	PrintMessage("Loading data...");
	GetTGMTsvm()->LoadData(svmFile);

	PrintMessage("Loading image...");
	std::vector<std::string> files = TGMTfile::GetImageFilesInDir(inDir);
	PrintMessage("Loaded %d images", files.size());


	for (int i = 0; i < files.size(); i++)
	{
		SET_CONSOLE_TITLE("%d / %d", i + 1, files.size());
		cv::Mat mat = cv::imread(files[i], cv::IMREAD_GRAYSCALE);
		if (mat.cols != w || mat.rows != h)
		{
			cv::resize(mat, mat, cv::Size(w, h));
		}
		int result =  GetTGMTsvm()->Predict(mat);

		std::string fileName = TGMTfile::GetFileName(files[i]);
		PrintMessage("%s: %c", fileName.c_str(), (char)result);
		std::string targetFile = TGMTutil::FormatString("%s%c\\%s", outDir.c_str(), result, fileName.c_str());
		
		TGMTfile::Move_File(files[i], targetFile);
	}


	PrintSuccess("Classify complete");
	STOP_AND_PRINT_COUNT_TIME("Classify");
	getchar();
	return 0;
}