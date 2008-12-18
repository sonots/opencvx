#ifdef _MSC_VER
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#pragma warning(disable: 4996)
#endif

#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>
#include "../cvbackground.h"
#include "../cvxmorphological.h"
#include <ctype.h>

int main(int argc, char** argv)
{
    IplImage* img      = NULL;
    IplImage* ref      = NULL;
    IplImage* crop     = NULL;
    IplImage* mask     = NULL;
    CvCapture* cap     = NULL;

    if ( argc == 1 || (argc >= 2 && strlen (argv[1]) == 1 && isdigit (argv[1][0])) )
    {
        cap = cvCreateCameraCapture (argc == 2 ? argv[1][0] - '0' : 0);
    }
    else if ( argc >= 2 )
    {
        cap = cvCaptureFromFile(argv[1]);
    }
    img = cvQueryFrame(cap);
    if( !img )
    {
        printf("bad cap \n"); exit(1);
    }
    ref  = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );
    crop = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );
    mask = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );

    cvNamedWindow( "Window", 1 );
    cvSetZero( ref );
    for( int frame = 1; img ; img = cvQueryFrame(cap), frame++ ) {
        if( img->origin != IPL_ORIGIN_TL ) cvFlip( img, img ); // prevent flipping on Windows

        //// preprocessing if you want
        //cvSmooth( img, img, CV_GAUSSIAN, 3, 3 );

        //// body
        cvBackground( img, ref, mask, 100 );

        //// postprocessing if you want
        //cvSmooth( mask, mask, CV_MEDIAN, 3 ); // noise clean
        //cvOpening( mask, mask, NULL, 3 ); // noise clean
        //cvClosing( mask, mask, NULL, 3 ); // fill contour

        //// show
        //cvCvtScale( mask, mask, 255 ); cvShowImage( "Window", mask ); cvWaitKey(0);
        cvSetZero( crop ); cvCopy( img, crop, mask );
        cvShowImage( "Window", crop );
        int k = cvWaitKey( 100 ); // wait 100m sec for viewing
        if( k == 27 ) break; // ESC
        cvCopy( img, ref );
    }

    cvDestroyWindow( "Window" );
    cvReleaseCapture( &cap );
    cvReleaseImage( &ref );
    cvReleaseImage( &crop );
    cvReleaseImage( &mask );
    return 0;
}
