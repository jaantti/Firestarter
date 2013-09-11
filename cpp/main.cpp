#include <cv.h>
#include <highgui.h>
#include <unistd.h>
#include <rs232.h>

using namespace cv;
using namespace std;

int baudrate = 9600;
int serialport = 24; //24 /dev/ttyACM0, 25 /dev/ttyACM1

int rel_pos = 0;
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
vector<Point2f> findBlobCenter(Mat, int);
bool compareContourAreas (vector<Point>, vector<Point>);

int main(){

    if(RS232_OpenComport(serialport, baudrate))
    {
        cout << "Can not open comport\n";

    return(0);
    }

    RS232_cputs(serialport, "?\n");

    unsigned char buf[11];

    usleep(100000);

    int n = RS232_PollComport(serialport, buf, 10);

    if (n) {
        cout << "n: " << n << endl;
        cout << buf << endl;
    }

    RS232_CloseComport(serialport);

    namedWindow("aken");
    namedWindow("aken2");

    VideoCapture capture(1);
//    capture.set(CV_CAP_PROP_FRAME_WIDTH, 320.0);
//    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240.0);

    Mat img, img_hsv;


    while (!true){
        //cout <<"1"<<endl;
        capture >> img;
        //cout <<"2"<<endl;
        cvtColor(img, img_hsv, CV_BGR2HSV);
        //cout <<"3"<<endl;
        //imshow("aken2", img_hsv);
        //cout <<"4"<<endl;
        Mat tr_img = thresholdedImg(img_hsv, blue_t4);
        imshow("aken", tr_img);

        vector<Point2f> point = findBlobCenter(tr_img, 50);

        cout << point << endl;


        if (waitKey(10) == 27) break;

    }

    return 0;
}

Mat thresholdedImg(const Mat img, int* colour){
    //reads an image from the feed and thresholds it using the provided min/max HSV values'''
    Mat img_thresholded = img.clone();

    inRange(img, Scalar(colour[0], colour[1], colour[2]), Scalar(colour[3], colour[4], colour[5]), img_thresholded);

    //imshow("aken", img_thresholded); //show picture for calibrating
    return img_thresholded;
}

vector<Point2f> findBlobCenter(Mat img_thresholded, int minSize){
    //using a binary image finds the center of the biggest blob and returns it's coordinates as a tuple'''
    //OutputArrayOfArrays contours;
    //OutputArray hierarchy;

    //Mat contours;
    //Mat hierarchy;

    //vector<vector<Point> > contours;
    //vector<Vec4i> hierarchy;

    // find contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(img_thresholded, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

    // sort contours
    sort(contours.begin(), contours.end(), compareContourAreas);

    // grab contours
    vector<Point> biggestContour = contours[contours.size()-1];

    double area = contourArea(Mat(biggestContour));

    vector<Moments> mu(1);
    mu[0] = moments(biggestContour, false);
    vector<Point2f> mc(1);
    mc[0] = Point2f(mu[0].m10/mu[0].m00, mu[0].m01/mu[0].m00);

    return mc;

}

bool compareContourAreas (vector<Point> contour1, vector<Point> contour2 ) {
    double i = fabs( contourArea(Mat(contour1)) );
    double j = fabs( contourArea(Mat(contour2)) );
    return ( i < j );
}
