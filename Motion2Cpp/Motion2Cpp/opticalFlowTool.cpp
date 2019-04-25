#include "opticalFlowTool.h"


/* Drawing Arrow for Optical Flow */
//void drawArrow(
//	cv::Mat& imgDisplay,
//	const std::vector<cv::Point2f>& featuresPrev,
//	const std::vector<cv::Point2f>& featuresCurr,
//	const int cornerCount,
//	const char* featureFound
//){
//
//	static int i, lineThickness = 1;
//	static cv::Scalar lineColor = CV_RGB(100, 200, 250);
//	static double angle, hypotenuse, tmpCOS, tmpSIN;
//	static cv::Point p, q;
//	static const double PI_DIV_4 = CV_PI / 4;
//
//	// Draw the flow field
//	for (i = 0; i < cornerCount; ++i)
//	{
//		// if the feature point wasn't be found
//		if (featureFound[i] == 0) {
//			continue;
//		}
//
//		p.x = static_cast<int>(featuresPrev[i].x);
//		p.y = static_cast<int>(featuresPrev[i].y);
//		q.x = static_cast<int>(featuresCurr[i].x);
//		q.y = static_cast<int>(featuresCurr[i].y);
//
//		angle = atan2(static_cast<double>(p.y - q.y), static_cast<double>(p.x - q.x));
//		hypotenuse = sqrt(pow(p.y - q.y, 2.0) + pow(p.x - q.x, 2.0));
//
//
//		q.x = static_cast<int>(p.x - 10 * hypotenuse * cos(angle));
//		q.y = static_cast<int>(p.y - 10 * hypotenuse * sin(angle));
//
//		// '|'
//		cv::line(imgDisplay, p, q, lineColor, lineThickness, CV_AA, 0);
//
//		tmpCOS = 3 * cos(angle + PI_DIV_4);
//		tmpSIN = 3 * sin(angle + PI_DIV_4);
//
//		p.x = static_cast<int>(q.x + tmpCOS);
//		p.y = static_cast<int>(q.y + tmpSIN);
//		// '/'
//		cv::line(imgDisplay, p, q, CV_RGB(255, 0, 0), lineThickness, CV_AA, 0);
//
//		p.x = static_cast<int>(q.x + tmpCOS);
//		p.y = static_cast<int>(q.y + tmpSIN);
//		// '\'
//		cv::line(imgDisplay, p, q, CV_RGB(255, 0, 0), lineThickness, CV_AA, 0);
//	}
//}



void fireLikeRegion(cv::Mat& mask, const cv::Point pt1, const cv::Point pt2){

	static const int stepMask = mask.step / sizeof(uchar);
	static uchar* dataMask = NULL;
	dataMask = reinterpret_cast<uchar*>(mask.data);
	static int i, j;

	for (i = pt1.y; i < pt2.y; ++i) {
		for (j = pt1.x; j < pt2.x; ++j) {
			dataMask[i * stepMask + j] = 255;
		}
	}
}

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
	std::vector<std::vector<cv::Point>>& contour,
	std::vector<cv::Vec4i>& hierarchy,
	std::vector<OFRect> & vecOFRect,
	const RectThrd & trd,
	std::vector<cv::Point2f>& featuresPrev,
	std::vector<cv::Point2f>& featuresCurr
){

	static cv::Rect aRect;
	static cv::Point p;
	static unsigned int countCtrP;

	int idxFeature = 0;

	/* thresholding on connected component */
	for (int i = 0; i < contour.size(); ++i) {  // contour-based visiting

		/* Recting the Contour with smallest rectangle */
		aRect = cv::boundingRect(contour[i]);

		/* checking the area */
		if (aRect.width > trd.rectWidth && aRect.height > trd.rectHeight && fabs(cv::contourArea(contour[i])) > trd.cntrArea) {
			/* for each contour pixel count	*/
			countCtrP = 0;
			//<________________________DRAW CONTOURS AND LOAD MODEL PREDICT_________________________>
			cout<<"predict"<<endl;
			//drawContours(threshImg, contours, iContour, Scalar(255), CV_FILLED);
			//rectContour = boundingRect( contours[i] );
			//for(int r = 0; r < rectContour.height; r++)
			//	for(int c = 0; c <  rectContour.width; c++)
			//	{
			//		threshImg.at<uchar>(rectContour.x + c , rectContour.y + r) = 0;
			//	}
			////<________________________PREDICT________________________>
 		//	Rect bCon =  boundingRect(contour[i]);
			//Mat crop_img = frame(bCon);
			//resize(crop_img,crop_img,cv::Size(64,64));
			//cvtColor(crop_img,crop_img,CV_BGR2GRAY);
			//imshow("crop", crop_img);
			//ComputeLBPFeatureVector_Uniform(crop_img, Size(CELL_SIZE, CELL_SIZE), feature_);
			//int result = svm->predict(feature_);
			//if(result == 0)
			//	rectangle(frame,Point (bCon.x,bCon.y), Point (bCon.x + bCon.width,bCon.y + bCon.height), Scalar(0, 255, 0), 2);
			//else
			//	cout<<"1"<<endl;
		
		}
			/* access points on each contour */
			for (int j = 0; j < contour[i].size(); ++j){
				p = contour[i][j];
				//printf(" (%d,%d)\n", p->x, p->y );
				cv::Point2f feature(static_cast<float>(p.x), static_cast<float>(p.y));
				if (idxFeature == 0) {
					featuresPrev.pop_back();
				}
				featuresPrev.push_back(feature);
				++countCtrP;
				++idxFeature;
			}

			/* push to tmp vector for quick access ofrect node */
			vecOFRect.push_back(ofRect(aRect, countCtrP));
	}
}


/* assign feature points to fire-like obj and then push to multimap
input:
vecOFRect:      fire-like obj
status:			the feature stutas(found or not) after tracking
featuresPrev:	previous feature points
featuresCurr:   current feature points after tracking

output:
mulMapOFRect:	new candidate fire-like obj in current frame(with rectangle and motion vector information)

*/
void assignFeaturePoints(
	std::multimap< int, OFRect > & mulMapOFRect,
	std::vector< OFRect > & vecOFRect,
	std::vector<uchar> status,
	std::vector<cv::Point2f>& featuresPrev,
	std::vector<cv::Point2f>& featuresCurr
){
	static std::vector< OFRect >::iterator itVecOFRect;
	static unsigned int foundCount = 0;

	int i = 0; // feature point index

	// visit each ofrect in vecOFRect 
	for (itVecOFRect = vecOFRect.begin(); itVecOFRect != vecOFRect.end(); ++itVecOFRect){

		foundCount = 0;

		// contour points count
		for (int p = 0; p < (*itVecOFRect).countCtrP; ++p){
			// if the feature point was be found
			if (status[i] == 0) {
				++i;
				continue;
			}
			else{
				/* push feature to vector of ofrect */
				(*itVecOFRect).vecFeature.push_back(feature(featuresPrev[i], featuresCurr[i]));
				++i;
				++foundCount;
			}
		}

		(*itVecOFRect).countDetected = foundCount;

		/* insert ofrect to multimap */
		mulMapOFRect.insert(std::pair< int, OFRect >((*itVecOFRect).rect.x, *itVecOFRect));
	}
	/* clear up container */
	vecOFRect.clear();
}