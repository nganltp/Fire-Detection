#include "TGMTcolor.h"
#include "TGMTdebugger.h"
#include "TGMTutil.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string TGMTcolor::GetColorName(cv::Mat imgInput, cv::Point p)
{
	if (!imgInput.data)
		return "Error";

	cv::Mat imgHSV;
	cv::cvtColor(imgInput, imgHSV, CV_BGR2HSV);

	cv::Vec3b pixel = imgHSV.at<cv::Vec3b>(p);
	

#if DEBUG	
	int h = pixel[0];
	float s = pixel[1];
	float v = pixel[2];
	std::cout << h * 2 << " - " << s / 255.f << " - " << v / 255.f << "\n";
#endif
	
	cv::Scalar color = GetPixelColorType(pixel);
	if (color == BLACK)
		return "Black";
	else if (color == BLUE)
		return "Blue";
	else if (color == CYAN)
		return "Cyan";
	else if (color == GREY)
		return "Grey";
	else if (color == GREEN)
		return "Green";
	else if (color == ORANGE)
		return "Orange";
	else if (color == PINK)
		return "Pink";
	else if (color == PURPLE)
		return "Purple";
	else if (color == RED)
		return "Red";
	else if (color == WHITE)
		return "White";
	else if (color == YELLOW)
		return "Yellow";
	else 
		return "unknown";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string TGMTcolor::GetColorName(cv::Mat imgInput, int x, int y)
{
	return GetColorName(imgInput, cv::Point(x, y));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Scalar TGMTcolor::GetPixelColorType(int H, int S, int V)
{
	if (V < 75)
		return BLACK;
	else if (V > 190 && S < 27)
		return WHITE;
	else if (S < 53 && V < 185)
		return GREY;
	else
	{	// Is a color
		if (H < 14)
			return RED;
		else if (H < 25)
			return ORANGE;
		else if (H < 34)
			return YELLOW;
		else if (H < 73)
			return GREEN;
		else if (H < 102)
			return CYAN;
		else if (H < 127)
			return BLUE;
		else if (H < 149)
			return PURPLE;
		else if (H < 175)
			return PINK;
		else	// full circle 
			return RED;	// back to Red
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Scalar TGMTcolor::GetPixelColorType(cv::Vec3b pixel)
{
	uchar H = pixel[0];  	// Hue
	uchar S = pixel[1];		// Saturation
	uchar V = pixel[2]; 	// Value (Brightness)
	return GetPixelColorType(H, S, V);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Scalar TGMTcolor::GetPixelValue(cv::Mat matInput, cv::Point p)
{
	cv::Vec3b pixel = matInput.at<cv::Vec3b>(p.x, p.y);

	PrintMessage("(%d, %d, %d)", pixel[0], pixel[1], pixel[2]);

	return cv::Scalar(pixel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Scalar TGMTcolor::GetRandomColor()
{
	int R = TGMTutil::GetRandomInt();	
	int G = TGMTutil::GetRandomInt();
	int B = TGMTutil::GetRandomInt();
	return cv::Scalar(B, G, R);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat TGMTcolor::FilterColor(cv::Mat matInput, uchar minH, uchar maxH, uchar minS, uchar maxS, uchar minV, uchar maxV, bool isHSV)
{
	cv::Scalar lower = cv::Scalar(minH, minS, minV);
	cv::Scalar higher = cv::Scalar(maxH, maxS, maxV);
	return FilterColor(matInput, lower, higher, isHSV);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat TGMTcolor::FilterColor(cv::Mat matInput, cv::Scalar lower, cv::Scalar higher, bool isHSV )
{
	cv::Mat matHsv;
	if (!isHSV)
		cv::cvtColor(matInput, matHsv, CV_BGR2HSV);
	else
		matHsv = matInput;

	cv::Mat mask, matResult;
	cv::inRange(matHsv, lower, higher, mask);

	matInput.copyTo(matResult, mask);
	return matResult;
}