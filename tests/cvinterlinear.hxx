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
#include "cvinterlinear.h"

#include <cxxtest/TestSuite.h>

class CvTest : public CxxTest::TestSuite
{
public:
    void test_do()
    {
        double a[] = { 
            0, 100, 200,
            0, 200, 200,
            0, 50, 50
        };
        CvMat mat = cvMat( 3, 3, CV_64FC1, a );
        CvScalar color;
        color = cvInterLinear( &mat, cvPoint2D32f( 1.5, 0.5 ) );
        TS_ASSERT_DELTA( color.val[0], 175, 0.00001 );
        color = cvInterLinear( &mat, cvPoint2D32f( 1.5, -0.4 ) );
        TS_ASSERT_DELTA( color.val[0], 150, 0.00001 );
    }
};
