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
#include "../cvanglemean.h"

int main( int argc, char** argv )
{
    double a[] = { 
        358, 362
    };
    CvMat mat = cvMat( 1, 2, CV_64FC1, a );
    CvScalar mean = cvAngleMean( &mat );
    printf("%f\n", mean.val[0]);
    double w[] = {
        0.4, 0.6
    };
    CvMat wmat = cvMat( 1, 2, CV_64FC1, w );
    mean = cvAngleMean( &mat, &wmat );
    printf("%f\n", mean.val[0]);
    return 0;
}
/* 0
 */
