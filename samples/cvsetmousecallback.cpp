#ifdef _MSC_VER
#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cxcore.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "highgui.lib")
#pragma warning(disable: 4996)
#endif

#include <cv.h>
#include <highgui.h>
#include <stdio.h>

/* グローバル変数 */
CvFont font;
IplImage *img = 0;

/* プロトタイプ宣言 */
void on_mouse (int event, int x, int y, int flags, void *param);

int
main (int argc, char *argv[])
{
  int c;

  // (1)画像領域を確保し，初期化する
  img = cvCreateImage (cvSize (640, 480), IPL_DEPTH_8U, 3);
  cvZero (img);
  cvInitFont (&font, CV_FONT_HERSHEY_DUPLEX, 0.4, 0.4);

  // (2)ウィンドウを作成し，マウスイベントに対するコールバック関数を登録する
  cvNamedWindow ("Image", CV_WINDOW_AUTOSIZE);
  cvSetMouseCallback ("Image", on_mouse);
  cvShowImage ("Image", img);

  // (3)'Esc'キーが押された場合に終了する
  while (1) {
    c = cvWaitKey (0);
    printf("%d\n", c);
    if (c == '\x1b')
      return 1;
  }

  cvDestroyWindow ("Image");
  cvReleaseImage (&img);

  return 0;
}

/* コールバック関数 */
void
on_mouse (int event, int x, int y, int flags, void *param = NULL)
{
  char str[64];
  static int line = 0;
  const int max_line = 15, w = 15, h = 30;

  // (4)マウスイベントを取得
  switch (event) {
  case CV_EVENT_MOUSEMOVE:
    sprintf (str, "(%d,%d) %s", x, y, "MOUSE_MOVE");
    break;
  case CV_EVENT_LBUTTONDOWN:
    sprintf (str, "(%d,%d) %s", x, y, "LBUTTON_DOWN");
    break;
  case CV_EVENT_RBUTTONDOWN:
    sprintf (str, "(%d,%d) %s", x, y, "RBUTTON_DOWN");
    break;
  case CV_EVENT_MBUTTONDOWN:
    sprintf (str, "(%d,%d) %s", x, y, "MBUTTON_DOWN");
    break;
  case CV_EVENT_LBUTTONUP:
    sprintf (str, "(%d,%d) %s", x, y, "LBUTTON_UP");
    break;
  case CV_EVENT_RBUTTONUP:
    sprintf (str, "(%d,%d) %s", x, y, "RBUTTON_UP");
    break;
  case CV_EVENT_MBUTTONUP:
    sprintf (str, "(%d,%d) %s", x, y, "MBUTTON_UP");
    break;
  case CV_EVENT_LBUTTONDBLCLK:
    sprintf (str, "(%d,%d) %s", x, y, "LBUTTON_DOUBLE_CLICK");
    break;
  case CV_EVENT_RBUTTONDBLCLK:
    sprintf (str, "(%d,%d) %s", x, y, "RBUTTON_DOUBLE_CLICK");
    break;
  case CV_EVENT_MBUTTONDBLCLK:
    sprintf (str, "(%d,%d) %s", x, y, "MBUTTON_DOUBLE_CLICK");
    break;
  }

  // (5)マウスボタン，修飾キーを取得
  if (flags & CV_EVENT_FLAG_LBUTTON)
    strcat (str, " + LBUTTON");
  if (flags & CV_EVENT_FLAG_RBUTTON)
    strcat (str, " + RBUTTON");
  if (flags & CV_EVENT_FLAG_MBUTTON)
    strcat (str, " + MBUTTON");
  if (flags & CV_EVENT_FLAG_CTRLKEY)
    strcat (str, " + CTRL");
  if (flags & CV_EVENT_FLAG_SHIFTKEY)
    strcat (str, " + SHIFT");
  if (flags & CV_EVENT_FLAG_ALTKEY)
    strcat (str, " + ALT");

  // (6)マウス座標，イベント，修飾キーなどを画像に描画，表示
  if (line > max_line) {
    cvGetRectSubPix (img, img, cvPoint2D32f (320 - 0.5, 240 - 0.5 + h));
    cvPutText (img, str, cvPoint (w, 20 + h * max_line), &font, CV_RGB (0, 200, 100));
  }
  else {
    cvPutText (img, str, cvPoint (w, 20 + h * line), &font, CV_RGB (0, 200, 100));
  }
  line++;
  cvShowImage ("Image", img);
}
