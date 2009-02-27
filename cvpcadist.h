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

#ifndef CV_PCADIST_INCLUDED
#define CV_PCADIST_INCLUDED

#ifndef NO_DOXYGEN
CVAPI(void) 
cvMatPcaDist( const CvMat* samples, const CvMat* avg, 
              const CvMat* eigenvectors, CvMat* dists );
CV_INLINE double 
cvPcaDist( const CvMat* sample, const CvMat* avg, 
           const CvMat* eigenvectors );
#endif

/**
 * Distance between sample and PCA subspace, i.e, reconstruction error
 *
 * @param samples             D x N sample vectors
 * @param avg                 D x 1 mean vector
 * @param eigenvectors        M x D or D x M (automatically adjusted) eigen vectors
 * @param dists               1 x N computed distances
 *
 * @see CVAPI(void) cvCalcPCA( const CvArr* data, CvArr* avg, CvArr* eigenvalues, CvArr* eigenvectors, int flags );
 * @see CVAPI(void) cvProjectPCA( const CvArr* data, const CvArr* avg, const CvArr* eigenvectors, CvArr* result );
 * @see CVAPI(void) cvBackProjectPCA( const CvArr* proj, const CvArr* avg,const CvArr* eigenvects, CvArr* result );
 */
CVAPI(void) 
cvMatPcaDist( const CvMat* samples, const CvMat* avg, 
              const CvMat* eigenvectors, CvMat* dists )
{
    int D = samples->rows;
    int N = samples->cols;
    int M = (eigenvectors->rows == D) ? eigenvectors->cols : eigenvectors->rows;
    int type = samples->type;
    int d, n;
    CvMat *_eigenvectors; // cvProjectPCA requires M x D vec
    CvMat *proj = cvCreateMat( N, M, type );
    CvMat *subproj, subprojhdr;
    CvMat *samples0 = cvCreateMat( D, N, type ); // mean subtracted samples
    CvMat *subsamples0, subsamples0hdr;
    CvMat *_proj;
    CV_FUNCNAME( "cvMatPcaDist" );
    __BEGIN__;
    CV_ASSERT( CV_IS_MAT(samples) );
    CV_ASSERT( CV_IS_MAT(avg) );
    CV_ASSERT( CV_IS_MAT(eigenvectors) );
    CV_ASSERT( D == eigenvectors->rows || D == eigenvectors->cols );
    CV_ASSERT( D == avg->rows && 1 == avg->cols );
    CV_ASSERT( 1 == dists->rows && N == dists->cols );
    if( D == eigenvectors->rows ) { // support transpose
        _eigenvectors = cvCreateMat( M, D, type );
        cvT( eigenvectors, _eigenvectors );
    } else {
        _eigenvectors = (CvMat*)eigenvectors;
    }
    
    //cvProjectPCA( samples, avg, _eigenvectors, proj );
    for( n = 0; n < N; n++ ) { // want mean subtracted samples for laster too
        for( d = 0; d < D; d++ ) {
            cvmSet( samples0, d, n, cvmGet( samples, d, n ) - cvmGet( avg, d, 0 ) );
        }
    }
    _proj = cvCreateMat( M, N, type );
    cvMatMul( _eigenvectors, samples0, _proj );
    cvT( _proj, proj );
    cvReleaseMat( &_proj );

    // distance from feature space
    for( n = 0; n < N; n++ ) {
        subsamples0 = cvGetCol( samples0, &subsamples0hdr, n );
        subproj = cvGetRow( proj, &subprojhdr, n );
        cvmSet( dists, 0, n, pow(cvNorm( subsamples0, NULL, CV_L2 ),2) - pow(cvNorm( subproj, NULL, CV_L2 ),2) );
    }

    cvReleaseMat( &proj );
    cvReleaseMat( &samples0 );
    if( D == eigenvectors->rows ) {
        cvReleaseMat( &_eigenvectors );
    }
    __END__;
}

/**
 * Distance between sample and PCA subspace, i.e, reconstruction error
 *
 * @param sample              D x 1 feature vector
 * @param avg                 D x 1 mean vector
 * @param eigenvectors        M x D eigen vectors
 * @return double
*/
CV_INLINE double 
cvPcaDist( const CvMat* sample, const CvMat* avg, const CvMat* eigenvalues, 
           const CvMat* eigenvectors )
{
    double prob;
    CvMat *_dists  = cvCreateMat( 1, 1, sample->type );

    cvMatPcaDist( sample, avg, eigenvectors, _dists );
    prob = cvmGet(_dists, 0, 0);

    cvReleaseMat( &_dists );
    return prob;
}


#endif
