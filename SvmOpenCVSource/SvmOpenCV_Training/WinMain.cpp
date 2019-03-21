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
	debug_out(3, "SvmOpenCV_Training.exe -in <directory> -out <file> -w <width> -h <height>\n");
		
	std::cout << "-in is directory contain sub directories, with each sub directory contain images same class, name of each sub directory only 1 character\n"
		<< "-out is file name you choose\n";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1)
	{
		PrintHelp();
		getchar();
		return;
	}		
	else if (argc < 9)
	{
		//missing parameter
		TGMTutil::CheckParameterExist(argc, argv, "-in");
		TGMTutil::CheckParameterExist(argc, argv, "-out");
		TGMTutil::CheckParameterExist(argc, argv, "-w");
		TGMTutil::CheckParameterExist(argc, argv, "-h");

		getchar();
		return;
	}

	std::string dir = TGMTutil::GetParameter(argc, argv, "-in");
	std::string fileOuput = TGMTutil::GetParameter(argc, argv, "-out");
	int w = stoi(TGMTutil::GetParameter(argc, argv, "-w")); 
	int h = stoi(TGMTutil::GetParameter(argc, argv, "-h"));

	if (!TGMTfile::DirExist(dir))
	{
		PrintError("Directory \"%s\" does not exist", dir.c_str());
		return;
	}
	if (TGMTfile::FileExist(fileOuput))
	{
		PrintMessage("Do you want replace file \"%s\"? (Y/N)", fileOuput.c_str());
		std::string userInput;
		std::cin >> userInput;
		if (userInput != "Y" && userInput != "y")
		{
			return;
		}
	}
	
	START_COUNT_TIME("train_svm");

	std::vector<std::string> files = TGMTfile::GetImageFilesInDir(dir, true);
	std::vector<cv::Mat> mats;
	std::vector<int> labels;
	for (int i = 0; i < files.size(); i++)
	{
		std::cout<<"\rLoading image "<< i + 1 << " / " << files.size();
		std::string filePath = files[i];
		std::string parentDir = TGMTfile::GetParentDir(filePath);
		if (parentDir.length() > 2)
			continue;


		int label = (char)parentDir[0];
		labels.push_back(label);

		cv::Mat mat = cv::imread(filePath, CV_LOAD_IMAGE_GRAYSCALE);
		cv::threshold(mat, mat, 10, 255, CV_THRESH_BINARY);
		if (mat.cols != w || mat.rows != h)
		{
			cv::resize(mat, mat, cv::Size(w, h));
		}
			
		mats.push_back(mat);		
	}

	SET_CONSOLE_TITLE("Traning data...");
	GetTGMTsvm()->TrainData(mats, labels);
	GetTGMTsvm()->SaveData(fileOuput);

	
	STOP_AND_PRINT_COUNT_TIME("train_svm");
	getchar();
}

