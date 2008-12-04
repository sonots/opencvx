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
#ifndef CV_PARTICLE_INCLUDED
#define CV_PARTICLE_INCLUDED

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

#include <time.h>
#include "cvsetrow.h"
#include "cvsetcol.h"
#include "cvlogsum.h"
#include "cvdrawrectangle.h"
#include "cvconvrect.h"

/******************************* Structures **********************************/

typedef struct CvParticle {
    // config
    int num_states;    // Number of tracking states, e.g., 4 if x, y, width, height
    int num_observes;  // Number of observation models, e.g., 2 if color model and shape model
    int num_particles; // Number of particles
    bool logprob;      // probs are log probabilities
    // transition
    CvMat* dynamics;   // num_states x (num_states + 1). +1 for noise. Dynamics model.
    CvRNG  rng;        // Random seed
    CvMat* std;        // num_states x 1. Standard deviation for gaussian noise
    CvMat* bound;      // num_states x 3 (lowerbound, upperbound, circular flag 0 or 1)
                       // Set lowerbound == upperbound to express no bound
    // particle states
    CvMat* particles;  // num_states x num_particles. linked with probs. 
    CvMat* probs;      // num_observes x num_particles. linked with particles.
    CvMat* particle_probs; // 1 x num_particles. marginalization respect to observation models
    CvMat* observe_probs;  // num_observes x 1.  marginalization respect to tracking states
} CvParticle;

/**************************** Function Prototypes ****************************/

CvParticle* cvCreateParticle( int num_states, int num_observes, int num_particles, bool logprob = false );
void cvParticleSetDynamics( CvParticle* p, const CvMat* dynamics );
void cvParticleSetNoise( CvParticle* p, CvRNG rng, const CvMat* std );
void cvParticleSetBound( CvParticle* p );
void cvParticleInit( CvParticle* p, const CvParticle* init = NULL );
void cvReleaseParticle( CvParticle** p );

void cvParticleUpdateByTime( CvParticle* p ); // syntax sugar
void cvParticleResample( CvParticle* p );
void cvParticleTransition( CvParticle* p );

void _cvParticleMarginalize( CvParticle* p );
void _cvParticleNormalize( CvParticle* p );
int  _cvParticleMaxParticle( const CvParticle* p );
void _cvParticleBound( CvParticle* p );

void cvParticlePrint( const CvParticle* p, int particle_id );
void cvParticleDrawRectangle( const CvParticle* p, IplImage* frame, CvScalar color, 
                              int pid = -1, int rectform = CV_RECT_NORMAL ); // CV_RECT_CENTER
CvMat* cvParticleGetParticle( const CvParticle* p, int particle_id );

/*************************** Function Definitions ****************************/

/**
 * Get a particle state
 *
 * Do not cvReleaseMat
 *
 * @param particle
 * @param particle_id 
 * @param CvMat*       num_states x 1 matrix
 */
CvMat* cvParticleGetParticle( const CvParticle* p, int particle_id )
{
    CvMat statehdr;
    return cvGetCol( p->particles, &statehdr, particle_id );
}

/**
 * Draw a particle or particles by rectangle on given image
 *
 * available only when first 5 states are (x,y,width,height) and rotation angle in degree
 *
 * @param particle
 * @param img        image to be drawn
 * @param color      color of rectangle
 * @param [pid = -1] particle id. If -1, all particles are drawn
 */
void cvParticleDrawRectangle( const CvParticle* p, IplImage* img, CvScalar color, int pid, int rectform )
{
    int i = 0;
    if( pid == -1 ) {
        for( i = 0; i < p->num_particles; i++ ) {
            cvParticleDrawRectangle( p, img, color, i, rectform );
        }
    } else {
        CvRect rect;
        double rotate;
        rect.x = cvRound( cvmGet( p->particles, 0, pid ) );
        rect.y = cvRound( cvmGet( p->particles, 1, pid ) );
        rect.width = cvRound( cvmGet( p->particles, 2, pid ) );
        rect.height = cvRound( cvmGet( p->particles, 3, pid ) );
        rotate = cvmGet( p->particles, 4, pid );
        if( rectform != CV_RECT_NORMAL) {
            rect = cvConvRect( rect, rotate, rectform, CV_RECT_NORMAL );
        }
        cvDrawRectangle( img, rect, rotate, cvPoint(0,0), color );
    }
}

/**
 * Print states of a particle
 *
 * @param particle
 * @param particle_id
 */
void cvParticlePrint( const CvParticle*p, int particle_id )
{
    int i, k = 0;
    printf( "Particle %d\n", particle_id );
    printf( " State" );
    /*
    for( i = 0; i < p->num_states; i++ )
    {
        printf( "%6d ", i );
    }
    printf( "\n" );
    printf( "      " );*/
    for( i = 0; i < p->num_states; i++ )
    {
        printf( "%6.1f ", cvmGet( p->particles, i, particle_id ) );
    }
    printf( "\n" );

    printf( " Prob " );
    /*
    for( i = 0; i < p->num_observes; i++ )
    {
        printf( "%6d ", i );
    }
    printf( "\n" );
    printf( "      " );*/
    for( i = 0; i < p->num_observes; i++ )
    {
        printf( "%6.1f ", cvmGet( p->probs, i, particle_id ) );
    }
    printf( "\n" );
    fflush( stdout );
}

/**
 * Estimates subsequent model state
 * Call after observation mesurements
 *
 * syntax sugar to call
 *   cvParticleResample
 *   cvParticleTransition
 *
 * @param particle
 */
void cvParticleUpdateByTime( CvParticle* p )
{
    // measure likeli
    cvParticleResample( p );
    cvParticleTransition( p );
}

/**
 * Re-samples a set of particles according to their probs to produce a
 * new set of unweighted particles
 *
 * Simply copy, not uniform randomly selects
 *
 * @param particle
 */
void cvParticleResample( CvParticle* p )
{
    int i, j, np, k = 0;
    CvMat* particle, hdr;
    CvMat* new_particles = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    double prob;
    int max_loc;

    _cvParticleMarginalize( p );
    _cvParticleNormalize( p );

    k = 0;
    for( i = 0; i < p->num_particles; i++ )
    {
        particle = cvGetCol( p->particles, &hdr, i );
        prob = cvmGet( p->particle_probs, 0, i );
        prob = p->logprob ? exp( prob ) : prob;
        np = cvRound( prob * p->num_particles );
        for( j = 0; j < np; j++ )
        {
            cvSetCol( particle, new_particles, k++ );
            if( k == p->num_particles )
                goto exit;
        }
    }

    max_loc = _cvParticleMaxParticle( p );
    particle = cvGetCol( p->particles, &hdr, max_loc );
    while( k < p->num_particles )
        cvSetCol( particle, new_particles, k++ );

exit:
    cvReleaseMat( &p->particles );
    p->particles = new_particles;
}

/**
 * Get id of the most probable particle
 *
 * @param particle
 * @return int
 */
int _cvParticleMaxParticle( const CvParticle* p )
{
    double minval, maxval;
    CvPoint min_loc, max_loc;
    cvMinMaxLoc( p->particle_probs, &minval, &maxval, &min_loc, &max_loc );
    return max_loc.x;
}

/**
 * Create particle_probs, and observe_probs by marginalizing
 *
 * @param particle
 * @todo priors
 */
void _cvParticleMarginalize( CvParticle* p )
{
    if( p->logprob )
    {
        int np, no;
        CvScalar logsum;
        CvMat *particle_prob, *observe_prob, hdr;
        // number of particles of the same state represents priors
        for( np = 0; np < p->num_particles; np++ )
        {
            particle_prob = cvGetCol( p->probs, &hdr, np );
            logsum = cvLogSum( particle_prob );
            cvmSet( p->particle_probs, 0, np, logsum.val[0] );
        }
        // @todo: priors
        for( no = 0; no < p->num_observes; no++ )
        {
            observe_prob = cvGetRow( p->probs, &hdr, no );
            logsum = cvLogSum( particle_prob );
            cvmSet( p->observe_probs, no, 0, logsum.val[0] );
        }
    }
    else
    {
        // number of particles of the same state represents priors
        cvReduce( p->probs, p->particle_probs, -1, CV_REDUCE_SUM );
        // @todo: priors
        cvReduce( p->probs, p->observe_probs, -1, CV_REDUCE_SUM );
    }
}

/**
 * Normalize particle_probs and observe_probs so they sum to 1.0
 *
 * @param particle
 */
void _cvParticleNormalize( CvParticle* p )
{
    // normalize particle_probs
    if( p->logprob )
    {
        CvScalar logsum = cvLogSum( p->particle_probs );
        cvSubS( p->particle_probs, logsum, p->particle_probs );
    } 
    else
    {
        CvScalar sum = cvSum( p->particle_probs );
        cvScale( p->particle_probs, p->particle_probs, 1.0 / sum.val[0] );
    }

    // normalize observe_probs
    if( p->logprob )
    {
        CvScalar logsum = cvLogSum( p->observe_probs );
        cvSubS( p->observe_probs, logsum, p->observe_probs );
    } 
    else
    {
        CvScalar sum = cvSum( p->observe_probs );
        cvScale( p->observe_probs, p->observe_probs, 1.0 / sum.val[0] );
    }
}

/**
 * Bound particle states
 *
 * @param particle
 */
void _cvParticleBound( CvParticle* p )
{
    int row, col;
    double lower, upper;
    bool circular;
    int depend;
    CvMat* stateparticles, hdr;
    float state;
    // @todo:     np.width   = (double)MAX( 2.0, MIN( maxX - 1 - x, width ) );
    for( row = 0; row < p->num_states; row++ )
    {
        lower = cvmGet( p->bound, row, 0 );
        upper = cvmGet( p->bound, row, 1 );
        circular = (bool) cvmGet( p->bound, row, 2 );
        if( lower == upper ) continue; // no bound flag
        if( circular ) {
            for( col = 0; col < p->num_particles; col++ ) {
                state = cvmGet( p->particles, row, col );
                state = state < lower ? state + upper : ( state >= upper ? state - upper : state );
                cvmSet( p->particles, row, col, state );
            }
        } else {
            stateparticles = cvGetRow( p->particles, &hdr, row );
            cvMinS( stateparticles, upper, stateparticles );
            cvMaxS( stateparticles, lower, stateparticles );
        }
    }
}

/**
 * Samples new particles from given particles
 *
 * Currently suppports only linear dynamics and linear combinations.
 * Write up a function by yourself to supports nonlinear dynamics or
 * nonlinear combinations and call your function instead of
 * cvParticleTrantion( p );
 * Other functions should not necessary be modified.
 *
 * @param particle
 */
void cvParticleTransition( CvParticle* p )
{
    int i, j;
    CvMat* dynam_state, dynam_state_hdr;
    CvMat* dynam_noise, dynam_noise_hdr;
    CvMat* transits = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    CvMat* noisesT  = cvCreateMat( p->num_particles, p->num_states, p->particles->type );
    CvMat* noiseT, noiseThdr;
    CvScalar std;
    
    // dynamics
    dynam_state = cvGetCols( p->dynamics, &dynam_state_hdr, 0, p->num_states );
    dynam_noise = cvGetCol( p->dynamics, &dynam_noise_hdr, p->num_states );
    cvMatMul( dynam_state, p->particles, transits );
    
    // noise
    for( i = 0; i < p->num_states; i++ ) {
        float coef = cvmGet( dynam_noise, i, 0 );
        noiseT = cvGetCol( noisesT, &noiseThdr, i );
        //std = cvScalar( cvmGet( p->std, i, 0 ) );
        //cvRandArr( p->rng, noiseT, CV_RAND_NORMAL, cvScalar(0), std );
        //cvScale( noiseT, noiseT, coef );
        if( coef == 0.0 ) {
            cvZero( noiseT );
        } else {
            std = cvScalar( cvmGet( p->std, i, 0 ) );
            cvRandArr( &p->rng, noiseT, CV_RAND_NORMAL, cvScalar(0), std );
            if( coef != 1.0 ) {
                cvScale( noiseT, noiseT, coef );
            }
        }
    }
    
    // dynamics + noise
    // cvT( noisesT, noises );
    // cvAdd( transits, noises, p->particles );
    for( i = 0; i < p->num_states; i++ ) {
        for( j = 0; j < p->num_particles; j++ ) {
            cvmSet( p->particles, i, j, cvmGet( transits, i, j ) + cvmGet( noisesT, j, i ) );
        }
    }

    cvReleaseMat( &transits );
    cvReleaseMat( &noisesT );

    _cvParticleBound( p );
}

/**
 * Initialize states
 *
 * @param particle
 * @param [init = NULL] Manually tell initial states. 
 *                      Use num_particles as number of initial candidates for this.
 *                      If not given, uniform randomly initialize
 */
void cvParticleInit( CvParticle* p, const CvParticle* init )
{
    int i, j, k;
    if( init )
    {
        int *num_copy;
        CvMat onestate;

        int divide = p->num_particles / init->num_particles;
        int remain = p->num_particles - divide * init->num_particles;
        num_copy = (int*) malloc( init->num_particles * sizeof(int) );
        for( i = 0; i < init->num_particles; i++ )
        {
            num_copy[i] = divide + ( i < remain ? 1 : 0 );
        }
        
        k = 0;
        for( i = 0; i < init->num_particles; i++ )
        {
            cvGetCol( init->particles, &onestate, i );
            for( j = 0; j < num_copy[i]; j++ )
            {
                cvSetCol( &onestate, p->particles, k++ );
            }
        }

        free( num_copy );
    } 
    else
    {
        CvScalar lower, upper;
        CvMat* onenoiseT = cvCreateMat( p->num_particles, 1, p->particles->type );
        CvMat* onenoise  = cvCreateMat( 1, p->num_particles, p->particles->type );
        for( i = 0; i < p->num_states; i++ )
        {
            lower = cvScalar( cvmGet( p->bound, i, 0 ) );
            upper = cvScalar( cvmGet( p->bound, i, 1 ) );
            cvRandArr( &p->rng, onenoiseT, CV_RAND_UNI, lower, upper );
            cvT( onenoiseT, onenoise );
            cvSetRow( onenoise, p->particles, i );
        }
        cvReleaseMat( &onenoise );
        cvReleaseMat( &onenoiseT );
    }
}

/**
 * Set lowerbound and upperbound for tracking states
 *
 * @param particle
 * @param bound    num_states x 3 (lowerbound, upperbound, circular flag 0 or 1)
 *                 Set lowerbound == upperbound to express no bound
 */
void cvParticleSetBound( CvParticle* p, const CvMat* bound )
{
    CV_FUNCNAME( "cvParticleSetBound" );
    __BEGIN__;
    CV_ASSERT( p->num_states == bound->rows );
    CV_ASSERT( 3 == bound->cols );
    //cvCopy( bound, p->bound );
    cvConvert( bound, p->bound );
    __END__;
}

/**
 * Set noise model
 *
 * @param particle
 * @param rng      random seed. refer cvRNG(time(NULL))
 * @param std      num_states x 1. standard deviation for gaussian noise
 */
void cvParticleSetNoise( CvParticle* p, CvRNG rng, const CvMat* std )
{
    CV_FUNCNAME( "cvParticleSetNoise" );
    __BEGIN__;
    CV_ASSERT( p->num_states == std->rows );
    p->rng = rng;
    //cvCopy( std, p->std );
    cvConvert( std, p->std );
    __END__;
}

/**
 * Set dynamics model
 *
 * @param particle
 * @param dynamics (num_states) x (num_states + 1). dynamics model
 *    new_state = dynamics(:,1:end-1) * curr_state + dyanmics(:,end) * noise;
 */
void cvParticleSetDynamics( CvParticle* p, const CvMat* dynamics )
{
    CV_FUNCNAME( "cvParticleSetDynamics" );
    __BEGIN__;
    CV_ASSERT( p->num_states == dynamics->rows );
    CV_ASSERT( p->num_states + 1 == dynamics->cols );
    //cvCopy( dynamics, p->dynamics );
    cvConvert( dynamics, p->dynamics );
    __END__;
}

/**
 * Release Particle filter structure
 *
 * @param particle
 * @return void
 */
void cvReleaseParticle( CvParticle** particle )
{
    CvParticle *p = NULL;
    CV_FUNCNAME( "cvReleaseParticle" );
    __BEGIN__;
    p = *particle;
    if( !p ) EXIT;
    
    cvReleaseMat( &p->dynamics );
    cvReleaseMat( &p->std );
    cvReleaseMat( &p->bound );
    cvReleaseMat( &p->particles );
    cvReleaseMat( &p->probs );
    cvFree( &p );
    __END__;
}

/**
 * Allocate Particle filter structure
 *
 * @param num_states    Number of tracking states, e.g., 4 if x, y, width, height
 * @param num_observes  Number of observation models, e.g., 2 if color model and shape model
 * @param num_particles Number of particles
 * @param [logprob = false]
 *                      The probs parameter is log probabilities or not
 * @return CvParticle*
 */
CvParticle* cvCreateParticle( int num_states, int num_observes, int num_particles, bool logprob )
{
    CvParticle *p = NULL;
    CvMat *subdynamics, subdynamicshdr;
    CV_FUNCNAME( "cvCreateParticle" );
    __BEGIN__;
    CV_ASSERT( num_states > 0 );
    CV_ASSERT( num_observes > 0 );
    CV_ASSERT( num_particles > 0 );
    p = (CvParticle *) cvAlloc( sizeof( CvParticle ) );
    p->num_particles = num_particles;
    p->num_states    = num_states;
    p->num_observes  = num_observes;
    p->dynamics      = cvCreateMat( num_states, num_states + 1, CV_32FC1 );
    p->rng           = 1;
    p->std           = cvCreateMat( num_states, 1, CV_32FC1 );
    p->bound         = cvCreateMat( num_states, 3, CV_32FC1 );
    p->particles     = cvCreateMat( num_states, num_particles, CV_32FC1 );
    p->probs         = cvCreateMat( num_observes, num_particles, CV_64FC1 );
    p->particle_probs = cvCreateMat( 1, num_particles, CV_64FC1 );
    p->observe_probs  = cvCreateMat( num_observes, 1, CV_64FC1 );
    p->logprob        = logprob;

    // Default dynamics: next state = curr state + noise
    cvSetIdentity( p->dynamics, cvScalar(1.0) );
    subdynamics = cvGetCol( p->dynamics, &subdynamicshdr, num_states ); // noise coef
    cvSet( subdynamics, cvScalar(1.0) );
    
    cvSet( p->std, cvScalar(1.0) ); // no reason
    cvZero( p->bound );

    __END__;
    return p;
}

#ifdef _MSC_VER // MS Visual Studio
#pragma warning(pop)
#endif

#endif
