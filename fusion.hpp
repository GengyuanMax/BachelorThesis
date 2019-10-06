//
//  fusion.hpp
//  opencv_test
//
//  Created by 张耕源 on 2018/4/10.
//  Copyright © 2018 张耕源. All rights reserved.
//

#ifndef fusion_hpp
#define fusion_hpp

#include <stdio.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/photo/photo.hpp>
#include "stitch2.hpp"

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define CLAMP0255(a)  CLAMP(a,0,255)
#define pi_h 1.5707963

using namespace cv;


void addWeightBlend(Mat unreg, Mat& ROI, int position);
void addWeightBlend(Mat img1, Mat img2, int position, Mat& ROI);
void flatFieldCorrection(Mat img, Mat field, Mat& dst);
void illuminationCompensation(Mat& img);
Mat multiBandBlend(Mat img1, Mat img2);

//test
void seamlessMosaic(Mat unreg, Mat& ROI, int position);

#endif /* fusion_hpp */
