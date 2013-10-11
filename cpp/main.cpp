#include <cv.h>
#include <highgui.h>
#include <unistd.h>
#include "rs232.h"
#include <sstream>
#include "capture.h"
#include "segmentation.h"
using namespace cv;
using namespace std;

int baudrate = 9600;
int motor1 = 24; //parem
int motor2 = 25; //vasak   24 /dev/ttyACM0, 25 /dev/ttyACM1

double rel_pos = 0;
int rel_pos_ball = 0;
int count_ = 0;
int count_goal_find = 0;
int count_goal = 0;
int c = 0;
int switch_ = 0;
bool was_close = false;
//fb_list = [(255, 255), (255, 255), (255, 255), (255, 255)]
int fb[] = {0, 0};
int spd1 = 0;
int spd2 = 0;

int orange_tty[] = {8, 138, 43, 17, 255, 255};
int yellow_tty[] = {34, 196, 92, 41, 255, 165};
int blue_tty[] = {92, 66, 32, 108, 205, 57};
int green_tty[] = {61, 138, 53, 73, 255, 210};
int black_tty[] = {63, 83, 0, 79, 143, 95};

int orange_t4[] = {6, 198, 193, 18, 255, 255};
int yellow_t4[] = {23, 236, 171, 27, 255, 255};
int blue_t4[] = {0, 0, 0, 127, 127, 255};
int green_t4[] = {35, 136, 0, 55, 255, 255};
int black_t4[] = {17, 0, 0, 41, 255, 138};

Mat thresholdedImg(Mat, int*);
vector<Point2f> findBlobCenter(Mat, double);
bool compareContourAreas (vector<Point>, vector<Point>);
void findBall(double, double);
void write_spd(int write1, int write2);

string to_string (int Number ){
    ostringstream ss;
    ss << Number;
    return ss.str();
}

int main(){

    //init motors
   /* if(RS232_OpenComport(motor1, baudrate))
    {
        cout << "Can not open /dev/ttyACM0\n";
        return(0);
    }
    if(RS232_OpenComport(motor2, baudrate)){
        cout << "Can not open /dev/ttyACM1\n";
        return(0);
    }
*/

    //RS232_cputs(serialport, "?\n");

    /*unsigned char buf[11];
    usleep(100000);
    int n = RS232_PollComport(serialport, buf, 10);
    if (n) {
        cout << "n: " << n << endl;
        cout << buf << endl;
    }*/

    namedWindow("aken");
    namedWindow("aken2");
    SEGMENTATION segm(640, 480);
    VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    Mat img, img_hsv;
    unsigned char *data;
    while (true){
        capture >> img;
        data = img.data;
        segm.readThresholds("conf");

        segm.thresholdImage( data );
    	segm.EncodeRuns();
    	segm.ConnectComponents();
    	segm.ExtractRegions();
    	segm.SeparateRegions();
    	segm.SortRegions();

		int x1, x2, y1, y2;
		if(segm.colors[ORANGE].list!=NULL){
		x1 = segm.colors[ORANGE].list->x1;
		y1 = segm.colors[ORANGE].list->y1;
		x2 = segm.colors[ORANGE].list->x2;
		y2 = segm.colors[ORANGE].list->y2;
        //cout << segm.colors[ORANGE].list->next << endl;
		cv::rectangle( img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0,0,255) );
        region *nextReg = segm.colors[ORANGE].list->next;

        for(int i = 0; i<segm.colors[ORANGE].num-1; i++){
            int x3, y3, x4, y4;
            x3 = nextReg->x1;
            y3 = nextReg->y1;
            x4 = nextReg->x2;
            y4 = nextReg->y2;
            nextReg = nextReg->next;
        //cout << segm.colors[ORANGE].list->next << endl;
		cv::rectangle( img, cv::Point(x3, y3), cv::Point(x4, y4), cv::Scalar(0,0,255) );
        }
		}


        //cout<<segm.colors[ORANGE].num << endl;
        cvtColor(img, img_hsv, CV_BGR2HSV);
        imshow("aken", img_hsv);
        //Mat tr_img = thresholdedImg(img_hsv, orange_t4);
        imshow("aken2", img);

        //vector<Point2f> point = findBlobCenter(tr_img, 5.0);

        //cout << point[0].x  << ", " << point[0].y << endl;
        //findBall(point[0].x, point[0].y);

        if (waitKey(10) == 27) break;

    }
  //  RS232_CloseComport(motor1);
    //RS232_CloseComport(motor2);

    return 0;
}

Mat thresholdedImg(const Mat img, int* colour){
    //reads an image from the feed and thresholds it using the provided min/max HSV values'''
    Mat img_thresholded = img.clone();

    inRange(img, Scalar(colour[0], colour[1], colour[2]), Scalar(colour[3], colour[4], colour[5]), img_thresholded);

    //imshow("aken", img_thresholded); //show picture for calibrating
    return img_thresholded;
}

vector<Point2f> findBlobCenter(Mat img_thresholded, double minSize){
    //using a binary image finds the center of the biggest blob and returns it's coordinates as a tuple'''



    // find contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(img_thresholded, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

    if (!contours.empty()){
        sort(contours.begin(), contours.end(), compareContourAreas);

        // grab contours
        vector<Point> biggestContour = contours[contours.size()-1];

        double area = contourArea(Mat(biggestContour));

        if (area >= minSize){
            vector<Moments> mu(1);
            mu[0] = moments(biggestContour, false);
            vector<Point2f> mc(1);
            mc[0] = Point2f(mu[0].m10/mu[0].m00, mu[0].m01/mu[0].m00);
            return mc;
        }
    }

    vector<Point2f> mc(1);
    return mc;

}

bool compareContourAreas (vector<Point> contour1, vector<Point> contour2 ) {
    double i = fabs( contourArea(Mat(contour1)) );
    double j = fabs( contourArea(Mat(contour2)) );
    return ( i < j );
}

void findBall(double x, double y){

    int max_spd = 40, slower_by = 20;

    rel_pos = (x-320)/320;

    if (rel_pos > 0) { //blob right of center
        write_spd(max_spd - (int)(rel_pos*slower_by), -max_spd);
    }
    else if (rel_pos < 0) { //blob left of center
        write_spd(max_spd, -max_spd - (int)(rel_pos*slower_by));
    }
    else { //blob exactly in the middle
        write_spd(max_spd, -max_spd);
    }

    /*
    if (x >= 310 && x <= 330){
        RS232_cputs(motor1, "sd30\n");
        RS232_cputs(motor2, "sd-30\n");
    }
    if (x < 310){
        RS232_cputs(motor1, "sd30\n");
        RS232_cputs(motor2, "sd0\n");
    }
    if (x >330){
        RS232_cputs(motor1, "sd0\n");
        RS232_cputs(motor2, "sd-30\n");
    }*/

}

void write_spd(int write1, int write2){
    stringstream ss1, ss2;

    ss1 << "sd" << write1 << "\n";
    ss2 << "sd" << write2 << "\n";

    RS232_cputs(motor1, ss1.str().c_str());
    RS232_cputs(motor2, ss2.str().c_str());
}
