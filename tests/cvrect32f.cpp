#ifdef _MSC_VER
#pragma warning(disable:4996)
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#endif

#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>
#include "cvrect32f.h"

int main(int argc, char** argv)
{
    CvRect32f rect32f = cvRect32f( 10, 15, 30, 40, 45 );
    CvBox32f box32f = cvBox32fFromRect32f( rect32f );
    cvPrintBox32f( box32f );
    rect32f = cvRect32fFromBox32f( box32f );
    cvPrintRect32f( rect32f );
    CvBox2D box2d = cvBox2DFromRect32f( rect32f );
    cvPrintBox2D( box2d );
}
