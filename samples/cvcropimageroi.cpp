#ifdef _MSC_VER
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#endif

#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include "cvcropimageroi.h"

int main( int argc, char** argv )
{
    IplImage* img = NULL, *crop = NULL;
    int x = 0, y = 0, width = 1, height = 1;
    float angle = 0, shear_x = 0, shear_y = 0;
    if( argc < 2 )
    {
        fprintf( stderr, "Usage: cvcropimageroi <img> <x y width height angle shear_x shear_y>\n" );
        return 1;
    }

    img = cvLoadImage( argv[1] );
    if( argc >= 6 )
    {
        x = atoi( argv[2] );
        y = atoi( argv[3] );
        width = atoi( argv[4] );
        height = atoi( argv[5] );
    }
    if( argc >= 7 )
    {
        angle = atof( argv[6] );
    }
    if( argc >= 9 )
    {
        shear_x = atof( argv[7] );
        shear_y = atof( argv[8] );
    }

    crop = cvCreateImage( cvSize(width,height), img->depth, img->nChannels); 
    cvCropImageROI( img, crop, cvRect32f(x,y,width,height,angle), cvPoint2D32f(shear_x,shear_y) );

    cvNamedWindow( "Image", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Image", img );
    cvNamedWindow( "Crop", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Crop", crop );
    cvWaitKey( 0 );
    cvDestroyWindow( "Image" );
    cvDestroyWindow( "Crop" );

    cvReleaseImage( &crop );

    return 0;
}
