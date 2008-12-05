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

int main( int argc, char** argv )
{
    char        *referencefile = NULL;
    char        *templatefile = NULL;
    IplImage    *img;
    IplImage    *tpl;
    IplImage    *res;
    CvPoint     minloc, maxloc;
    double      minval, maxval;
    int         img_width, img_height;
    int         tpl_width, tpl_height;
    int         res_width, res_height;
    int         i;
    bool        show = false;

    /* check for arguments */
    for( i = 1; i < argc; i++ ) {
        if( !strcmp( argv[i], "-show" ) ) {
            show = true;
        } else if( argv[i][0] == '-' ) {
            fprintf( stderr, "ERROR: Unknown option %s\n", argv[i] );
            return 1;
        } else if( referencefile == NULL ) {
            referencefile = argv[i];
        } else {
            templatefile = argv[i];
        }
    }
    if( referencefile == NULL || templatefile == NULL ) {
        fprintf( stderr, "Usage: templatematch <reference> <template> [-show]\n" );
        return 1;
    }

    /* load reference image */
    img = cvLoadImage( referencefile, CV_LOAD_IMAGE_COLOR );
    if( img == 0 ) {
        fprintf( stderr, "Cannot load file %s!\n", referencefile );
        return 1; 
    }

    /* load template image */
    tpl = cvLoadImage( templatefile, CV_LOAD_IMAGE_COLOR );
    if( tpl == 0 ) {
        fprintf( stderr, "Cannot load file %s!\n", templatefile );
        return 1;
    }

    /* get image's properties */
    img_width  = img->width;
    img_height = img->height;
    tpl_width  = tpl->width;
    tpl_height = tpl->height;
    res_width  = img_width - tpl_width + 1;
    res_height = img_height - tpl_height + 1;

    /* create new image for template matching computation */
    res = cvCreateImage( cvSize( res_width, res_height ), IPL_DEPTH_32F, 1 );

    /* choose template matching method to be used */
    cvMatchTemplate( img, tpl, res, CV_TM_SQDIFF );
    /*cvMatchTemplate( img, tpl, res, CV_TM_SQDIFF_NORMED );
    cvMatchTemplate( img, tpl, res, CV_TM_CCORR );
    cvMatchTemplate( img, tpl, res, CV_TM_CCORR_NORMED );
    cvMatchTemplate( img, tpl, res, CV_TM_CCOEFF );
    cvMatchTemplate( img, tpl, res, CV_TM_CCOEFF_NORMED );*/

    cvMinMaxLoc( res, &minval, &maxval, &minloc, &maxloc, 0 );

    printf("%g\n", minval);
    /* draw red rectangle */
    cvRectangle( img, 
        cvPoint( minloc.x, minloc.y ), 
        cvPoint( minloc.x + tpl_width, minloc.y + tpl_height ),
        cvScalar( 0, 0, 255, 0 ), 1, 0, 0 );

    /* print */
    printf( "%d %d %d %d\n", minloc.x, minloc.y, tpl_width, tpl_height );

    /* display images */
    if( show ) {
        cvNamedWindow( "reference", CV_WINDOW_AUTOSIZE );
        cvNamedWindow( "template", CV_WINDOW_AUTOSIZE );
        cvShowImage( "reference", img );
        cvShowImage( "template", tpl );

        /* wait until user press a key to exit */
        cvWaitKey( 0 );

        /* free memory */
        cvDestroyWindow( "reference" );
        cvDestroyWindow( "template" );
    }

    /* free memory */
    cvReleaseImage( &img );
    cvReleaseImage( &tpl );
    cvReleaseImage( &res );

    return 0;
}
