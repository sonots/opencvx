#ifdef _MSC_VER
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#endif

#include <stdio.h>
#include <stdlib.h>
#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"
#include "../cvxmat.h"
#include "../cvgaussnorm.h"

int main( int argc, char** argv )
{
    IplImage *img = cvLoadImage( "lena.png" );
    IplImage *gry = cvCreateImage( cvGetSize(img), img->depth, 1 );
    CvMat *grynormed = cvCreateMat( img->height, img->width, CV_64FC1 );

    //cvCvtColor( img, gry, CV_BGR2GRAY );
    //cvImgGaussNorm( gry, grynormed );
    //cvPrintMat( grynormed );

    CvMat *normed = cvCreateMat( img->height, img->width, CV_MAKETYPE(CV_64F, 3) );
    cvImgGaussNorm( img, normed );
    cvPrintMat( normed );
}

