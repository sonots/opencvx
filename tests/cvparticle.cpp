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
#include "../cvparticle.h"

#include "../cvxmat.h"
#include "../cvxrectangle.h"
#include "../cvcropimageroi.h"
#include "../cvdrawrectangle.h"
#include "../cvskincolorpeer.h"
#include <time.h>

void cvParticleBound2( CvParticle* p, CvSize imgsz )
{
    for( int np = 0; np < p->num_particles; np++ ) 
    {
        double x = cvmGet( p->particles, 0, np );
        double y = cvmGet( p->particles, 1, np );
        double width = cvmGet( p->particles, 2, np );
        double height = cvmGet( p->particles, 3, np );
        width = ( width < imgsz.width - x ) ? width : imgsz.width - x;
        height = ( height < imgsz.height - y ) ? height : imgsz.height - y;
        cvmSet( p->particles, 2, np, width );
        cvmSet( p->particles, 3, np, height );
    }
}

void cvParticleConfig( CvParticle* p, CvSize imgsz )
{
    // curr_x <= curr_x + dx + noise where dx = curr_x - prev_x
    // prev_x <= curr_x
    double dynamicsarr[] = {
        2, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1,
        0, 2, 0, 0, 0, 0, -1, 0, 0, 0, 1,
        0, 0, 2, 0, 0, 0, 0, -1, 0, 0, 1,
        0, 0, 0, 2, 0, 0, 0, 0, -1, 0, 1,
        0, 0, 0, 0, 2, 0, 0, 0, 0, -1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    };
    CvMat dynamics = cvMat( p->num_states, p->num_states + 1, CV_64FC1, dynamicsarr );
    CvRNG rng = cvRNG( time( NULL ) );
    double stdarr[] = {
        3.0,
        3,0,
        2.0,
        2.0,
        1.0,
        0,
        0,
        0,
        0,
        0
    };
    CvMat std = cvMat( p->num_states, 1, CV_64FC1, stdarr );
    double boundarr[] = {
        0, imgsz.width - 1, false,
        0, imgsz.height - 1, false,
        1, imgsz.width, false,
        1, imgsz.height, false,
        0, 360, true,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };
    CvMat bound = cvMat( p->num_states, 3, CV_64FC1, boundarr );
    cvParticleSetDynamics( p, &dynamics );
    cvParticleSetNoise( p, rng, &std );
    cvParticleSetBound( p, &bound );

    /*
    CvParticle* init = cvCreateParticle( NUM_STATES, 1, num_objects );
    for( i = 0; i < num_objects; i++ ) {
        cvmSet( init->particles, 0, i, regions[i].x + (regions[i].width - 1)/2.0 );
        cvmSet( init->particles, 1, i, regions[i].y + (regions[i].height - 1)/2.0 );
        cvmSet( init->particles, 2, i, regions[i].width );
        cvmSet( init->particles, 3, i, regions[i].height );
        cvmSet( init->particles, 4, i, 0.0 );
        cvmSet( init->particles, 5, i, regions[i].x + (regions[i].width - 1)/2.0 );
        cvmSet( init->particles, 6, i, regions[i].y + (regions[i].height - 1)/2.0 );
        cvmSet( init->particles, 7, i, regions[i].width );
        cvmSet( init->particles, 8, i, regions[i].height );
        cvmSet( init->particles, 9, i, 0.0 );
    }
    cvParticleInit( p, init );
    cvReleaseParticle( &init );
    */
    cvParticleInit( p );
}

int main( int argc, char** argv )
{
    IplImage *img, *frame;
    IplImage *patch, *mask;
    int i;

    cvNamedWindow( "Show" );
    frame = img = cvLoadImage( "avgface.jpg" );

    int num_states    = 10; // x, y, width, height, rotate and its previous(s)
    int num_observes  = 1;
    int num_particles = 100;
    CvParticle *p = cvCreateParticle( num_states, num_observes, num_particles );
    cvParticleConfig( p, cvGetSize(frame) );
    cvParticleTransition( p );
    cvParticleBound2( p, cvGetSize(frame) );

    while( 1 )
    {
        // frame = cvQueryFrame (capture);
        frame = (IplImage*) cvClone( img );

        // likelihood measurements
        CvRect rect;
        double x, y, width, height, rotate;
        CvScalar likeli;
        for( i = 0; i < p->num_particles; i++ ) 
        {
            rect.x = cvRound( cvmGet( p->particles, 0, i ) );
            rect.y = cvRound( cvmGet( p->particles, 1, i ) );
            rect.width = cvRound( cvmGet( p->particles, 2, i ) );
            rect.height = cvRound( cvmGet( p->particles, 3, i ) );
            rotate = cvmGet( p->particles, 4, i );

            patch = cvCreateImage( cvSize(rect.width,rect.height), frame->depth, frame->nChannels );
            mask  = cvCreateImage( cvGetSize(patch), IPL_DEPTH_8U, 1 );

            cvCropImageROI( frame, patch, rect, rotate );
            cvSkinColorPeer( patch, mask );
            likeli = cvSum( mask );
            cvmSet( p->probs, 0, i, likeli.val[0] / (double)(rect.width * rect.height) );

            cvReleaseImage( &patch );
            cvReleaseImage( &mask );
        }

        // draw all particles
        cvParticleDrawRectangle( p, frame, CV_RGB(0,0,255) );
        // draw most likeli particle
        int maxp = _cvParticleMaxParticle( p );
        cvParticleDrawRectangle( p, frame, CV_RGB(255,0,0), maxp );
        cvParticlePrint( p, maxp );

        cvShowImage( "Show", frame );
        char c = cvWaitKey( 1000 );
        if(c == '\x1b')
            break;

        cvParticleResample( p );
        cvParticleTransition( p );
        //cvParticleBound2( p, cvGetSize(frame) );
    }
    cvDestroyWindow( "Show" );

    cvReleaseImage( &frame );
    cvReleaseParticle( &p );
    return 0;
}

