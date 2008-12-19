/** @file */
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
#ifndef CV_MULS_INCLUDED
#define CV_MULS_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

/**
 * Multiply a scalar to an array
 *
 * @param  src       source array
 * @param  scalar    scalar to multiply
 * @param  dst       destination array
 */
CVAPI(void) cvMulS( const CvArr *src, const CvScalar scalar, CvArr *dst )
{
    IplImage* _src = (IplImage*)src, _srchdr;
    IplImage* _dst = (IplImage*)dst, _dsthdr;
    int ch, row, col;
    CV_FUNCNAME( "cvMulS" );
    __BEGIN__;
    if( !CV_IS_IMAGE(_src) )
    {
        CV_CALL( _src = cvGetImage( _src, &_srchdr ) );
    }
    if( !CV_IS_IMAGE(_dst) )
    {
        CV_CALL( _dst = cvGetImage( _dst, &_dsthdr ) );
    }
    CV_ASSERT( _src->width == _dst->width );
    CV_ASSERT( _src->height == _dst->height );
    CV_ASSERT( _src->nChannels == _dst->nChannels );

    // cvScale does not support COI
//     for( ch = 0; ch < _src->nChannels; ch++ )
//     {
//         cvSetImageCOI( _src, ch + 1 ); 
//         cvScale( _src, _dst, scalar.val[ch] );
//     }

    for( row = 0; row < _src->height; row++ )
    {
        for( col = 0; col < _src->width; col++ )
        {
            CvScalar value = cvGet2D( _src, row, col );
            for( ch = 0; ch < _src->nChannels; ch++ )
                value.val[ch] *= scalar.val[ch];
            cvSet2D( _dst, row, col, value );
        }
    }
    __END__;
}

#endif
