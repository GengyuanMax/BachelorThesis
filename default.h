//
//  default.h
//  opencv_test
//
//  Created by 张耕源 on 13/03/2018.
//  Copyright © 2018 张耕源. All rights reserved.
//

#ifndef default_h
#define default_h

#include <stdio.h>


//default window
const char* result_window = "result window";
const char* present_frame = "present frame";
const char* last_frame = "last frame";

//video source
const char* video_name = "/Users/GengyuanMax/工作/毕业设计/素材/Test3.wmv";

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
const char* DEFAULT_DETECTOR = "SIFT";
const char* DEFAULT_DESCRIPTOR = "SIFT";


#endif /* default_h */
