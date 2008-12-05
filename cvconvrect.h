/** @file
* The MIT License
* 
* Copyright (c) 2008, Naotoshi Seo <sonots(at)sonots.com>
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
#ifndef CV_CONVRECT_INCLUDED
#define CV_CONVRECT_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define CV_RECT_NORMAL 1
#define CV_RECT_CENTER 2
CvRect cvConvRect(const CvRect rect, double rotate, int from, int to);

/**
 * Convert a rectangle format
 *
 * CV_RECT_NORMAL - [x; y; width; height; xy_rotate]
 *                  where x, y are upper-left corner coordinates
 *                  and xy_rotate is rotation as (x,y) is center
 * CV_RECT_CENTER - [cx; cy; width; height; cxy_rotate]
 *                  where cx, cy are center coordinates
 *                  and cxy_rotate is rotation as (cx,cy) is center
 *              3 - [x; y; width; height; cxy_rotate]
 *                  where x, y are center coordinates
 *                  and cxy_rotate is rotation as (cx,cy) is center
 *
 * @param rect      [x; y; width; height]
 * @param rotate    rotation in degree. 
 *                  Positive values mean couter-clockwise rotation
 * @param from      CV_RECT_NORMAL or CV_RECT_CENTER or 3
 * @param to        CV_RECT_NORMAL or CV_RECT_CENTER or 3
 * @return converted rectangle format
 *
 */
CvRect cvConvRect(const CvRect rect, double rotate, int from, int to)
{
    CvRect ret = rect;
    if( from == CV_RECT_NORMAL ) // [x y width height xy_rotate]
    {
        double x, y, width, height, cx, cy;
        x = rect.x;
        y = rect.y;
        width = rect.width;
        height = rect.height;
        // [cx cy width height cxy_rotate]
        if( to == CV_RECT_CENTER || to == 3 ) {
            CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
            cv2DRotationMatrix( cvPoint2D32f( x, y ), rotate, 1.0, R );
            cx = ( 2*x + width - 1 ) / 2.0;
            cy = ( 2*y + height - 1 ) / 2.0;
            ret.x = (int)( cvmGet( R, 0, 0 ) * cx + cvmGet( R, 0, 1 ) * cy + cvmGet( R, 0, 2 ) );
            ret.y = (int)( cvmGet( R, 1, 0 ) * cx + cvmGet( R, 1, 1 ) * cy + cvmGet( R, 1, 2 ) );
            cvReleaseMat( &R );
        }
        // [x y width height cxy_rotate]
        if( to == 3 ) {
            ret = cvConvRect(ret, rotate, CV_RECT_CENTER, 3);
        }
    }
    else if( from == CV_RECT_CENTER ) // [cx; cy; width; height; cxy_rotate]
    { 
        double x, y, width, height, cx, cy;
        cx = rect.x;
        cy = rect.y;
        width = rect.width;
        height = rect.height;
        // [x y width height cxy_rotate]
        if( to == 3 || to == CV_RECT_NORMAL ) {
            x = ( 2*cx + 1 - width ) / 2.0;
            y = ( 2*cy + 1 - height ) / 2.0;
            ret.x = (int)x;
            ret.y = (int)y;
        }
        // [x y width height xy_rotate]
        if( to == CV_RECT_NORMAL ) {
            CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
            cv2DRotationMatrix( cvPoint2D32f( cx, cy ), rotate, 1.0, R );
            ret.x = (int)( cvmGet( R, 0, 0 ) * x + cvmGet( R, 0, 1 ) * y + cvmGet( R, 0, 2 ) );
            ret.y = (int)( cvmGet( R, 1, 0 ) * x + cvmGet( R, 1, 1 ) * y + cvmGet( R, 1, 2 ) );
            cvReleaseMat( &R );
        }
    }
    else if( from == 3 ) // [x; y; width; height; cxy_rotate]
    {
        double x, y, width, height, cx, cy;
        cx = rect.x;
        cy = rect.y;
        width = rect.width;
        height = rect.height;
        // [cx cy width height center_rotate]
        if( to == CV_RECT_CENTER || to == CV_RECT_NORMAL ) {
            cx = ( 2*x + width - 1 ) / 2.0;
            cy = ( 2*y + height - 1 ) / 2.0;
            ret.x = (int)cx;
            ret.y = (int)cy;
        }
        if( to == CV_RECT_NORMAL ) {
            ret = cvConvRect(ret, rotate, 2, CV_RECT_NORMAL );
        }
    }
    return ret;
}


#endif
