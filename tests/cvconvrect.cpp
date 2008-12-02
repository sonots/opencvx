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
#include "../cvconvrect.h"
#include "../cvxrectangle.h"

int main( int argc, char** argv )
{
    CvRect rect = cvRect( 10, 8, 3, 5 );
    rect = cvConvRect( rect, 0.0, RECT_NORMAL, RECT_CENTER );
    cvPrintRect( rect );
    rect = cvConvRect( rect, 0.0, RECT_CENTER, RECT_NORMAL );
    cvPrintRect( rect );
    rect = cvConvRect( rect, 45.0, RECT_NORMAL, RECT_CENTER );
    cvPrintRect( rect );
    rect = cvConvRect( rect, 45.0, RECT_CENTER, RECT_NORMAL );
    cvPrintRect( rect );
    return 0;
}
/*
11 10 3 5
10 8 3 5
12 8 3 5
9 7 3 5 (because of integer casting)
*/

