/**
// cvskincolorpeer.h
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
#ifndef CV_SKINCOLOR_PEER_INCLUDED
#define CV_SKINCOLOR_PEER_INCLUDED

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#endif

#include "cv.h"
#include "cvaux.h"

void cvSkinColorPeer( const IplImage* img, IplImage* mask );

/**
// cvSkinColorPeer - Skin Color Detection by Peer, et.al [1]
//
// @param img Input image
// @param mask Generated mask image. 1 for skin and 0 for others
// 
// References)
//  [1] P. Peer, J. Kovac, J. and F. Solina, ”Human skin colour
//  clustering for face detection”, In: submitted to EUROCON –
//  International Conference on Computer as a Tool , 2003.
//     (R>95)^(G>40)^(B>20)^
//     (max{R,G,B}-min{R,G,B}>15)^
//     (|R -G|>15)^(R>G)^(R>B)
*/
void cvSkinColorPeer( const IplImage* img, IplImage* mask )
{
    cvSet( mask, cvScalarAll(0) );
    for( int y = 0; y < img->height; y++ )
    {
        for( int x = 0; x < img->width; x++ )
        {
            uchar b = img->imageData[img->widthStep * y + x * 3];
            uchar g = img->imageData[img->widthStep * y + x * 3 + 1];
            uchar r = img->imageData[img->widthStep * y + x * 3 + 2];

            if( r > 95 && g > 40 && b > 20 && 
                max( max( r, g ), b ) - min( min( r, g ), b ) > 15 &&
                abs( r - g ) > 15 && r > g && r > b )
            {
                mask->imageData[mask->widthStep * y + x] = 1;
            }
        }
    }
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
