#pragma once
#include "stdafx.h"
#ifdef _MANAGED
#else
#include <functional>
#include <thread>
#include <atomic>
#endif


#define GetTGMTcamera TGMTcamera::GetInstance

class TGMTcamera
{
	static TGMTcamera* instance;
	cv::VideoCapture mCap;
	
	std::atomic<bool> m_isRunning = false;
#ifdef _MANAGED
#else
	std::thread m_threadGrab;
#endif
	
	void CameraGrabbed();
public:
	static TGMTcamera* GetInstance()
	{
		if (!instance)
			instance = new TGMTcamera();
		return instance;
	}
	TGMTcamera();
	~TGMTcamera();

	//only for test camera
	void PlayCamera(int cameraID = 0, int delayMilisecond = 30);

	
	cv::Mat GetImageFromCamera(int cameraID = 0);

#ifdef _MANAGED
#else
	std::function<void(cv::Mat)> OnNewFrame;
	std::function<void(int key)> OnKeyEvent;
#endif

	

	void StartCamera(int cameraID = 0);
	void StartCamera(std::string cameraURL);
	
	void StopCamera();

	

	//record video from camera
	void RecordVideo(int cameraID, std::string videoFileOutputPath);

};

