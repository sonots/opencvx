/**
// cvxbackground.h
//
// Copyright (c) 2008, Naotoshi Seo. All rights reserved.
//
// The program is free to use for non-commercial academic purposes,
// but for course works, you must understand what is going inside to 
// use. The program can be used, modified, or re-distributed for any 
// purposes only if you or one of your group understand not only 
// programming codes but also theory and math behind (if any). 
// Please contact the authors if you are interested in using the 
// program without meeting the above conditions.
*/
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )
#pragma comment( lib, "cv.lib" )
#pragma comment( lib, "cxcore.lib" )
#pragma comment( lib, "cvaux.lib" )
#endif

#include "cv.h"
#include "cvaux.h"

#ifndef CV_BACKGROUND_INCLUDED
#define CV_BACKGROUND_INCLUDED

/**
// Obtain non-background pixels using reference image (such as previous frame in video )
//
// @param mg     The target image
// @param ref    The reference image. Usually the previous frame of video
// @param mask   The generated mask image where 0 is for bg and 1 is for non-bg. Must be 8U and 1 channel
// @param [thresh = 100] The threshold. [0 - 255^2] for single channel image. [0 - 255^2 * 3] for 3 channel image.
// @return void
*/
CVAPI(void) cvBackgroundSimple( const IplImage* _img, const IplImage* _ref, IplImage* _mask, int thresh = 100 )
{
    CV_FUNCNAME( "cvBackgroundSimple" ); // error handling
    __BEGIN__;
    CV_ASSERT( _img->width == _ref->width );
    CV_ASSERT( _img->width == _mask->width );
    CV_ASSERT( _img->height == _ref->height );
    CV_ASSERT( _img->height == _ref->height );
    CV_ASSERT( _img->nChannels == _ref->nChannels );
    CV_ASSERT( _mask->nChannels == 1 );
    CV_ASSERT( _mask->depth == IPL_DEPTH_8U );

    IplImage *img = cvCreateImage( cvGetSize(_img), IPL_DEPTH_32F, _img->nChannels );
    IplImage *ref = cvCreateImage( cvGetSize(_img), IPL_DEPTH_32F, _img->nChannels );
    IplImage *mask = cvCreateImage( cvGetSize(_img), IPL_DEPTH_32F, 1 );
    cvConvert( _img, img );
    cvConvert( _ref, ref );
    cvSub( img, ref, img );
    cvMul( img, img, img ); // square

    if( img->nChannels > 1 )
    {        
        IplImage* imgB = cvCreateImage( cvGetSize(img), img->depth, 1 );
        IplImage* imgG = cvCreateImage( cvGetSize(img), img->depth, 1 );
        IplImage* imgR = cvCreateImage( cvGetSize(img), img->depth, 1 );
        cvSplit( img, imgB, imgG, imgR, NULL );
        cvAdd( imgB, imgG, mask );
        cvAdd( mask, imgR, mask );
        cvReleaseImage( &imgB );
        cvReleaseImage( &imgG );
        cvReleaseImage( &imgR );
    }
    else
    {
        cvCopy( img, mask );
    }
    cvThreshold( mask, _mask, thresh, 1, CV_THRESH_BINARY );

    cvReleaseImage( &img );
    cvReleaseImage( &ref );
    cvReleaseImage( &mask );
    __END__;
}

/*
//this is a sample for foreground detection functions
//this is not training, but using pre-determined params
//See cvCreateGaussainBGModel in ./cvaux/src/cvbgfg_gaussmix.cpp
//This is setting paramets such as CV_BGFG_MOG_BACKGROUND_THRESHOLD (cvaux.h)
//cvUpdateBGStatModel means just setting bg_model->foreground, background
int main(int argc, char** argv)
{
    IplImage*       tmp_frame = NULL;
    CvCapture*      video = NULL;

    video = cvCaptureFromFile(argv[1]);
    tmp_frame = cvQueryFrame(video);
    if(!tmp_frame)
    {
        printf("bad video \n");
        exit(0);
    }

    cvNamedWindow("BG", 1);
    cvNamedWindow("FG", 1);

    //create BG model   
    CvBGStatModel* bg_model = cvCreateGaussianBGModel( tmp_frame );
     
    for( int fr = 1;tmp_frame; tmp_frame = cvQueryFrame(video), fr++ )
    {
        cvUpdateBGStatModel( tmp_frame, bg_model );
        cvShowImage("BG", bg_model->background);
        cvShowImage("FG", bg_model->foreground);
        int k = cvWaitKey(5);
        if( k == 27 ) break;
        //printf("frame# %d \r", fr);
    }


    cvReleaseBGStatModel( &bg_model );
    cvReleaseCapture(&video);

    return 0;
}*/

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif