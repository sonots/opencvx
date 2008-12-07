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
#ifndef CV_AFFINEIMAGE_INCLUDED
#define CV_AFFINEIMAGE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>

#include "cvinvaffine.h"

#define CV_AFFINE_SAME 0
#define CV_AFFINE_FULL 1
CVAPI(IplImage*) cvAffineImage( const IplImage* src, const CvMat* affine, CvPoint* origin = NULL,
                                int flags = CV_AFFINE_FULL, CvScalar color = CV_RGB(0,0,0) );

/**
 * Affine transform of an image
 *
 * Do not forget cvReleaseImage( &ret );
 *
 * @param src       Image
 * @param affine    2 x 3 Affine transform matrix
 * @param origin    The coordinate of origin (the coordinate in original image respective to 
 *                  the transformed image origin). 
 *                  Useful when CV_AFFINE_FULL is used.
 * @param flags     CV_AFFINE_SAME - Outside image coordinates are cut off
 *                  CV_AFFINE_FULL - Fully contain the original image pixel values
 * @return IplImage*
 * @see cvWarpAffine - this does not support CV_AFFINE_FULL, but supports
 *                     several interpolation methods and so on.
 */
CVAPI(IplImage*) cvAffineImage( const IplImage* src, const CvMat* affine, CvPoint* origin, int flags, CvScalar color )
{
    IplImage* dst;
    int minx = INT_MAX;
    int miny = INT_MAX;
    int maxx = INT_MIN;
    int maxy = INT_MIN;
    CV_FUNCNAME( "cvAffineImage" );
    __BEGIN__;
    CV_ASSERT( src->depth == IPL_DEPTH_8U );
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );

    CvPoint pt[4];
    // original 4 corner
    pt[0].x = 0;              pt[0].y = 0;
    pt[1].x = src->width - 1; pt[1].y = 0;
    pt[2].x = 0;              pt[2].y = src->height - 1;
    pt[3].x = src->width - 1; pt[3].y = src->height - 1;
    // 4 corner after transformed
    for( int i = 0; i < 4; i++ )
    {
        pt[i].x = cvRound( pt[i].x * cvmGet( affine, 0, 0 ) + 
                           pt[i].y * cvmGet( affine, 0, 1 ) + 
                           cvmGet( affine, 0, 2 ) );
        pt[i].y = cvRound( pt[i].x * cvmGet( affine, 1, 0 ) + 
                           pt[i].y * cvmGet( affine, 1, 1 ) + 
                           cvmGet( affine, 1, 2 ) );
    }
    // min, max
    for( int i = 0; i < 4; i++ )
    {
        minx = MIN( pt[i].x, minx );
        miny = MIN( pt[i].y, miny );
        maxx = MAX( pt[i].x, maxx );
        maxy = MAX( pt[i].y, maxy );
    }
    // target image width and height
    int width, height;
    if( flags == CV_AFFINE_FULL )
    {
        width = maxx - minx + 1;
        height = maxy - miny + 1;
    }
    else if( flags == CV_AFFINE_SAME )
    {
        width = src->width;
        height = src->height;
        minx = miny = 0;
        maxx = src->width - 1;
        maxy = src->height - 1;
    }
    if( origin != NULL )
    {
        origin->x = minx;
        origin->y = miny;
    }
    dst = cvCreateImage( cvSize(width, height), src->depth, src->nChannels );
    cvSet( dst, color );

    // inverse affine
    CvMat* invaffine = cvCreateMat( 2, 3, affine->type );
    cvInvAffine( affine, invaffine );
    
    // loop based on image coordinates of transformed image
    for( int x = 0; x < width; x++ )
    {
        int xx = x + minx;
        for( int y = 0; y < height; y++ )
        {
            int yy = y + miny;
            int xp = cvRound( xx * cvmGet( invaffine, 0, 0 ) + 
                              yy * cvmGet( invaffine, 0, 1 ) + 
                              cvmGet( invaffine, 0, 2 ) );
            int yp = cvRound( xx * cvmGet( invaffine, 1, 0 ) + 
                              yy * cvmGet( invaffine, 1, 1 ) + 
                              cvmGet( invaffine, 1, 2 ) );
            if( xp < 0 || xp >= src->width || yp < 0 || yp >= src->height ) continue;
            for( int ch = 0; ch < src->nChannels; ch++ )
            {
                dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                    = src->imageData[src->widthStep * yp + xp * src->nChannels + ch];
            }
        }
    }
    cvReleaseMat( &invaffine );
    __END__;
    return dst;

}


#endif
