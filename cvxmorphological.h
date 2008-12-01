/**
// cvxmorphological.h
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
#ifndef CV_MORPHOLOGICAL_INCLUDED
#define CV_MORPHOLOGICAL_INCLUDED

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4996 )
#pragma comment( lib, "cv.lib" )
#pragma comment( lib, "cxcore.lib" )
#pragma comment( lib, "cvaux.lib" )
#endif

#include "cv.h"
#include "cvaux.h"

#include "cvopening.h"
#include "cvclosing.h"
#include "cvsandwichfill.h"

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
