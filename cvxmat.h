/**
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
#ifndef CV_MATEXT_INCLUDED
#define CV_MATEXT_INCLUDED

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

/**
* Set array row or row span
*
* @param subarr Input sub array
* @param arr Target array
* @param start_row Zero-based index of the starting row (inclusive) of the span. 
* @param end_row Zero-based index of the ending row (exclusive) of the span. 
* @return void
* @see cvSetRow( subarr, arr, row ) // cvSetRows( subarr, arr, row, row + 1 )
* @todo support not only CvMat and IplImage but also CvMatND and CvSparseMat
*/
CVAPI( void ) cvSetRows( const CvArr* subarr, CvArr* arr, int start_row, int end_row )
{
    CV_FUNCNAME( "cvSetRows" );
    __BEGIN__;
    int row, col, elem;
    int coi = 0;
    CvMat* submat = (CvMat*)subarr, submatstub;
    CvMat* mat = (CvMat*)arr, matstub;

    if( !CV_IS_MAT(submat) )
    {
        CV_CALL( submat = cvGetMat( submat, &submatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(mat) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    CV_ARE_TYPES_EQ( submat, mat ); // cxtypes.h
    //CV_ARE_DEPTHS_EQ( submat, mat );
    //CV_ARE_CNS_EQ( submat, mat );
    //CV_ARE_SIZES_EQ( submat, mat );
    CV_ASSERT( 0 <= start_row && end_row <= mat->rows );
    CV_ASSERT( end_row - start_row == submat->rows );
    CV_ASSERT( mat->cols == submat->cols );

    int elem_size = CV_ELEM_SIZE( mat->type ); 
    // FYI: CV_ELEM_SIZE returns not number_of_channels, but number_of_uchar*
    // nChannels = CV_MAT_CN( mat->type ); refer cxarray.cpp#cvRawDataToScalar to separate into channel values
    for( row = start_row; row < end_row; row++ )
    {
        for( col = 0; col < mat->cols; col++ )
        {
            //cvSet2D( mat, row, col, cvGet2D( submat, row - start_row, col ) );
            for( elem = 0; elem < elem_size; elem++ )
            {
                // cvPtr2D( mat, row, col )[elem] = cvPtr2D( submat, row - start_row, col )[elem];
                (mat->data.ptr + ((size_t)mat->step * row) + (elem_size * col))[elem] = 
                    (submat->data.ptr + ((size_t)submat->step * (row - start_row)) + (elem_size * col))[elem];
            }
        }
    }
    __END__;
}

/**
* Set array row
* 
* @param subarr Input sub array
* @param arr Target array
* @param row Zero-based index of the row. 
* @return void
* @uses cvSetRows
*/
CV_INLINE void cvSetRow( const CvArr* subarr, CvArr* arr, int row )
{
    cvSetRows( subarr, arr, row, row + 1 );
}


/**
* Set array col or col span
*
* Example)
*    IplImage* img    = cvCreateImage( cvSize(4,4), IPL_DEPTH_8U, 1 ); // width(cols), height(rows)
*    IplImage* subimg = cvCreateImage( cvSize(1,4), IPL_DEPTH_8U, 1 );
*    cvSet( img, cvScalar(1) );
*    cvZero( subimg );
*    cvSetCol( subimg, img, 2 );
*    cvMatPrint( img );
*
* @param subarr Input sub array
* @param arr Target array
* @param start_col Zero-based index of the starting col (inclusive) of the span. 
* @param end_col Zero-based index of the ending col (exclusive) of the span. 
* @return void
* @see cvSetCol( subarr, arr, col ) // cvSetCols( subarr, arr, col, col + 1 )
* @todo support not only CvMat and IplImage but also CvMatND and CvSparseMat
*/
CVAPI( void ) cvSetCols( const CvArr* subarr, CvArr* arr, int start_col, int end_col )
{
    CV_FUNCNAME( "cvSetCols" );
    __BEGIN__;
    int col, row, elem;
    int coi = 0;
    CvMat* submat = (CvMat*)subarr, submatstub;
    CvMat* mat = (CvMat*)arr, matstub;

    if( !CV_IS_MAT(submat) )
    {
        CV_CALL( submat = cvGetMat( submat, &submatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(mat) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    CV_ARE_TYPES_EQ( submat, mat );
    CV_ARE_DEPTHS_EQ( submat, mat );
    CV_ASSERT( 0 <= start_col && end_col <= mat->cols );
    CV_ASSERT( end_col - start_col == submat->cols );
    CV_ASSERT( mat->rows == submat->rows );

    int elem_size = CV_ELEM_SIZE( mat->type );
    for( col = start_col; col < end_col; col++ )
    {
        for( row = 0; row < mat->rows; row++ )
        {
            for( elem = 0; elem < elem_size; elem++ )
            {
                (mat->data.ptr + ((size_t)mat->step * row) + (elem_size * col))[elem] = 
                    (submat->data.ptr + ((size_t)submat->step * row) + (elem_size * (col - start_col)))[elem];
            }
        }
    }
    __END__;
}

/**
* Set array col
* 
* @param subarr Input sub array
* @param arr Target array
* @param col Zero-based index of the col. 
* @return void
* @uses cvSetCols
*/
CV_INLINE void cvSetCol( const CvArr* subarr, CvArr* arr, int col )
{
    cvSetCols( subarr, arr, col, col + 1 );
}

/**
* Concatinate arrays
*
* Example)
*    IplImage* img    = cvCreateImage( cvSize(4,4), IPL_DEPTH_8U, 1 ); // width(cols), height(rows)
*    IplImage* subimg = cvCreateImage( cvSize(1,4), IPL_DEPTH_8U, 1 );
*    IplImage* catimg = cvCreateImage( cvSize(5,4), IPL_DEPTH_8U, 1 );
*    cvSet( img, cvScalar(1) );
*    cvSet( subimg, cvScalar(0) );
*    cvHcat( img, subimg, catimg ); // 4x4 + 4x1 = 4x5
*    cvMatPrint( catimg );
*
* @param src1 Input array 1
* @param src2 Input array 2
* @param dst Target array
* @param dim 0 horizontally, 1 vertically, -1 flexibly
* @see cvHcat( src1, src2, dst ) // cvCat( src1, src2, dst, 0 )
* @see cvVcat( src1, src2, dst ) // cvCat( src1, src2, dst, 1 )
* @todo support not only CvMat and IplImage but also CvMatND and CvSparseMat
*/
//CV_IMPL void
CVAPI( void ) cvCat( const CvArr* src1arr, const CvArr* src2arr, CvArr* dstarr, int dim = -1 )
{
    CV_FUNCNAME( "cvCat" );
    __BEGIN__;
    int coi = 0;
    CvMat *src1 = (CvMat*)src1arr, src1stub;
    CvMat *src2 = (CvMat*)src2arr, src2stub;
    CvMat *dst  = (CvMat*)dstarr, dststub;

    if( !CV_IS_MAT(src1) )
    {
        src1 = cvGetMat( src1, &src1stub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(src2) )
    {
        src2 = cvGetMat( src2, &src2stub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(dst) )
    {
        dst = cvGetMat( dst, &dststub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( dim == -1 )
    {
        if( src1->rows == src2->rows && src1->rows == dst->rows )
        {
            dim = 0;
        }
        else if( src1->cols == src2->cols && src1->cols == dst->cols )
        {
            dim = 1;
        }
        else
        {
            CV_ERROR( CV_StsBadArg, "The size of matrices does not fit to concatinate." );
        }
    }
    if( dim == 0 ) // horizontal cat
    {
        CV_ASSERT( src1->rows == src2->rows && src1->rows == dst->rows );
        CV_ASSERT( src1->cols + src2->cols == dst->cols );
    }
    else if( dim == 1 ) // vertical cat
    {
        CV_ASSERT( src1->cols == src2->cols && src1->cols == dst->cols );
        CV_ASSERT( src1->rows + src2->rows == dst->rows );
    }
    else
    {
        CV_ERROR( CV_StsBadArg, "The dim is 0 (horizontal) or 1 (vertical) or -1 (flexible)." );
    }

    if( dim == 0 ) // horizontal cat
    {
        cvSetCols( src1, dst, 0, src1->cols );
        cvSetCols( src2, dst, src1->cols, src1->cols + src2->cols );
    }
    else // vertical cat
    {
        cvSetRows( src1, dst, 0, src1->rows );
        cvSetRows( src2, dst, src1->rows, src1->rows + src2->rows );
    }
    __END__;
}
#define cvHcat( src1, src2, dst ) cvCat( (src1), (src2), (dst), 0 )
#define cvVcat( src1, src2, dst ) cvCat( (src1), (src2), (dst), 1 )

CV_INLINE void cvPrintMatProperty( const CvMat* mat )
{
    printf("rows: %d\n", mat->rows);
    printf("cols: %d\n", mat->cols);
    printf("type: %d\n", mat->type);
    printf("step: %d\n", mat->step);
}

CV_INLINE void cvPrintIplImageProperty( const IplImage* img )
{
    printf("width: %d\n", img->width);
    printf("height: %d\n", img->height);
    printf("depth: %d\n", img->depth);
    printf("nChannels: %d\n", img->nChannels);
}

/**
* Print array
*
* @param arr array
* @return void
*/
CV_INLINE void cvPrintMat( const CvArr* arr )
{
    CV_FUNCNAME( "cvPrintMat" );
    __BEGIN__;
    int row, col, ch;
    int coi = 0;
    CvMat matstub, *mat = (CvMat*)arr;
    int depth, nChannels;
    CvScalar value;
    if( !CV_IS_MAT( mat ) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub, &coi ) ); // i.e. IplImage to CvMat
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    depth = CV_MAT_DEPTH( mat->type );
    nChannels = CV_MAT_CN( mat->type );
    for( row = 0; row < mat->rows; row++ )
    {
        for( col = 0; col < mat->cols; col++ )
        {
            value = cvGet2D( mat, row, col );
            if( nChannels > 1 )
            {
                printf( "(%lf", value.val[0] );
                for( ch = 1; ch < nChannels; ch++ )
                {
                    printf( " %lf", value.val[ch] );
                }
                printf( ") " );
            }
            else
            {
                printf( "%lf ", value.val[0] );
            }
        }
        printf("\n");
    }
    __END__;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
