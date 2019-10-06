#if 0

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include "toupcam.h"
#include "stitch.hpp"

#pragma pack(push)
#pragma pack(1)
typedef struct {
    unsigned char        bfType[2];
    unsigned             bfSize;
    unsigned short    bfReserved1;
    unsigned short     bfReserved2;
    unsigned            bfOffBits;
} BITMAPFILEHEADER;
#pragma pack(pop)

int flag = 0;

static void DataCallback(const void* pData, const BITMAPINFOHEADER* pHeader, int bSnap, void* pCallbackCtx)
{
    static int cnt = 0;
    static timespec sts;
    timespec ts;
    if (0 == pData)
        return;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    if (0 == cnt)
        sts = ts;
    time_t d = ts.tv_sec - sts.tv_sec;
    double fr = 0.0;
    if (d > 0)
        fr = cnt / (double)d;
    printf("%04d: framerate = %.1f, %p, snap = %d, width = %d, height = %d\n",
            ++cnt,       //帧数
            fr,          //帧率
            pData,       //
            bSnap,       //是否静态抓拍
            pHeader->biWidth,
            pHeader->biHeight);
    
    char str[256];
    sprintf(str, "/Users/GengyuanMax/工作/毕业设计/pic/%08d.bmp", cnt);
    FILE* fp = fopen(str, "wb");
    if (fp)
    {
        flag = 0;
        BITMAPFILEHEADER fh = { 0 };
        fh.bfType[0] = 0x42;
        fh.bfType[1] = 0x4d;
        fh.bfSize = sizeof(fh) + sizeof(BITMAPINFOHEADER) + pHeader->biSizeImage;
        fh.bfOffBits = sizeof(fh) + sizeof(BITMAPINFOHEADER);
        fwrite(&fh, sizeof(fh), 1, fp);
        fwrite(pHeader, sizeof(BITMAPINFOHEADER), 1, fp);
        fwrite(pData, pHeader->biSizeImage, 1, fp);
        fclose(fp);
        flag = 1;
    }
}



int main(int argc, char** argv)
{
    rlimit rl = {0, 0};
    rl.rlim_cur = rl.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &rl);
    
    stitch s;
    
    ToupcamInst ti[TOUPCAM_MAX];
    int ret = Toupcam_Enum(ti);
    printf("enum: ret = %d\n", ret);
    if (ret > 0)
    {
        printf("open: %s, %s\n", ti[0].displayname, ti[0].id);
        HToupCam h = Toupcam_Open(ti[0].id);
        printf("open handle: %p\n", h);
        if (h)
        {
            int MaxSpeed = Toupcam_get_MaxSpeed(h);
            printf("max put speed: %d\n", MaxSpeed);
            Toupcam_put_Speed(h, 1);
            Toupcam_put_eSize(h, 0);
            ret = Toupcam_StartPushMode(h, DataCallback, NULL);
            printf("start result: %d\n", ret);
            
            Mat img;
            namedWindow("demo");
            
            if (ret >= 0)
            {
                char str[256];
                int count = 1;
                while(1){
                    char c = waitKey(100);
                    if (c == 27) {
                        break;
                    }
                    
                    sprintf(str, "/Users/GengyuanMax/工作/毕业设计/pic/%08d.bmp", count);
                    img = imread(str);
                    cout << str;
                    imshow("demo", img);
                    count++;
                    
                }
            }
            printf("close\n");
            Toupcam_Close(h);
        }
    }
}

#endif

