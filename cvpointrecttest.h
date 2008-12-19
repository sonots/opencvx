/** @file */
/* The MIT License
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
#ifndef CV_POINTRECTTEST_INCLUDED
#define CV_POINTRECTTEST_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include "cvrect32f.h"
#include "cvcreateaffine.h"
#include "cvrectpoints.h"

// double
// cvPointRect32fTest( const CvRect32f& rect, CvPoint2D32f pt, 
//                     int measure_dist = 0, CvPoint2D32f shear = cvPoint2D32f(0,0) );
// CV_INLINE double 
// cvPointRectTest( const CvRect& rect, CvPoint2D32f pt, int measure_dist = 0 );

/**
 * Point in rectangle test (Check whether point is in rectangle)
 *
 * @param rect           Rectangle (x,y,width,height), and angle if want.
 * @param pt             The point tested against the contour. 
 * @param measure_dist   0 - return positive, negative, 0 
 *                       if inside, outside, on the rectangle boundary 
 *                       respectively. 
 *                       1 - return singned distance between the point 
 *                       and the nearest rectangle edge. 
 * @param shear          shear deformation parameter (of affine transform)
 * @return               result of test
 * @see cvPointPolygonTest, cvPointRectTest
 */
CVAPI(double)
cvPointRect32fTest( const CvRect32f& rect, CvPoint2D32f pt, 
                    int measure_dist CV_DEFAULT(0), 
                    CvPoint2D32f shear CV_DEFAULT(cvPoint2D32f(0,0)) )
{
    CvPoint2D32f points[4];
    CvMat *contour = cvCreateMat( 1, 4, CV_32FC2 );

    cvRect32fPoints( rect, points, shear );
    for( int i = 0; i < 4; i++ )
    {
        CV_MAT_ELEM( *contour, CvPoint2D32f, 0, i ) = points[i];
    } // cvPointSeqFromMat

    /* // CV_32FC2 is not possible 
    CvPoint point;
    CvSeq *contour = cvCreateSeq( CV_32SC2, sizeof( CvSeq ), sizeof( CvPoint2D32f ), cvCreateMemStorage( 0 ) );
    for( int i = 0; i < 4; i++ )
    {
        point = cvPointFrom32f( points[i] );
        cvSeqPush( contour, &point );
        }*/

    double test = cvPointPolygonTest( contour, pt, measure_dist );

    cvReleaseMat( &contour );
    //cvClearMemStorage( contour->storage );
    return test;
}

/**
 * Point in rectangle test (Check whether point is in rectangle)
 * 
 * @param rect           Rectangle (x,y,width,height)
 * @param pt             The point tested against the contour. 
 * @param measure_dist   0 - return positive, negative, 0 
 *                       if inside, outside, on the rectangle boundary 
 *                       respectively. 
 *                       1 - return singned distance between the point 
 *                       and the nearest rectangle edge. 
 * @return               result of test
 * @return double
 * @see cvPointPolygonTest, cvPointRect32fTest
 */
CV_INLINE double 
cvPointRectTest( const CvRect& rect, CvPoint2D32f pt, 
                 int measure_dist CV_DEFAULT(0) )
{
    return cvPointRect32fTest( cvRect32fFromRect( rect ), pt, measure_dist );
}

#endif
