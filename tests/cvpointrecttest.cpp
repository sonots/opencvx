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
#include <time.h>
#include "cvpointrecttest.h"
#include <iostream>
using namespace std;

int
main (int argc, char **argv)
{
    CvRect32f rect = cvRect32f( -10, -10, 30, 30, 45 );
    CvPoint2D32f pt = cvPoint2D32f( 400, 0 );
    cout << cvPointRect32fTest( rect, pt, 1 ) << endl;
    return 0;
}
