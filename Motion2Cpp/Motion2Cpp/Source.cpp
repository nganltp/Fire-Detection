#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/ml.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#include <iostream>
#include <string>
#include <ctime>
using namespace cv;
using namespace std;
using namespace cv::ml;

#define CELL_SIZE 8

string model_ = "D:\\work\\GIT\\Motion2Cpp\\Motion2Cpp\\LBP-SVM-model.xml";
	//const string& model = model_;
	Ptr<SVM> svm = cv::ml::SVM::load<cv::ml::SVM>(model_);
	Mat feature;

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


void predict(){
	//const string& model = model_;
	Ptr<SVM> svm = cv::ml::SVM::load<cv::ml::SVM>(model_);
    Mat feature;
    string imgpath = "C:\\Users\\gvc\\Downloads\\Microsoft.SkypeApp_kzf8qxf38zg5c!App\\All (2)\\2.jpg";
    Mat img = imread(imgpath, CV_LOAD_IMAGE_GRAYSCALE);
	resize(img,img,cv::Size(64,64));
    ComputeLBPFeatureVector_Uniform(img, Size(CELL_SIZE, CELL_SIZE), feature);
    int result = svm->predict(feature);
	cout << result<< endl;
}
int main( int argc, char** argv )
{
    VideoCapture cap("D:\\work\\EarlyFireDetection-master\\EarlyFireDetection\\EarlyFireDetection\\test_6.mp4"); // open the default camera
    if(!cap.isOpened()){
		cout << "Error opening video stream or file" << endl;
    return -1;
  }

	Mat grayImg, frameDelta, firstFrame;
	int bgThresh = 25, minContourArea = 80;

	Rect rectContour;
	time_t start,end;
	while(1)
    {
		time(&start);
        Mat frame;
        cap >> frame; // get a new frame from camera
		int width = 500;
		int height = int(frame.rows*500/frame.cols);
		resize(frame,frame,cv::Size(width,height));
        cvtColor(frame, grayImg, COLOR_BGR2GRAY);
		Mat threshImg = Mat::zeros(grayImg.rows, grayImg.cols, CV_8UC1);
        GaussianBlur(grayImg, grayImg, Size(21,21), 1.5, 1.5);
       if(firstFrame.empty())
		{
			firstFrame =  grayImg.clone();
		}
		//<________________________________________MOTION_________________________________________>
		//compute the absolute difference between the current frame and
		absdiff(firstFrame, grayImg, frameDelta);
		threshold(frameDelta, threshImg, bgThresh,255, THRESH_BINARY);
		//dilate(threshImg, threshImg, iterations = 2); 
		vector<Vec4i> hierarchy;
		vector<vector<Point>> contours;
		//cout<<contours.size();
		findContours(threshImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); 
		
		//<________________________DRAW CONTOURS AND LOAD MODEL PREDICT_________________________>
		//for( int iContour  = 0; iContour < contours.size(); iContour ++ )
		//{
		//	cout<<"have contour"<<endl;
		//	if (contourArea(contours[iContour]) > minContourArea)
		//	{
		//		cout<<"predict"<<endl;
		//		//drawContours(threshImg, contours, iContour, Scalar(255), CV_FILLED);
		//		//rectContour = boundingRect( contours[i] );
		//		//for(int r = 0; r < rectContour.height; r++)
		//		//	for(int c = 0; c <  rectContour.width; c++)
		//		//	{
		//		//		threshImg.at<uchar>(rectContour.x + c , rectContour.y + r) = 0;
		//		//	}
		//		//<________________________PREDICT________________________>
		//		Rect bCon =  boundingRect(contours[iContour]);
		//		Mat crop_img = frame(bCon);
		//		resize(crop_img,crop_img,cv::Size(64,64));
		//		imshow("crop", crop_img);
		//		//ComputeLBPFeatureVector_Uniform(crop_img, Size(CELL_SIZE, CELL_SIZE), feature);
		//		//int result = svm->predict(feature);
		//		//if(result == 0)
		//			//rectangle(frame,Point (bCon.x,bCon.y), Point (bCon.x + bCon.width,bCon.y + bCon.height), Scalar(0, 255, 0), 2);
		//	}
		//}
		time(&end);
		float seconds = difftime(end,start);
		//cout<< "Fps: " <<(float) 1/seconds << endl;
		/* assign feature points and get the number of feature */
		//getContourFeatures(imgDisplay2, imgDisplay, contour, hierarchy, vecOFRect, rThrd, featuresPrev, featuresCurr);
		

		imshow("thresh",threshImg);
		imshow("frame", frame);
		char c=(char)waitKey(25);
		if(c==27)
			break;
        
   }
	cap.release();
	destroyAllWindows();
	return 0;
}