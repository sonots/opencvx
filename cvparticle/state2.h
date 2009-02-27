/** @file
 * Rotated rectangle state particle filter +
 * 2nd order AR dynamics model ( in fact, next = current + speed + noise )
 *
 * Use this file as a template of definitions of states and 
 * state transition model for particle filter
 *
 * Currently cvparticle.h supports only linear combination of states transition
 * model only. you may create another cvParticleTransition to support more complex
 * non-linear state transition model. Most of other functions still should be 
 * available modifications
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
#ifndef CV_PARTICLE_ROTRECT2_H
#define CV_PARTICLE_ROTRECT2_H

#include "cvparticle.h"
#include "cvdrawrectangle.h"
#include "cvcropimageroi.h"
#include "cvrect32f.h"
#include <float.h>
using namespace std;

/********************** Definition of a particle *****************************/

int num_states = 10;

// Definition of meanings of 10 states.
// This kinds of structures is not necessary to be defined, 
// but I recommend to define them because it makes clear meanings of states
typedef struct CvParticleState {
    double x;        // center coord of a rectangle
    double y;        // center coord of a rectangle
    double width;    // width of a rectangle
    double height;   // height of a rectangle
    double angle;    // rotation around center. degree
    double xp;       // previous center coord of a rectangle
    double yp;       // previous center coord of a rectangle
    double widthp;   // previous width of a rectangle
    double heightp;  // previous height of a rectangle
    double anglep;   // previous rotation around center. degree
} CvParticleState;

// Definition of dynamics model
// new_particle = cvMatMul( dynamics, particle ) + noise
// curr_x =: curr_x + dx + noise = curr_x + (curr_x - prev_x) + noise
// prev_x =: curr_x
double dynamics[] = {
    2, 0, 0, 0, 0, -1, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 2, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 2, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 2, 0, 0, 0, 0, -1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
};

/********************** Function Prototypes *********************************/

#ifndef NO_DOXYGEN
// Functions for CvParticleState structure ( constructor, getter, setter )
inline CvParticleState cvParticleState( double x, 
                                        double y, 
                                        double width, 
                                        double height, 
                                        double angle = 0,
                                        double xp = 0, 
                                        double yp = 0, 
                                        double widthp = 0, 
                                        double heightp = 0,
                                        double anglep =0 );
CvParticleState cvParticleStateFromMat( const CvMat* state );
void cvParticleStateToMat( const CvParticleState &state, CvMat* state_mat );
CvParticleState cvParticleStateGet( const CvParticle* p, int p_id );
void cvParticleStateSet( CvParticle* p, int p_id, const CvParticleState &state );

// Particle Filter configuration
void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std );
void cvParticleStateAdditionalBound( CvParticle* p, CvSize imsize );

// Utility Functions
void cvParticleStateDisplay( const CvParticleState& state, IplImage* frame, CvScalar color );
void cvParticleStatePrint( const CvParticleState& state );
#endif

/****************** Functions for CvParticleState structure ******************/

// This kinds of state definitions are not necessary, 
// but helps readability of codes for sure.

/**
 * Constructor
 */
inline CvParticleState cvParticleState( double x, 
                                        double y, 
                                        double width, 
                                        double height, 
                                        double angle,
                                        double xp, 
                                        double yp, 
                                        double widthp, 
                                        double heightp, 
                                        double anglep )
{
    CvParticleState state = { x, y, width, height, angle, 
                              xp, yp, widthp, heightp, anglep };
    return state;
}

/**
 * Convert a matrix state representation to a state structure
 *
 * @param state     num_states x 1 matrix
 */
CvParticleState cvParticleStateFromMat( const CvMat* state )
{
    CvParticleState s;
    s.x       = cvmGet( state, 0, 0 );
    s.y       = cvmGet( state, 1, 0 );
    s.width   = cvmGet( state, 2, 0 );
    s.height  = cvmGet( state, 3, 0 );
    s.angle   = cvmGet( state, 4, 0 );
    s.xp      = cvmGet( state, 5, 0 );
    s.yp      = cvmGet( state, 6, 0 );
    s.widthp  = cvmGet( state, 7, 0 );
    s.heightp = cvmGet( state, 8, 0 );
    s.anglep  = cvmGet( state, 9, 0 );
    return s;
}

/**
 * Convert a state structure to CvMat
 *
 * @param state        A CvParticleState structure
 * @param state_mat    num_states x 1 matrix
 * @return void
 */
void cvParticleStateToMat( const CvParticleState& state, CvMat* state_mat )
{
    cvmSet( state_mat, 0, 0, state.x );
    cvmSet( state_mat, 1, 0, state.y );
    cvmSet( state_mat, 2, 0, state.width );
    cvmSet( state_mat, 3, 0, state.height );
    cvmSet( state_mat, 4, 0, state.angle );
    cvmSet( state_mat, 5, 0, state.xp );
    cvmSet( state_mat, 6, 0, state.yp );
    cvmSet( state_mat, 7, 0, state.widthp );
    cvmSet( state_mat, 8, 0, state.heightp );
    cvmSet( state_mat, 9, 0, state.anglep );
}

/**
 * Get a state from a particle filter structure
 *
 * @param p         particle filter struct
 * @param p_id      particle id
 */
CvParticleState cvParticleStateGet( const CvParticle* p, int p_id )
{
    CvMat* state, hdr;
    state = cvGetCol( p->particles, &hdr, p_id );
    return cvParticleStateFromMat( state );
}

/**
 * Set a state to a particle filter structure
 *
 * @param state     A CvParticleState structure
 * @param p         particle filter struct
 * @param p_id      particle id
 * @return void
 */
void cvParticleStateSet( CvParticle* p, int p_id, const CvParticleState& state )
{
    CvMat* state_mat, hdr;
    state_mat = cvGetCol( p->particles, &hdr, p_id );
    cvParticleStateToMat( state, state_mat );
}

/*************************** Particle Filter Configuration *********************************/

/**
 * Configuration of Particle filter
 */
void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std )
{
    // config dynamics model
    CvMat dynamicsmat = cvMat( p->num_states, p->num_states, CV_64FC1, dynamics );

    // config random noise standard deviation
    CvRNG rng = cvRNG( time( NULL ) );
    double stdarr[] = {
        std.x,
        std.y,
        std.width,
        std.height,
        std.angle,
        0,
        0,
        0,
        0,
        0
    };
    CvMat stdmat = cvMat( p->num_states, 1, CV_64FC1, stdarr );

    // config minimum and maximum values of states
    // lowerbound, upperbound, circular flag (useful for degree)
    // lowerbound == upperbound to express no bounding
    double boundarr[] = {
        0, imsize.width - 1, false,
        0, imsize.height - 1, false,
        1, imsize.width, false,
        1, imsize.height, false,
        0, 360, true,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };
    CvMat boundmat = cvMat( p->num_states, 3, CV_64FC1, boundarr );
    cvParticleSetDynamics( p, &dynamicsmat );
    cvParticleSetNoise( p, rng, &stdmat );
    cvParticleSetBound( p, &boundmat );
}

/**
 * @todo
 * CvParticle does not support this type of bounding currently
 * Call after transition
 */
void cvParticleStateAdditionalBound( CvParticle* p, CvSize imsize )
{
    for( int np = 0; np < p->num_particles; np++ ) 
    {
        double x      = cvmGet( p->particles, 0, np );
        double y      = cvmGet( p->particles, 1, np );
        double width  = cvmGet( p->particles, 2, np );
        double height = cvmGet( p->particles, 3, np );
        width = MIN( width, imsize.width - x ); // another state x is used
        height = MIN( height, imsize.height - y ); // another state y is used
        cvmSet( p->particles, 2, np, width );
        cvmSet( p->particles, 3, np, height );
    }
}

/***************************** Utility Functions ****************************************/

/**
 * Draw tracking state on an image
 */
void cvParticleStateDisplay( const CvParticleState& state, IplImage* img, CvScalar color )
{
    CvBox32f box32f = cvBox32f( state.x, state.y, state.width, state.height, state.angle );
    CvRect32f rect32f = cvRect32fFromBox32f( box32f );
    cvDrawRectangle( img, rect32f, cvPoint2D32f(0,0), color );
}

/**
 * Print the tracking state
 */
void cvParticleStatePrint( const CvParticleState& state )
{
    printf( "x :%.2f ", state.x );
    printf( "y :%.2f ", state.y );
    printf( "width :%.2f ", state.width );
    printf( "height :%.2f ", state.height );
    printf( "angle :%.2f\n", state.angle );
    printf( "xp:%.2f ", state.xp );
    printf( "yp:%.2f ", state.yp );
    printf( "widthp:%.2f ", state.widthp );
    printf( "heightp:%.2f ", state.heightp );
    printf( "anglep:%.2f\n", state.anglep );
    fflush( stdout );
}

#endif

