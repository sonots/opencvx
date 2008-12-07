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
#include "../cvputimageroi.h"

int main(int argc, char** argv)
{
    IplImage* patch    = NULL;
    IplImage* img      = NULL;

    if( argc < 3 )
    {
        fprintf( stderr, "./cvputimageroi <patch> <image>\n");
        exit( 1 );
    }
    patch = cvLoadImage( argv[1] );
    img = cvLoadImage( argv[2] );
    cvNamedWindow( "Before" );
    cvShowImage( "Before", img );
    cvPutImageROI( patch, img, 
                   cvRect32f(img->width/2.0, img->height/2.0,
                             img->width/4.0, img->height/4.0,
                             45) );
    cvNamedWindow( "After" );
    cvShowImage( "After", img );
    cvWaitKey( 0 );
}
