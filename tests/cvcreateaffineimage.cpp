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
#include "../cvxmat.h"
#include "../cvcreateaffine.h"
#include "../cvcreateaffineimage.h"

int main( int argc, char** argv )
{
    IplImage* img;
    if( argc < 2 )
    {
        fprintf( stderr, "./a.exe <image_file>" );
        exit( 1 );
    }
    img = cvLoadImage( argv[1] );
    CvMat* affine = cvCreateMat( 2, 3, CV_64FC1 );
    cvCreateAffine( affine, cvRect32f( 0, 0, 1, 1/2.0, 10 ), cvPoint2D32f( 0, 0 ) );
    IplImage* transformed;

    transformed = cvCreateAffinedImage( img, affine, CV_AFFINE_SAME );
    cvNamedWindow( "hoge" );
    cvShowImage( "hoge", transformed );
    cvWaitKey( 0 );
    cvReleaseImage( &transformed );

    CvPoint origin;
    transformed = cvCreateAffineImage( img, affine, CV_AFFINE_FULL, &origin );
    cvNamedWindow( "hoge" );
    cvShowImage( "hoge", transformed );
    printf( "origin.x = %d origin.y = %d\n", origin.x, origin.y );
    cvWaitKey( 0 );
    cvReleaseImage( &transformed );

    IplImage *mask = cvCreateAffineMask( img, affine, CV_AFFINE_FULL );
    cvNamedWindow( "hoge" );
    //cvShowImage( "hoge", mask ); // 1 is 1/255...
    printf( "origin.x = %d origin.y = %d\n", origin.x, origin.y );
    cvWaitKey( 0 );
    cvReleaseImage( &mask );

    cvReleaseMat( &affine );
}
