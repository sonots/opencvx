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

#include "cvcreateaffine.h"

CVAPI(void) cvCropImageROI( IplImage* img, IplImage* dst, CvRect rect, double rotate = 0, CvPoint shear = cvPoint(0,0) );
CV_INLINE void cvShowCroppedImage( const char* w_name, IplImage* orig, const CvRect rect, double rotate = 0, CvPoint shear = cvPoint(0,0) );

/**
// Crop image with rotated and sheared rectangle
//
// @param IplImage* img          The target image
// @param IplImage* dst          The cropped image
//    IplImage* dst = cvCreateImage( cvSize( rect.width, rect.height ), img->depth, img->nChannels );
// @param CvRect    rect         The translation (x, y) and scaling (width, height) parameter or the rectangle region
// @param double    [rotate = 0] The Rotation parameter in degree
// @param CvPoint   [shear = cvPoint(0,0)]
//                               The shear deformation parameter shx and shy
// @return void
*/
CVAPI(void) cvCropImageROI( IplImage* img, IplImage* dst, CvRect rect, double rotate, CvPoint shear )
{
    CV_FUNCNAME( "cvCropImageROI" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( dst->width == rect.width );
    CV_ASSERT( dst->height == rect.height );


    if( rotate == 0 && shear.x == 0 && shear.y == 0 && 
        rect.x >= 0 && rect.y >= 0 && 
        rect.x + rect.width < img->width && rect.y + rect.height < img->height )
    {
        cvSetImageROI( img, rect );
        cvCopy( img, dst );
        cvResetImageROI( img );                
    }
    else if( shear.x == 0 && shear.y == 0 )
    {
        int x, y, ch, xp, yp;
        CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), rotate, 1.0, R );
        cvZero( dst );

        for( x = 0; x < rect.width; x++ )
        {
            for( y = 0; y < rect.height; y++ )
            {
                xp = (int)( cvmGet( R, 0, 0 ) * x + cvmGet( R, 0, 1) * y ) + rect.x;
                yp = (int)( cvmGet( R, 1, 0 ) * x + cvmGet( R, 1, 1) * y ) + rect.y;
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                        = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
                }
            }
        }
        cvReleaseMat( &R );
    }
    else
    {
        int x, y, ch, xp, yp;
        CvMat* affine = cvCreateMat( 2, 3, CV_32FC1 );
        CvMat* xy     = cvCreateMat( 3, 1, CV_32FC1 );
        CvMat* xyp    = cvCreateMat( 2, 1, CV_32FC1 );
        cvCreateAffine( affine, rect, rotate, shear );
        cvmSet( xy, 2, 0, 1.0 );
        cvZero( dst );

        for( x = 0; x < rect.width; x++ )
        {
            cvmSet( xy, 0, 0, x / (double) rect.width );
            for( y = 0; y < rect.height; y++ )
            {
                cvmSet( xy, 1, 0, y / (double) rect.height );
                cvMatMul( affine, xy, xyp );
                xp = (int)cvmGet( xyp, 0, 0 );
                yp = (int)cvmGet( xyp, 1, 0 );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                        = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
                }
            }
        }
        cvReleaseMat( &affine );
        cvReleaseMat( &xy );
        cvReleaseMat( &xyp );
    }
    __END__;
}

/**
// Show Cropped Image
//
// @param char*     w_name          Window name
// @param IplImage* orig            Image to be cropped
// @param CvRect    rect            Rectangle region to crop
// @param double    [rotate = 0]    Rotation degree of rectangle
// @param CvPoint   [shear = cvPoint(0,0)]
//                                  The shear deformation parameter shx and shy
// @return void
// @uses cvCropImageROI
*/
CV_INLINE void cvShowCroppedImage( const char* w_name, IplImage* orig, const CvRect rect, double rotate, CvPoint shear )
{
    if( rect.width <= 0 || rect.height <= 0 ) return;
    IplImage* crop = cvCreateImage( cvSize( rect.width, rect.height ), orig->depth, orig->nChannels );
    cvCropImageROI( orig, crop, rect, rotate, shear );
    cvShowImage( w_name, crop );
    cvReleaseImage( &crop );
}

#ifdef _MSC_VER // MS Visual Studio
#pragma warning(pop)
#endif

#endif
