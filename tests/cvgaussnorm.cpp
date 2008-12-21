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
#include "opencvx/cvxmat.h"
#include "opencvx/cvgaussnorm.h"

int main( int argc, char** argv )
{
    IplImage *img = cvLoadImage( "lena.png" );
    CvMat *normed = cvCreateMat( img->height, img->width, CV_MAKETYPE(CV_64F, 3) );
    cvGaussNormImage( img, normed );
    cvPrintMat( normed );
}

