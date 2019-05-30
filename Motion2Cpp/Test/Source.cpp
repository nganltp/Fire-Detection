#include <iostream>
#include <fstream>
#include <string>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
//
using namespace cv;
using namespace std;


//static const int low_Y = 133, low_U = 126, low_V = 0;
//static const int high_Y = 255, high_U = 220, high_V = 115;

int low_Y = 133, low_U = 126, low_V = 0;
int high_Y = 255, high_U = 220, high_V = 115;


void on_low_Y_thresh_trackbar(int, void *)
{
	low_Y = min(high_Y - 1, low_Y);
	setTrackbarPos("Low H", "Threshold Selection", low_Y);
}

void on_high_Y_thresh_trackbar(int, void *)
{
	high_Y = max(high_Y, low_Y + 1);
	setTrackbarPos("High H", "Threshold Selection", high_Y);
}

void on_low_U_thresh_trackbar(int, void *)
{
	low_U = min(high_U - 1, low_U);
	setTrackbarPos("Low S", "Threshold Selection", low_U);
}

void on_high_U_thresh_trackbar(int, void *)
{
	high_U = max(high_U, low_U + 1);
	setTrackbarPos("High S", "Threshold Selection", high_U);
}

void on_low_V_thresh_trackbar(int, void *)
{
	low_V = min(high_V-1, low_V);
	setTrackbarPos("Low V", "Threshold Selection", low_V);
}

void on_high_V_thresh_trackbar(int, void *)
{
	high_V = max(high_V, low_V + 1);
	setTrackbarPos("High V", "Threshold Selection", high_V);
}

void checkByYUV(const cv::Mat& imgSrc, cv::Mat& mask) {

	static const int step = imgSrc.step / sizeof(uchar);
	static uchar* dataSrc = NULL;
	dataSrc = reinterpret_cast<uchar*>(imgSrc.data);

	// mask
	static const int stepMask = mask.step / sizeof(uchar);
	static uchar* dataMask = NULL;
	dataMask = reinterpret_cast<uchar*>(mask.data);

	int isize = mask.rows * mask.cols;
	int posU = isize;
	int posV = isize * 5/4;
	int posY1, posY2, posY3, posY4;
	//int i;
	for(posU; posU < isize * 5/4; posU++)
	{
		if(dataSrc[posU] >= low_U && dataSrc[posU] <= high_U && dataSrc[posV] >= low_V && dataSrc[posV] <= high_V) //check UV value
		{

			int thuong =(int)( (posU - isize) /(mask.cols/2) );
			int du = (posU - isize)%(mask.cols/2);
			posY1 = thuong * 2*mask.cols + 2*du;
			posY2 = posY1 + 1;
			posY3 = posY1 + mask.cols;
 			posY4 = posY3 + 1;

			if(dataSrc[posY1] >= low_Y && dataSrc[posY1]  <= high_Y)
				dataMask[posY1] = static_cast<uchar>(255);

			if(dataSrc[posY2] >= low_Y && dataSrc[posY2] <= high_Y)
				dataMask[posY2] = static_cast<uchar>(255);

			if(dataSrc[posY3] >= low_Y && dataSrc[posY3] <= high_Y)
				dataMask[posY3] = static_cast<uchar>(255);

			if(dataSrc[posY4] >= low_Y && dataSrc[posY4] <= high_Y)
				dataMask[posY4] = static_cast<uchar>(255);
		}
		//posU +=1;
		posV +=1;
	}
	//std::cout<<i;
}


//int main()
//{
//	Mat frame, yuv, mask;
//	cv::Mat image;
//		image = cv::imread("RGB.png", CV_LOAD_IMAGE_COLOR);   // Read the file
//
//		if(! image.data )                              // Check for invalid input
//		{
//			cout <<  "Could not open or find the image" << std::endl ;
//			return -1;
//		}
//	//cv::resize(image, image, cv::Size(200,200));
//	cv::cvtColor(image, yuv, cv::COLOR_RGB2YUV_I420);
//	cv::Mat maskYUV(cv::Size(1026,632),CV_8UC1);
//	maskYUV.setTo(cv::Scalar(0, 0));
//	namedWindow("Threshold Selection", WINDOW_NORMAL);

//	createTrackbar("Low Y", "Threshold Selection", &low_Y, 255, on_low_Y_thresh_trackbar);
//	createTrackbar("High Y", "Threshold Selection", &high_Y, 255, on_high_Y_thresh_trackbar);
//	createTrackbar("Low U", "Threshold Selection", &low_U, 255, on_low_U_thresh_trackbar);
//	createTrackbar("High U", "Threshold Selection", &high_U, 255, on_high_U_thresh_trackbar);
//	createTrackbar("Low V", "Threshold Selection", &low_V, 255, on_low_V_thresh_trackbar);
//	createTrackbar("High V", "Threshold Selection", &high_V, 255, on_high_V_thresh_trackbar);
//	
//	while ((char)waitKey(1) != 'q')
//	{
//		//inRange(hsv, Scalar(low_Y, low_U, low_V), Scalar(high_Y, high_U, high_V), mask);
//		
//		checkByYUV(yuv, maskYUV);
//		imshow("frame", image);
//		imshow("mask", maskYUV);
//	}	
//
//	return 0;
//}

void checkByRGB(const cv::Mat& imgSrc, cv::Mat& mask) {
	static const int step = imgSrc.step / sizeof(uchar);
	static const int RT = 230; //default 230

	static uchar* dataSrc = NULL;
	dataSrc = reinterpret_cast<uchar*>(imgSrc.data);

	// mask
	static const int stepMask = mask.step / sizeof(uchar);
	static uchar* dataMask = NULL;
	dataMask = reinterpret_cast<uchar*>(mask.data);

	static int i = 0, j = 0, k = 0, idx = 0;
	for (i = 0; i < imgSrc.size().height; ++i) {
		for (j = 0, k = 0; j < step; j += 3, ++k) {
			idx = i * step + j;
			if (dataSrc[idx + 2] > RT && dataSrc[idx + 2] >= dataSrc[idx + 1] && dataSrc[idx + 1] > dataSrc[idx]) {  // RGB color model determine rule
				dataMask[i * stepMask + k] = static_cast<uchar>(255);
			}
		}
	}
}

//
//int main()
//{
//	cv::Mat image_YUV, image_RBG;
//    image_RBG = cv::imread("RGB.png", CV_LOAD_IMAGE_COLOR);   // Read the file
//
//    if(! image_RBG.data )                              // Check for invalid input
//    {
//        cout <<  "Could not open or find the image" << std::endl ;
//        return -1;
//    }
//
//	cv::cvtColor(image_RBG, image_YUV, cv::COLOR_RGB2YUV);
//	cv::Size sizeImg = image_RBG.size();
//
//	cv::Mat maskRGB(sizeImg, CV_8UC1);
//	maskRGB.setTo(cv::Scalar(0, 0));
//	checkByRGB(image_RBG, maskRGB);
//
//	cv::Mat maskYUV(sizeImg,CV_8UC1);
//	maskYUV.setTo(cv::Scalar(0, 0));
//	checkByYUV(image_YUV, maskYUV);
//
//	//cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
//	cv::imshow("image_RBG",image_RBG );                   // Show our image inside it.
//	cv::imshow("image_YUV",image_YUV);
//	imshow("YUV mask",maskYUV);
//    imshow("BGR mask", maskRGB);
//	cv::waitKey(0); 
//	
//	return 0;
//}