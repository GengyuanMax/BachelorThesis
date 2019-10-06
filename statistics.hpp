//
//  statistics.hpp
//  opencv_test
//
//  Created by 张耕源 on 22/03/2018.
//  Copyright © 2018 张耕源. All rights reserved.
//

#ifndef statistics_hpp
#define statistics_hpp

#include <stdio.h>
#include <vector>
#include <math.h>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

void meanStd(const vector<float>& array, float& mean, float& variance);
float sqrSumRt(float a, float b);
void meanValue(InputArray src, OutputArray mean);

#endif /* statistics_hpp */
