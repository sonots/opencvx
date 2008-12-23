#ifdef _MSC_VER
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#endif

#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>

#include "cvhaarlike.h"
#include "cvxmat.h"

int main(int argc, char** argv)
{
    IplImage* img = NULL;
    IplImage* sum = NULL;
    CvMat* integral = NULL, hdr;

    img = cvCreateImage( cvSize(3,3), IPL_DEPTH_8U, 1 );
    CV_IMAGE_ELEM( img, uchar, 0, 0 ) = 10;
    CV_IMAGE_ELEM( img, uchar, 0, 1 ) = 11;
    CV_IMAGE_ELEM( img, uchar, 0, 2 ) = 12;
    CV_IMAGE_ELEM( img, uchar, 1, 0 ) = 13;
    CV_IMAGE_ELEM( img, uchar, 1, 1 ) = 14;
    CV_IMAGE_ELEM( img, uchar, 1, 2 ) = 15;
    CV_IMAGE_ELEM( img, uchar, 2, 0 ) = 16;
    CV_IMAGE_ELEM( img, uchar, 2, 1 ) = 17;
    CV_IMAGE_ELEM( img, uchar, 2, 2 ) = 18;

    printf("Output of cvIntegral\n");
    sum = cvCreateImage( cvSize(img->width+1, img->height+1),
                         IPL_DEPTH_64F, img->nChannels );
    cvIntegral( img, sum );
    cvPrintMat( sum );

    printf("Integral Image. Output of cvIntegral is more useful to use.\n");
    integral = cvGetRows( sum, &hdr, 1, sum->height );
    integral = cvGetCols( integral, &hdr, 1, sum->width );
    cvPrintMat( integral );

    // Haar-like wavelets basis
    CvIntHaarFeatures* haarvec = icvCreateIntHaarFeatures( cvSize(3,3), 0, 0 );
    printf("width=%d height=%d\n", haarvec->winsize.width, haarvec->winsize.height );
    for( int i = 0; i < haarvec->count; i++ )
    {
        printf("i=%d\n", i);
        double feature = 0;
        for( int j = 0; j < CV_HAAR_FEATURE_MAX; j++ )
        {
            printf(" j=%d: ", j );
            int x = haarvec->feature[i].rect[j].r.x;
            int y = haarvec->feature[i].rect[j].r.y;
            int w = haarvec->feature[i].rect[j].r.width;
            int h = haarvec->feature[i].rect[j].r.height;
            int p0 = haarvec->fastfeature[i].rect[j].p0;
            int p1 = haarvec->fastfeature[i].rect[j].p1;
            int p2 = haarvec->fastfeature[i].rect[j].p2;
            int p3 = haarvec->fastfeature[i].rect[j].p3;
            float weight =  haarvec->fastfeature[i].rect[j].weight;
            printf("x=%d y=%d w=%d h=%d ", x, y, w, h );
            printf("p0=%d p1=%d p2=%d p3=%d ", p0, p1, p2, p3);
            printf("weight=%f ", weight );
            double rectsum = 0;
            if( weight != 0.0 )
            {
//                  rectsum = CV_IMAGE_ELEM( sum, double, y, x )
//                  - CV_IMAGE_ELEM( sum, double, y, x + w )
//                  - CV_IMAGE_ELEM( sum, double, y + h, x )
//                  + CV_IMAGE_ELEM( sum, double, y + h, x + w );
                 rectsum = CV_IMAGE_ELEM( sum, double, 0, p0 )
                 - CV_IMAGE_ELEM( sum, double, 0, p1 )
                 - CV_IMAGE_ELEM( sum, double, 0, p2 )
                 + CV_IMAGE_ELEM( sum, double, 0, p3 );
            }
            printf("rectsum=%f\n", rectsum);
            feature += rectsum * weight;
        }
        printf(" haar-like feature:%f\n", feature);
    }
    cvReleaseImage( &img );
    cvReleaseImage( &sum );
    return 0;
}
