#ifdef _MSC_VER
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#pragma warning(disable: 4996)
#endif

#include <cv.h>
#include <highgui.h>
#include <ctype.h>
#include <stdio.h>

int
main (int argc, char **argv)
{
  CvCapture *capture = 0;
  IplImage *frame = 0;
  CvVideoWriter *writer;
  int c, num = 0;
  CvFont font;
  //char str[64];

  double fps, width, height;
  if ( argc == 1 || (argc >= 2 && strlen (argv[1]) == 1 && isdigit (argv[1][0])) )
  {
    capture = cvCreateCameraCapture (argc == 2 ? argv[1][0] - '0' : 0);
    fps     = 20.0;
    width   = 320;
    height  = 240;
    //fps     = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    //cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, width);
    //cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, height);
  }
  else if ( argc >= 2 )
  {
    capture = cvCaptureFromFile(argv[1]);
    fps     = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    width   = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    height  = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
  }
  printf ("fps=%f width=%f height=%f\n", fps, width, height);

  printf ("Write to cap.avi. Finish with Esc.\n");
  // FourCC http://www.fourcc.org/codecs.php
  //writer = cvCreateVideoWriter( "cap.avi", 
  //                              CV_FOURCC('D','I','V','X'),
  //                              fps, cvSize((int)width,(int)height) );
  writer = cvCreateVideoWriter( "cap.avi", -1,
      fps, cvSize((int)width,(int)height) );

  //cvInitFont (&font, CV_FONT_HERSHEY_COMPLEX, 0.7, 0.7);
  cvNamedWindow ("Capture", CV_WINDOW_AUTOSIZE);

  while (1) {
    frame = cvQueryFrame (capture);
    if( frame == NULL ) break;
    //snprintf (str, 64, "%03d[frame]", num);
    //cvPutText (frame, str, cvPoint (10, 20), &font, CV_RGB (0, 255, 100));
    cvWriteFrame (writer, frame);
    cvShowImage ("Capture", frame);
    num++;
    c = cvWaitKey (10);
    if (c == 'q') // exit
      break;
  }
  cvReleaseVideoWriter (&writer);
  cvReleaseCapture (&capture);
  cvDestroyWindow ("Capture");

  return 0;
}
