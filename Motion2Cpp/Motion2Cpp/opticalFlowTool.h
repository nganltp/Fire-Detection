/* Tool function for optical flow */

#ifndef OPTICALFLOWTOOL_H
#define OPTICALFLOWTOOL_H

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "ds.h"


///* Drawing Arrow for Optical Flow */
//void drawArrow(
//	cv::Mat& imgDisplay,
//	const std::vector<cv::Point2f>& featuresPrev,
//	const std::vector<cv::Point2f>& featuresCurr,
//	const int cornerCount,
//	const char* featureFound
//);
//

void fireLikeRegion(cv::Mat& mask, const cv::Point pt1, const cv::Point pt2);
void ComputeLBPFeatureVector_Uniform(const Mat &srcImage, Size cellSize, Mat &featureVector);
void ComputeLBPImage_Uniform(const Mat &srcImage, Mat &LBPImage);

/* get the feature points from contour
input:
imgDisplayCntr      : img for display contours
imgDisplayFireRegion: img for boxing the fire-like region with rectangle
contour             : after cvFindContour()
trd                 : threshold
output:
vecOFRect           : fire-like obj will be assign to this container
featuresPrev        : previous contours points
featuresCurr        : current contours points
return:
the number of contour points
*/
void getContourFeatures(
	Mat &srcImage,
	std::vector<std::vector<cv::Point>>& contour,
	std::vector<cv::Vec4i>& hierarchy,
	std::vector< OFRect >& vecOFRect,
	const RectThrd& trd,
	std::vector<cv::Point2f>& featuresPrev,
	std::vector<cv::Point2f>& featuresCurr
);

void checkByRGB(const cv::Mat& imgSrc, const cv::Mat& maskMotion, cv::Mat& mask);

/* assign feature points to fire-like obj and then push to multimap
input:
vecOFRect:      fire-like obj
status:			the feature stutas(found or not) after tracking
featuresPrev:	previous feature points
featuresCurr:   current feature points after tracking

output:
mulMapOFRect:	new candidate fire-like obj in current frame(with rectangle and motion vector information)

*/

void checkByYUV(const cv::Mat& imgSrc, const cv::Mat& maskMotion, cv::Mat& mask);
void assignFeaturePoints(
	std::multimap< int, OFRect > & mulMapOFRect,
	std::vector< OFRect > & vecOFRect,
	std::vector<uchar> status,
	std::vector<cv::Point2f>& featuresPrev,
	std::vector<cv::Point2f>& featuresCurr
);

#endif 