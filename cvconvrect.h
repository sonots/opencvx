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

#ifdef _MSC_VER // MS Visual Studio
#pragma warning(push)
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib") 
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#endif
#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#define CV_RECT_NORMAL 1
#define CV_RECT_CENTER 2
CvRect cvConvRect(const CvRect rect, double rotate, int from, int to);

/**
% cvuConvRect - (CV Utility) Convert a region of interest (Rect) format
%
% Synopsis
%   [rect] = cvConvRect(rect, rotate, from, to);
%
% Inputs ([]s are optional)
%   (vector) rect     [x; y; width; height]
%   (scalar) rotate   rotation in degree. 
%                     Positive values mean couter-clockwise rotation
%   (scalar) from     
%   (scalar) to       
%                     CV_RECT_NORMAL - [x; y; width; height; xy_rotate]
%                     where x, y are upper-left corner coordinates
%                     and xy_rotate is rotation as (x,y) is center
%                     CV_RECT_CENTER - [cx; cy; width; height; cxy_rotate]
%                     where cx, cy are center coordinates
%                     and cxy_rotate is rotation as (cx,cy) is center
%                     3 - [x; y; width; height; cxy_rotate]
%                     where x, y are center coordinates
%                     and cxy_rotate is rotation as (cx,cy) is center
%
% Outputs ([]s are optional)
%   (vector) rect    Converted representation
*/
CvRect cvConvRect(const CvRect rect, double rotate, int from, int to)
{
    CvRect ret = rect;
    if( from == CV_RECT_NORMAL )
    {
        // [x y width height xy_rotate]
        double x = rect.x;
        double y = rect.y;
        double width = rect.width;
        double height = rect.height;
        double cx, cy;
        // [cx cy width height cxy_rotate]
        if( to == CV_RECT_CENTER || to == 3 ) {
            CvMat* RMat = cvCreateMat( 2, 3, CV_32FC1 );
            cv2DRotationMatrix( cvPoint2D32f( x, y ), rotate, 1.0, RMat );
            cx = (2*x + width - 1)/2.0;
            cy = (2*y + height - 1)/2.0;
            ret.x = (int)(cvmGet( RMat, 0, 0 ) * cx + cvmGet( RMat, 0, 1 ) * cy + cvmGet( RMat, 0, 2 ));
            ret.y = (int)(cvmGet( RMat, 1, 0 ) * cx + cvmGet( RMat, 1, 1 ) * cy + cvmGet( RMat, 1, 2 ));
        }
        // [x y width height cxy_rotate]
        if( to == 3 ) {
            ret = cvConvRect(ret, rotate, CV_RECT_CENTER, 3);
        }
    }
    else if( from == CV_RECT_CENTER ) // [cx; cy; width; height; cxy_rotate]
    { 
        double cx = rect.x;
        double cy = rect.y;
        double width = rect.width;
        double height = rect.height;
        double x, y;
        // [x y width height cxy_rotate]
        if( to == 3 || to == CV_RECT_NORMAL ) {
            x = ( 2*cx + 1 - width ) / 2.0;
            y = ( 2*cy + 1 - height ) / 2.0;
            ret.x = (int)x;
            ret.y = (int)y;
        }
        // [x y width height xy_rotate]
        if( to == CV_RECT_NORMAL ) {
            CvMat* RMat = cvCreateMat( 2, 3, CV_32FC1 );
            cv2DRotationMatrix( cvPoint2D32f( cx, cy ), rotate, 1.0, RMat );
            ret.x = (int)(cvmGet( RMat, 0, 0 ) * x + cvmGet( RMat, 0, 1 ) * y + cvmGet( RMat, 0, 2 ));
            ret.y = (int)(cvmGet( RMat, 1, 0 ) * x + cvmGet( RMat, 1, 1 ) * y + cvmGet( RMat, 1, 2 ));
        }
    }
    else if( from == 3 )
    {
        // [x; y; width; height; cxy_rotate]
        double x = rect.x;
        double y = rect.y;
        double width = rect.width;
        double height = rect.height;
        double cx, cy;
        // [cx cy width height center_rotate]
        if( to == CV_RECT_CENTER || to == CV_RECT_NORMAL ) {
            cx = (2*x + width - 1)/2.0;
            cy = (2*y + height - 1)/2.0;
            ret.x = (int)cx;
            ret.y = (int)cy;
        }
        if( to == CV_RECT_NORMAL ) {
            ret = cvConvRect(ret, rotate, 2, CV_RECT_NORMAL );
        }
    }
    return ret;
}

#ifdef _MSC_VER // MS Visual Studio
#pragma warning(pop)
#endif

#endif
