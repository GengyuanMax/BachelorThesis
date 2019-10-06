//
//  fusion.cpp
//  opencv_test
//
//  Created by 张耕源 on 2018/4/10.
//  Copyright © 2018 张耕源. All rights reserved.
//

#include "fusion.hpp"

void addWeightBlend(Mat unreg, Mat& ROI, int position){
    double weight = 0;
    int cols = unreg.cols;
    int rows = unreg.rows;
    int channels = unreg.channels();
    int rc = rows*channels;
    
    //从点p开始逐像素拷贝
    for (int i=0; i<rows; i++) {
        uchar* p_unreg = unreg.ptr<uchar>(i);
        uchar* p_ROI = ROI.ptr<uchar>(i);
        for (int j=0; j<cols; j++) {
            if (p_ROI[3*j]==0 && p_ROI[3*j+1]==0 && p_ROI[3*j+2]==0) {
                p_ROI[3*j] = p_unreg[3*j];
                p_ROI[3*j+1] = p_unreg[3*j+1];
                p_ROI[3*j+2] = p_unreg[3*j+2];
            } else{
                switch (position) {
                    case RLC: 
                        weight = (1-cos(1-i/rows))*(1-cos((rc/channels)/cols));
                        break;
                    case LLC:
                        weight = (1-cos(1-i/rows))*(1-cos(1-(rc/channels)/cols));
                        break;
                    case LUC:
                        weight = (1-cos(i/rows))*(1-cos(1-(rc/channels)/cols));
                        break;
                    case RUC:
                        weight = (1-cos(i/rows))*(1-cos((rc/channels)/cols));
                        break;
                    case CTR:
                        weight = 0;
                        break;
                    case RGT:
                        weight = 1-cos((rc/channels)/cols);
                        break;
                    case LFT:
                        weight = 1-cos(1-((rc/channels)/cols));
                        break;
                    case UPR:
                        weight = 1-cos(i/rows);
                        break;
                    case LWR: 
                        weight = 1-cos(1-i/rows);
                        break;
                    default:
                        break;
                }
                
                p_ROI[3*j] = (1-weight)*p_ROI[3*j] + weight*p_unreg[3*j];
                p_ROI[3*j+1] = (1-weight)*p_ROI[3*j+1] + weight*p_unreg[3*j+1];
                p_ROI[3*j+2] = (1-weight)*p_ROI[3*j+2] + weight*p_unreg[3*j+2];
            }
        }
    }
}

void addWeightBlend(Mat img1, Mat img2, int position, Mat& ROI){
    double weight = 0;
    int cols = img1.cols;
    int rows = img1.rows;
    
    //从点p开始逐像素拷贝
    for (int i=0; i<rows; i++) {
        uchar* p1 = img1.ptr<uchar>(i);
        uchar* p2 = img2.ptr<uchar>(i);
        uchar* p3 = ROI.ptr<uchar>(i);
        for (int j=0; j<cols; j++) {
            switch (position) {
                case RLC:
                    weight = (1-cos((1-i/rows)))*(1-cos(j/cols));
                    break;
                case LLC:
                    weight = (1-cos((1-i/rows)))*(1-cos((1-(j/cols))));
                    break;
                case LUC:
                    weight = (1-cos(i/rows))*(1-cos((1-(j/cols))));
                    break;
                case RUC:
                    weight = (1-cos(i/rows))*(1-cos(j/cols));
                    break;
                case CTR:
                    weight = 1;
                    break;
                case RGT:
                    weight = 1-cos(j/cols);
                    break;
                case LFT:
                    weight = 1-cos((1-j/cols));
                    break;
                case UPR:
                    weight = 1-cos(i/rows);
                    break;
                case LWR:
                    weight = 1-cos((1-i/rows));
                    break;
                default:
                    break;
            }
            p3[3*j] = (1-weight)*p2[3*j] + weight*p1[3*j];
            p3[3*j+1] = (1-weight)*p2[3*j+1] + weight*p1[3*j+1];
            p3[3*j+2] = (1-weight)*p2[3*j+2] + weight*p1[3*j+2];
        }
    }
}

void flatFieldCorrection(Mat img, Mat field, Mat& dst){
    if (img.channels()!=field.channels()) {
        printf("Flat Field Correction Requires Size Match!");
        return;
    }
    
    vector<double> field_mean;
    vector<Mat> img_chn;
    vector<Mat> field_chn;
    vector<Mat> dst_chn;
    vector<Mat>::iterator i, f;
    
    meanValue(field, field_mean);
    
    //split channels
    for (int i=0; i<img.channels(); i++) {
        split(img, img_chn);
        split(field, field_chn);
    }
    
    //correction channel by channel
    for (int i=0; i<img_chn.size(); i++) {
        //normalize
        double scale = field_mean.at(i);
        Mat temp_i = img_chn.at(i);
        Mat temp_f = field_chn.at(i);
        Mat temp_d;
        
        divide(temp_i, temp_f, temp_d, scale);
        
        //Correction of originally saturated pixels
        for (int i=0; i<temp_i.rows; i++) {
            for (int j=0; j<temp_i.cols; j++) {
                if (temp_i.at<uchar>(i, j)==255) {
                    temp_d.at<uchar>(i, j)=255;
                }
                if (temp_i.at<uchar>(i, j)<0) {
                    temp_d.at<uchar>(i, j)=0;
                }
                if (temp_d.at<uchar>(i, j)>255) {
                    temp_d.at<uchar>(i, j)=255;
                }
                if (temp_d.at<uchar>(i, j)<0) {
                    temp_d.at<uchar>(i, j)=0;
                }
            }
        }
        
        dst_chn.push_back(temp_d);
    }
    
    //merge channels
    merge(dst_chn, dst);
}

void illuminationCompensation(Mat& img){
    Mat img_gray;
    
    cvtColor(img, img_gray, CV_RGB2BGR);
    
    float thresh = 0.05;
    int hist[256] = {0};
    
    for (int i=0; i<img_gray.rows; i++) {
        for (int j=0; j<img_gray.cols; j++) {
            int gray = img_gray.at<uchar>(i, j);
            hist[gray]++;
        }
    }
    
    int pixelNum = img_gray.cols*img_gray.rows;
    int calnum = 0;
    int num=0;
    
    for (int i=0; i<256; i++) {
        if ((float)calnum/pixelNum < thresh) {
            calnum+=hist[255-i];
            num = i;
        } else{
            break;
        }
    }
    
    int avrgray = 0;
    calnum = 0;
    for (int i=255; i>=255-num; i--) {
        avrgray += hist[i]*i;
        calnum += hist[i];
    }
    avrgray /= calnum;
    
    float coef = 255.0/(float) avrgray;
    
    for (int i=0; i<img.rows; i++) {
        for (int j=0; j<img.cols; j++) {
            img.at<Vec3b>(i, j)[0] = CLAMP0255(img.at<Vec3b>(i, j)[0]*coef + 0.5);
            img.at<Vec3b>(i, j)[1] = CLAMP0255(img.at<Vec3b>(i, j)[1]*coef + 0.5);
            img.at<Vec3b>(i, j)[2] = CLAMP0255(img.at<Vec3b>(i, j)[2]*coef + 0.5);
        }
    }
}

//dst 需要先create
Mat multiBandBlend(Mat img1, Mat img2){
    
    int level = 2;
    Mat dst;
    
    dst.create(img1.size(), img1.type());
    dst.setTo(0);
    
    //Gaussian Pyramid
    vector<Mat> GaussPyr1, GaussPyr2;
    GaussPyr1.push_back(img1);
    for (int i=0; i<level; i++) {
        Mat temp;
        pyrDown(GaussPyr1.at(i), temp, GaussPyr1.at(i).size()/2);
        GaussPyr1.push_back(temp);
    }
    
    GaussPyr2.push_back(img2);
    for (int i=0; i<level; i++) {
        Mat temp;
        pyrDown(GaussPyr2.at(i), temp, GaussPyr2.at(i).size()/2);
        GaussPyr2.push_back(temp);
    }
    
    //Laplacian Pyramid
    vector<Mat> LapPyr1, LapPyr2;
    for (int i=0; i<level; i++) {
        Mat temp;
        Mat Lap;
        pyrUp(GaussPyr1.at(i+1), temp, GaussPyr1.at(i).size());
        Lap = GaussPyr1.at(i) - temp;
        LapPyr1.push_back(Lap);
    }
    LapPyr1.push_back(GaussPyr1.at(level));
    
    for (int i=0; i<level; i++) {
        Mat temp;
        Mat Lap;
        pyrUp(GaussPyr2.at(i+1), temp, GaussPyr2.at(i).size());
        Lap = GaussPyr2.at(i) - temp;
        LapPyr2.push_back(Lap);
    }
    LapPyr2.push_back(GaussPyr2.at(level));
    
    //add weight
    for (int i=0; i<=level; i++) {
        int rows = LapPyr1.at(i).rows;
        int cols = LapPyr2.at(i).cols;
        
        Mat temp1, temp2, temp_d;
        temp1 = LapPyr1.at(i);
        temp2 = LapPyr2.at(i);
        temp_d.create(temp1.size(), temp1.type());
        
        for (int m=0; m<rows; m++) {
            uchar* p_1 = temp1.ptr<uchar>(m);
            uchar* p_2 = temp2.ptr<uchar>(m);
            uchar* p_d = temp_d.ptr<uchar>(m);
            
            for (int n=0; n<cols; n++) {
//                double weight;
//                if (n<cols/3) {
//                    weight = 1;
//                }
//                else if (n<cols*2/3) {
//                    weight = (double)(2-3*n/cols);
//                }
//                else
//                    weight = 0;
                double weight;
//                if (n<cols/2) {
//                    weight = 1;
//                } else {
//                    weight = 0;
//                }
                weight = 1-n/cols;
                
                
                p_d[3*n] = weight*p_1[3*n] + (1-weight)*p_2[3*n];
                p_d[3*n+1] = weight*p_1[3*n+1] + (1-weight)*p_2[3*n+1];
                p_d[3*n+2] = weight*p_1[3*n+2] + (1-weight)*p_2[3*n+2];
            }
        }
        
        resize(temp_d, temp_d, dst.size());
        dst += temp_d;
    }
    
    return dst;
}


//test
//p是左上点，shift是dx dy
void seamlessMosaic(Mat unreg, Mat& ROI, int position){
    double weight = 0;
    int cols = unreg.cols;
    int rows = unreg.rows;
    int channels = unreg.channels();
    int rc = cols*channels;
    //    Mat ROI = mosaic(Rect(p, Point(p.y+unreg.cols, p.x+unreg.rows)));
    
    
    
    //从点p开始逐像素拷贝
    for (int i=0; i<rows; i++) {
        uchar* p_unreg = unreg.ptr<uchar>(i);
        uchar* p_ROI = ROI.ptr<uchar>(i);
        for (int j=0; j<cols; j++) {
            if (p_ROI[3*j]==0 && p_ROI[3*j+1]==0 && p_ROI[3*j+2]==0) {
                p_ROI[3*j] = p_unreg[3*j];
                p_ROI[3*j+1] = p_unreg[3*j+1];
                p_ROI[3*j+2] = p_unreg[3*j+2];
            } else{
                switch (position) {
                    case RLC:
                        weight = (1-cos(1-i/rows))*(1-cos((rc/channels)/cols));
                        break;
                    case LLC:
                        weight = (1-cos(1-i/rows))*(1-cos(1-(rc/channels)/cols));
                        break;
                    case LUC:
                        weight = (1-cos(i/rows))*(1-cos(1-(rc/channels)/cols));
                        break;
                    case RUC:
                        weight = (1-cos(i/rows))*(1-cos((rc/channels)/cols));
                        break;
                    case CTR:
                        weight = 0;
                        break;
                    case RGT:
                        weight = 1-cos((rc/channels)/cols);
                        break;
                    case LFT:
                        weight = 1-cos(1-((rc/channels)/cols));
                        break;
                    case UPR:
                        weight = 1-cos(i/rows);
                        break;
                    case LWR:
                        weight = 1-cos(1-i/rows);
                        break;
                    default:
                        break;
                }
                
                p_ROI[3*j] = (1-weight)*p_ROI[3*j] + weight*p_unreg[3*j];
                p_ROI[3*j+1] = (1-weight)*p_ROI[3*j+1] + weight*p_unreg[3*j+1];
                p_ROI[3*j+2] = (1-weight)*p_ROI[3*j+2] + weight*p_unreg[3*j+2];
            }
        }
        
    }
    
    
}


