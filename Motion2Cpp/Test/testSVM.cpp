#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/ml.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream> //include for read xml file
#include <string>

//#include <QString>

using namespace std;
using namespace cv;
using namespace cv::ml;

#define FireSamNO 500    
#define NoneSamNO 500    
#define CELL_SIZE 8

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

void train(){

    Ptr<SVM> svm = SVM::create();
    svm->setC(1000.1);
    //svm->setCoef0(0);
    svm->setDegree(3);
    svm->setGamma(0.0001);
    svm->setKernel(SVM::LINEAR);
    //svm->setNu(0);
    //svm->setP(0);
    svm->setType(SVM::C_SVC);
    //svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

    string imgpath;
    Mat sampleFeatureMat;
    Mat sampleLabelMat;
	double raitoRandom =0.8;

	string line[1500];
	ifstream myfile ("savePathFireImg.txt");
	if (myfile.is_open())
	{
		for(int i = 0; i< 1000;i++)
			getline(myfile,line[i]);
		myfile.close();
	}

	else cout << "Unable to open file"; 
	int dem = 0;
	Mat yuv, yuv_gray;
    for(int num = 0; num < FireSamNO*raitoRandom; num++){
		dem++;
        imgpath = line[num];
        Mat feature;
        Mat img = imread(imgpath);
		cvtColor(img,yuv,COLOR_RGB2YUV_I420);
		cvtColor(yuv, yuv_gray, COLOR_YUV2GRAY_I420);
	
        //cout << "Processing FireSamNO" << imgpath<< endl;
        ComputeLBPFeatureVector_Uniform(yuv_gray, Size(CELL_SIZE, CELL_SIZE), feature);
        sampleFeatureMat.push_back(feature);
        sampleLabelMat.push_back(0);
    }
	cout<<"data train fire "<<dem<<endl;
	dem = 0;
	cout<<"---------------------------------------------------------------------->"<<endl;
	for(int num = 500; num < (FireSamNO + NoneSamNO*raitoRandom); num++){
		dem++;
        imgpath = line[num];
        Mat feature;
        Mat img = imread(imgpath);
		cvtColor(img,yuv,COLOR_RGB2YUV_I420);
		cvtColor(yuv, yuv_gray, COLOR_YUV2GRAY_I420);
        //cout << "Processing NoneSamNO" << imgpath << endl;
        ComputeLBPFeatureVector_Uniform(yuv_gray, Size(CELL_SIZE, CELL_SIZE), feature);
        sampleFeatureMat.push_back(feature);
        sampleLabelMat.push_back(1);
    }
	cout<<"data train None fire " << dem<<endl;
	svm->train(sampleFeatureMat, ROW_SAMPLE, sampleLabelMat);
    svm->save("D:\\work\\GIT\\Fire-Detection\\Motion2Cpp\\Motion2Cpp\\LBP-SVM-model.xml");
    cout << "train done" << endl;
}

void predict_img(){
	string model_ = "D:\\work\\GIT\\Fire-Detection\\Motion2Cpp\\Motion2Cpp\\LBP-SVM-model.xml";
	//const string& model = model_;
	Ptr<SVM> svm = cv::ml::SVM::load<cv::ml::SVM>(model_);
    Mat feature;
    string imgpath = "D:\\work\\GIT\\TestImg\\F2.jpg";
    Mat img = imread(imgpath);
	Mat yuv,yuv_gray;

	cvtColor(img,yuv,COLOR_RGB2YUV_I420);
	cvtColor(yuv, yuv_gray, COLOR_YUV2GRAY_I420);

	resize(yuv_gray,yuv_gray,cv::Size(64,64));
    ComputeLBPFeatureVector_Uniform(yuv_gray, Size(CELL_SIZE, CELL_SIZE), feature);
    int result = svm->predict(feature);
	cout << result<< endl;
}
	
int main()
{
	//train();

	predict_img();
	
	/*Mat img = imread("D:\\work\\GIT\\Fire-Detection\\Motion2Cpp\\Test\\number.png");
	Mat yuv, yuv_gray, img_gray;

	cvtColor(img,yuv,COLOR_RGB2YUV_I420);
	cvtColor(yuv, yuv_gray, COLOR_YUV2GRAY_I420);
	cvtColor(img, img_gray, COLOR_RGB2GRAY);

	cout<<img.cols << "x" << img.rows<<endl;
	cout<< yuv.cols << "x" << yuv.rows <<endl;
	cout<< yuv_gray.cols << "x" << yuv_gray.rows <<endl;

	imshow("bgr", img);
	imshow("yuv",yuv);
	imshow("bgr_gray", img_gray);
	imshow("yuv_gray",yuv_gray);
	waitKey(0);*/
	system("pause");
    return 0;
}