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

int main( int argc, char** argv )
{
	CvMat* noise = cvCreateMat( 10, 2, CV_64FC1 );
	CvRNG rng[] = { cvRNG(time(NULL)), cvRNG(time(NULL)) };
    // seems only cvScalar(first) is effective
	cvRandArr( rng, noise, CV_RAND_UNI, cvScalar(0,0), cvScalar(1,10) );
	cvPrintMat( noise );
    return 0;
}

