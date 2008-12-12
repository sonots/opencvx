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
#ifndef CV_GMMPDF_INCLUDED
#define CV_GMMPDF_INCLUDED


#include "cv.h"
#include "cvaux.h"
#include <stdio.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvgausspdf.h"

CVAPI(void) cvMatGmmPdf( const CvMat* samples, const CvMat* means, CvMat** covs, const CvMat* weights, CvMat* probs, bool normalize = false );
CVAPI(double) cvGmmPdf( const CvMat* sample, const CvMat* means, CvMat** covs, const CvMat* weights, CvMat* probs = NULL, bool normalize = false );

/**
// cvMatGmmPdf - compute gaussian mixture pdf for a set of sample vectors
//
// Example)
// @code
//    const int D = 2;
//    const int N = 3;
//    const int K = 2;
//
//    double vs[] = { 3, 4, 5,
//                    3, 4, 5 }; // col vectors
//    double ms[] = { 3, 5, 
//                    3, 5 }; // col vectors
//    double cs0[] = { 1, 0,
//                     0, 1 };
//    double cs1[] = { 1, 0.1,
//                     0.1, 1 };
//    double ws[] = { 0.5, 0.5 };
//
//    CvMat vecs = cvMat(D, N, CV_64FC1, vs);
//    CvMat means = cvMat(D, K, CV_64FC1, ms);
//    CvMat **covs = (CvMat**)cvAlloc( K * sizeof(*covs) );
//    covs[0] = &cvMat(D, D, CV_64FC1, cs0);
//    covs[1] = &cvMat(D, D, CV_64FC1, cs0);
//    CvMat weights = cvMat( 1, K, CV_64FC1, ws);
//    CvMat *probs = cvCreateMat(K, N, CV_64FC1);
//    cvMatGmmPdf( &vecs, &means, covs, &weights, probs, false);
//    cvMatPrint( probs );
//    cvReleaseMat( &probs );
//    cvFree( &covs );
// @endcode
//
// @param samples   D x N data vector (Note: not N x D for clearness of matrix operation)
// @param means     D x K mean vector
// @param covs      (D x D) x K covariance matrix for each cluster
// @param weights   1 x K weights
// @param probs     K x N or 1 x N computed probabilites
// @param [normalize = false] Compute normalization term or not
// @uses cvMatGaussPdf
*/
void cvMatGmmPdf( const CvMat* samples, const CvMat* means, CvMat** covs, const CvMat* weights, CvMat* probs, bool normalize )
{
    int D = samples->rows;
    int N = samples->cols;
    int K = means->cols;
    int type = samples->type;
    CV_FUNCNAME( "cvMatGmmPdf" ); // error handling
    __BEGIN__;
    CV_ASSERT( CV_IS_MAT(samples) );
    CV_ASSERT( CV_IS_MAT(means) );
    for( int k = 0; k < K; k++ )
        CV_ASSERT( CV_IS_MAT(covs[k]) );
    CV_ASSERT( CV_IS_MAT(weights) );
    CV_ASSERT( CV_IS_MAT(probs) );
    CV_ASSERT( D == means->rows ); 
    for( int k = 0; k < K; k++ )
        CV_ASSERT( D == covs[k]->rows && D == covs[k]->cols ); // D x D
    CV_ASSERT( 1 == weights->rows && K == weights->cols ); // 1 x K
    CV_ASSERT( ( 1 == probs->rows || K == probs->rows ) && N == probs->cols ); // 1 x N or K x N

    CvMat *mean = cvCreateMat( D, 1, type );
    const CvMat *cov;
    CvMat *_probs = cvCreateMat( 1, N, type );
    cvZero( probs );
    for( int k = 0; k < K; k++ )
    {
        cvGetCol( means, mean, k );
        cov = covs[k];
        cvMatGaussPdf( samples, mean, cov, _probs, normalize );
        cvConvertScale( _probs, _probs, cvmGet( weights, 0, k ) );
        if( 1 == probs->rows )
        {
            cvAdd( probs, _probs, probs );
        }
        else
        {
            for( int n = 0; n < N; n++ )
            {
                cvmSet( probs, k, n, cvmGet( _probs, 0, n ) );
            }
        }
    }

    cvReleaseMat( &mean );
    cvReleaseMat( &_probs );

    __END__;
}

/**
// cvGmmPdf - compute gaussian mixture pdf
//
// @param sample    D x 1 sample vector
// @param means     D x K mean vector for each cluster
// @param covs      (D x D) x K covariance matrix for each cluster
// @param weights   1 x K weights
// @param probs     K x 1 probabilities for each cluster if want
// @param [normalize = false] use normalization term or not
// @return double prob
// @uses cvMatGmmPdf
*/
double cvGmmPdf( const CvMat* sample, const CvMat* means, CvMat** covs, const CvMat* weights, CvMat* probs, bool normalize )
{
    double prob;
    CvMat* _probs;
    int K = means->cols;
    if( probs ) 
        _probs = probs;
    else
        _probs = cvCreateMat( K, 1, sample->type );

    cvMatGmmPdf( sample, means, covs, weights, _probs, normalize );
    prob = cvSum( _probs ).val[0];

    if( !probs )
        cvReleaseMat( &probs );
    return prob;
}


#endif
