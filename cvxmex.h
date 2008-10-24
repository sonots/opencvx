/**
* OpenCV versus Matlab C Library (MEX) interfaces
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
#pragma warning(push)
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "libmat.lib")
#pragma comment(lib, "libmx.lib")
#endif

#include "cv.h"
#include "mat.h"
#include "matrix.h"

#ifndef CV_MEX_INCLUDED
#define CV_MEX_INCLUDED

/**
* Convert IplImage depth to cvMat depth
*
* One may use CV_MAKETYPE(depth, nChannel) to create cvMat type
*
* @param int IplImage depth
* @return int cvMat depth
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
* @reference cxcore/src/_cxcore.h#icvIplToCvDepth
* @reference ./cxcore/src/cxtables.cpp
*/
CVAPI(int) cvmxIplToCvDepth(int depth)
{
    /*
    define CV_8U   0
    define CV_8S   1
    define CV_16U  2
    define CV_16S  3
    define CV_32S  4
    define CV_32F  5
    define CV_64F  6
    define CV_USRTYPE1 7 */
    /*
    typedef enum {
    mxUNKNOWN_CLASS,
    mxCELL_CLASS,
    mxSTRUCT_CLASS,
    mxLOGICAL_CLASS,
    mxCHAR_CLASS,
    mxDOUBLE_CLASS,
    mxSINGLE_CLASS,
    mxINT8_CLASS,
    mxUINT8_CLASS,
    mxINT16_CLASS,
    mxUINT16_CLASS,
    mxINT32_CLASS,
    mxUINT32_CLASS,
    mxINT64_CLASS,
    mxUINT64_CLASS,
    mxFUNCTION_CLASS
    } mxClassID; */
    /*
    #define IPL_DEPTH_SIGN 0x80000000

    #define IPL_DEPTH_1U     1
    #define IPL_DEPTH_8U     8
    #define IPL_DEPTH_16U   16
    #define IPL_DEPTH_32F   32
    #define IPL_DEPTH_64F   64

    #define IPL_DEPTH_8S  (IPL_DEPTH_SIGN| 8)
    #define IPL_DEPTH_16S (IPL_DEPTH_SIGN|16)
    #define IPL_DEPTH_32S (IPL_DEPTH_SIGN|32)
    */
    static const signed char IplToCvDepth[] =
    {
        -1, -1, CV_8U, CV_8S, CV_16U, CV_16S, -1, -1,
        CV_32F, CV_32S, -1, -1, -1, -1, -1, -1, CV_64F, -1
    };
    return IplToCvDepth[(((depth) & 255) >> 2) + ((depth) < 0)];
}

/**
* Get mxClassID from IplImage depth
* 
* @param int depth IplImage depth
* @return mxClassID
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
*/
CVAPI(mxClassID) cvmxClassIDFromIplDepth(int depth)
{
    static const unsigned char ClassIDFromIplDepth[] =
    {
        0, 0, mxUINT8_CLASS, mxINT8_CLASS, mxUINT16_CLASS, mxINT16_CLASS, 0, 0,
        mxSINGLE_CLASS, mxINT32_CLASS, 0, 0, 0, 0, 0, 0, mxDOUBLE_CLASS, 0
    };
    if (depth == IPL_DEPTH_1U) {
        return mxLOGICAL_CLASS;
    } else {
        return ClassIDFromIplDepth[(((depth) & 255) >> 2) + ((depth) < 0)];
    }
}

/**
* Convert mxClassID to IplImage depth 
* 
* @param int depth IplImage's depth
* @return mxClassID
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
*/
CVAPI(int) cvmxClassIDToIplDepth(mxClassID classid)
{
    static const signed int ClassIDToIplDepth[] = {
        0,
        0,
        0,
        IPL_DEPTH_1U,
        0,
        IPL_DEPTH_32F,
        IPL_DEPTH_64F,
        IPL_DEPTH_8S,
        IPL_DEPTH_8U,
        IPL_DEPTH_16S,
        IPL_DEPTH_16U,
        IPL_DEPTH_32S,
        0,
        0,
        0,
        0
    };
    return ClassIDToIplDepth[classid];
}

/**
* Get mxClassID from cvMat type
*
* @param int cvMat type
* @return mxClassID
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
*/
CV_INLINE mxClassID cvmxClassIDFromCvDepth(int type)
{
    return cvmxClassIDFromIplDepth(cvCvToIplDepth(type));
}

/**
* Convert mxClassID to cvMat depth
*
* One may use CV_MAKETYPE(depth, nChannel) to create cvMat type
*
* @param mxClassID classid
* @return int cvMat depth
* @see cvmxClassIDFromCvDepth
* @see cvmxClassIDToCvDepth
* @see cvmxClassIDFromIplDepth
* @see cvmxClassIDToIplDepth
* @see cvmxIplToCvDepth
* @see cvCvToIplDepth
*/
CV_INLINE int cvmxClassIDToCvDepth(mxClassID classid)
{
    return cvmxIplToCvDepth(cvmxClassIDToIplDepth(classid));
}


/**
* Convert CvArr to mxArray. Allocate memory too
*
* Currently support only uint8, float, double
* Currently support only 1 channel (grayscale) and 3 channels (rgb etc)
*
* @param CvArr* arr
* @return mxArray*
*/
CVAPI(mxArray*) cvmxArrayFromCvArr(const CvArr* arr)
{
    CV_FUNCNAME( "cvmxArraFromCvArr" );
    IplImage matstub, *mat = (IplImage*)arr;
    int coi = 0;
    int nChannel, nRow, nCol, nDim, dims[3];
    mxClassID classid; mxArray* mxarr = NULL;
    int row, col, z;

    __BEGIN__;
    if (!CV_IS_IMAGE(mat)) {
        CV_CALL(mat = cvGetImage(mat, &matstub));
    }
    //ToDo: cvMatND  //(CvMatND*) arr //CV_IS_MATND(mat)
    nChannel = mat->nChannels;
    nRow     = mat->height;
    nCol     = mat->width;
    nDim     = 2;
    classid  = cvmxClassIDFromIplDepth(mat->depth);
    dims[0]  = nRow; dims[1] = nCol; dims[2] = nChannel;
    if (nChannel > 1) nDim = 3;
    mxarr    = mxCreateNumericArray(nDim, dims, classid, mxREAL);

    if (classid == mxUINT8_CLASS) {
        unsigned char *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    mxData[dims[0] * dims[1] * z + dims[0] * col + row]
                        = ((unsigned char*)mat->imageData)[mat->widthStep * row + nChannel * col + z];
                }
            }
        }
    } else if (classid == mxDOUBLE_CLASS) {
        double *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    mxData[dims[0] * dims[1] * z + dims[0] * col + row]
                        = ((double*)mat->imageData)[mat->widthStep * row + nChannel * col + z];
                }
            }
        }
    } else if (classid == mxSINGLE_CLASS) {
        float *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    mxData[dims[0] * dims[1] * z + dims[0] * col + row]
                        = ((float*)mat->imageData)[mat->widthStep * row + nChannel * col + z];
                }
            }
        }
    }
    __END__;
    return mxarr;
}

/**
* Convert mxArray to CvArr. Allocate memory too
*
* Currently support only uint8, float, double
* Currently support only 1 channel (grayscale) and 3 channels (rgb etc)
*
* @param mxArray* mxarr
* @return CvArr*
*/
CVAPI(CvArr*) cvmxArrayToCvArr(const mxArray* mxarr)
{
    CV_FUNCNAME( "cvmxArrayToCvArr" );
    IplImage *mat = NULL;
    int depth, nChannel = 1;
    int nDim;
    const int *dims;
    mxClassID classid;
    int row, col, z;

    __BEGIN__;
    classid = mxGetClassID(mxarr);
    nDim = mxGetNumberOfDimensions(mxarr);
    dims = mxGetDimensions(mxarr);
    if (nDim >= 3) {
        nChannel = dims[2];
    }
    depth = cvmxClassIDToIplDepth(mxGetClassID(mxarr));
    mat = cvCreateImage(cvSize(dims[1], dims[0]), depth, nChannel);

    if (classid == mxUINT8_CLASS) {
        unsigned char *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    ((unsigned char*)mat->imageData)[mat->widthStep * row + nChannel * col + z]
                        = mxData[dims[0] * dims[1] * z + dims[0] * col + row];
                }
            }
        }
    } else if (classid == mxDOUBLE_CLASS) {
        double *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    ((double*)mat->imageData)[mat->widthStep * row + nChannel * col + z]
                        = mxData[dims[0] * dims[1] * z + dims[0] * col + row];
                }
            }
        }
    } else if (classid == mxSINGLE_CLASS) {
        float *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    ((float*)mat->imageData)[mat->widthStep * row + nChannel * col + z]
                        = mxData[dims[0] * dims[1] * z + dims[0] * col + row];
                }
            }
        }
    }
    __END__;
    return (CvArr*)mat;
}

/**
* Convert mxArray to CvArr. Allocate memory too
*
* Currently support only uint8, float, double
* Currently support only 1 channel (grayscale) and 3 channels (rgb etc)
*
* @param mxArray* mxarr
* @return IplImage*
*/
CV_INLINE IplImage* cvmxArrayToIplImage(const mxArray* mxarr)
{
    return cvmxArrayToCvArr(mxarr);
}

/**
* Convert mxArray to CvMat. Allocate memory too
*
* Currently support only uint8, float, double
* Currently support only 1 channel (grayscale) and 3 channels (rgb etc)
*
* @param mxArray* mxarr
* @return CvMat*
*/
CV_INLINE CvMat* cvmxArrayToCvMat(const mxArray* mxarr)
{
    int coi = 0;
    CvMat mathdr, *mat = cvmxArrayToCvArr(mxarr);
    if( !CV_IS_MAT(mat) )
    {
        mat = cvGetMat(mat, &mathdr, &coi, 0);
    }
    return mat;
}

/**
* Print mxArray 
*
* Currently support only uint8, float, double
* Currently support only upto 3-D. 
*
* @param mxArray* mxarr
* @return void
* @see mlfPrintMatrix
*/
CVAPI(void) cvmxPrint3DMatrix(const mxArray* mxarr)
{
    int nDim;
    const int *dims;
    int row, col, z, nChannel = 1;
    mxClassID classid;
    classid = mxGetClassID(mxarr);
    nDim = mxGetNumberOfDimensions(mxarr);
    dims = mxGetDimensions(mxarr);
    if (nDim >= 3) nChannel = dims[2];

    if (classid == mxUINT8_CLASS) {
        unsigned char *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    printf("%d ", mxData[
                        dims[0] * dims[1] * z + dims[0] * col + row]);
                }
                printf("\n");
            }
            printf("\n");
        }
    } else if (classid == mxDOUBLE_CLASS) {
        double *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    printf("%lf ", mxData[
                        dims[0] * dims[1] * z + dims[0] * col + row]);
                }
                printf("\n");
            }
            printf("\n");
        }
    } else if (classid == mxSINGLE_CLASS) {
        float *mxData = mxGetData(mxarr);
        for (z = 0; z < nChannel; z++) {
            for (row = 0; row < dims[0]; row++) {
                for (col = 0; col < dims[1]; col++) {
                    printf("%lf ", mxData[
                        dims[0] * dims[1] * z + dims[0] * col + row]);
                }
                printf("\n");
            }
            printf("\n");
        }
    }
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif