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
#ifndef CV_WAITFPS_INCLUDED
#define CV_WAITFPS_INCLUDED

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

clock_t cvWaitFps( double frame_per_sec, clock_t start = 0 );

/**
 * Wait second per frame
 *
 * Example)
 * clock_t start = clock();
 * fps = cvGetCaptureProperty(video, CV_CAP_PROP_FPS);
 * // process
 * cvWaitFps( fps, start );
 *
 * @param fps      frame per second video property
 * @param [start = 0]
 *                 start time
 * @return clock_t wait time
 */
clock_t cvWaitFps( double fps, clock_t start )
{
    clock_t msec_per_frame = (clock_t)( 1.0 / (double) fps * 1000 );
    clock_t current = clock();
    clock_t wait_time;
    if( start == 0 ) {
        wait_time = msec_per_frame;
    } else {
        wait_time = msec_per_frame - (current - start);
    }
    //while( clock() - current < wait_time ) {}
    cvWaitKey( wait_time > 0 ? wait_time : 1 );
    return wait_time;
}

#ifdef _MSC_VER // MS Visual Studio
#pragma warning(pop)
#endif

#endif
