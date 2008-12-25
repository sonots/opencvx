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
#ifndef CV_INTER_LINEAR_INCLUDED
#define CV_INTER_LINEAR_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

/**
 * Bilinear Interpolation
 *
 * Get a color at a floating position using bilinear interpolation method
 * Floating origin is the center of (0,0) pixel.
 *
 * @param  arr     array
 * @param  pt      position
 * @return color
 * @see cvInterNn
 * @see cvResize has CV_INTER_LINEAR but it was not reusable. 
 */
CVAPI(CvScalar) cvInterLinear( const CvArr *arr, 
                               CvPoint2D32f pos )
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
    ix = cvFloor( pos.x );
    iy = cvFloor( pos.y );
    dx = pos.x - ix;
    dy = pos.y - iy;
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


#endif
