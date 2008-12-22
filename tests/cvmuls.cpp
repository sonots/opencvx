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
#include "cvxmat.h"
#include "cvmuls.h"

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
    CvMat* dst = cvCreateMat( D, N, CV_32FC1 );
    cvMulS( &mat, cvScalar(0.5), dst );
    cvPrintMat( dst );
    return 0;
}


