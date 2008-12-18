/** @file
 * Particle Filter
 *
 * Currently suppports only linear state transition model. 
 * Write up a function by yourself to supports nonlinear dynamics
 * such as Taylor series model and call your function instead of 
 * cvParticleTransition( p ). 
 * Other functions should not necessary be modified.
 *
 * cvCreateParticle -> cvPartcileSetXxx -> cvParticleInit
 * -> loop { cvParticleTransition -> cvParticleResample )
 * -> cvReleaseParticle
 */
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
#ifndef CV_PARTICLE_INCLUDED
#define CV_PARTICLE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include <time.h>
#include "cvsetrow.h"
#include "cvsetcol.h"
#include "cvlogsum.h"
#include "cvanglemean.h"
#include "cvrandgauss.h"

/******************************* Structures **********************************/
/**
 * Particle Filter structure
 */
typedef struct CvParticle {
    // config
    int num_states;    /**< Number of tracking states, e.g.,
                          4 if x, y, width, height */
    int num_observes;  /**< Number of observation models, e.g., 
                          2 if color model and shape model */
    int num_particles; /**< Number of particles */
    bool logprob;      /**< probs are log probabilities */
    // transition
    CvMat* dynamics;   /**< num_states x num_states. Dynamics model. */
    CvRNG  rng;        /**< Random seed */
    CvMat* std;        /**< num_states x 1. Standard deviation for gaussian noise
                           Set standard deviation == 0 for no noise */
    CvMat* stds;       /**< num_states x num_particles. Std for each state and
                          each particle. "std" is used if "stds" is not set. */
    CvMat* bound;      /**< num_states x 3 (lowerbound, upperbound, 
                          wrap_around (like angle) flag 0 or 1)
                          Set lowerbound == upperbound to express no bound */
    // particle states
    CvMat* particles;      /**< num_states x num_particles. The particles. 
                              The transition states values of all particles. */
    CvMat* probs;          /**< num_observes x num_particles. The likelihood of 
                              each particle respect to the particle id in "particles" */
    CvMat* trans_probs;    /**< 1 x num_particles. Likelihoods of transition states. 
                              Marginalized respect to observation models */
    CvMat* observe_probs;  /**< num_observes x 1. Likelihoods of observation states. 
                              Marginalized respect to tracking states */
    CvMat* observe_priors; /**< num_observes x 1. Prior probs for observation states. */
} CvParticle;

/**************************** Function Prototypes ****************************/

//CvParticle* cvCreateParticle( int num_states, int num_observes, int num_particles, bool logprob );
//void cvReleaseParticle( CvParticle** p );

//void cvParticleSetDynamics( CvParticle* p, const CvMat* dynamics );
//void cvParticleSetNoise( CvParticle* p, CvRNG rng, const CvMat* std );
//void cvParticleSetBound( CvParticle* p, const CvMat* bound );

// int  cvParticleGetMax( const CvParticle* p );
// void cvParticleGetMean( const CvParticle* p, CvMat* meanp );
// void cvParticlePrint( const CvParticle* p, int p_id );

// void cvParticleMarginalize( CvParticle* p );
// void cvParticleBound( CvParticle* p );

// void cvParticleInit( CvParticle* p, const CvParticle* init );
// void cvParticleTransition( CvParticle* p );
// void cvParticleResample( CvParticle* p, bool marginal );

/*************************** Constructor / Destructor *************************/

/**
 * Allocate Particle filter structure
 *
 * @param num_states    Number of tracking states, e.g., 4 if x, y, width, height
 * @param num_observes  Number of observation models, e.g., 2 if color model and shape model
 * @param num_particles Number of particles
 * @param logprob       The probs parameter is log probabilities or not
 * @return CvParticle*
 */
CvParticle* cvCreateParticle( int num_states, int num_observes, 
                              int num_particles, bool logprob = false )
{
    CvParticle *p = NULL;
    CV_FUNCNAME( "cvCreateParticle" );
    __BEGIN__;
    CV_ASSERT( num_states > 0 );
    CV_ASSERT( num_observes > 0 );
    CV_ASSERT( num_particles > 0 );
    p = (CvParticle *) cvAlloc( sizeof( CvParticle ) );
    p->num_particles = num_particles;
    p->num_states    = num_states;
    p->num_observes  = num_observes;
    p->dynamics      = cvCreateMat( num_states, num_states, CV_32FC1 );
    p->rng           = 1;
    p->std           = cvCreateMat( num_states, 1, CV_32FC1 );
    p->bound         = cvCreateMat( num_states, 3, CV_32FC1 );
    p->particles     = cvCreateMat( num_states, num_particles, CV_32FC1 );
    p->probs         = cvCreateMat( num_observes, num_particles, CV_64FC1 );
    p->trans_probs = cvCreateMat( 1, num_particles, CV_64FC1 );
    p->observe_probs  = cvCreateMat( num_observes, 1, CV_64FC1 );
    p->observe_priors = cvCreateMat( num_observes, 1, CV_64FC1 );
    p->logprob        = logprob;
    p->stds           = NULL;

    // Default dynamics: next state = curr state + noise
    cvSetIdentity( p->dynamics, cvScalar(1.0) );
    cvSet( p->std, cvScalar(1.0) );

    cvZero( p->bound );
    if( p->logprob )
        cvSet( p->observe_priors, cvScalar( log( 1.0 / num_observes ) ) ); // uniform
    else
        cvSet( p->observe_priors, cvScalar( 1.0 / num_observes ) ); // uniform

    __END__;
    return p;
}

/**
 * Release Particle filter structure
 *
 * @param particle
 */
void cvReleaseParticle( CvParticle** particle )
{
    CvParticle *p = NULL;
    CV_FUNCNAME( "cvReleaseParticle" );
    __BEGIN__;
    p = *particle;
    if( !p ) EXIT;
    
    CV_CALL( cvReleaseMat( &p->dynamics ) );
    CV_CALL( cvReleaseMat( &p->std ) );
    CV_CALL( cvReleaseMat( &p->bound ) );
    CV_CALL( cvReleaseMat( &p->particles ) );
    CV_CALL( cvReleaseMat( &p->probs ) );
    CV_CALL( cvReleaseMat( &p->trans_probs ) );
    CV_CALL( cvReleaseMat( &p->observe_probs ) );
    CV_CALL( cvReleaseMat( &p->observe_priors ) );
    if( p->stds != NULL )
        CV_CALL( cvReleaseMat( &p->stds ) );

    CV_CALL( cvFree( &p ) );
    __END__;
}

/***************************** Setter ***************************************/

/**
 * Set dynamics model
 *
 * @param particle
 * @param dynamics (num_states) x (num_states). dynamics model
 *    new_state = dynamics * curr_state + noise
 */
void cvParticleSetDynamics( CvParticle* p, const CvMat* dynamics )
{
    CV_FUNCNAME( "cvParticleSetDynamics" );
    __BEGIN__;
    CV_ASSERT( p->num_states == dynamics->rows );
    CV_ASSERT( p->num_states == dynamics->cols );
    //cvCopy( dynamics, p->dynamics );
    cvConvert( dynamics, p->dynamics );
    __END__;
}

/**
 * Set noise model
 *
 * @param particle
 * @param rng      random seed. refer cvRNG(time(NULL))
 * @param std      num_states x 1. standard deviation for gaussian noise
 *                 Set standard deviation == 0 for no noise
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
 * Set lowerbound and upperbound used for bounding tracking state transition
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

/************************ Utility ******************************************/

/**
 * Get id of the most probable particle
 *
 * @param particle
 * @return int
 */
int cvParticleGetMax( const CvParticle* p )
{
    double minval, maxval;
    CvPoint min_loc, max_loc;
    cvMinMaxLoc( p->trans_probs, &minval, &maxval, &min_loc, &max_loc );
    return max_loc.x;
}

/**
 * Get the mean state (particle)
 *
 * @param particle
 * @param meanp     num_states x 1, CV_32FC1 or CV_64FC1
 * @return void
 */
void cvParticleGetMean( const CvParticle* p, CvMat* meanp )
{
    CvMat* probs = NULL;
    CvMat* particles_i, hdr;
    int i, j;
    CV_FUNCNAME( "cvParticleGetMean" );
    __BEGIN__;
    CV_ASSERT( meanp->rows == p->num_states && meanp->cols == 1 );
    if( !p->logprob )
    {
        probs = p->trans_probs;
    }
    else
    {
        probs = cvCreateMat( 1, p->num_particles, p->trans_probs->type );
        cvExp( p->trans_probs, probs );
    }

    for( i = 0; i < p->num_states; i++ )
    {
        int circular = (int) cvmGet( p->bound, i, 2 );
        if( !circular ) // usual mean
        {
            particles_i = cvGetRow( p->particles, &hdr, i );
            double mean = 0;
            for( j = 0; j < p->num_particles; j++ )
            {
                mean += cvmGet( particles_i, 0, j ) * cvmGet( probs, 0, j );
            }
            cvmSet( meanp, i, 0, mean );
        }
        else // wrapped mean (angle)
        {
            double wrap = cvmGet( p->bound, i, 1 ) - cvmGet( p->bound, i, 0 );
            particles_i = cvGetRow( p->particles, &hdr, i );
            CvScalar mean = cvAngleMean( particles_i, probs, wrap );
            cvmSet( meanp, i, 0, mean.val[0] );
        }
    }

    if( probs != p->trans_probs )
        cvReleaseMat( &probs );
    __END__;
}


/**
 * Print states of a particle
 *
 * @param particle
 * @param p_id      particle id
 */
void cvParticlePrint( const CvParticle*p, int p_id = -1 )
{
    if( p_id == -1 ) // print all
    {
        int n;
        for( n = 0; n < p->num_particles; n++ )
        {
            cvParticlePrint( p, n );
        }
    }
    else {
        int i;
        for( i = 0; i < p->num_states; i++ )
        {
            printf( "%6.1f ", cvmGet( p->particles, i, p_id ) );
        }
        printf( "\n" );
        fflush( stdout );
    }
}

/****************************** Helper functions ******************************/
/**
 * Do marginalization to obtain observe_probs (probabilities for
 * observation states) and trans_probs (probabilities for 
 * transition states). Normalization is done too.
 *
 * @param particle
 * @note Used by See also functions
 * @see cvParticleResample
 */
void cvParticleMarginalize( CvParticle* p )
{
    if( !p->logprob )
    {
        {
            // let transition states be t, observation states be i, observation be z
            // p(t|z) = alpha p(z|t) p(t)
            // p(t) comes from sampling methods => \sum_t particle->(z|t)
            // p(z|t) = \int p(z|t,i) p(i|t) di = (now) \int p(z|t,i) p(i) di
            CvMat* probs_n, hdr;
            CvMat* weighted_probs_n = cvCreateMat( p->num_observes, 1, CV_64FC1 );
            CvScalar avg_n;
            for( int n = 0; n < p->num_particles; n++ ) {
                probs_n = cvGetCol( p->probs, &hdr, n );
                cvMul( probs_n, p->observe_priors, weighted_probs_n );
                avg_n = cvSum( weighted_probs_n );
                cvmSet( p->trans_probs, 0, n, avg_n.val[0] );
            }
            cvReleaseMat( &weighted_probs_n );
            // normalize alpha
            CvScalar sum = cvSum( p->trans_probs );
            cvScale( p->trans_probs, p->trans_probs, 1.0 / sum.val[0] );
        }
        {
            // p(i|z) = alpha p(z|i) p(i)
            // p(z|i) = \int p(z|t,i)p(t|i) dt = \int p(z|t,i) p(t) dt
            // p(t) comes from sampling methods => p(z|i) = \sum_t particle->probs
            cvReduce( p->probs, p->observe_probs, CV_REDUCE_SUM );
            cvMul( p->observe_probs, p->observe_priors, p->observe_probs );
            // normalize alpha
            CvScalar sum = cvSum( p->observe_probs );
            cvScale( p->observe_probs, p->observe_probs, 1.0 / sum.val[0] );
        }
    }
    else // log version
    {
        // translate Mul -> Sum, Sum -> LogSum
        // 1/val -> -val
        {
            CvMat* probs_n, hdr;
            CvMat* weighted_probs_n = cvCreateMat( p->num_observes, 1, CV_64FC1 );
            CvScalar avg_n;
            for( int n = 0; n < p->num_particles; n++ ) {
                probs_n = cvGetCol( p->probs, &hdr, n );
                cvAdd( probs_n, p->observe_priors, weighted_probs_n ); // observe_priors also log prob
                avg_n = cvLogSum( weighted_probs_n );
                cvmSet( p->trans_probs, 0, n, avg_n.val[0] );
            }
            cvReleaseMat( &weighted_probs_n );
            // normalize alpha
            CvScalar sum = cvLogSum( p->trans_probs );
            cvSubS( p->trans_probs, sum, p->trans_probs );
        }
        {
            CvMat* probs_i, hdr;
            for( int i = 0; i < p->num_observes; i++ )
            {
                probs_i = cvGetRow( p->probs, &hdr, i );
                CvScalar logsum = cvLogSum( probs_i );
                cvmSet( p->observe_probs, i, 0, logsum.val[0] );
            }
            cvAdd( p->observe_probs, p->observe_priors, p->observe_probs ); // observe_priors also log prob
            // normalize alpha
            CvScalar sum = cvLogSum( p->observe_probs );
            cvSubS( p->observe_probs, sum, p->observe_probs );
        }
    }
}

/**
 * Apply lower bound and upper bound for particle states.
 *
 * @param particle
 * @note Used by See also functions
 * @see cvParticleTransition
 */
void cvParticleBound( CvParticle* p )
{
    int row, col;
    double lower, upper;
    int circular;
    CvMat* stateparticles, hdr;
    float state;
    // @todo:     np.width   = (double)MAX( 2.0, MIN( maxX - 1 - x, width ) );
    for( row = 0; row < p->num_states; row++ )
    {
        lower = cvmGet( p->bound, row, 0 );
        upper = cvmGet( p->bound, row, 1 );
        circular = (int) cvmGet( p->bound, row, 2 );
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

/******************* Main (Related to Algorithm) *****************************/

/**
 * Initialize states
 *
 * If initial states are given, these states are uniformly copied.
 * If not given, states are uniform randomly sampled within lowerbound 
 * and upperbound regions.
 *
 * @param particle
 * @param init       initial states.
 */
void cvParticleInit( CvParticle* p, const CvParticle* init = NULL )
{
    int i, j, k;
    if( init )
    {
        int *num_copy;
        CvMat init_particle;

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
            cvGetCol( init->particles, &init_particle, i );
            for( j = 0; j < num_copy[i]; j++ )
            {
                cvSetCol( &init_particle, p->particles, k++ );
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
 * Samples new particles from given particles
 *
 * Currently suppports only linear combination of states transition model. 
 * Write up a function by yourself to supports nonlinear dynamics
 * such as Taylor series model and call your function instead of this function. 
 * Other functions should not necessary be modified.
 *
 * @param particle
 * @note Uses See also functions inside.
 * @see cvParticleBound
 */
void cvParticleTransition( CvParticle* p )
{
    int i, j;
    CvMat* transits = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    CvMat* noises   = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    CvMat* noise, noisehdr;
    double std;
    
    // dynamics
    cvMatMul( p->dynamics, p->particles, transits );
    
    // noise generation
    if( p->stds == NULL )
    {
        for( i = 0; i < p->num_states; i++ )
        {
            std = cvmGet( p->std, i, 0 );
            noise = cvGetRow( noises, &noisehdr, i );
            if( std == 0.0 )
                cvZero( noise );
            else
                cvRandArr( &p->rng, noise, CV_RAND_NORMAL, cvScalar(0), cvScalar( std ) );
        }
    }
    else
    {
        for( i = 0; i < p->num_states; i++ )
        {
            for( j = 0; j < p->num_particles; j++ )
            {
                std = cvmGet( p->stds, i, j );
                if( std == 0.0 )
                    cvmSet( noises, i, j, 0.0 );
                else
                    cvmSet( noises, i, j, cvRandGauss( &p->rng, std ) );
            }
        }
    }

    // dynamics + noise
    cvAdd( transits, noises, p->particles );

    cvReleaseMat( &transits );
    cvReleaseMat( &noises );

    cvParticleBound( p );
}

/**
 * Re-samples a set of particles according to their probs to produce a
 * new set of unweighted particles
 *
 * Simply copy, not uniform randomly selects
 *
 * @param particle
 * @note Uses See also functions inside.
 * @see cvParticleMarginalize
 * @see cvParticleGetMax
 */
void cvParticleResample( CvParticle* p, bool marginal = true )
{
    int i, j, np, k = 0;
    CvMat* particle, hdr;
    CvMat* new_particles = cvCreateMat( p->num_states, p->num_particles, p->particles->type );
    double prob;
    int max_loc;

    if( marginal )
    {
        cvParticleMarginalize( p );
    }

    k = 0;
    for( i = 0; i < p->num_particles; i++ )
    {
        particle = cvGetCol( p->particles, &hdr, i );
        prob = cvmGet( p->trans_probs, 0, i );
        prob = p->logprob ? exp( prob ) : prob;
        np = cvRound( prob * p->num_particles );
        for( j = 0; j < np; j++ )
        {
            cvSetCol( particle, new_particles, k++ );
            if( k == p->num_particles )
                goto exit;
        }
    }

    max_loc = cvParticleGetMax( p );
    particle = cvGetCol( p->particles, &hdr, max_loc );
    while( k < p->num_particles )
        cvSetCol( particle, new_particles, k++ );

exit:
    cvReleaseMat( &p->particles );
    p->particles = new_particles;
}

#endif
