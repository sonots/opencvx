/** @file */
/* Copyright (c) 2008, Naotoshi Seo. All rights reserved.
 *
 * The program is free to use for non-commercial academic purposes,
 * but for course works, you must understand what is going inside to 
 * use. The program can be used, modified, or re-distributed for any 
 * purposes only if you or one of your group understand not only 
 * programming codes but also theory and math behind (if any). 
 * Please contact the authors if you are interested in using the 
 * program without meeting the above conditions.
*/

#include "cv.h"
#include "cvaux.h"
#include <stdio.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef CV_PCAPROBDIST_INCLUDED
#define CV_PCAPROBDIST_INCLUDED
#include "cvpcadist.h"
#include "cvlogsum.h"

#ifndef NO_DOXYGEN
CVAPI(void) 
cvMatPcaProbDist( const CvMat* samples, const CvMat* avg, 
                  const CvMat* eigenvectors, double sqsigma, CvMat* probs,
                  int normalize, bool logprob );
CV_INLINE double 
cvPcaProbDist( const CvMat* sample, const CvMat* avg, 
               const CvMat* eigenvectors, double sqsigma, 
               int normalize, bool logprob );
#endif

/**
 * Distance between sample and PCA subspace, i.e, reconstruction error
 * Probabiliticlly model with Gaussian
 *
 * @param samples             D x N sample vectors
 * @param avg                 D x 1 mean vector
 * @param eigenvectors        M x D or D x M (automatically adjusted) eigen vectors
 * @param sqsigma             A scalar representing sigma^2, i.e, variances
 *                            of distribution of reconstruction errors.
 *                            Estimate in training data. 
 * @param probs               1 x N computed likelihood probabilities
 * @param normalize           Compute normalization term or not
 *                            0 - nothing
 *                            1 - normalization term
 *                            2 - normalize so that sum becomes 1.0
 * @param logprob             Log probability or not
 *
 * @see CVAPI(void) cvCalcPCA( const CvArr* data, CvArr* avg, CvArr* eigenvalues, CvArr* eigenvectors, int flags );
 * @see CVAPI(void) cvProjectPCA( const CvArr* data, const CvArr* avg, const CvArr* eigenvectors, CvArr* result );
 * @see CVAPI(void) cvBackProjectPCA( const CvArr* proj, const CvArr* avg,const CvArr* eigenvects, CvArr* result );
 * @see cvMatPcaDist
 */
CVAPI(void) 
cvMatPcaProbDist( const CvMat* samples, const CvMat* avg, 
                  const CvMat* eigenvectors, double sqsigma, CvMat* probs,
                  int normalize CV_DEFAULT(1), bool logprob CV_DEFAULT(false) )
{
    int D = samples->rows;
    int N = samples->cols;
    int type = samples->type;
    CvMat *dists = cvCreateMat( 1, N, type );
    CV_FUNCNAME( "cvMatPcaProbDist" );
    __BEGIN__;
    CV_ASSERT( CV_IS_MAT(samples) );
    CV_ASSERT( CV_IS_MAT(avg) );
    CV_ASSERT( CV_IS_MAT(eigenvectors) );
    CV_ASSERT( D == eigenvectors->rows || D == eigenvectors->cols );
    CV_ASSERT( D == avg->rows && 1 == avg->cols );
    CV_ASSERT( 1 == probs->rows && N == probs->cols );

    cvMatPcaDist( samples, avg, eigenvectors, dists );
    cvScale( dists, probs, -1/(2*sqsigma) );

    if( normalize == 1 )
    {
        cvSubS( probs, cvScalar(log(sqrt(2*M_PI*sqsigma))), probs );
    }
    else if( normalize == 2 )
    {
        CvScalar logsum = cvLogSum( probs );
        cvSubS( probs, logsum, probs );
    }

    if( !logprob ) // finally make exp
    {
        cvExp( probs, probs );
    }
    
    cvReleaseMat( &dists );
    __END__;
}

/**
 * Distance between sample and PCA subspace, i.e, reconstruction error
 * Probabiliticlly model with Gaussian
 *
 * @param sample              D x 1 feature vector
 * @param avg                 D x 1 mean vector
 * @param eigenvectors        M x D eigen vectors
 * @param sqsigma             A scalar representing sigma^2, i.e, variances
 *                            of distribution of reconstruction errors.
 *                            Estimate in training data. 
 * @param normalize           Compute normalization term or not
 *                            0 - nothing
 *                            1 - normalization term
 *                            2 - normalize so that sum becomes 1.0
 * @param logprob             Log probability or not
 * @return double
*/
CV_INLINE double 
cvPcaProbDist( const CvMat* sample, const CvMat* avg, 
               const CvMat* eigenvectors, double sqsigma, 
               int normalize CV_DEFAULT(1), bool logprob CV_DEFAULT(false) )
{
    double prob;
    CvMat *_probs  = cvCreateMat( 1, 1, sample->type );

    cvMatPcaProbDist( sample, avg, eigenvectors, sqsigma, _probs, normalize, logprob );
    prob = cvmGet( _probs, 0, 0);

    cvReleaseMat( &_probs );
    return prob;
}


#endif
