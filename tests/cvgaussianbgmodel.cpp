#ifdef _MSC_VER
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#endif

#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>

//this is a sample for foreground detection functions
int main(int argc, char** argv)
{
    IplImage*       tmp_frame = NULL;
    CvCapture*      video = NULL;

    video = cvCaptureFromFile(argv[1]);
    tmp_frame = cvQueryFrame(video);
    if(!tmp_frame)
    {
        printf("./cvgaussainbgmodel <video_filename>\n");
        exit(0);
    }

    cvNamedWindow("BG", 1);
    cvNamedWindow("FG", 1);

    // create BG model   
    CvBGStatModel* bg_model = cvCreateGaussianBGModel( tmp_frame );
     
    for( int fr = 1;tmp_frame; tmp_frame = cvQueryFrame(video), fr++ )
    {
        cvUpdateBGStatModel( tmp_frame, bg_model );
        cvShowImage("BG", bg_model->background);
        cvShowImage("FG", bg_model->foreground);
        int k = cvWaitKey(5);
        if( k == 27 ) break;
        printf("frame# %d \r", fr);
    }


    cvReleaseBGStatModel( &bg_model );
    cvReleaseCapture(&video);

    return 0;
}
