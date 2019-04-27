#include "opticalFlowTool.h"
#define CELL_SIZE 8

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

void ComputeLBPImage_Uniform(const Mat &srcImage, Mat &LBPImage)
{
    CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);
    LBPImage.create(srcImage.size(), srcImage.type());

    Mat extendedImage;
    copyMakeBorder(srcImage, extendedImage, 1, 1, 1, 1, BORDER_DEFAULT);

    static const int table[256] = { 1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 0, 0, 9, 0, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 14, 0, 15, 16, 17, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0, 0, 20, 0, 21, 22, 23, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25,
        0, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 27, 0, 28, 29, 30, 31, 0, 32, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 36, 37, 38, 0, 39, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42
        , 43, 44, 0, 45, 0, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 47, 48, 49, 0, 50, 0, 0, 0, 51, 52, 53, 0, 54, 55, 56, 57, 58 };

    int heightOfExtendedImage = extendedImage.rows;
    int widthOfExtendedImage = extendedImage.cols;
    int widthOfLBP=LBPImage.cols;
    uchar *rowOfExtendedImage = extendedImage.data+widthOfExtendedImage+1;
    uchar *rowOfLBPImage = LBPImage.data;
    for (int y = 1; y <= heightOfExtendedImage - 2; ++y,rowOfExtendedImage += widthOfExtendedImage, rowOfLBPImage += widthOfLBP)
    {
        uchar *colOfExtendedImage = rowOfExtendedImage;
        uchar *colOfLBPImage = rowOfLBPImage;
        for (int x = 1; x <= widthOfExtendedImage - 2; ++x, ++colOfExtendedImage, ++colOfLBPImage)
        {
            int LBPValue = 0;
            if (colOfExtendedImage[0 - widthOfExtendedImage - 1] >= colOfExtendedImage[0])
                LBPValue += 128;
            if (colOfExtendedImage[0 - widthOfExtendedImage] >= colOfExtendedImage[0])
                LBPValue += 64;
            if (colOfExtendedImage[0 - widthOfExtendedImage + 1] >= colOfExtendedImage[0])
                LBPValue += 32;
            if (colOfExtendedImage[0 + 1] >= colOfExtendedImage[0])
                LBPValue += 16;
            if (colOfExtendedImage[0 + widthOfExtendedImage + 1] >= colOfExtendedImage[0])
                LBPValue += 8;
            if (colOfExtendedImage[0 + widthOfExtendedImage] >= colOfExtendedImage[0])
                LBPValue += 4;
            if (colOfExtendedImage[0 + widthOfExtendedImage - 1] >= colOfExtendedImage[0])
                LBPValue += 2;
            if (colOfExtendedImage[0 - 1] >= colOfExtendedImage[0])
                LBPValue += 1;
            colOfLBPImage[0] = table[LBPValue];
        } // x

    }// y
}


void ComputeLBPFeatureVector_Uniform(const Mat &srcImage, Size cellSize, Mat &featureVector)
{
    CV_Assert(srcImage.depth() == CV_8U&&srcImage.channels() == 1);

    Mat LBPImage;
    ComputeLBPImage_Uniform(srcImage,LBPImage);

    int widthOfCell = cellSize.width;
    int heightOfCell = cellSize.height;
    int numberOfCell_X = srcImage.cols / widthOfCell;
	int numberOfCell_Y = srcImage.rows / heightOfCell;

    int numberOfDimension = 58 * numberOfCell_X*numberOfCell_Y;
    featureVector.create(1, numberOfDimension, CV_32FC1);
    featureVector.setTo(Scalar(0));

    int stepOfCell=srcImage.cols;
    int index = -58;
    float *dataOfFeatureVector=(float *)featureVector.data;
    for (int y = 0; y <= numberOfCell_Y - 1; ++y)
    {
        for (int x = 0; x <= numberOfCell_X - 1; ++x)
        {
            index+=58;

            Mat cell = LBPImage(Rect(x * widthOfCell, y * heightOfCell, widthOfCell, heightOfCell));
            uchar *rowOfCell=cell.data;
            int sum = 0; 
            for(int y_Cell=0;y_Cell<=cell.rows-1;++y_Cell,rowOfCell+=stepOfCell)
            {
                uchar *colOfCell=rowOfCell;
                for(int x_Cell=0;x_Cell<=cell.cols-1;++x_Cell,++colOfCell)
                {
                    if(colOfCell[0]!=0)
                    {
                        ++dataOfFeatureVector[index + colOfCell[0]-1];
                        ++sum;
                    }
                }
            }
            for (int i = 0; i <= 57; ++i)
                dataOfFeatureVector[index + i] /= sum;
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
string model_ = "D:\\work\\GIT\\Fire-Detection\\Motion2Cpp\\Motion2Cpp\\LBP-SVM-model.xml";
//const string& model = model_;
Ptr<SVM> svm = cv::ml::SVM::load<cv::ml::SVM>(model_);
Mat feature_;

void getContourFeatures(
	Mat &srcImage,
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
			//<________________________PREDICT________________________>
 			/*Rect bCon =  boundingRect(contour[i]);
			Mat crop_img = srcImage(bCon);
			resize(crop_img,crop_img,cv::Size(64,64));
			cvtColor(crop_img,crop_img,CV_BGR2GRAY);
			imshow("crop", crop_img);
			ComputeLBPFeatureVector_Uniform(crop_img, Size(CELL_SIZE, CELL_SIZE), feature_);
			int result = svm->predict(feature_);
			if(result == 0)
			{*/
				//rectangle(srcImage,Point (bCon.x,bCon.y), Point (bCon.x + bCon.width,bCon.y + bCon.height), Scalar(0, 255, 0), 2);
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
			//}
			//else
			//{
			//	cout<<"1"<<endl;
			//}
			vecOFRect.push_back(ofRect(aRect, countCtrP));
		}
			/* push to tmp vector for quick access ofrect node */
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