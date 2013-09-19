#include <cv.h>
#include <highgui.h>

///Parem hiireklahv väljastab pixli HSV väärtuse, vasak klahv väljastab praegused treshi sätted.

using namespace cv;
using namespace std;

const int maximum_h = 179;
const int maximum_s = 255;
const int maximum_v = 255;

int h_min;
int s_min;
int v_min;
int h_max;
int s_max;
int v_max;

int h;
int s;
int v;

Mat thresholdedImg(Mat, int*);
Mat img, img_hsv;

void mouseEvent(int evt, int x, int y, int flags, void* param){
    if(evt==CV_EVENT_LBUTTONDOWN){
        //printf("%d %d\n",x,y);
        h = img.at<Vec3b>(y,x)[0];
        s = img.at<Vec3b>(y,x)[1];
        v = img.at<Vec3b>(y,x)[2];
        printf("%d %d %d\n",h,s,v);
    }
    if(evt==CV_EVENT_RBUTTONDOWN){
        printf("%d %d %d %d %d %d\n",h_min, s_min, v_min, h_max, s_max, v_max);
    }
}

int main( int argc, char** argv ){

    VideoCapture capture(0);

 /// Initialize values
    h_min = 0;
    s_min = 0;
    v_min = 0;
    h_max = 0;
    s_max = 0;
    v_max = 0;

 namedWindow("Aken",1);
 namedWindow("Värv");

 createTrackbar( "H min", "Aken", &h_min, maximum_h, NULL);
 createTrackbar( "S min", "Aken", &s_min, maximum_s, NULL);
 createTrackbar( "V min", "Aken", &v_min, maximum_v, NULL);

 createTrackbar( "H max", "Aken", &h_max, maximum_h, NULL);
 createTrackbar( "S max", "Aken", &s_max, maximum_s, NULL);
 createTrackbar( "V max", "Aken", &v_max, maximum_v, NULL);

cvSetMouseCallback("Värv", mouseEvent, 0);

 while (true){
        capture >> img;
        cvtColor(img, img_hsv, CV_BGR2HSV);
        imshow("Värv", img);
        int tresh[] = {h_min, s_min, v_min, h_max, s_max, v_max};
        Mat tr_img = thresholdedImg(img_hsv, tresh);
        imshow("Aken", tr_img);
        if (waitKey(10) == 27) break;
    }
}

Mat thresholdedImg(const Mat img, int* colour){
    //reads an image from the feed and thresholds it using the provided min/max HSV values'''
    Mat img_thresholded = img.clone();

    inRange(img, Scalar(colour[0], colour[1], colour[2]), Scalar(colour[3], colour[4], colour[5]), img_thresholded);

    //imshow("aken", img_thresholded); //show picture for calibrating
    return img_thresholded;
}
