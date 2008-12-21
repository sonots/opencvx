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
#include "opencvx/cvlogsum.h"

int main( int argc, char** argv )
{
    int D = 4, N = 3;
    double a[] = { 
        0.4854,    0.9157,    0.0357,
        0.8003,    0.7922,    0.8491,
        0.1419,    0.9595,    0.9340,
        0.4218,    0.6557,    0.6787
    };
    CvMat mat = cvMat( D, N, CV_64FC1, a );
    CvScalar sum = cvSum( &mat );
    printf( "%lf\n", sum.val[0] );
    cvLog( &mat, &mat );
    CvScalar logsum = cvLogSum( &mat );
    printf( "%lf\n", exp( logsum.val[0] ) );
    return 0;
}


