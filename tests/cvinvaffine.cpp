#include <stdio.h>
#include <stdlib.h>
#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"

#include "opencvx/cvxmat.h"
#include "opencvx/cvinvaffine.h"
#include "opencvx/cvcreateaffine.h"
#include "opencvx/cvrect32f.h"

int main()
{
    CvMat* affine = cvCreateMat( 2, 3, CV_64FC1 );
    CvMat* invaffine = cvCreateMat( 2, 3, CV_64FC1 );
    cvCreateAffine( affine, cvRect32f( 3, 10, 2, 20, 45 ), cvPoint2D32f( 4, 5 ) );
    cvInvAffine( affine, invaffine );
    cvPrintMat( affine );
    cvPrintMat( invaffine );
}
/*
4.949747 16.970563 3.000000
2.121320 11.313708 10.000000
[ 0 0 1]

0.565685 -0.848528 6.788225
-0.106066 0.247487 -2.156676
[ 0 0 1]
*/
