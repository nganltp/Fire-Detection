#include "ds.h"
#include "opticalFlowTool.h"
#include "fireBehaviorAnalysis.h"

/* Non-named namespace, global constants */
namespace{

	/* Background Mode */
#if defined (BGS_MODE) && (BGS_MODE == ON)
	const int BGM_FRAME_COUNT = 20;
#else
	const int BGM_FRAME_COUNT = 0;
#endif 

	/* Optical Flow Parameters */
	const int MAX_CORNERS = 10000;
	const int WIN_SIZE = 5; //default: 5

	/* Processing Window Size (Frame) */
	const unsigned int PROCESSING_WINDOWS = 15; //default: 15

	/* Fire-like Region Threshold */
	const int RECT_WIDTH_THRESHOLD = 5; //default: 5
	const int RECT_HEIGHT_THRESHOLD = 5; //default: 5
	const int CONTOUR_AREA_THRESHOLD = 12; //default: 12
	const int CONTOUR_POINTS_THRESHOLD = 12; //default: 12
}

bool checkContourPoints(Centroid & ctrd, const unsigned int thrdcp, const unsigned int pwindows) {

	std::deque< std::vector< Feature > >::iterator itrDeq = ctrd.dOFRect.begin();

	unsigned int countFrame = 0;

	// contour points of each frame
	for (; itrDeq != ctrd.dOFRect.end(); ++itrDeq){

		if ((*itrDeq).size() < thrdcp) {
			++countFrame;
		}
	}

	return (countFrame < pwindows / 3) ? true : false;
}


void motionOrientationHist(std::vector< Feature > & vecFeature, unsigned int orien[4]){

	std::vector< Feature >::iterator itrVecFeature;

	/* each point of contour  */
	for (itrVecFeature = vecFeature.begin(); itrVecFeature != vecFeature.end(); ++itrVecFeature) {

		/* orientation */
		if ((*itrVecFeature).perv.x >= (*itrVecFeature).curr.x){
			if ((*itrVecFeature).perv.y >= (*itrVecFeature).curr.y){
				++orien[0];   // up-left 
			}
			else{
				++orien[2];   // down-left
			}
		}
		else{
			if ((*itrVecFeature).perv.y >= (*itrVecFeature).curr.y){
				++orien[1];   // up-right
			}
			else{
				++orien[3];   // down-right
			}
		}
	}
}

double getEnergy(std::vector< Feature > & vecFeature, unsigned int & staticCount, unsigned int & totalPoints){

	std::vector< Feature >::iterator itrVecFeature;

	/* initialization */
	double tmp, energy = 0.0;

	/* each contour point */
	for (itrVecFeature = vecFeature.begin(); itrVecFeature != vecFeature.end(); ++itrVecFeature) {

		/* energy */
		tmp = pow(abs((*itrVecFeature).curr.x - (*itrVecFeature).perv.x), 2) + pow(abs((*itrVecFeature).curr.y - (*itrVecFeature).perv.y), 2);
		if (tmp < 1.0){
			++staticCount;
		}
		else if (tmp < 100.0){
			energy += tmp;
			++totalPoints;
		}
	}
	return energy;
}

bool checkContourEnergy(Centroid & ctrd, const unsigned int pwindows) {

	std::deque< std::vector< Feature > >::iterator itrDeq = ctrd.dOFRect.begin();

	std::vector< Feature >::iterator itrVecFeature;

	unsigned int staticCount = 0, orienCount = 0, orienFrame = 0, totalPoints = 0, passFrame = 0, staticFrame = 0;
	unsigned int orien[4] = { 0 };

	/* contour motion vector of each frame */
	for (; itrDeq != ctrd.dOFRect.end(); ++itrDeq){
		/* flash */
		staticCount = staticFrame = orienCount = staticFrame = staticCount = totalPoints = 0;
		/* energy analysis */
		if (getEnergy((*itrDeq), staticCount, totalPoints) > totalPoints >> 1){
			++passFrame;
		}
		if (staticCount > (*itrDeq).size() >> 1){
			++staticFrame;
		}

		/* flash */
		memset(orien, 0, sizeof(unsigned int) << 2);

		/* orientation analysis */
		motionOrientationHist((*itrDeq), orien);

		for (int i = 0; i < 4; ++i){
			if (orien[i] == 0){
				++orienCount;
			}
		}
		if (orienCount >= 1){
			++orienFrame;
		}
	}
	/* by experience */
	static const unsigned int thrdPassFrame = pwindows >> 1, thrdStaticFrame = pwindows >> 2, thrdOrienFrame = (pwindows >> 3) + 1;

	return (passFrame > thrdPassFrame && staticFrame < thrdStaticFrame && orienFrame < thrdOrienFrame) ? true : false;
}

void matchCentroid(
	cv::Mat& imgFireAlarm,
	std::list< Centroid > & listCentroid,
	std::multimap< int, OFRect > & mulMapOFRect,
	int currentFrame, const int thrdcp,
	const unsigned int pwindows
){

	static std::list< Centroid >::iterator itCentroid;		             // iterator of listCentroid
	static std::multimap< int, OFRect >::iterator itOFRect, itOFRectUp;  // iterator of multimapBRect

	static bool flagMatch = false;                                       // checking the list(centroid) and map(rect) match or not
	static cv::Rect* pRect = NULL;
	static cv::Rect rectFire(0, 0, 0, 0);


	/* Check listCentroid node by node */
	itCentroid = listCentroid.begin();
	static unsigned int rectCount = 0;

	while (itCentroid != listCentroid.end()) {
		/* setup mulMapBRect upper bound */
		itOFRectUp = mulMapOFRect.upper_bound((*itCentroid).centroid.x);

		flagMatch = false;

		/* visit mulMapOFRect between range [itlow,itup) */
		for (itOFRect = mulMapOFRect.begin(); itOFRect != itOFRectUp; ++itOFRect) {

			/* for easy access info */
#ifndef CENTROID
#define CENTROID (*itCentroid).centroid 				
#endif			
			/* for quick access info */
			pRect = &(*itOFRect).second.rect;

			/* matched */
			if (CENTROID.y >= (*pRect).y && ((*pRect).x + (*pRect).width) >= CENTROID.x && ((*pRect).y + (*pRect).height) >= CENTROID.y) {

				/* push rect to the matched listCentorid node */
				(*itCentroid).vecRect.push_back(*pRect);

				/* push vecFeature to matched listCentorid node */
				(*itCentroid).dOFRect.push_back((*itOFRect).second.vecFeature);
				if (++((*itCentroid).countFrame) > pwindows)
						/* recting the fire region */
						cv::rectangle(imgFireAlarm, rectFire, Scalar(255, 0, 0), 3);
				/* Update countFrame and judge the threshold of it */
				if (++((*itCentroid).countFrame) == pwindows) { // ==
					/* GO TO PROCEESING DIRECTION MOTION */
					if (judgeDirectionsMotion((*itCentroid).vecRect, rectFire) && checkContourPoints(*itCentroid, thrdcp, pwindows) && checkContourEnergy(*itCentroid, pwindows)) {

						/* recting the fire region */
						cv::rectangle(imgFireAlarm, rectFire, Scalar(255, 0, 0), 3);
						//imshow("rect",imgFireAlarm);
						cout << "Alarm: " << currentFrame << endl;
						//(*itCentroid).countFrame = 0;
						//cv::imshow("Video", imgFireAlarm);

						/* halt */

						/* HALT_MODE */
#if defined (HALT_MODE) && (HALT_MODE == ON)
						cvWaitKey(0);
#endif 
						/* checking the optical flow information */
						//writeContourPointsInfo( ofInfoFileFolder, *itCentroid, currentFrame, rectCount );

						/* save as image */
						//sprintf( outfile, RectImgFilePath, currentFrame-PROCESSING_WINDOWS+1, currentFrame );
						//cvSaveImage( outfile, imgDisplay );

						/* save rect information */
						//fileStream< CvRect > fsOut;
						//sprintf( outfile, RectInfoFilePath, currentFrame-PROCESSING_WINDOWS+1, ++rectCount );
						//fsOut.WriteXls( (*itCentroid).vecRect, outfile, currentFrame-PROCESSING_WINDOWS+1 );

						// then go to erase it
					}
					else{
						break;   // if not on fire go to erase it
					}
				}

				/* mark this rect as matched */
				(*itOFRect).second.match = true;
				flagMatch = true;
				++itCentroid;
				break;    // if mateched break the inner loop
			}
			// if ended the maprect and not mateched anyone go to erase it

		} // for (multimapBRect)

		/* if not found erase node */
		if (!flagMatch) {
			itCentroid = listCentroid.erase(itCentroid); 
		}
	}

	//cout << "======================================================================================" << endl;
	/* push new rect to listCentroid */
	for (itOFRect = mulMapOFRect.begin(); itOFRect != mulMapOFRect.end(); ++itOFRect) {

		if (!(*itOFRect).second.match) {
			/* push new node to listCentroid */
			listCentroid.push_back(centroid((*itOFRect).second));
			//cout << "after rect: " << endl;
			//cout << (*itBRect).second << endl;	x
		}
	}

	//cout <<"after list count: "<< listCentroid.size() << endl;

	/* clear up container */
	mulMapOFRect.clear();
}


int main()
{
	string namevideo = "NONE_lamp.mp4";
	VideoCapture cap("D:\\work\\GIT\\testVideo\\" + namevideo); // open the default camera

    if(!cap.isOpened()){
		cout << "Error opening video stream or file" << endl;
    return -1;
  }
	cv::Mat imgSrc;
	cap.read(imgSrc);
	if (imgSrc.empty()) {
		fprintf(stderr, "Cannot open video!\n");
		return 1;
	}
	//int w = 500;
	//int h = int(imgSrc.rows*500/imgSrc.cols);
	int w = 427;//640
	int h = 240;//360
	//VideoWriter video("D:\\work\\GIT\\writeVideo\\out.avi", -1 ,20, Size(w,h),true);
	cv::resize(imgSrc, imgSrc, cv::Size(w, h));
	cv::Size sizeImg = imgSrc.size();
	Mat frameDelta, firstFrame;

	Mat imgCurr(sizeImg, CV_8UC1);
	Mat imgGray(sizeImg, CV_8UC1);
	int bgThresh = 25, minContourArea = 80;

	Mat maskRGB(sizeImg, CV_8UC1);

	Rect rectContour;
	time_t start,end;
	
	// notify the current frame 
	unsigned long currentFrame = 0;

	/* Rect Motion */
	std::list< Centroid > listCentroid;							  // Centroid container
	std::vector< OFRect > vecOFRect;							  // tmp container for ofrect
	std::multimap< int, OFRect > mulMapOFRect;					  // BRect container
	
	// Buffer for Pyramid image  
	std::vector<cv::Point2f> featuresPrev;
	std::vector<cv::Point2f> featuresCurr;
	
	// Pyramid Lucas-Kanade 
	std::vector<uchar> featureFound;
	std::vector<float> featureErrors;
	cv::Size sizeWin(WIN_SIZE, WIN_SIZE);
	RectThrd rThrd = rectThrd(RECT_WIDTH_THRESHOLD, RECT_HEIGHT_THRESHOLD, CONTOUR_AREA_THRESHOLD);

	Mat element = getStructuringElement(CV_SHAPE_RECT, Size(3,5), Point(1,2));



	while(1)
    {
		maskRGB.setTo(cv::Scalar(0, 0));
		featuresPrev.clear();
		featuresPrev.push_back(cv::Point2f(0, 0));
		featuresCurr.clear();
		featuresCurr.push_back(cv::Point2f(0, 0));
		
		double t = (double)getTickCount();

        cap.read(imgSrc);  // get the first frame 
		if (imgSrc.empty()) {
			break;   // exit if unsuccessful or Reach the end of the video
		}
		cv::resize(imgSrc, imgSrc, sizeImg);
		
		// convert rgb to gray 
		cv::cvtColor(imgSrc, imgGray, CV_BGR2GRAY);

		cap.read(imgSrc); // get the second frame

		if (imgSrc.empty()) {
			break;
		}
		cv::resize(imgSrc, imgSrc, sizeImg);
		// the second frame ( gray level )
		cv::cvtColor(imgSrc, imgCurr, CV_BGR2GRAY);
		
		//video.write(imgSrc);
		
		Mat threshImg = Mat::zeros(sizeImg, CV_8UC1);
        GaussianBlur(imgGray, imgGray, Size(21,21), 1.5, 1.5);
       if(firstFrame.empty())
		{
			firstFrame =  imgGray.clone();
		}
//		//<________________________________________MOTION_________________________________________>
		//compute the absolute difference between the current frame and
		absdiff(firstFrame, imgGray, frameDelta);
		threshold(frameDelta, threshImg, bgThresh,255, THRESH_BINARY);
		//dilate(threshImg, threshImg, iterations = 2); 

		checkByRGB(imgSrc,threshImg,maskRGB);
		cv::imshow("MaskRGB", maskRGB);
		dilate(maskRGB, maskRGB,element);

		vector<Vec4i> hierarchy;
		vector<vector<Point>> contour;
		//cout<<contours.size();
		findContours(/*threshImg*/maskRGB.clone(), contour, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); 

		//________________________________EARLY FIRE DETECTION___________________________________
		/* assign feature points and get the number of feature */
		getContourFeatures(imgSrc,contour, hierarchy, vecOFRect, rThrd, featuresPrev, featuresCurr);

		// Pyramid L-K Optical Flow
		cv::calcOpticalFlowPyrLK(
			imgGray,
			imgCurr,
			featuresPrev,
			featuresCurr,
			featureFound,
			featureErrors,
			sizeWin,
			2,
			cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.3),
			0);

		/* assign feature points to fire-like obj and then push to multimap */
		assignFeaturePoints(mulMapOFRect, vecOFRect, featureFound, featuresPrev, featuresCurr);

		/* compare the mulMapOFRect space with listCentroid space, if matching insert to listCentroid space as candidate fire-like obj */
		matchCentroid(imgSrc, listCentroid, mulMapOFRect, currentFrame, CONTOUR_POINTS_THRESHOLD, PROCESSING_WINDOWS);

		//writer.write(imgFireAlarm);
		currentFrame++;
		//std::cout << "< Frame >: " << currentFrame++ << endl;

		t =((double)cvGetTickCount()-t)/getTickFrequency();
		//cout<<"FPS"<<1/t<<endl;
		

		imshow("thresh",threshImg);
		imshow("imgSrc", imgSrc);
		char c=(char)waitKey(25);
		if(c==27)
			break;
        
   }

	cap.release();
	imgCurr.release();
	imgGray.release();
	frameDelta.release();
	cv::destroyAllWindows();
	return 0;
}