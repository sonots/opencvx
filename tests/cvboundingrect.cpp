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
#include "../cvrect32f.h"

int
main (int argc, char **argv)
{
    CvMemStorage *storage = cvCreateMemStorage( 0 );
    CvSeq *points = cvCreateSeq( CV_SEQ_ELTYPE_POINT, sizeof( CvSeq ), sizeof( CvPoint ), storage );
    cvSeqPush( points, &cvPoint( 100, 50 ) );
    cvSeqPush( points, &cvPoint( 50, 100 ) );
    cvSeqPush( points, &cvPoint( 50, 150 ) );
    cvSeqPush( points, &cvPoint( 150, 50 ) );

    CvRect rect = cvBoundingRect( points );
    cvPrintRect( rect );

    cvReleaseMemStorage( &storage );
    return 0;
}
