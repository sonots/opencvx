/** @file
 * 
 * Particle Filter
 * Observation mode: Moghaddam's PCA DIFS + DFFS
 * State transition model: Rotated Rectangle + 2nd order AR model
 *                         ( next = current + speed + noise )
 *
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
#ifdef _MSC_VER
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#endif

#include <stdio.h>
#include <stdlib.h>
#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"

#include "../cvxmat.h"
#include "../cvxrectangle.h"
#include "../cvrect32f.h"
#include "../cvcropimageroi.h"
#include "../cvdrawrectangle.h"
#include <time.h>

// General particle filter structure
// cvParticleXXX are functions in this
#include "../cvparticle.h"
// Rotated rectangle + 2nd AR model state model
// cvParticleStateXXX are functions in this
#include "../cvparticlestaterect2.h"
// PCA DIFS + DFFS observertion model
// cvParticleObserveXXX are functions in this
#include "../cvparticleobservepcadiffs.h"

/****************************** Global *****************************/

// cvparticlestaterect2.h
extern int num_states;
// cvparticleobservepcadiffs.h
extern int num_observes;
extern CvSize feature_size;
extern string data_dir;
extern string data_pcaval;
extern string data_pcaavg;
extern string data_pcavec;
int num_particles = 1000;
char export_filename[2048];
const char* export_format = "%s_%04d.png";

/******************************* Structures ************************/

typedef struct IcvMouseParam {
    CvPoint loc1;
    CvPoint loc2;
    char* win_name;
    IplImage* frame;
} IcvMouseParam;

/**************************** Function Prototypes ******************/

void icvGetRegion( IplImage*, CvRect* );
void icvMouseCallback( int, int, int, int, void* );

/**************************** Main ********************************/
int main( int argc, char** argv )
{
    IplImage *frame, *reference, *tmp;
    char* vid_file;
    CvCapture* video;
    int frame_num = 0;

    // arg_parse
    if( argc == 1 ) {
        vid_file = argv[1];
    } else if( argc > 2 ) {
        vid_file = argv[1];
        data_dir = string( argv[2] );
    } else {
        fprintf( stderr, "./cvparticle <vid_file> <pca_data_dir = ''>\n" );
        fprintf( stderr, " PCA data must be stored as %s, %s, \n"
                 " and %s under <pca_data_dir>.\n",
                 data_pcaval.c_str(),
                 data_pcavec.c_str(), data_pcaavg.c_str() );
        exit( 1 );
    }

    // load trained PCA data
    cvParticleObserveInitialize();

    // read a video
    video = cvCaptureFromFile( vid_file );
    if( ! video ) {
        fprintf( stderr, "couldn't open video file %s", vid_file );
        exit( 1 );
    }
    frame = cvQueryFrame( video );

    // allows user to select initial region
    printf( "Select a region to record an appearance to track\n" );
    CvRect region;
    icvGetRegion( frame, &region );

    // configure particle filter
    bool logprob = true;
    CvParticle *particle = cvCreateParticle( num_states, num_observes, num_particles, logprob );
    CvParticleState std = cvParticleState (
        3.0,
        3.0,
        2.0,
        2.0,
        1.0,
        0, 0, 0, 0, 0
    );
    cvParticleStateConfig( particle, cvGetSize(frame), std );

    // initialize particle filter
    CvParticleState s;
    CvParticle *init_particle;
    init_particle = cvCreateParticle( num_states, 1, 1 );
    CvRect32f region32f = cvRect32fFromRect( region );
    CvBox32f box = cvBox32fFromRect32f( region32f ); // centerize
    s = cvParticleState( box.cx, box.cy, box.width, box.height, 0.0,
                         box.cx, box.cy, box.width, box.height, 0.0 );
    cvParticleStateSet( init_particle, 0, s );
    cvParticleInit( particle, init_particle );
    cvReleaseParticle( &init_particle );

    CvMat* meanp = cvCreateMat( num_states, 1, CV_64FC1 );
    while( frame = cvQueryFrame( video ) )
    {
        // state transition (and bouding inside)
        cvParticleTransition( particle );

        // likelihood measurements
        cvParticleObserveLikelihood( particle, frame );

        // draw all particles
        for( int i = 0; i < particle->num_particles; i++ )
        {
            CvParticleState s = cvParticleStateGet( particle, i );
            cvParticleStateDraw( s, frame, CV_RGB(0,0,255) );
        }
        // draw most probable particle
        printf( "Most probable particle's state\n" );
        int maxp_id = cvParticleMaxParticle( particle );
        CvParticleState maxs = cvParticleStateGet( particle, maxp_id );
        cvParticleStateDraw( maxs, frame, CV_RGB(255,0,0) );
        cvParticleStatePrint( maxs );
        
        // marginalization and normalization beforehand to get mean
        cvParticleMarginalize( particle );

        // draw mean particle's state
        printf( "Mean particle's state\n");
        cvParticleMeanParticle( particle, meanp );
        CvParticleState means = cvParticleStateFromMat( meanp );
        cvParticleStateDraw( means, frame, CV_RGB(255,0,0) );
        cvParticleStatePrint( means );

        // resampling
        cvParticleResample( particle, false );
        // flag flase is whether to do marginal and normalize, now it is done already

        cvShowImage( "Show", frame );
        // save pictures
        sprintf( export_filename, export_format, vid_file, frame_num );
        printf( "Export: %s\n", export_filename ); fflush( stdout );
        cvSaveImage( export_filename, frame );

        char c = cvWaitKey( 1000 );
        if(c == '\x1b')
            break;
    }

    cvParticleObserveFinalize();
    cvDestroyWindow( "Show" );
    cvReleaseImage( &tmp );
    cvReleaseImage( &reference );
    cvReleaseParticle( &particle );
    return 0;
}

/**
 * Allows the user to interactively select the initial object region
 *
 * @param frame  The frame of video in which objects are to be selected
 * @param region A pointer to an array to be filled with rectangles
 */
void icvGetRegion( IplImage* frame, CvRect* region )
{
    IcvMouseParam p;

    /* use mouse callback to allow user to define object regions */
    p.win_name = "Show";
    p.frame = frame;

    cvNamedWindow( p.win_name, 1 );
    cvShowImage( p.win_name, frame );
    cvSetMouseCallback( p.win_name, &icvMouseCallback, &p );
    cvWaitKey( 0 );
    //cvDestroyWindow( win_name );

    /* extract regions defined by user; store as a rectangle */
    region->x      = min( p.loc1.x, p.loc2.x );
    region->y      = min( p.loc1.y, p.loc2.y );
    region->width  = max( p.loc1.x, p.loc2.x ) - region->x + 1;
    region->height = max( p.loc1.y, p.loc2.y ) - region->y + 1;
}

/**
 * Mouse callback function that allows user to specify the 
 * initial object region. 
 * Parameters are as specified in OpenCV documentation.
 */
void icvMouseCallback( int event, int x, int y, int flags, void* param )
{
    IcvMouseParam* p = (IcvMouseParam*)param;
    IplImage* clone;
    static int pressed = FALSE;

    /* on left button press, remember first corner of rectangle around object */
    if( event == CV_EVENT_LBUTTONDOWN )
    {
        p->loc1.x = x;
        p->loc1.y = y;
        pressed = TRUE;
    }

    /* on left button up, finalize the rectangle and draw it */
    else if( event == CV_EVENT_LBUTTONUP )
    {
        p->loc2.x = x;
        p->loc2.y = y;
        clone = (IplImage*)cvClone( p->frame );
        cvRectangle( clone, p->loc1, p->loc2, CV_RGB(255,255,255), 1, 8, 0 );
        cvShowImage( p->win_name, clone );
        cvReleaseImage( &clone );
        pressed = FALSE;
    }

    /* on mouse move with left button down, draw rectangle */
    else if( event == CV_EVENT_MOUSEMOVE  &&  flags & CV_EVENT_FLAG_LBUTTON )
    {
        clone = (IplImage*)cvClone( p->frame );
        cvRectangle( clone, p->loc1, cvPoint(x, y), CV_RGB(255,255,255), 1, 8, 0 );
        cvShowImage( p->win_name, clone );
        cvReleaseImage( &clone );
    }
}
