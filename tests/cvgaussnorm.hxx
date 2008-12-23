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
#include "cvgaussnorm.h"

#include <cxxtest/TestSuite.h>

class CvGaussNormTest : public CxxTest::TestSuite
{
public:
    void testGaussNormImage()
    {
        IplImage *img = cvCreateImage( cvSize(2, 2), IPL_DEPTH_8U, 1 );
        CV_IMAGE_ELEM( img, uchar, 0, 0 ) = 206;
        CV_IMAGE_ELEM( img, uchar, 0, 1 ) = 200;
        CV_IMAGE_ELEM( img, uchar, 1, 0 ) = 136;
        CV_IMAGE_ELEM( img, uchar, 1, 1 ) = 254;
        CvMat *normed = cvCreateMat( img->height, img->width, CV_MAKETYPE(CV_64F, img->nChannels) );
        cvGaussNormImage( img, normed );
        TS_ASSERT_DELTA( cvmGet( normed, 0, 0 ), 0.1668, 0.0001 );
        TS_ASSERT_DELTA( cvmGet( normed, 0, 1 ), 0.0238, 0.0001 );
        TS_ASSERT_DELTA( cvmGet( normed, 1, 0 ), -1.5013, 0.0001 );
        TS_ASSERT_DELTA( cvmGet( normed, 1, 1 ),  1.3106, 0.0001 );
    }
};
