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
#ifndef CV_WEIGHTEDMEAN_INCLUDED
#define CV_WEIGHTEDMEAN_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include <float.h>
#include <math.h>

// name cvMean is already in compat.h
CvScalar cvWeightedMean( const CvArr *arr, const CvArr *weight = NULL );

/**
 * cvWeightedMean
 *
 * Compute mean of elements of an array for each channel independently
 *
 * @param  arr       array having values. 
 * @param  [weight = NULL]
 *                   weight to compute mean. The deafult is 1/num (uniform)
 *                   The size and number of channels must be same with arr.
 * @return mean
 */
CvScalar cvWeightedMean( const CvArr *arr, const CvArr *weight )
{
    CvMat* mat, matstub;
    CvMat* wmat, wmatstub;
    CvScalar mean = cvScalar(0,0,0,0);
    int row, col, ch;
    int nChannels;
    CvScalar elem;
    CvScalar welem;
    CV_FUNCNAME( "cvWeightedMean" );
    __BEGIN__;

    if( weight == NULL )
    {
        return cvAvg( arr );
    }

    mat = (CvMat*)arr;
    if( !CV_IS_MAT(mat) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub ) );
    }
    wmat = (CvMat*)weight;
    if( !CV_IS_MAT(wmat) )
    {
        CV_CALL( wmat = cvGetMat( wmat, &wmatstub ) );
    }
    CV_ASSERT( mat->rows == wmat->rows && 
               mat->cols == wmat->cols &&
               CV_MAT_CN(mat->type) == CV_MAT_CN(wmat->type) );

    // CvMul require mat->type == wmat->type
    // cvMul -> return cvSum
    nChannels = CV_MAT_CN(mat->type);
    for( row = 0; row < mat->rows; row++ )
    {
        for( col = 0; col < mat->cols; col++ )
        {
            elem = cvGet2D( mat, row, col );
            welem = cvGet2D( wmat, row, col );
            for( ch = 0; ch < nChannels; ch++ )
            {
                mean.val[ch] += elem.val[ch] * welem.val[ch];
            }
        }
    }
    __END__;
    return mean;
}


#endif
