#ifndef FIREBEHAVIORANALYSIS_H
#define FIREBEHAVIORANALYSIS_H

#include <vector>
#include "opencv/cv.h"
#include "ds.h"

//* Counting the foldback point at each directions */
void flodbackPoint(const std::vector< cv::Rect > & vecRect, DirectionsCount & count);


//* Analysis the rect information */
bool judgeDirectionsMotion(const std::vector< cv::Rect > & vecRect, cv::Rect & rectFire);



#endif 