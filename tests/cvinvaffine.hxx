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
#include "cvcreateaffine.h"
#include "cvinvaffine.h"

#include <cxxtest/TestSuite.h>

class CvGaussNormTest : public CxxTest::TestSuite
{
public:
    void testInvAffine()
    {
        CvMat* affine = cvCreateMat( 2, 3, CV_64FC1 );
        CvMat* invaffine = cvCreateMat( 2, 3, CV_64FC1 );
        cvCreateAffine( affine, cvRect32f( 3, 10, 2, 20, 45 ), cvPoint2D32f( 4, 5 ) );
        cvInvAffine( affine, invaffine );
        double ans[] = {
            0.565685, -0.848528, 6.788225,
            -0.106066, 0.247487, -2.156676,
        };
        CvMat Ans = cvMat( 2, 3, CV_64FC1, ans );
        for( int i = 0; i < 2; i++ ) {
            for( int j = 0; j < 3; j++ ) {
                TS_ASSERT_DELTA( cvmGet( invaffine, i, j ), cvmGet( &Ans, i, j ), 0.0001 );
            }
        }
    }
};
