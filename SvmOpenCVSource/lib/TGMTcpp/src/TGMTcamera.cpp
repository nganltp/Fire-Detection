#include "TGMTcamera.h"
#include "TGMTdebugger.h"
#ifdef WIN32
#include <Windows.h>
#endif
#include <mutex> 
#include "TGMTfile.h"
#include "TGMTdraw.h"
#include "TGMTutil.h"

TGMTcamera* TGMTcamera::instance = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGMTcamera::TGMTcamera()
{
	m_isRunning = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGMTcamera::~TGMTcamera()
{
	mCap.~VideoCapture();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTcamera::PlayCamera(int cameraID, int delayMilisecond)
{
	 // open the camera
	cv::VideoCapture cap = cv::VideoCapture(cameraID);
	std::string windowName = TGMTutil::FormatString("Camera %d", cameraID);
	if(cap.isOpened())  // check if we succeeded
	{
		cv::Mat frame;
		while(true)
		{			
			cap >> frame;
			
			cv::imshow(windowName, frame);
			if (cv::waitKey(delayMilisecond) == VK_ESCAPE) break;
		}
	}
	else
	{
		PrintError("Not found any camera");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat TGMTcamera::GetImageFromCamera(int cameraID)
{
	if (!mCap.isOpened())
	{
		mCap = cv::VideoCapture(cameraID);
		ASSERT(mCap.isOpened(), "Can not get image from webcam");
	}
	cv::Mat frame;
	
	mCap >> frame;
	return frame;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTcamera::StartCamera(int cameraID)
{
	ASSERT(OnNewFrame, "You must set callback function before use this function");

	StopCamera();
	mCap = cv::VideoCapture(cameraID);
	ASSERT(mCap.isOpened(), "Can not read camera %d", cameraID);
	
	m_isRunning = true;
	m_threadGrab = std::thread(&TGMTcamera::CameraGrabbed, this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTcamera::StartCamera(std::string cameraURL)
{
	ASSERT(OnNewFrame, "You must set callback function before use this function");

	StopCamera();
	mCap = cv::VideoCapture(cameraURL);
	ASSERT(mCap.isOpened(), "Can not read camera %s", cameraURL.c_str());

	m_isRunning = true;
	m_threadGrab = std::thread(&TGMTcamera::CameraGrabbed, this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTcamera::StopCamera()
{
	if (m_isRunning)
	{
		m_isRunning = false;
		m_threadGrab.join();
		mCap.release();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTcamera::CameraGrabbed()
{
	cv::Mat frame;

	while (mCap.isOpened() && m_isRunning)
	{
		mCap.read(frame);

		OnNewFrame(frame);
		cv::waitKey(50);
	}
	frame.~Mat();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TGMTcamera::RecordVideo(int cameraID, std::string videoFileOutputPath)
{
	TODO("Not implemented yet");
	cv::VideoCapture inputVideo(cameraID);
	if (!inputVideo.isOpened())
	{
		PrintError("Could not open camera %d", cameraID);
		return;
	}

	cv::VideoWriter outputVideo;
}