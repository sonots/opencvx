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
#ifndef CV_CROPIMAGEROI_INCLUDED
#define CV_CROPIMAGEROI_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvrect32f.h"
#include "cvcreateaffine.h"
#include "cvcreateaffineimage.h"

CVAPI(void) cvPutImageROI( const IplImage* src,
                           IplImage* dst,
                           CvRect32f rect32f = cvRect32f(0,0,1,1,0),
                           CvPoint2D32f shear = cvPoint2D32f(0,0) );

/**
 * Put a source image on the specified region on a target image 
 *
 * @param src          The source image
 * @param dst          The target image
 * @param [rect32f = cvRect32f(0,0,1,1,0)]
 *                     The rectangle region (x,y,width,height) to put
 *                     the rotation angle in degree where roation center is (x,y)
 * @param [shear = cvPoint2D32f(0,0)]
 *                     The shear deformation parameter shx and shy
 * @return void
 */
CVAPI(void) cvPutImageROI( const IplImage* src,
                           IplImage* dst, 
                           CvRect32f rect32f, 
                           CvPoint2D32f shear )
{
    CvRect rect = cvRectFromRect32f( rect32f );
    float tx, ty, sx, sy, angle;
    CV_FUNCNAME( "cvPutImageROI" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( src->depth == dst->depth );
    CV_ASSERT( src->nChannels == dst->nChannels );

    CvMat* affine = cvCreateMat( 2, 3, CV_32FC1 );
    tx = 0;
    ty = 0;
    sx = rect32f.width / src->width;
    sy = rect32f.height / src->height;
    angle = rect32f.angle;
    cvCreateAffine( affine, cvRect32f( tx, ty, sx, sy, angle ), shear );

    CvPoint origin;
    IplImage* img = cvCreateAffineImage( src, affine, CV_AFFINE_FULL, &origin, CV_RGB(0,0,0) );
    IplImage* mask  = cvCreateAffineMask( src, affine, CV_AFFINE_FULL );
    for( int xp = 0; xp < img->width; xp++ )
    {
        int x = xp + rect.x + origin.x;
        for( int yp = 0; yp < img->height; yp++ )
        {
            int y = yp + rect.y + origin.y;
            if( x < 0 || x >= dst->width || y < 0 || y >= dst->height ) continue;
            if( CV_IMAGE_ELEM( mask, uchar, yp, xp ) == 0 ) continue;
            for( int ch = 0; ch < img->nChannels; ch++ )
            {
                dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                    = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
            }
        }
    }
    cvReleaseMat( &affine );
    cvReleaseImage( &img );
    cvReleaseImage( &mask );
    __END__;
}

#endif
