//
//  main.cpp
//  opencv_test
//
//  Created by 张耕源 on 13/03/2018.
//  Copyright © 2018 张耕源. All rights reserved.
//


#include "stitch2.hpp"

using namespace cv;
using namespace std;

#if 1
int main(){
    Mat ori;
    Mat unreg1, unreg2, unreg;
    stitch s;
    long frameNum;
    

    VideoCapture video("/Users/GengyuanMax/工作/毕业设计/素材/Test3.wmv");

    if(!video.isOpened()){
        cout << "cannot open video";
        return -1;
    }
    
    
    video.set(CV_CAP_PROP_POS_FRAMES, 0);
    video >> ori;
    s.initial(ori);

    frameNum = video.get(CAP_PROP_FRAME_COUNT);
    
    for (int f=0; f<frameNum; f++) {
        char c = waitKey(20);
        if (c == 27) {
            break;
        }
        video.grab();
        video >> unreg;
        s.registerWith(unreg, false);
        s.mosaicShow();
    }
    s.mosaicSave(); 
    
    
    
}
#endif

#if 0
int main(){
    Mat img, field, dst;
    
    img = imread("/Users/GengyuanMax/Desktop/3.png");
    field = imread("/Users/GengyuanMax/Desktop/d1.png");
//
//    flatFieldCorrection(img, field, dst);
//    imwrite("/Users/GengyuanMax/Desktop/t3.png", dst);
    
    illuminationCompensation(img);
    imwrite("/Users/GengyuanMax/Desktop/i03.png", img);
}
#endif


