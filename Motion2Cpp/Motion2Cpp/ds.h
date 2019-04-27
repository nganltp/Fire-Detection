#ifndef DS_H
#define DS_H

#include "opencv/cv.h"
#include <iostream>     /* C-PlusPlus library */
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/ml.hpp>
#include "opencv/cv.h"
#include "opencv/cxcore.h"

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#include <string>
#include <ctime>

/* STL Library */
#include <map>
#include <list>
#include <deque>
#include <vector>

using namespace cv;
using namespace std;
using namespace cv::ml;



typedef struct RectThrd {

	int rectWidth;
	int rectHeight;
	int cntrArea;


}RectThrd;

/* for initialize rectThrd node */
CV_INLINE RectThrd rectThrd(const int rectWidth, const int rectHeight, const int cntrArea){

	RectThrd rt;

	rt.rectWidth = rectWidth;
	rt.rectHeight = rectHeight;
	rt.cntrArea = cntrArea;

	return rt;
}


//* Optical Flow feature points */
typedef struct Feature{
	cv::Point2f perv;
	cv::Point2f curr;
}Feature;

//* for initialize Feature node */
CV_INLINE Feature feature(const cv::Point2f prev, const cv::Point2f curr){

	Feature fr;

	fr.perv = prev;
	fr.curr = curr;

	return fr;
}


//* rect node(rect space) */
typedef struct OFRect{

	bool match;							// determine whether the rect is match or not
	int countCtrP;						// the pixel count of contour
	int countDetected;                  // the pixel count of contour which is only be detected
	cv::Rect rect;						// rect
	std::vector< Feature > vecFeature;	// optical flow feature points

}OFRect;

//* for initialize ofrect node */
CV_INLINE OFRect ofRect(cv::Rect rect, const int countCtrP){

	OFRect ofr;

	ofr.match = false;
	ofr.countCtrP = countCtrP;
	ofr.rect = rect;

	return ofr;
}



//* marker node */
typedef struct Centroid{

	int countFrame;									 // how many frame the centroid keeping in the region
	cv::Point centroid;								 // first detected centroid
	std::vector< cv::Rect > vecRect;					 // rect information
	std::deque< std::vector< Feature > > dOFRect;  // optical flow feature points

}Centroid;

//* for initailize the new centroid node */
CV_INLINE Centroid centroid(OFRect ofrect){

	Centroid ctrd;

	ctrd.countFrame = 1;  // first node
	ctrd.centroid = cvPoint(ofrect.rect.x + (ofrect.rect.width >> 1), ofrect.rect.y + (ofrect.rect.height >> 1));  // centroid position
	ctrd.vecRect.push_back(ofrect.rect);            // push rect information 
	ctrd.dOFRect.push_back(ofrect.vecFeature);           // push contour optical flow feature(after optical flow) 

	return ctrd;
}

typedef struct DirectionsCount{

	int countRight;
	int countUp;
	int countDown;
	int countLeft;

}DirectionsCount;


CV_INLINE void zeroCount(DirectionsCount & count){
	count.countDown = count.countLeft = count.countRight = count.countUp = 0;
}


#endif