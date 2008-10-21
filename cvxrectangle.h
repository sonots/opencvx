/**
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
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )
#pragma comment( lib, "cv.lib" )
#pragma comment( lib, "cxcore.lib" )
#pragma comment( lib, "cvaux.lib" )
#endif

#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>
#include <math.h>

#ifndef CV_RECTANGLE_INCLUDED
#define CV_RECTANGLE_INCLUDED

/**
// Create a affine transform matrix
//
// @param CvMat* affine                   The 2 x 3 CV_32FC1|CV_64FC1 affine matrix to be created
// @param CvRect [rect = cvRect(0,0,1,1)] The translation (x, y) and scaling (width, height) parameter
// @param double [rotate = 0]             The rotation parameter in degree
// @param double [shear = 0]              The shear deformation orientation parameter in degree
// @todo shear orientation would be not enough, having shear_x, shear_y is much easier to control
// @return void
// @Book{Hartley2004,
//    author = "Hartley, R.~I. and Zisserman, A.",
//    title = "Multiple View Geometry in Computer Vision",
//    edition = "Second",
//    year = "2004",
//    publisher = "Cambridge University Press, ISBN: 0521540518"
// } 
*/
CVAPI(void) cvCreateAffine( CvMat* affine, CvRect rect, double rotate = 0, double shear = 0 )
{
    CV_FUNCNAME( "cvCreateAffine" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );

    CvMat* rotatetmp = cvCreateMat( 2, 3, CV_32FC1 );
    CvMat* rotation  = cvCreateMat( 2, 2, CV_32FC1 );
    CvMat* neg_shear = cvCreateMat( 2, 2, CV_32FC1 );
    CvMat* scale     = cvCreateMat( 2, 2, CV_32FC1 );
    CvMat* pos_shear = cvCreateMat( 2, 2, CV_32FC1 );

    // [ a b tx 
    //   c d ty ]
    cvmSet( affine, 0, 2, rect.x );
    cvmSet( affine, 1, 2, rect.y );
    // [a b; c d] = Rotation * Shear(-phi) * Scale * Shear(phi)
    cvZero( scale );
    cvmSet( scale, 0, 0, rect.width );
    cvmSet( scale, 1, 1, rect.height );
    cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), rotate, 1.0, rotatetmp );
    cvmSet( rotation, 0, 0, cvmGet( rotatetmp, 0, 0 ) );
    cvmSet( rotation, 0, 1, cvmGet( rotatetmp, 0, 1 ) );
    cvmSet( rotation, 1, 0, cvmGet( rotatetmp, 1, 0 ) );
    cvmSet( rotation, 1, 1, cvmGet( rotatetmp, 1, 1 ) );
    cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), -shear, 1.0, rotatetmp );
    cvmSet( neg_shear, 0, 0, cvmGet( rotatetmp, 0, 0 ) );
    cvmSet( neg_shear, 0, 1, cvmGet( rotatetmp, 0, 1 ) );
    cvmSet( neg_shear, 1, 0, cvmGet( rotatetmp, 1, 0 ) );
    cvmSet( neg_shear, 1, 1, cvmGet( rotatetmp, 1, 1 ) );
    cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), shear, 1.0, rotatetmp );
    cvmSet( pos_shear, 0, 0, cvmGet( rotatetmp, 0, 0 ) );
    cvmSet( pos_shear, 0, 1, cvmGet( rotatetmp, 0, 1 ) );
    cvmSet( pos_shear, 1, 0, cvmGet( rotatetmp, 1, 0 ) );
    cvmSet( pos_shear, 1, 1, cvmGet( rotatetmp, 1, 1 ) );
    cvMatMul( rotation, neg_shear, neg_shear );
    cvMatMul( neg_shear, scale, scale );
    cvMatMul( scale, pos_shear, pos_shear );
    cvmSet( affine, 0, 0, cvmGet( pos_shear, 0, 0 ) );
    cvmSet( affine, 0, 1, cvmGet( pos_shear, 0, 1 ) );
    cvmSet( affine, 1, 0, cvmGet( pos_shear, 1, 0 ) );
    cvmSet( affine, 1, 1, cvmGet( pos_shear, 1, 1 ) );

    cvReleaseMat( &rotatetmp );
    cvReleaseMat( &rotation );
    cvReleaseMat( &neg_shear );
    cvReleaseMat( &scale );
    cvReleaseMat( &pos_shear );
    __END__;
}

/**
// Crop image with rotated and sheared rectangle (affine transformation of (0,0,1,1) rectangle)
//
// @param IplImage* img       The target image
// @param IplImage* dst       The cropped image
//    IplImage* dst = cvCreateImage( cvSize( rect.width, rect.height ), img->depth, img->nChannels );
// @param CvRect rect         The translation (x, y) and scaling (width, height) parameter or the rectangle region
// @param double [rotate = 0] The rotation parameter in degree
// @param double [shear = 0]  The shear deformation orientation parameter in degree
// @return void
*/
CVAPI(void) cvCropImageROI( IplImage* img, IplImage* dst, CvRect rect, double rotate = 0, double shear = 0 )
{
    CV_FUNCNAME( "cvCropImageROI" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( dst->width == rect.width );
    CV_ASSERT( dst->height == rect.height );


    if( rotate == 0 && shear == 0 && 
        rect.x >= 0 && rect.y >= 0 && 
        rect.x + rect.width < img->width && rect.y + rect.height < img->height )
    {
        cvSetImageROI( img, rect );
        cvCopy( img, dst );
        cvResetImageROI( img );                
    }
    else
    {
        int x, y, z, xp, yp;
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
                for( z = 0; z < img->nChannels; z++ )
                {
                    dst->imageData[dst->widthStep * y + x * dst->nChannels + z]
                    = img->imageData[img->widthStep * yp + xp * img->nChannels + z];
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
// Draw an rotated and sheared rectangle (affine transformation of (0,0,1,1) rectangle)
//
// @param IplImage* img       The image to be drawn rectangle
// @param CvRect rect         The translation (x, y) and scaling (width, height) parameter or the rectangle region
// @param double [rotate = 0] The rotation parameter in degree
// @param double [shear = 0]  The shear deformation orientation parameter in degree
// @param CvScalar  [color  = CV_RGB(255, 255, 0)] 
//                                  Line color (RGB) or brightness (grayscale image). 
// @param int       [thickness = 1] Thickness of lines that make up the rectangle. Negative values, e.g. CV_FILLED, make the function to draw a filled rectangle. 
// @param int       [line_type = 8] Type of the line, see cvLine description. 
// @param int       [shift = 0]     Number of fractional bits in the point coordinates. 
// @todo thickness, line_type, and shift are available only when rotate == 0 && shear == 0 currently. 
// @return void
*/
CVAPI(void) cvDrawRectangle( IplImage* img, CvRect rect, double rotate = 0, double shear = 0, 
                            CvScalar color = CV_RGB(255, 255, 255), int thickness = 1, int line_type = 8, int shift = 0)
{
    CV_FUNCNAME( "cvDrawRectangle" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );

    if( rotate == 0 && shear == 0 )
    {
        CvPoint pt1 = cvPoint( rect.x, rect.y );
        CvPoint pt2 = cvPoint( rect.x + rect.width, rect.y + rect.height );
        cvRectangle( img, pt1, pt2, color, thickness, line_type, shift );
    }
    else
    {
        int x, y, z, xp, yp;
        CvMat* affine = cvCreateMat( 2, 3, CV_32FC1 );
        CvMat* xy     = cvCreateMat( 3, 1, CV_32FC1 );
        CvMat* xyp    = cvCreateMat( 2, 1, CV_32FC1 );
        cvmSet( xy, 2, 0, 1.0 );
        cvCreateAffine( affine, rect, rotate, shear );

        for( x = 0; x < rect.width; x++ )
        {
            cvmSet( xy, 0, 0, x / (double) rect.width );
            for( y = 0; y < rect.height; y += max(1, rect.height - 1) )
            {
                cvmSet( xy, 1, 0, y / (double) rect.height );
                cvMatMul( affine, xy, xyp );
                xp = (int)cvmGet( xyp, 0, 0 );
                yp = (int)cvmGet( xyp, 1, 0 );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( z = 0; z < img->nChannels; z++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + z] = (char)color.val[z];
                }
            }
        }
        for( y = 0; y < rect.height; y++ )
        {
            cvmSet( xy, 1, 0, y / (double) rect.height );
            for( x = 0; x < rect.width; x += max( 1, rect.width - 1) )
            {
                cvmSet( xy, 0, 0, x / (double) rect.width );
                cvMatMul( affine, xy, xyp );
                xp = (int)cvmGet( xyp, 0, 0 );
                yp = (int)cvmGet( xyp, 1, 0 );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( z = 0; z < img->nChannels; z++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + z] = (char)color.val[z];
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
// Compute Norm between two points
//
// @param CvPoint p1              A point 1
// @param CVPoint p2              A point 2
// @param int [norm_type = CV_L2] CV_L2 to compute L2 norm (euclidean distance)
//                                CV_L1 to compute L1 norm (abs)
// @return double
*/
CV_INLINE double cvPointNorm( CvPoint p1, CvPoint p2, int norm_type = CV_L2 )
{
    if( norm_type == CV_L1 )
        return abs( p2.x - p1.x ) + abs( p2.y - p1.y );
    else
        return sqrt( pow( (double)p2.x - p1.x, 2 ) + pow( (double)p2.y - p1.y, 2 ) );
}

/**
// Print cvRect parameters
//
// @param CvRect rect
// @return void
*/
CV_INLINE void cvPrintRect( const CvRect &rect )
{
    printf( "%d %d %d %d\n", rect.x, rect.y, rect.width, rect.height );
}

/**
// Show Image and Rectangle
//
// @param char*     w_name          Window name
// @param IplImage* img             Image to be shown
// @param CvRect    rect            Rectangle to be shown
// @param double    [rotate = 0]    Rotation degree of rectangle
// @param double    [shear  = 0]    Shear deformation orientation in degree of rectangle
// @param CvScalar  [color  = CV_RGB(255, 255, 0)] 
//                                  Line color (RGB) or brightness (grayscale image). 
// @todo: Below parameters are available only when rotate == 0 && shear == 0 currently. 
// @param int       [thickness = 1] Thickness of lines that make up the rectangle. Negative values, e.g. CV_FILLED, make the function to draw a filled rectangle. 
// @param int       [line_type = 8] Type of the line, see cvLine description. 
// @param int       [shift = 0]     Number of fractional bits in the point coordinates. 
// @return void
// @uses cvDrawRectangle
*/
CV_INLINE void cvShowImageAndRectangle( const char* w_name, const IplImage* img, const CvRect& rect, double rotate = 0, double shear = 0,
                                       CvScalar color = CV_RGB(255, 255, 0), int thickness = 1, int line_type = 8, int shift = 0)
{
    if( rect.width <= 0 || rect.height <= 0 ) { cvShowImage( w_name, img ); return; }
    IplImage* clone = cvCloneImage( img );
    cvDrawRectangle( clone, rect, rotate, shear, color, thickness, line_type, shift );
    cvShowImage( w_name, clone );
    cvReleaseImage( &clone );
}

/**
// Show Cropped Image
//
// @param char*     w_name          Window name
// @param IplImage* orig            Image to be cropped
// @param CvRect    rect            Rectangle region to crop
// @param double    [rotate = 0]    Rotation degree of rectangle
// @param double    [shear  = 0]    Shear deformation orientation in degree of rectangle
// @return void
// @uses cvCropImageROI
*/
CV_INLINE void cvShowCroppedImage( const char* w_name, IplImage* orig, const CvRect rect, double rotate = 0, double shear = 0 )
{
    if( rect.width <= 0 || rect.height <= 0 ) return;
    IplImage* crop = cvCreateImage( cvSize( rect.width, rect.height ), orig->depth, orig->nChannels );
    cvCropImageROI( orig, crop, rect, rotate, shear );
    cvShowImage( w_name, crop );
    cvReleaseImage( &crop );
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
#pragma warning( pop )
#endif

#endif