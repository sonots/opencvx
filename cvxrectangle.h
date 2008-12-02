/**
* The MIT License
* 
* Copyright (c) 2008, Naotoshi Seo <sonots(at)umd.edu>
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#ifndef CV_RECTANGLE_INCLUDED
#define CV_RECTANGLE_INCLUDED

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#endif

#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>
#include <math.h>
using namespace std;
#include "cvcreateaffine.h"
#include "cvdrawrectangle.h"
#include "cvcropimageroi.h"
#include "cvpointnorm.h"
#include "cvconvrect.h"

/**
// Print cvRect parameters
//
// @param CvRect rect
// @return void
*/
CV_INLINE void cvPrintRect( const CvRect &rect )
{
    printf( "%d %d %d %d\n", rect.x, rect.y, rect.width, rect.height );
    fflush( stdout );
}

/**
// If the rectangle runs off outside image, pick only inside regions
//
// @param CvRect rect
// @param CvPoint max
// @return CvRect
*/
CV_INLINE CvRect cvValidateRect( CvRect rect, CvPoint max )
{
    if( rect.x < 0 ) // left side is outside boundary
    {
        rect.width += rect.x; // width is reduced
        rect.x = 0;
    }
    if( rect.y < 0 ) // top side is outside boundary
    {
        rect.height += rect.y; // height is reduced
        rect.y = 0;
    }
    if( rect.x + rect.width >= max.x ) // right side is outside boundary
    {
        rect.width = max.x - rect.x;
    }
    if( rect.y + rect.height >= max.y ) // bottom side is outside boundary
    {
        rect.height = max.y - rect.y;
    }
    return rect;
}

/**
// Create a rectangle mask image
//
// @param IplImage* mask            The mask image (Single channel, IPL_DEPTH_8U) to be created. 0 or 1 values
// @param CvRect    rect            The rectangle region
// @param CvMat*    [affine = NULL] The affine transform matrix
// @param bool      [fill = true]   Fill the rectangle region or not
// @return void
*/
//CVAPI(void) cvCreateImageROI( IplImage* mask, const CvRect& rect, CvMat* _affine = NULL, bool fill = true )
//{
//    CV_FUNCNAME( "cvCreateImageROI" );
//    __BEGIN__;
//
//    int x, y, xp, yp;
//    CvMat* affine = NULL;
//    CvMat* xy  = cvCreateMat( 3, 1, CV_32FC1 ); 
//    CvMat* xyp = cvCreateMat( 2, 1, CV_32FC1 );
//
//    if( _affine != NULL )
//    {
//        CV_ASSERT( _affine->rows == 2 && _affine->cols == 3 );
//        affine = _affine;
//    }
//    else
//    {
//        affine = cvCreateMat( 2, 3, CV_32FC1 );
//        cvZero( affine );
//        cvmSet( affine, 0, 0, 1.0 );
//        cvmSet( affine, 1, 1, 1.0 );
//        // No transform
//    }
//    cvZero( mask );
//    cvmSet( xy, 2, 0, 1.0 );
//
//
//    if( fill )
//    {
//        for( x = 0; x < rect.width; x++ )
//        {
//            cvmSet( xy, 0, 0, x );
//            for( y = 0; y < rect.height; y++ )
//            {
//                cvmSet( xy, 1, 0, y );
//                cvMatMul( affine, xy, xyp );
//                xp = (int)cvmGet( xyp, 0, 0 );
//                yp = (int)cvmGet( xyp, 1, 0 );
//                if( xp < 0 || xp >= mask->width || yp < 0 || yp >= mask->height ) continue;
//                mask->imageData[mask->widthStep * yp + xp] = 1;
//            }
//        }
//    }
//    else
//    {
//        for( x = 0; x < rect.width; x++ )
//        {
//            cvmSet( xy, 0, 0, x );
//            for( y = 0; y < rect.height; y += max(1, rect.height - 1) )
//            {
//                cvmSet( xy, 1, 0, y );
//                cvMatMul( affine, xy, xyp );
//                xp = (int)cvmGet( xyp, 0, 0 );
//                yp = (int)cvmGet( xyp, 1, 0 );
//                if( xp < 0 || xp >= mask->width || yp < 0 || yp >= mask->height ) continue;
//                mask->imageData[mask->widthStep * yp + xp] = 1;
//            }
//        }
//        for( y = 0; y < rect.height; y++ )
//        {
//            cvmSet( xy, 1, 0, y );
//            for( x = 0; x < rect.width; x += max( 1, rect.width - 1) )
//            {
//                cvmSet( xy, 0, 0, x );
//                cvMatMul( affine, xy, xyp );
//                xp = (int)cvmGet( xyp, 0, 0 );
//                yp = (int)cvmGet( xyp, 1, 0 );
//                if( xp < 0 || xp >= mask->width || yp < 0 || yp >= mask->height ) continue;
//                mask->imageData[mask->widthStep * yp + xp] = 1;
//            }
//        }
//    }
//
//    if( _affine == NULL ) cvReleaseMat( &affine );
//    cvReleaseMat( &xy );
//    cvReleaseMat( &xyp );
//
//    __END__;
//}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
