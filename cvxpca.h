/**
// cvxpca.h
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

#ifndef CV_PCA_INCLUDED
#define CV_PCA_INCLUDED

//void cvCalcPCA( const CvArr* data, CvArr* avg,
//                CvArr* eigenvalues, CvArr* eigenvectors, int flags );
//void cvProjectPCA( const CvArr* data, const CvArr* avg,
//                   const CvArr* eigenvectors, CvArr* result );
//void cvBackProjectPCA( const CvArr* proj, const CvArr* avg,
//                       const CvArr* eigenvects, CvArr* result );
void cvPcaDiffs( const CvMat* samples, const CvMat* avg, const CvMat* eigenvalues, 
		 const CvMat* eigenvectors, CvMat* probs, 
		 bool normalize = false, bool logprob = true );

/**
% cvPcaDiffs - Distance "in" and "from" feature space [1]
%
% Synopsis
%   [p] = cvPcaDiffs(X, U, Me, Lambda, normalize, logprob, lambdapad)
%
% Description
%   This computes a distance between a point to a PCA subspace as sum of
%   distance-from-feature space (DFFS) and distance-in-feature-space
%   (DIFS). The DFFS is essentially a reconstruction error and the
%   DIFS measures (mahalanobis) distance between projected point and 
%   origin of PCA subpsace. DFFS and DIFS can be used to measure 
%   approximated likelihood Gaussian density distribution. 
%   See [1] for more details. 
%
% Inputs ([]s are optional)
%   (matrix) X        D x N matrix representing feature vectors by columns
%                     where D is the number of dimensions and N is the
%                     number of vectors.
%   (matrix) U        M x D matrix representing the principal components
%                     (eigenvectors) where M is the number of components.
%   (vector) Me       D x 1 vector representing the mean vector of X
%                     vectors.
%   (vector) Lambda   min(NT-1,D) x 1 vector representing the eigenvalues
%                     respective to the principal components axis
%                     (or variances in the principle axis) where NT is the
%                     number of training samples. 
%   (bool)   [normalize = 0]
%                     0 - Do not apply the normalization term
%                     1 - Apply the normalization term
%                     2 - Normalize so that prob sum bocomes 1 respect to N
%   (bool)   [logprob = true]
%                     Return log probabilities
%   (bool)   [lambdapad = 0]
%                     0 - Do nothing
%                     1 - Zero padding when NT-1 < D so that Lambda to be D x 1
%                     2 - Lambda is padded by power law curve fitting when
%                         to be D x 1 when NT-1 < D. 
%                     This padding should be done beforehand once when you
%                     repeatedly use. Use cvPcaLambdaFit.m. Refer [2]
%
% Outputs ([]s are optional)
%   (vector) d        1 x N vector representing distance between X(:,n) vs
%                     the pca space.
%   (vector) p        1 x N vector representing estimated likelihood 
%                     Gaussian density p(x|PCA). The Gaussian is a 
%                     continuous densitiy function so it may have a value 
%                     more than 1.0. Do normalization later. 
%

% References
%   [1] @INPROCEEDINGS{Moghaddam95probabilisticvisual,
%     author = {Baback Moghaddam and Alex Pentl},
%     title = {Probabilistic visual learning for object detection},
%     booktitle = {},
%     year = {1995},
%     pages = {786--793}
%   }
%   [2] @ARTICLE{Moghaddam02principalmanifolds,
%     author = {Baback Moghaddam},
%     title = {Principal manifolds and probabilistic subspaces for visual recognition},
%     journal = {IEEE Transactions on Pattern Analysis and Machine Intelligence},
%     year = {2002},
%     volume = {24},
%     pages = {780--788}
%
% Authors
%   Naotoshi Seo <sonots(at)sonots.com>
%
% License
%   The program is free to use for non-commercial academic purposes,
%   but for course works, you must understand what is going inside to use.
%   The program can be used, modified, or re-distributed for any purposes
%   if you or one of your group understand codes (the one must come to
%   court if court cases occur.) Please contact the authors if you are
%   interested in using the program without meeting the above conditions.
%
% Changes
%   11/18/2008  First Edition
*/
void cvMatPcaDiffs( const CvMat* samples, const CvMat* avg, const CvMat* eigenvalues, const CvMat* eigenvectors, CvMat* probs, bool normalize, bool logprob )
{
    int D = samples->rows;
    int N = samples->cols;
    int M = eigenvectors->rows;
    int type = samples->type;
    int nEig = eigenvalues->rows;
    int row, col;
    double normterm = 0;
    CvMat *proj = cvCreateMat( N, M, type );
    CvMat *subproj = cvCreateMat( 1, M, type );
    CvMat *scsubproj = cvCreateMat( 1, M, type );
    CvMat *sqrt_pLambda = cvCreateMat( M, 1, type );
    CvMat *rLambda = cvCreateMat( nEig - M, 1, type );
    CvMat *DIFS = cvCreateMat( 1, N, type );
    CvMat *DFFS = cvCreateMat( 1, N, type );
    CvMat *samples0 = cvCreateMat( D, N, type ); // mean subtracted samples
    CvMat *subsamples0 = cvCreateMat( D, 1, type );
    CvScalar rho;
    CV_FUNCNAME( "cvMatPcaDiffs" );
    __BEGIN__;
    CV_ASSERT( CV_IS_MAT(samples) );
    CV_ASSERT( CV_IS_MAT(avg) );
    CV_ASSERT( CV_IS_MAT(eigenvalues) );
    CV_ASSERT( CV_IS_MAT(eigenvectors) );
    CV_ASSERT( D == avg->rows && 1 == avg->cols );
    CV_ASSERT( D == eigenvectors->cols );
    CV_ASSERT( 1 == probs->rows && N == probs->cols );

    cvZero( DIFS );
    cvZero( DFFS );
    cvProjectPCA( samples, avg, eigenvectors, proj );

    // distance in feature space
    if( M > 0 ) {
      for( row = 0; row < M; row++ ) {
	cvmSet( sqrt_pLambda, row, 0, sqrt( cvmGet( eigenvalues, row, 0 ) ) );
      }
      for( col = 0; col < N; col++ ) {
	cvGetRow( proj, subproj, col );
	cvCopy( subproj, scsubproj );
	for( row = 0; row < M; row++ ) {
	  cvmSet( scsubproj, 0, row, cvmGet( scsubproj, 0, row ) / cvmGet( sqrt_pLambda, row, 0 ) );
	}
	cvmSet( DIFS, 0, col, pow(cvNorm( scsubproj, NULL, CV_L2 ), 2) );
      }
      if( normalize ) {
	for( row = 0; row < M; row++ ) {
	  normterm += log( cvmGet( sqrt_pLambda, row, 0 ) );
	}
	normterm += log(2*M_PI)*(M/2.0);
      }
    }

    // distance from feature space
    if( nEig > M ) {
      for( col = 0; col < N; col++ ) {
	for( row = 0; row < D; row++ ) {
	  cvmSet( samples0, row, col, cvmGet( samples, row, col ) - cvmGet( avg, row, 0 ) );
	}
      }
      for( col = 0; col < N; col++ ) {
	cvGetCol( samples0, subsamples0, col );
	cvGetRow( proj, subproj, col );
	cvmSet( DFFS, 0, col, pow(cvNorm( subsamples0, NULL, CV_L2 ),2) - pow(cvNorm( subproj, NULL, CV_L2 ),2) );
      }
      for( row = 0; row < nEig - M; row++ ) {
	cvmSet( rLambda, row, 0, cvmGet( eigenvalues, row + M, 0 ) );
      }
      rho = cvAvg( rLambda );
      for( col = 0; col < N; col++ ) {
	cvmSet( DFFS, 0, col, cvmGet( DFFS, 0, col ) / rho.val[0] );
      }
      if( normalize ) {
	normterm += log(2*M_PI*rho.val[0]) * ((nEig - M)/2.0);
      }
    }
    
    // logp sum
    for( col = 0; col < N; col++ ) {
      cvmSet( probs, 0, col, cvmGet( DIFS, 0, col ) / (-2) + cvmGet( DFFS, 0, col) / (-2) - normterm );
    }
    if( !logprob ) {
      for( col = 0; col < N; col++ ) {
	cvmSet( probs, 0, col, exp(cvmGet( probs, 0, col )) );
      }
    }
    cvReleaseMat( &proj );
    cvReleaseMat( &subproj );
    cvReleaseMat( &scsubproj );
    cvReleaseMat( &sqrt_pLambda );
    cvReleaseMat( &rLambda );
    cvReleaseMat( &DIFS );
    cvReleaseMat( &DFFS );
    cvReleaseMat( &samples0 );
    cvReleaseMat( &subsamples0 );
    __END__;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
