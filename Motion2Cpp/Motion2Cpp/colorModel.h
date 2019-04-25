#ifndef COLORMODEL_H
#define COLORMODEL_H

#include "opencv/cxcore.h"

/* Find the minimal value of R G B */
template <typename T>
T minrgb(const T r, const T g, const T b) {
	return (r <= g && r <= b) ? r : (g < b) ? g : b;
}


/**
*	@Purpose: check fire-like pixels by rgb model base on reference method
*			  This function will change fire-like pixels to red
*	@Parameter:
*		frame: input source image
*		mask: output mask
*/
void checkByRGB(const cv::Mat& imgSrc, cv::Mat& mask);

/**
*	@Purpose: check fire-like pixels by rgb model base on reference method
*			  This function will change fire-like pixels to red
*	@Parameter:
*		frame: input source image
*		mask: output mask
*/
void checkByRGB(const cv::Mat& imgSrc, const cv::Mat& maskMotion, cv::Mat& maskRGB);


/**
*   @ Function: Convert RGB to HSI
*   H: 0~360(degree)  HUE_R = 0 , HUE_G = 120 , HUE_B = 240
*   S&I: 0~1
*   @Parameter:       all img require same size
*
*                          [depth]           [channel]
*		     imgRGB:     IPL_DEPTH_8U			 3
*		     imgHSI:     IPL_DEPTH_64F			 3
*		     maskRGB:    IPL_DEPTH_8U			 1
*/
void RGB2HSIMask(const cv::Mat& imgRGB, cv::Mat& imgHSI, cv::Mat& maskRGB);

/**
*	@Purpose: check fire-like pixels by rgb model base on reference method
*			  This function will change fire-like pixels to red
*	@Parameter:
*		frame: input source image
*		mask: output mask
*/
void checkByHSI(const cv::Mat& imgRGB, const cv::Mat& imgHSI, cv::Mat& maskRGB, cv::Mat& maskHSI);


/**
*	@Function: markup the intrest region based on mask
*  @Parameter
*		src: input image
*		backup: output image (for display)
*		mask: input mask
*/
void regionMarkup(const cv::Mat& imgSrc, cv::Mat& imgBackup, cv::Mat& mask);




#endif