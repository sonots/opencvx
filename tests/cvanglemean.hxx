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
#include "cvanglemean.h"

#include <cxxtest/TestSuite.h>

class CvAngleMeanTest : public CxxTest::TestSuite
{
public:
    void test_anglemean()
    {
        double a[] = { 
            358, 362
        };
        CvMat mat = cvMat( 1, 2, CV_64FC1, a );
        CvScalar mean = cvAngleMean( &mat );
        TS_ASSERT_DELTA( mean.val[0], 0, 0.00001 );
    }

    void testWeightedAngleMean()
    {
        double a[] = { 
            358, 362
        };
        CvMat mat = cvMat( 1, 2, CV_64FC1, a );
        double w[] = {
            0.4, 0.6
        };
        CvMat wmat = cvMat( 1, 2, CV_64FC1, w );
        CvScalar mean = cvAngleMean( &mat, &wmat );
        TS_ASSERT_DELTA( mean.val[0], 0.4001, 0.0001 );
    }
};
