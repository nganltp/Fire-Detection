#include <iostream>
#include <fstream>
#include <string>
using namespace std;
int main()
{
	 ofstream file;
	file.open ("D:\\work\\GIT\\Fire-Detection\\Motion2Cpp\\Motion2Cpp\\test.txt", ios::app);
	file <<"I. resize + cvtColor + GaussianBlur\nII. Motion\n"
			<<"III. checkByRGB\n"
			<<"IV. dilate + findContours + getContourFeatures"
			<<"V. calcOpticalFlowPyrLK\n"
			<<"VI. assignFeaturePoints\n"
			<<"VII. matchCentroid\n"
			<<"___________________________evaluate___________________________\n"

;



	file.close();
	system("pause");

	return 0;
}