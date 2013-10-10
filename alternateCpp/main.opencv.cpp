#include <cv.h>
#include <highgui.h>
#include <unistd.h>
#include "rs232.h"
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <cstdlib>

using namespace cv;
using namespace std;

int baudrate = 115200;
int motor1 = 24; //parem
int motor2 = 25; //vasak   24 /dev/ttyACM0, 25 /dev/ttyACM1
int coil = 2; // /dev/ttyACM2

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

bool init_serial_dev();
void close_serial();
unsigned char *serial_read(int);
Mat thresholdedImg(Mat, int*);
vector<Point2f> findBlobCenter(Mat, double);
bool compareContourAreas (vector<Point>, vector<Point>);
void findBall(double, double);
void write_spd(int write1, int write2);
char getBall();


int main(){

    init_serial_dev();
    //RS232_cputs(serialport, "?\n");
    //Lugemise aeg
    /*unsigned char buf[11];
    usleep(100000);
    int n = RS232_PollComport(serialport, buf, 10);
    if (n) {
        cout << "n: " << n << endl;
        cout << buf << endl;
    }*/

    namedWindow("aken");
    namedWindow("aken2");

    VideoCapture capture(0);

    Mat img, img_hsv;

    RS232_cputs(motor1, "gb\n");

    while (true){

        capture >> img;
        cvtColor(img, img_hsv, CV_BGR2HSV);
        imshow("aken2", img_hsv);
        Mat tr_img = thresholdedImg(img_hsv, orange_t4);
        imshow("aken", tr_img);

        vector<Point2f> point = findBlobCenter(tr_img, 5.0);

        cout << point[0].x  << ", " << point[0].y << endl;
        getBall();
        findBall(point[0].x, point[0].y);


        if (waitKey(10) == 27) break;

    }

    close_serial();
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

char getBall(){
    unsigned char buf[11];
    RS232_PollComport(motor2, buf, 10);
    RS232_cputs(motor2, "gb\n");
    return buf[3];
}

void boom(){

}

unsigned char *serial_read(int id){
    unsigned char buf[15];
    int n = RS232_PollComport(id, buf, 15);
    if (n) {
        return buf;
    }
    else{
        return 0;
    }
}

bool init_serial_dev(){

    if(RS232_OpenComport(motor1, baudrate))
    {
        cout << "Can not open /dev/ttyACM0\n";
        return(false);
    }
    if(RS232_OpenComport(motor2, baudrate)){
        cout << "Can not open /dev/ttyACM1\n";
        return(false);
    }
    if(RS232_OpenComport(coil, baudrate)){
        cout << "Can not open /dev/ttyACM2\n";
        return(false);
    }

    RS232_cputs(motor1, "?\n");
    unsigned char *m1 = serial_read(motor1);
    RS232_cputs(motor2, "?\n");
    unsigned char *m2 = serial_read(motor2);
    RS232_cputs(coil, "?\n");
    unsigned char *c0 = serial_read(coil);
    bool out_status = true;

    if(m1 && m2 && c0){
        if(m1[4]=='0'){
            if(m2[4]=='1'){
                coil = 2;
            }
            else{
                coil = 1;
                motor2 = 2;
            }
        }
        else if(m1[4] == '1'){
            if(m2[4] == '0'){
                motor1 = 1;
                motor2 = 0;
                coil = 2;
            }
            else{
                motor1 = 2;
                motor2 = 0;
                coil = 1;
            }
        }
        else{
            if(m2[4] == '1'){
                motor1 = 2;
                coil = 0;
            }
            else{
                motor1 = 2;
                motor2 = 1;
                coil = 0;
            }
        }
    }
    else out_status = false;

    return out_status;
}

void close_serial(){
    RS232_CloseComport(motor1);
    RS232_CloseComport(motor2);
    RS232_CloseComport(coil);
}
