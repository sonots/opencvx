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
#include "cvmuls.h"

#include <cxxtest/TestSuite.h>

class MyTest : public CxxTest::TestSuite
{
public:
    void test_do()
    {
        int D = 1, N = 2;
        double a[] = { 
            0.4854,    0.9157
        };
        CvMat mat = cvMat( D, N, CV_64FC1, a );
        CvMat* dst = cvCreateMat( D, N, CV_32FC1 );
        cvMulS( &mat, cvScalar(0.5), dst );
        TS_ASSERT_DELTA( cvmGet( dst, 0, 0 ), 0.2427, 0.0001 );
        TS_ASSERT_DELTA( cvmGet( dst, 0, 1 ), 0.4578, 0.0001 );
    }
};
