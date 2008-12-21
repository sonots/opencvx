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
#include "opencvx/cvpcadiffs.h"

int main( int argc, char** argv )
{
    int D = 4, N = 3;
    double a[] = { 
        0.4854,    0.9157,    0.0357,
        0.8003,    0.7922,    0.8491,
        0.1419,    0.9595,    0.9340,
        0.4218,    0.6557,    0.6787
    };
    CvMat mat = cvMat( D, N, CV_64FC1, a ); // 64FC1 for double
    CvMat *avg = cvCreateMat( D, 1, CV_64FC1 );
    CvMat *eigenvalues = cvCreateMat( MIN(D,N-1), 1, CV_64FC1 );
    CvMat *eigenvectors = cvCreateMat( MIN(D, N-1), D, CV_64FC1 );
    cvCalcPCA( &mat, avg, eigenvalues, eigenvectors, CV_PCA_DATA_AS_COL );
    printf("data (col vec)\n");
    cvPrintMat( &mat );
    printf("avg (col vec)\n");
    cvPrintMat( avg );
    printf("eigenvalues\n");
    cvPrintMat( eigenvalues );
    printf("eigenvectors (row vec)\n");
    cvPrintMat( eigenvectors );

    int M = eigenvectors->rows;
    CvMat *proj = cvCreateMat( N, M, CV_64FC1 );
    cvProjectPCA( &mat, avg, eigenvectors, proj );
    printf("proj (row vec)\n");
    cvPrintMat( proj );
    cvReleaseMat( &proj );

    M = 1;
    cvGetRows( eigenvectors, eigenvectors, 0, M );
    proj = cvCreateMat( N, M, CV_64FC1 );
    cvProjectPCA( &mat, avg, eigenvectors, proj );
    printf("reduced proj (row vec)\n");
    cvPrintMat( proj );
    cvReleaseMat( &proj );

    double b[] = { 
        0.478933, 0,
        0.813867, 0,
        0.678467, 0,
        0.585400, 0
    };
    N = 2;
    CvMat mat2 = cvMat( D, N, CV_64FC1, b );
    CvMat *loglikeli = cvCreateMat( 1, N, CV_64FC1 );
    cvMatPcaDiffs( &mat2, avg, eigenvalues, eigenvectors, loglikeli, 1, true );
    printf("data\n");
    cvPrintMat( &mat2 );
    printf("loglikeli\n");
    cvPrintMat( loglikeli );

    return 0;
}
/*
data (col vec)
0.485400 0.915700 0.035700
0.800300 0.792200 0.849100
0.141900 0.959500 0.934000
0.421800 0.655700 0.678700
avg (col vec)
0.478933
0.813867
0.678467
0.585400
eigenvalues
0.157483
0.129767
eigenvectors (row vec)
-0.021102 -0.022049 -0.956398 -0.290467
-0.997108 0.065402 0.009067 0.037619
proj (row vec)
0.560854 -0.018355
-0.297939 -0.431728
-0.262916 0.450083
reduced proj (row vec)
0.560854
-0.297939
-0.262916
data
0.478933 0.000000
0.813867 0.000000
0.678467 0.000000
0.585400 0.000000
loglikeli
0.107349 -5.936231
*/

