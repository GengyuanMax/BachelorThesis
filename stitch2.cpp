//
//  stitch.cpp
//  opencv_test
//
//  Created by 张耕源 on 13/03/2018.
//  Copyright © 2018 张耕源. All rights reserved.
//

#include "stitch2.hpp"

const int max_rate = 100;
int resize_rate = 50;

//define static menber
stitch* stitch::m_stitchSelf;

//constructor function
stitch::stitch(){
    m_stitchSelf = this;
    m_detector = FastFeatureDetector::create(28);
    m_descriptor = ORB::create();
    //m_descriptor = xfeatures2d::SurfDescriptorExtractor::create();
}

//functions
int stitch::position(const Point2f &basedPoint, const Point2f &oriPoint, const Size &mosaicSize, const Size &unregSize){
    //    if (basedPoint.x>=oriPoint.x && basedPoint.y>=oriPoint.y) {
    //        return RUC;
    //    }
    //    else if (basedPoint.x<=oriPoint.x && basedPoint.y<=oriPoint.y){
    //        return LLC;
    //    }
    //    else if (basedPoint.x<=oriPoint.x && basedPoint.y>=oriPoint.y) {
    //        return LUC;
    //    }
    //    else
    //        return RLC;
    int xpos=-1, ypos=-1;
    float dx = basedPoint.x - oriPoint.x;
    float dy = basedPoint.y - oriPoint.y;
    int dwidth = mosaicSize.width-unregSize.width;
    int dheight = mosaicSize.height-unregSize.height;
    
    if (dx<=dwidth && dx>=0) {
        xpos=xin;
    }
    else if(dx>=dwidth){
        xpos=xright;
    }
    else if(dx<=0){
        xpos=xleft;
    }
    
    if (dy<=dheight && dy>=0) {
        ypos=yin;
    }
    else if(dy>=dheight){
        ypos=yupper;
    }
    else if(dy<=0){
        ypos=ylower;
    }
    
    switch (xpos) {
        case xright:
            switch (ypos) {
                case ylower:
                    return RLC;
                    break;
                case yin:
                    return RGT;
                    break;
                case yupper:
                    return RUC;
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        case xin:
            switch (ypos) {
                case ylower:
                    return LWR;
                    break;
                case yin:
                    return CTR;
                    break;
                case yupper:
                    return UPR;
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        case xleft:
            switch (ypos) {
                case ylower:
                    return LLC;
                    break;
                case yin:
                    return LFT;
                    break;
                case yupper:
                    return LUC;
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        default:
            return -1;
            break;
    }
}

int stitch::position(const float &dx, const float &dy, const Size &mosaicSize, const Size &unregSize){
    int xpos=-1, ypos=-1;
    int dwidth = mosaicSize.width-unregSize.width;
    int dheight = mosaicSize.height-unregSize.height;
    
    if (dx<=dwidth && dx>=0) {
        xpos=xin;
    }
    else if(dx>=dwidth){
        xpos=xright;
    }
    else if(dx<=0){
        xpos=xleft;
    }
    
    if (dy<=dheight && dy>=0) {
        ypos=yin;
    }
    else if(dy>=dheight){
        ypos=yupper;
    }
    else if(dy<=0){
        ypos=ylower;
    }
    
    switch (xpos) {
        case xright:
            switch (ypos) {
                case ylower:
                    return RLC;
                    break;
                case yin:
                    return RGT;
                    break;
                case yupper:
                    return RUC;
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        case xin:
            switch (ypos) {
                case ylower:
                    return LWR;
                    break;
                case yin:
                    return CTR;
                    break;
                case yupper:
                    return UPR;
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        case xleft:
            switch (ypos) {
                case ylower:
                    return LLC;
                    break;
                case yin:
                    return LFT;
                    break;
                case yupper:
                    return LUC;
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        default:
            return -1;
            break;
    }
}

/**
 @function: featureCalc
 calculate bast matching point
 @param
 Mat const& img1: image1 at previous frame
 Mat const& img2: image2 at next frame
 Point2f& matchPoint1:
 Point2f& matchPoint2:
 **/
void stitch::featureCalc(const Mat& img1, const Mat& img2, Point2f& matchPoint1, Point2f& matchPoint2){
    Mat img1_gray, img2_gray;
    
    //convert color
    cvtColor(img1, img1_gray, CV_RGB2GRAY);
    cvtColor(img2, img2_gray, CV_RGB2GRAY);
    pyrDown(img1_gray, img1_gray, Size(img1_gray.cols/2, img1_gray.rows/2));
    pyrDown(img2_gray, img2_gray, Size(img2_gray.cols/2, img2_gray.rows/2));
    
    //feature detect
    vector<KeyPoint> kpoint_1, kpoint_2;
    m_detector->detect(img1_gray, kpoint_1);
    m_detector->detect(img2_gray, kpoint_2);
    
    //feature describe
    Mat desc_1, desc_2;
    m_descriptor->compute(img1_gray, kpoint_1, desc_1);
    m_descriptor->compute(img2_gray, kpoint_2, desc_2);
    
    //feature match
    desc_1.convertTo(desc_1, CV_32F);
    desc_2.convertTo(desc_2, CV_32F);
    Ptr<FlannBasedMatcher> featureMatcher = FlannBasedMatcher::create();
    vector<DMatch> matchPoints;
    featureMatcher->match(desc_2, desc_1, matchPoints, Mat());
    sort(matchPoints.begin(), matchPoints.end());
    
    //extract n best points
    int n = 15;
    vector<Point2f> opoint_1, opoint_2;
    vector<float> ddx, ddy;
    for (int i=0; i<n; i++) {
        opoint_2.push_back(kpoint_2[matchPoints[i].queryIdx].pt);
        opoint_1.push_back(kpoint_1[matchPoints[i].trainIdx].pt);
        ddx.push_back(kpoint_2[matchPoints[i].queryIdx].pt.x - kpoint_1[matchPoints[i].trainIdx].pt.x);
        ddy.push_back(kpoint_2[matchPoints[i].queryIdx].pt.y - kpoint_1[matchPoints[i].trainIdx].pt.y);
    }
    
    matchPoint1 = opoint_1[0];
    matchPoint2 = opoint_2[0];
}

void stitch::featureCalc(const Mat& img1, const Mat& img2, float& ddx, float& ddy){
    Mat img1_gray, img2_gray;
    ddx = 0;
    ddy = 0;
    
    //convert color
    cvtColor(img1, img1_gray, CV_RGB2GRAY);
    cvtColor(img2, img2_gray, CV_RGB2GRAY);
    //    pyrDown(img1_gray, img1_gray, Size(img1_gray.cols/2, img1_gray.rows/2));
    //    pyrDown(img2_gray, img2_gray, Size(img2_gray.cols/2, img2_gray.rows/2));
    
    //feature detect
    vector<KeyPoint> kpoint_1, kpoint_2;
    m_detector->detect(img1_gray, kpoint_1);
    m_detector->detect(img2_gray, kpoint_2);
    
    //feature describe
    Mat desc_1, desc_2;
    m_descriptor->compute(img1_gray, kpoint_1, desc_1);
    m_descriptor->compute(img2_gray, kpoint_2, desc_2);
    
    //feature match
    desc_1.convertTo(desc_1, CV_32F);
    desc_2.convertTo(desc_2, CV_32F);
    Ptr<FlannBasedMatcher> featureMatcher = FlannBasedMatcher::create();
    vector<DMatch> matchPoints;
    featureMatcher->match(desc_2, desc_1, matchPoints, Mat());
    sort(matchPoints.begin(), matchPoints.end());
    
    //RANSAC
    //    vector<int> queryIdxs(matchPoints.size()), trainIdxs(matchPoints.size());
    //    for (size_t i = 0; i<matchPoints.size(); i++) {
    //        queryIdxs[i] = matchPoints[i].queryIdx;
    //        trainIdxs[i] = matchPoints[i].trainIdx;
    //    }
    //    Mat H12;    //transform matrix
    //    int ransacReprojThreshold = 15;
    //
    //    vector<Point2f> points1;
    //    KeyPoint::convert(kpoint_1, points1, queryIdxs);
    //
    //    vector<Point2f> points2;
    //    KeyPoint::convert(kpoint_2, points2, trainIdxs);
    //
    //    H12 = findHomography(Mat(points1), Mat(points2), CV_RANSAC, ransacReprojThreshold);
    //    vector<char> matchesMask(matchPoints.size(), 0);
    //    Mat points1t;
    //    perspectiveTransform(Mat(points1), points1t, H12);
    //    for( size_t i1 = 0; i1 < points1.size(); i1++ )  //保存‘内点’
    //    {
    //        if(norm(points2[i1] - points1t.at<Point2f>((int)i1,0)) <= ransacReprojThreshold ) //给内点做标记
    //        {
    //            matchesMask[i1] = 1;
    //        }
    //    }
    
    //extract n best points
    int n = 15;
    vector<float> dx, dy;
    float x_mean=0, y_mean=0, x_var=0, y_var=0;
    for (int i=0; i<n && i<matchPoints.size(); i++) {
        dx.push_back(kpoint_2[matchPoints[i].queryIdx].pt.x - kpoint_1[matchPoints[i].trainIdx].pt.x);
        dy.push_back(kpoint_2[matchPoints[i].queryIdx].pt.y - kpoint_1[matchPoints[i].trainIdx].pt.y);
    }
    
    //检验方差
    meanStd(dx, x_mean, x_var);
    meanStd(dy, y_mean, y_var);
    
    ddx = -x_mean;
    ddy = -y_mean;
}

void stitch::featureCalc(const Mat& img1, const Mat& img2, float& ddx, float& ddy, float& dx_v, float& dy_v){
    Mat img1_gray, img2_gray;
    
    ddx = 0;
    ddy = 0;
    
    //convert color
    cvtColor(img1, img1_gray, CV_RGB2GRAY);
    cvtColor(img2, img2_gray, CV_RGB2GRAY);
    
    //feature detect
    vector<KeyPoint> kpoint_1, kpoint_2;
    m_detector->detect(img1_gray, kpoint_1);
    m_detector->detect(img2_gray, kpoint_2);
    
    //feature describe
    Mat desc_1, desc_2;
    m_descriptor->compute(img1_gray, kpoint_1, desc_1);
    m_descriptor->compute(img2_gray, kpoint_2, desc_2);
    
    //feature match
    desc_1.convertTo(desc_1, CV_32F);
    desc_2.convertTo(desc_2, CV_32F);
    Ptr<FlannBasedMatcher> featureMatcher = FlannBasedMatcher::create();
    vector<DMatch> matchPoints;
    featureMatcher->match(desc_2, desc_1, matchPoints, Mat());
    sort(matchPoints.begin(), matchPoints.end());
    
    //extract n best points
    int n = 15;
    vector<float> dx, dy;
    float x_mean=0, y_mean=0, x_var=0, y_var=0;
    for (int i=0; i<n && i<matchPoints.size(); i++) {
        dx.push_back(kpoint_2[matchPoints[i].queryIdx].pt.x - kpoint_1[matchPoints[i].trainIdx].pt.x);
        dy.push_back(kpoint_2[matchPoints[i].queryIdx].pt.y - kpoint_1[matchPoints[i].trainIdx].pt.y);
    }
    
    //检验方差
    meanStd(dx, x_mean, x_var);
    meanStd(dy, y_mean, y_var);
    
    ddx = -x_mean;
    ddy = -y_mean;
    dx_v = x_var;
    dy_v = y_var;
}


/**
 @function: registerWith
 calculate bast matching point
 @param
 unregistered Mat& unregistered
 */
void stitch::registerWith(const cv::Mat &unregistered, bool DEF_BEST_IMAGE_REG){
    Point2f unregBestPoint, lastframeImagePoint;
    float dx_m, dy_m;
    float dx_v, dy_v;
    int dx_r, dy_r;
    
    //不采取挑选模式
    if (!DEF_BEST_IMAGE_REG) {
        featureCalc(m_lastFrame, unregistered, dx_m, dy_m);
        
        //stitch unregistered onto mosaic
        Point2f basedImagePoint;
        Size Mosaic_size = m_Mosaic.size();
        
        
        basedImagePoint.x = lastframeImagePoint.x + m_lastFramePos.x;
        basedImagePoint.y = lastframeImagePoint.y + m_lastFramePos.y;
        
        dx_m+=m_lastFramePos.x;
        dy_m+=m_lastFramePos.y;
        
        dx_r = cvRound(dx_m);
        dy_r = cvRound(dy_m);
        
        seamlessUpdate(position(dx_m, dy_m, Mosaic_size, unregistered.size()), dx_r, dy_r, unregistered);
        
    }else{      //采取挑选模式
        Size Mosaic_size = m_Mosaic.size();
        
        //the first img
        featureCalc(m_lastFrame, unregistered, dx_m, dy_m, dx_v, dy_v);
        if (sqrSumRt(dx_v, dy_v)<THRESH_DEF_BEST_IMG) {
            buffer.push_back(unregistered);
            dx_l = dx_m;
            dy_l = dy_m;
        } else{
            if (buffer.size()==0) {
                return;
            } else{
                dx_r = cvRound(dx_l);
                dy_r = cvRound(dy_l);
                
                Mat bestImageDefined;
                bestImageDefined = buffer.back();
                updateMosaic(position(dx_l, dy_l, Mosaic_size, bestImageDefined.size()), dx_r, dy_r, bestImageDefined);
                buffer.clear();
            }
        }
    }
}



void stitch::updateMosaic(int position, float dxr, float dyr, const cv::Mat &unregistered){
    Mat mosaicROI_m, mosaicROI_un;
    Mat newMosaic;
    
    createROI(position, newMosaic, mosaicROI_m, mosaicROI_un, dxr, dyr, unregistered.size());
    m_Mosaic.copyTo(mosaicROI_m);
    unregistered.copyTo(mosaicROI_un);
    
    //update
    m_Mosaic =  newMosaic.clone();
    m_lastFrame = unregistered.clone();
}

void stitch::seamlessUpdate(int position, float dxr, float dyr, const cv::Mat& unregistered){
    Mat newMosaic;
    Mat mosaicROI_m, mosaicROI_un;
    Mat mask;
    
    createROI(position, newMosaic, mosaicROI_m, mosaicROI_un, dxr, dyr, unregistered.size());
    m_Mosaic.copyTo(mosaicROI_m);
    seamlessMosaic(unregistered, mosaicROI_un, position);
    
    //update
    m_Mosaic =  newMosaic.clone();
    m_lastFrame = unregistered.clone();
}

void stitch::mosaicShow(){
    Mat mosaic_resize;
    
    namedWindow(result_window, CV_WINDOW_NORMAL);
    createTrackbar("resize result window", result_window, &resize_rate, max_rate, resizeWindow);
    resizeWindow(0, 0);
    
    
}

void stitch::resizeWindow(int, void*){
    
    Mat mosaic_resize;
    Mat mosaic = m_stitchSelf->m_Mosaic;
    resize(mosaic, mosaic_resize, mosaic.size()*resize_rate/100);
    imshow(result_window, mosaic_resize);
}

void stitch::initial(const cv::Mat &img){
    m_Mosaic = img.clone();
    m_lastFrame = img.clone();
    m_lastFramePos = Point(0, 0);
}

void stitch::createROI(int position, Mat& newMosaic, Mat& mosaicROI_m, Mat& mosaicROI_un, float dxr, float dyr, Size unreg_size){
    switch (position) {
        case LLC:
            newMosaic.create(m_Mosaic.rows-dyr,
                             m_Mosaic.cols-dxr,
                             CV_8UC3);
            mosaicROI_m = newMosaic(Rect(Point(newMosaic.cols, newMosaic.rows),
                                         Point(newMosaic.cols-m_Mosaic.cols, newMosaic.rows-m_Mosaic.rows)));
            mosaicROI_un = newMosaic(Rect(Point(0, 0),
                                          Point(unreg_size.width, unreg_size.height)));
            m_lastFramePos = Point(0, 0);
            break;
        case LUC:
            newMosaic.create(unreg_size.height+dyr,
                             m_Mosaic.cols-dxr,
                             CV_8UC3);
            mosaicROI_m = newMosaic(Rect(Point(newMosaic.cols, 0),
                                         Point(newMosaic.cols-m_Mosaic.cols, m_Mosaic.rows)));
            mosaicROI_un = newMosaic(Rect(Point(0, newMosaic.rows),
                                          Point(unreg_size.width, newMosaic.rows-unreg_size.height)));
            m_lastFramePos = Point(0, dyr);
            break;
        case RLC:
            newMosaic.create(m_Mosaic.rows-dyr,
                             unreg_size.width+dxr,
                             CV_8UC3);
            mosaicROI_m = newMosaic(Rect(Point(0, newMosaic.rows),
                                         Point(m_Mosaic.cols, newMosaic.rows-m_Mosaic.rows)));
            mosaicROI_un = newMosaic(Rect(Point(newMosaic.cols, 0),
                                          Point(newMosaic.cols-unreg_size.width, unreg_size.height)));
            m_lastFramePos = Point(dxr, 0);
            break;
        case RUC:
            newMosaic.create(unreg_size.height+dyr,
                             unreg_size.width+dxr,
                             CV_8UC3);
            mosaicROI_m = newMosaic(Rect(Point(0, 0),
                                         Point(m_Mosaic.cols, m_Mosaic.rows)));
            mosaicROI_un = newMosaic(Rect(Point(newMosaic.cols, newMosaic.rows),
                                          Point(newMosaic.cols-unreg_size.width, newMosaic.rows-unreg_size.height)));
            m_lastFramePos = Point(dxr, dyr);
            break;
        case LFT:
            newMosaic.create(m_Mosaic.rows,
                             m_Mosaic.cols-dxr,
                             CV_8UC3);
            mosaicROI_m = newMosaic(Rect(Point(newMosaic.cols, newMosaic.rows),
                                         Point(newMosaic.cols-m_Mosaic.cols, newMosaic.rows-m_Mosaic.rows)));
            mosaicROI_un = newMosaic(Rect(Point(0, dyr),
                                          Point(unreg_size.width, dyr+unreg_size.height)));
            m_lastFramePos = Point(0, dyr);
            break;
        case RGT:
            newMosaic.create(m_Mosaic.rows,
                             unreg_size.width+dxr,
                             CV_8UC3);
            mosaicROI_m = newMosaic(Rect(Point(0, 0),
                                         Point(m_Mosaic.cols, m_Mosaic.rows)));
            mosaicROI_un = newMosaic(Rect(Point(newMosaic.cols, dyr),
                                          Point(newMosaic.cols-unreg_size.width, dyr+unreg_size.height)));
            m_lastFramePos = Point(dxr, dyr);
            break;
        case LWR:
            newMosaic.create(m_Mosaic.rows-dyr,
                             m_Mosaic.cols,
                             CV_8UC3);
            mosaicROI_m = newMosaic(Rect(Point(newMosaic.cols, newMosaic.rows),
                                         Point(newMosaic.cols-m_Mosaic.cols, newMosaic.rows-m_Mosaic.rows)));
            mosaicROI_un = newMosaic(Rect(Point(dxr, 0),
                                          Point(dxr+unreg_size.width, unreg_size.height)));
            m_lastFramePos = Point(dxr, 0);
            break;
        case UPR:
            newMosaic.create(unreg_size.height+dyr,
                             m_Mosaic.cols,
                             CV_8UC3);
            mosaicROI_m = newMosaic(Rect(Point(0, 0),
                                         Point(m_Mosaic.cols, m_Mosaic.rows)));
            mosaicROI_un = newMosaic(Rect(Point(dxr, newMosaic.rows),
                                          Point(dxr+unreg_size.width, newMosaic.rows-unreg_size.height)));
            m_lastFramePos = Point(dxr, dyr);
            break;
        case CTR:
            newMosaic.create(m_Mosaic.size(), CV_8UC3);
            m_Mosaic.copyTo(newMosaic);
            mosaicROI_un = newMosaic(Rect(Point(dxr, dyr),
                                          Point(dxr+unreg_size.width, dyr+unreg_size.height)));
            m_lastFramePos = Point(dxr, dyr);
            break;
        default:
            break;
    }
    
}

void stitch::mosaicSave(){
    imwrite("/Users/GengyuanMax/Desktop/Mosaic.png", m_Mosaic);
}
