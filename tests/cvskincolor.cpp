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
#include "opencvx/cvxskincolor.h"
#include "opencvx/cvxmorphological.h"

int main(int argc, char** argv)
{
    IplImage* img      = NULL;
    IplImage* mask     = NULL;
    IplImage* crop     = NULL;
    IplImage* dist     = NULL;
    CvCapture* cap     = NULL;
    int method = 0;

    if ( argc == 1 || (argc >= 2 && strlen (argv[1]) == 1 && isdigit (argv[1][0])) )
    {
        cap = cvCreateCameraCapture( argc == 2 ? argv[1][0] - '0' : 0 );
    }
    else if( argc >= 2 )
    {
        cap = cvCaptureFromFile( argv[1] );
    }
    img = cvQueryFrame(cap);
    if( !img )
    {
        printf("./cvskincolor <video_filename>\n"); exit( 1 );
    }

    if( argc >= 3 )
    {
        method = atoi( argv[2] );
    }
    if( !method )
    {
        std::cout << "Choose method:" << std::endl;
        std::cout << "1: cvSkinColorPeer" << std::endl;
        std::cout << "2: cvSkinColorCbCr" << std::endl;
        std::cout << "3: cvSkinColorGauss" << std::endl;
        std::cout << "4: cvSkinColorGmm" << std::endl;
        std::cin >> method;
    }

    crop = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );
    mask = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
    dist = cvCreateImage( cvGetSize(img), IPL_DEPTH_64F, 1 );

    cvNamedWindow( "Window", 1 );
    for( int frame = 1; img ; img = cvQueryFrame(cap), frame++ ) {
        if( img->origin != IPL_ORIGIN_TL ) cvFlip( img, img ); // prevent flipping on Windows

        //// preprocessing if you want
        //cvSmooth( img, img, CV_GAUSSIAN, 3, 3 );
        //cvEqualizeHist( img, img );
        //IplImage* sub = cvCreateImage( cvSize( img->width/2, img->height/2 ), img->depth, img->nChannels );
        //cvResize( img, sub ); // subsample to make process fast

        //// body
        switch( method )
        {
        case 1:
            cvSkinColorPeer( img, mask ); // fast
            break;
        case 2:
            cvSkinColorCrCb( img, mask, dist ); // 2nd fast, would be robust for illumination intesity change?
            break;
        case 3:
            cvSkinColorGauss( img, mask ); // worst as I expected
            break;
        case 4:
            cvSkinColorGmm( img, mask, 1.0, dist ); // best but takes time
            break;
        }

        //// postprocessing if you want
        //cvSmooth( mask, mask, CV_MEDIAN, 3 ); // noise clean
        //cvOpening( mask, mask, NULL, 3 ); // noise clean
        //cvClosing( mask, mask, NULL, 3 ); // fill contour

        //// show 
        //cvCvtScale( mask, mask, 255 ); cvShowImage( "Window", mask );
        cvSetZero( crop ); cvCopy( img, crop, mask );
        cvShowImage( "Window", crop );
        int k = cvWaitKey( 100 ); // wait 100m sec for viewing
        if( k == 27 ) break; // ESC
    }

    cvDestroyWindow( "Window" );
    cvReleaseCapture( &cap );
    cvReleaseImage( &crop );
    cvReleaseImage( &mask );
    cvReleaseImage( &dist );
}
