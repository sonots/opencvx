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
#ifndef CV_GET2D_INTER_INCLUDED
#define CV_GET2D_LINEAR_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

/**
 * Nearest Neighbor Interpolation
 *
 * Get a color at a floating position using nearest neighbor (round) method.
 *
 * @param  arr     array
 * @param  row
 * @param  col
 * @return color
 * @see cvInterLinear
 */
CVAPI(CvScalar) 
icvGet2DInterNn( const CvArr *arr, float row, float col )
{
    IplImage* img, imgstub;
    int ix, iy;
    CvScalar color;
    CV_FUNCNAME( "cvInterNn" );
    __BEGIN__;
    img = (IplImage*)arr;
    if( !CV_IS_IMAGE(img) )
    {
        CV_CALL( img = cvGetImage( img, &imgstub ) );
    }
    ix = cvRound( col );
    iy = cvRound( row );
    color = cvGet2D( img, iy, ix );
    __END__;
    return color;
}

/**
 * Bilinear Interpolation
 *
 * Get a color at a floating position using bilinear interpolation method
 * Floating origin is the center of (0,0) pixel.
 *
 * @param  arr     array
 * @param  row
 * @param  col
 * @return color
 */
CVAPI(CvScalar)
icvGet2DInterLinear( const CvArr *arr, float row, float col )
{
    IplImage* img, imgstub;
    int ix, iy;
    float dx, dy;
    CvPoint pt[4];
    CvScalar c[4];
    CvScalar color;
    int i, ch;
    CV_FUNCNAME( "cvInterLinear" );
    __BEGIN__;
    img = (IplImage*)arr;
    if( !CV_IS_IMAGE(img) )
    {
        CV_CALL( img = cvGetImage( img, &imgstub ) );
    }
    ix = cvFloor( col );
    iy = cvFloor( row );
    dx = col - ix;
    dy = row - iy;
    if( ix < 0 ) { // out of left
        pt[0].x = pt[1].x = pt[2].x = pt[3].x = 0;
    } else if( ix > img->width - 2 ) { // out of right
        pt[0].x = pt[1].x = pt[2].x = pt[3].x = img->width - 2;
    } else { // inside
        pt[0].x = pt[2].x = ix;
        pt[1].x = pt[3].x = ix + 1;
    }
    if( iy < 0 ) { // out of top
        pt[0].y = pt[1].y = pt[2].y = pt[3].y = 0;
    } else if( iy > img->height - 2 ) { // out of bottom
        pt[0].y = pt[1].y = pt[2].y = pt[3].y = img->height - 2;
    } else { // inside
        pt[0].y = pt[1].y = iy;
        pt[2].y = pt[3].y = iy + 1;
    }
    for( i = 0; i < 4; i++ )
        c[i] = cvGet2D( img, pt[i].y, pt[i].x );

    for( ch = 0; ch < img->nChannels; ch++ )
    {
        color.val[ch] = ( c[0].val[ch] * dx + c[1].val[ch] * (1 - dx) ) * dy
            + ( c[2].val[ch] * dx + c[3].val[ch] * (1 - dx) ) * (1 - dy);
    }
    __END__;
    return color;
}

/**
 * Get an element of array with interpolation method
 *
 * Get a color at a floating position using an interpolation method.
 * Floating origin is the center of (0,0) pixel.
 *
 * @param  arr     array
 * @param  row     floating row position
 * @param  col     floating col position
 * @param  inter   The interpolation method. 
 *   - CV_INTER_NN - Nearest Neighborhood method
 *   - CV_INTER_LINEAR - Bilinear interpolation
 * @return color
 * @see cvResize has "interpolation" method, 
 *      but it was not reusable. 
 */
CVAPI(CvScalar)
cvGet2DInter( const CvArr *arr, float row, float col,
              int inter CV_DEFAULT(CV_INTER_LINEAR) )
{
    CvScalar color;
    CV_FUNCNAME( "cvGet2DInter" );
    __BEGIN__;
    switch( inter )
    {
    case CV_INTER_NN:
        color = icvGet2DInterNn( arr, row, col );
        break;
    case CV_INTER_LINEAR:
        color = icvGet2DInterLinear( arr, row, col );
        break;
    default:
        CV_ERROR( 0, "No such interpolation method is available." );
        break;
    }
    __END__;
    return color;
}

#endif
