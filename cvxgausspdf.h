/**
// cvgausspdf.h
//
// Copyright (c) 2008, Naotoshi Seo. All rights reserved.
//
// The program is free to use for non-commercial academic purposes,
// but for course works, you must understand what is going inside to 
// use. The program can be used, modified, or re-distributed for any 
// purposes only if you or one of your group understand not only 
// programming codes but also theory and math behind (if any). 
// Please contact the authors if you are interested in using the 
// program without meeting the above conditions.
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
#include <stdio.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef CV_GAUSSPDF_INCLUDED
#define CV_GAUSSPDF_INCLUDED

/**
// cvMatGaussPdf - compute multivariate gaussian pdf for a set of sample vectors
//
// Example)
//    double vs[] = { 3, 4, 5,
//                    3, 4, 5 }; // col vectors
//    double m[] = { 5, 
//                   5 };
//    double a[] = { 1, 0,
//                   0, 1 };
//    CvMat vecs = cvMat(2, 3, CV_64FC1, vs);
//    CvMat mean = cvMat(2, 1, CV_64FC1, m);
//    CvMat cov  = cvMat(2, 2, CV_64FC1, a);
//    CvMat *probs = cvCreateMat(1, 3, CV_64FC1);
//    cvMatGaussPdf( &vecs, &mean, &cov, probs, false, false);
//    cvMatPrint( probs ); // 0.018316 0.367879 1.000000
//    cvMatGaussPdf( &vecs, &mean, &cov, probs, true, false);
//    cvMatPrint( probs ); // 0.002915 0.058550 0.159155
//    cvMatGaussPdf( &vecs, &mean, &cov, probs, false, true);
//    cvMatPrint( probs ); // -4.000000 -1.000000 -0.000000
//    cvMatGaussPdf( &vecs, &mean, &cov, probs, true, true);
//    cvMatPrint( probs ); // -5.837877 -2.837877 -1.837877
//    cvReleaseMat( &probs );
//
// @param samples   D x N data vector (Note: not N x D for clearness of matrix operation)
// @param mean      D x 1 mean vector
// @param cov       D x D covariance matrix
// @param probs     1 x N computed probabilites
// @param [normalize = false] Compute normalization term or not
// @param [logprob   = false] Log probability or not
// @return void
// @see cvCalcCovarMatrix, cvAvg
*/
void cvMatGaussPdf( const CvMat* samples, const CvMat* mean, const CvMat* cov, CvMat* probs, bool normalize = false, bool logprob = false )
{
    int D = samples->rows;
    int N = samples->cols;
    int type = samples->type;
    CV_FUNCNAME( "cvMatGaussPdf" ); // error handling
    __BEGIN__;
    CV_ASSERT( CV_IS_MAT(samples) );
    CV_ASSERT( CV_IS_MAT(mean) );
    CV_ASSERT( CV_IS_MAT(cov) );
    CV_ASSERT( CV_IS_MAT(probs) );
    CV_ASSERT( D == mean->rows && 1 == mean->cols );
    CV_ASSERT( D == cov->rows && D == cov->cols );
    CV_ASSERT( 1 == probs->rows && N == probs->cols );

    CvMat *invcov = cvCreateMat( D, D, type );
    cvInvert( cov, invcov, CV_SVD );

    CvMat *sample = cvCreateMat( D, 1, type );
    CvMat *subsample   = cvCreateMat( D, 1, type );
    CvMat *subsample_T = cvCreateMat( 1, D, type );
    CvMat *value       = cvCreateMat( 1, 1, type );
    double prob;
    for( int n = 0; n < N; n++ )
    {
        cvGetCol( samples, sample, n );

        cvSub( sample, mean, subsample );
        cvTranspose( subsample, subsample_T );
        cvMatMul( subsample_T, invcov, subsample_T );
        cvMatMul( subsample_T, subsample, value );
        prob = -0.5 * cvmGet(value, 0, 0);
        if( !logprob ) prob = exp( prob );

        cvmSet( probs, 0, n, prob );
    }
    if( normalize )
    {
        double norm = pow( 2* M_PI, D/2.0 ) * sqrt( cvDet( cov ) );
        if( logprob ) cvSubS( probs, cvScalar( log( norm ) ), probs );
        else cvConvertScale( probs, probs, 1.0 / norm );
    }
    
    cvReleaseMat( &invcov );
    cvReleaseMat( &sample );
    cvReleaseMat( &subsample );
    cvReleaseMat( &subsample_T );
    cvReleaseMat( &value );

    __END__;
}

/**
// cvGaussPdf - compute multivariate gaussian pdf
//
// Example)
//    double v[] = { 3, 
//                   3 };
//    double m[] = { 5, 
//                   5 };
//    double a[] = { 1, 0,
//                   0, 1 };
//    CvMat vec  = cvMat(2, 1, CV_64FC1, v);
//    CvMat mean = cvMat(2, 1, CV_64FC1, m);
//    CvMat cov  = cvMat(2, 2, CV_64FC1, a);
//    std::cout << cvGaussPdf( &vec, &mean, &cov, false ) << std::endl;
//
// @param sample    D x 1 data vector
// @param mean      D x 1 mean vector
// @param cov       D x D covariance matrix
// @param [normalize = false] Compute normalization term or not
// @param [logprob   = false] Log probability or not
// @return double   probability
// @see cvCalcCovarMatrix, cvAvg
// @uses cvMatGaussPdf
*/
double cvGaussPdf( const CvMat* sample, const CvMat* mean, const CvMat* cov, bool normalize = false, bool logprob = false )
{
    double prob;
    CvMat *_probs  = cvCreateMat( 1, 1, sample->type );

    cvMatGaussPdf( sample, mean, cov, _probs, normalize, logprob );
    prob = cvmGet(_probs, 0, 0);

    cvReleaseMat( &_probs );
    return prob;
}

/**
// cvMatGmmPdf - compute gaussian mixture pdf for a set of sample vectors
//
// Example)
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
//
// @param samples   D x N data vector (Note: not N x D for clearness of matrix operation)
// @param means     D x K mean vector
// @param covs      (D x D) x K covariance matrix for each cluster
// @param weights   1 x K weights
// @param probs     K x N or 1 x N computed probabilites
// @param [normalize = false] Compute normalization term or not
// @uses cvMatGaussPdf
*/
void cvMatGmmPdf( const CvMat* samples, const CvMat* means, CvMat** covs, const CvMat* weights, CvMat* probs, bool normalize = false )
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
double cvGmmPdf( const CvMat* sample, const CvMat* means, CvMat** covs, const CvMat* weights, CvMat* probs = NULL, bool normalize = false )
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

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif