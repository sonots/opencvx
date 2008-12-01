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
#ifndef CV_CREATEAFFINE_INCLUDED
#define CV_CREATEAFFINE_INCLUDED

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
#include <math.h>
using namespace std;

CVAPI(void) cvCreateAffine( CvMat* affine, CvRect rect = cvRect(0,0,1,1), double rotate = 0, CvPoint shear = cvPoint(0,0) );

/**
// Create a affine transform matrix
//
// @param CvMat* affine                   The 2 x 3 CV_32FC1|CV_64FC1 affine matrix to be created
// @param CvRect [rect = cvRect(0,0,1,1)] The translation (x, y) and scaling (width, height) parameter
// @param double [rotate = 0]             The Rotation parameter in degree
// @param CvPoint [shear = cvPoint(0,0)]  The shear deformation parameter shx and shy
// @return void
// @Book{Hartley2004,
//    author = "Hartley, R.~I. and Zisserman, A.",
//    title = "Multiple View Geometry in Computer Vision",
//    edition = "Second",
//    year = "2004",
//    publisher = "Cambridge University Press, ISBN: 0521540518"
// } 
*/
CVAPI(void) cvCreateAffine( CvMat* affine, CvRect rect, double rotate, CvPoint shear )
{
    CV_FUNCNAME( "cvCreateAffine" );
    __BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );

    // [ a b tx; c d ty]
    // [tx; ty] = Translation
    // (1) [a b; c d] = Rotation * Shear(-phi) * Scale * Shear(phi)
    // or
    // (2) [a b; c d] = Rotation * [sx shx; shy sy]

    CvMat* RotationMatrix = cvCreateMat( 2, 3, CV_32FC1 );
    CvMat* Rotation       = cvCreateMat( 2, 2, CV_32FC1 );
    CvMat* Scale          = cvCreateMat( 2, 2, CV_32FC1 );

    // [tx; ty]
    // Translation
    cvmSet( affine, 0, 2, rect.x );
    cvmSet( affine, 1, 2, rect.y );
    // [a b; c d] = Rotation * [sx shx; shy sy]
    // Rotation
    cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), rotate, 1.0, RotationMatrix );
    cvmSet( Rotation, 0, 0, cvmGet( RotationMatrix, 0, 0 ) );
    cvmSet( Rotation, 0, 1, cvmGet( RotationMatrix, 0, 1 ) );
    cvmSet( Rotation, 1, 0, cvmGet( RotationMatrix, 1, 0 ) );
    cvmSet( Rotation, 1, 1, cvmGet( RotationMatrix, 1, 1 ) );
    // [sx shx; shy sy]
    cvZero( Scale );
    cvmSet( Scale, 0, 0, rect.width );
    cvmSet( Scale, 1, 1, rect.height );
    cvmSet( Scale, 0, 1, shear.x );
    cvmSet( Scale, 1, 0, shear.y );
    // [a b; c d] = Rotation * [sx shx; shy sy]
    cvMatMul( Rotation, Scale, Scale );
    cvmSet( affine, 0, 0, cvmGet( Scale, 0, 0 ) );
    cvmSet( affine, 0, 1, cvmGet( Scale, 0, 1 ) );
    cvmSet( affine, 1, 0, cvmGet( Scale, 1, 0 ) );
    cvmSet( affine, 1, 1, cvmGet( Scale, 1, 1 ) );

    cvReleaseMat( &RotationMatrix );
    cvReleaseMat( &Rotation );
    cvReleaseMat( &Scale );
    __END__;
}

#ifdef _MSC_VER // MS Visual Studio
#pragma warning(pop)
#endif

#endif
