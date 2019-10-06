//
//  stitch.hpp
//  opencv_test
//
//  Created by 张耕源 on 13/03/2018.
//  Copyright © 2018 张耕源. All rights reserved.
//

#ifndef stitch2_hpp
#define stitch2_hpp

#include <stdio.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/photo/photo.hpp>
#include "statistics.hpp"
#include "fusion.hpp"

using namespace cv;
using namespace std;

//default window
const char* const result_window = "result window";
const char* const present_frame = "present frame";
const char* const last_frame = "last frame";

//video source
const char* const video_name = "/Users/GengyuanMax/工作/毕业设计/素材/Test3.wmv";

/* constant */
const int LLC = 0;  //unregistered @ left lower corner
const int LUC = 1;  //unregistered @ left upper corner
const int RLC = 2;  //unregistered @ right lower corner
const int RUC = 3;  //unregistered @ right upper corner
const int CTR = 4;
const int UPR = 5;
const int LWR = 6;
const int LFT = 7;
const int RGT = 8;
const int xleft = 0; //          |   lower
const int xin = 1;   //          |
const int xright = 2;//left------+------->right
const int yupper = 0;//          |
const int yin = 1;   //          |
const int ylower = 2;//          v   upper



/* User Default Parameter --- can be changed */
const int FRAME_HEIGHT = 1240;
const int FRAME_WIDTH = 2080;
const char* const DEFAULT_DETECTOR = "FAST";
const char* const DEFAULT_DESCRIPTOR = "SIFT";
const float THRESH_DEF_BEST_IMG = 0.3;

/* class definition*/
class stitch {
    Ptr<FastFeatureDetector> m_detector;
    Ptr<ORB> m_descriptor;
    //Ptr<xfeatures2d::SurfDescriptorExtractor> m_descriptor;
    Ptr<FlannBasedMatcher> m_featureMatcher = FlannBasedMatcher::create();
    Mat m_Mosaic;
    Mat m_lastFrame;
    Point2f m_lastFramePos;
    static stitch* m_stitchSelf;
    vector<Mat> buffer;
    float dx_l, dy_l;
    
    int position(const Point2f &basedPoint, const Point2f &oriPoint, const Size &mosaicSize, const Size &unregSize);
    int position(const float &dx, const float &dy, const Size &mosaicSize, const Size &unregSize);
    void featureCalc(const Mat& img1, const Mat& img2, Point2f& matchPoint1, Point2f& matchPoint2);
    void featureCalc(const Mat& img1, const Mat& img2, float& ddx, float& ddy);
    void featureCalc(const Mat& img1, const Mat& img2, float& ddx, float& ddy, float& dx_v, float& dy_v);
    void updateMosaic(int position, float dxr, float dyr, const cv::Mat &unregistered);
    void seamlessUpdate(int position, float dxr, float dyr, const cv::Mat &unregistered);
    void createROI(int position, Mat& newMosaic, Mat& mosaicROI_m, Mat& mosaicROI_un, float dxr, float dyr, Size unreg_size);
    static void resizeWindow(int, void*);
    
public:
    stitch();
    void registerWith(const Mat &unregistered, bool DEF_BEST_IMAGE_REG = false);
    void mosaicShow();
    void mosaicSave();
    void initial(const Mat& img);
    
};




#endif /* stitch_hpp */

