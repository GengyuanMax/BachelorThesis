//
//  statistics.cpp
//  opencv_test
//
//  Created by 张耕源 on 22/03/2018.
//  Copyright © 2018 张耕源. All rights reserved.
//

#include "statistics.hpp"
void meanStd(const vector<float>& array, float& mean, float& variance){
    unsigned long length = array.size();
    float sum = 0;
    float sq_sum = 0;
    
    for (int i=0; i<length; i++) {
        float v = array[i];
        sum+=v;
        sq_sum+=v*v;
    }
    
    mean = sum/length;
    variance = sq_sum/length - mean*mean;
}

float sqrSumRt(float a, float b){
    float sum;

    sum = a*a + b*b;
    return sqrt(sum);
}

void meanValue(InputArray src, OutputArray mean){
    Mat stddev;
    meanStdDev(src, mean, stddev);
}
