#include "TGMTshape.h"
#include "TGMTdebugger.h"
#include "TGMTimage.h"
#include "TGMTcolor.h"
#include "TGMTdraw.h"

//TGMTshape::TGMTshape()
//{
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//TGMTshape::~TGMTshape()
//{
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<cv::Vec2f> TGMTshape::DetectLine(cv::Mat matInput)
{
	std::vector<cv::Vec2f> result;
	cv::HoughLines(matInput, result, 1, CV_PI / 180, 100);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int TGMTshape::FindLineAndCircle(cv::Mat imgInput)
{
	if (!imgInput.data) // Check for invalid input
	{
		return 0;
	}
	cv::Mat gray;
	cvtColor(imgInput, gray, CV_BGR2GRAY);
	GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);

	// Tim duong thang
	cv::Mat canny;
	Canny(gray, canny, 100, 200, 3, false);
	std::vector<cv::Vec4i> lines;
	HoughLinesP(canny, lines, 1, CV_PI / 180, 50, 60, 10);

	
	// Ve duong thang, duong tron len anh
	for (int i = 0; i < lines.size(); i++)
	{
		cv::Vec4i l = lines[i];
		line(imgInput, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 2);
	}
	cv::imshow("Anh sau khi tim thay duong thang - Duong tron", imgInput);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<TGMTshape::Circle> TGMTshape::DetectCircle(cv::Mat matInput)
{
	cv::Mat gray = TGMTimage::ConvertToGray(matInput);
	GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);
	// Tim duong tron
	
	std::vector<cv::Vec3f> circles;
	std::vector<Circle> result;
	HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 100, 200, 100, 0, 0);
	for (size_t i = 0; i < circles.size(); i++)
	{
		result.push_back(Circle(circles[i]));
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat TGMTshape::DetectAndDrawLine(cv::Mat imgSrc)
{
	if (!imgSrc.data)
		return cv::Mat(1, 1, 0);
	cv::Mat imgBlur, imgGray, imgBinary;
	cv::cvtColor(imgSrc, imgGray, CV_BGR2GRAY);

	//convert BGR image to binary image
#if 0
	GaussianBlur(imgGray, imgBlur, cv::Size(3, 3), 2);
	imshow("imgBlur", imgBlur);
	cv::Canny(imgBlur, imgBinary, 50, 150, 3);
#else
	GaussianBlur(imgGray, imgBlur, cv::Size(9, 9), 2);
	cv::threshold(imgBlur, imgBinary, 0, 255, CV_THRESH_OTSU);
#endif
	imshow("imgBinary", imgBinary);
	//DEBUG_IMAGE(imgBlur, "IMG BLUR");

	//detect lines
#if 1

	std::vector<cv::Vec2f> lines;
	HoughLines(imgBinary, lines, 1, CV_PI / 180, 100);
	TGMTdraw::DrawLines(imgSrc, lines);
#else

	std::vector<cv::Vec4i> lines;
	HoughLinesP(imgBinary, lines, 1, CV_PI / 180, 50, 50, 10);
	TGMTshape::DrawLines(imgSrc, lines);
#endif
	return imgSrc;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat TGMTshape::DetectAndDrawLine(std::string filePath)
{
	return DetectAndDrawLine(cv::imread(filePath));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Point TGMTshape::GetCenterPoint(cv::Rect rect)
{
	return cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTshape::IsOverlap(cv::Rect rect1, cv::Rect rect2)
{
	int area = (rect1 & rect2).area();
	return area > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<cv::Rect> TGMTshape::ExpandRects(std::vector<cv::Rect> rects, float scaleRatioX, float scaleRatioY, cv::Mat currentMat)
{
	std::vector<cv::Rect> result;
	for (size_t i = 0; i < rects.size(); i++)
	{
		cv::Rect r = ExpandRect(rects[i], scaleRatioX, scaleRatioY);
		if (IsRectInsideMat(r, currentMat))
			result.push_back(r);
		else
			result.push_back(rects[i]);
	}
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Rect TGMTshape::ExpandRect(cv::Rect rect, float scaleRatioX, float scaleRatioY)
{
	if (scaleRatioX == 1 && scaleRatioY == 1)
		return rect;

	cv::Rect r = rect;
	r.x -= (r.width * scaleRatioX - r.width) / 2;
	r.y -= (r.height * scaleRatioY - r.height) / 2;
	r.width *= scaleRatioX;
	r.height *= scaleRatioY;

	if (IsValidRect(r))
	{
		return r;
	}
	return rect;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTshape::IsValidRect(cv::Rect rect)
{
	return rect.x >= 0 && rect.y >= 0 && rect.width > 0 && rect.height > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTshape::IsRectInsideMat(cv::Rect rect, cv::Mat matInput, int padding)
{	
	return rect.x > padding &&
		rect.y > padding &&
		rect.x + rect.width < matInput.cols - padding &&
		rect.y + rect.height < matInput.rows - padding;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TGMTshape::IsRectInsideRect(cv::Rect rectIn, cv::Rect rectOut)
{
	return rectIn.x >= rectOut.x &&
		rectIn.y >= rectOut.y &&
		rectIn.x + rectIn.width <= rectOut.x + rectOut.width &&
		rectIn.y + rectIn.height <= rectOut.y + rectOut.height;
}