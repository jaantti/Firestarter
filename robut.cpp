#include <cv.h>
#include <highgui.h>

using namespace cv;
using namespace std;

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
int blue_t4[] = {112, 53, 79, 120, 209, 97};
int green_t4[] = {35, 136, 0, 55, 255, 255};
int black_t4[] = {17, 0, 0, 41, 255, 138};

int main(){

    VideoCapture capture(0);
    capture.set(3, 320);
    capture.set(4, 240);

    return 0;
}

OutputArray thresholdedImg(Mat img, int* colour){
    //reads an image from the feed and thresholds it using the provided min/max HSV values'''
    Mat img_thresholded;

    int thr_min[] = {colour[0], colour[1], colour[2]};
    int thr_max[] = {colour[3], colour[4], colour[5]};

    Mat thresh_MIN = Mat(1, 3, CV_32FC1, &thr_min);
    Mat thresh_MAX = Mat(1, 3, CV_32FC1, &thr_max);

    inRange(img, thresh_MIN, thresh_MAX, img_thresholded);

    //cv2.imshow('thresholded', img_thresholded) #show picture for calibrating
    return img_thresholded;
}

int* findBlobCenter(Mat img_thresholded, int minSize, Mat img){
    //using a binary image finds the center of the biggest blob and returns it's coordinates as a tuple'''
    //OutputArrayOfArrays contours;
    //OutputArray hierarchy;
    Mat contours;
    Mat hierarchy;

    findContours(img_thresholded, contours, hierarchy, RETR_TREE, CHAIN_APPROX_TC89_KCOS);

    //finds the biggest blob
    int bigsize = 0;
    int biggest = -1;

    Size s = contours.size();

    for (int i = s.width; i >= 0; i--){
        int size_ = (int) contourArea(contours.at<float>(0,i));

        if (size_ >= minSize && size_ > bigsize){
            bigsize = size_;
            biggest = i;
        }
    }

    if (biggest != -1){
        Rect rect = boundingRect(contours.at<float>(0, biggest));
        int centroid_x = (int)(rect.x + rect.width/2);
        int centroid_y = (int)(rect.y + rect.height/2);

        int out[] = {centroid_x, centroid_y, bigsize};
        return out;
    }
    else return 0;

}
