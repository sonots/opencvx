/**
// cvxmorphological.h
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

#ifndef CV_MORPHOLOGICAL_INCLUDED
#define CV_MORPHOLOGICAL_INCLUDED

/**
// cvOpening - opening morphological operation
//
// opening operation would help to remove noise
//
// @param src Input Array
// @param dst Output Array
// @param [element = NULL] Kernel shape. see cvErode or cvDilate
// @param [iterations = 1]
// @return void
//
// References)
//  R. Gonzalez, R. Woods, "Digital Image Processing," chapter 9
*/
CVAPI( void ) cvOpening( const CvArr* src, CvArr* dst, IplConvKernel* element = NULL, int iterations = 1 )
{
    cvErode( src, dst, element, iterations );
    cvDilate( dst, dst, element, iterations );
}

/**
// cvClosing - closing morphological operation
//
// closing operation would help to fill disconnected contour
//
// @param src Input Array
// @param dst Output Array
// @param [element = NULL] Kernel shape. see cvErode or cvDilate
// @param [iterations = 1]
// @return void
//
// References)
//  R. Gonzalez, R. Woods, "Digital Image Processing," chapter 9
*/
CVAPI(void) cvClosing( const CvArr* src, CvArr* dst, IplConvKernel* element = NULL, int iterations = 1 )
{
    cvDilate( dst, dst, element, iterations );
    cvErode( src, dst, element, iterations );
}

/**
// cvSimpleFill - Search boundary (non-zero pixel) from both side and fill inside
//
// @param IplImage* src One channel image with 0 or 1 value (mask image)
// @param IplImage* dst
// @see cvSmooth( src, dst, CV_MEDIAN, 3 )
// @see cvClosing( src, dst, NULL, 3 )
*/
void cvSimpleFill( const IplImage* src, IplImage* dst )
{
    cvCopy( src, dst );
    for( int y = 0; y < dst->height; y++ )
    {
        int start = -1;
        int end = -1;
        for( int x = 0; x < dst->width - 1; x++)
        {
            int p1 = dst->imageData[dst->widthStep * y + x];
            int p2 = dst->imageData[dst->widthStep * y + x + 1];
            if( p1 > 0 && p2 > 0 )
            {
                start = x;
                break;
            }
        }
        for( int x = dst->width - 1; x > start; x--)
        {
            int p1 = dst->imageData[dst->widthStep * y + x];
            int p2 = dst->imageData[dst->widthStep * y + x + 1];
            if( p1 > 0 && p2 > 0 )
            {
                end = x;
                break;
            }
        }
        if( start != -1 && end != -1 )
        {
            for( int x = start; x <= end; x++)
            {
                dst->imageData[dst->widthStep * y + x] = 1;
            }
        }
    }
    for( int x = 0; x < dst->width; x++ )
    {
        int start = -1;
        int end = -1;
        for( int y = 0; y < dst->height - 1; y++)
        {
            int p1 = dst->imageData[dst->widthStep * y + x];
            int p2 = dst->imageData[dst->widthStep * y + x + 1];
            if( p1 > 0 && p2 > 0 )
            {
                start = y;
                break;
            }
        }
        for( int y = dst->height - 1; y > start; y--)
        {
            int p1 = dst->imageData[dst->widthStep * y + x];
            int p2 = dst->imageData[dst->widthStep * y + x + 1];
            if( p1 > 0 && p2 > 0 )
            {
                end = y;
                break;
            }
        }
        if( start != -1 && end != -1 )
        {
            for( int y = start; y <= end; y++)
            {
                dst->imageData[dst->widthStep * y + x] = 1;
            }
        }
    }
    //// Tried to use cvFindContours, but did not work for disconnected contours.
    //CvMemStorage* storage = cvCreateMemStorage(0);
    //CvSeq* contour = 0;
    //cvFindContours( dst, storage, &contour, 
    //    sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    //for( ; contour != 0; contour = contour->h_next )
    //{
    //    CvScalar color = cvScalar(255);
    //    cvDrawContours( dst, contour, color, color, -1, CV_FILLED, 8 );
    //}
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
