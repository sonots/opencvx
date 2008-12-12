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
#ifndef CV_ANGLEMEAN_INCLUDED
#define CV_ANGLEMEAN_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include <float.h>
#include <math.h>

CVAPI(CvScalar) cvAngleMean( const CvArr *arr, 
                             const CvArr *weight = NULL, 
                             double wrap = 360 );

/**
 * Compute mean of angle elements of an array
 *
 * Compute mean of angle elements of an array (each channel independently). 
 * There is a fact that 0 degrees and 360 degrees are identical angles, 
 * so that for example 180 degrees is not a sensible mean of 2 degrees and 
 * 358 degrees. This function computes means of angles by computing mean 
 * of cosine and sine and finally taking arc tangent. 
 *
 * @param  arr              array
 * @param  [weight = NULL]  weight to compute mean. 
 *                          The deafult is 1/num (uniform).
 *                          The size must be same with arr. 
 * @param  [wrap = 360]     The unit of wrapping around.
 *                          The defeault is 360 as angle.
 * @return angle mean for each channel
 */
CvScalar cvAngleMean( const CvArr *arr, 
                      const CvArr *weight, 
                      double wrap )
{
    CvMat* mat, matstub;
    CvMat* wmat = NULL, wmatstub;
    CvScalar mean = cvScalar(0,0,0,0);
    CV_FUNCNAME( "cvMean" );
    __BEGIN__;

    mat = (CvMat*)arr;
    if( !CV_IS_MAT(mat) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub ) );
    }
    if( weight != NULL )
    {
        wmat = (CvMat*)weight;
        if( !CV_IS_MAT(wmat) )
        {
            CV_CALL( wmat = cvGetMat( wmat, &wmatstub ) );
        }
        CV_ASSERT( 
            mat->rows == wmat->rows && 
            mat->cols == wmat->cols &&
            CV_MAT_CN(mat->type) == CV_MAT_CN(wmat->type) 
        );
    }
    int row, col, ch;
    int nChannels = CV_MAT_CN(mat->type);
    CvScalar elem;
    CvScalar mean_cos = cvScalar(0,0,0,0);
    CvScalar mean_sin = cvScalar(0,0,0,0);
    CvScalar welem;
    if( wmat == NULL ) // uniform
    {
        double w = 1.0 / (double)mat->rows * (double)mat->cols;
        welem = cvScalar( w, w, w, w );
    }
    for( row = 0; row < mat->rows; row++ )
    {
        for( col = 0; col < mat->cols; col++ )
        {
            elem = cvGet2D( mat, row, col );
            if( wmat != NULL ) welem = cvGet2D( wmat, row, col );
            for( ch = 0; ch < nChannels; ch++ )
            {
                mean_cos.val[ch] += 
                    cos( elem.val[ch] * 2*M_PI / wrap ) * welem.val[ch];
                mean_sin.val[ch] += 
                    sin( elem.val[ch] * 2*M_PI / wrap ) * welem.val[ch];
            }
        }
    }
    for( ch = 0; ch < nChannels; ch++ )
    {
        mean.val[ch] = 
            atan( mean_sin.val[ch] / mean_cos.val[ch] ) * wrap / (2*M_PI);
    }
    __END__;
    return mean;
}


#endif
