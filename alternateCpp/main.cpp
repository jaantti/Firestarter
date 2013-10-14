#include <cv.h>
#include <highgui.h>
#include <unistd.h>
#include "rs232.h"
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include "capture.h"
#include "segmentation.h"

using namespace cv;
using namespace std;

int baudrate = 115200;
int motor1 = 0; //parem
int motor2 = 1; //vasak   24 /dev/ttyACM0, 25 /dev/ttyACM1
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

void coil_charge();
void coil_ping();
void coil_boom();
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

	namedWindow("aken");
    SEGMENTATION segm(640, 480);
    VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    Mat img;
    unsigned char *data = NULL;

    coil_charge();

    while (true){

        coil_ping();

        capture >> img;
        data = img.data;
        segm.readThresholds("conf");

        segm.thresholdImage(data);
    	segm.EncodeRuns();
    	segm.ConnectComponents();
    	segm.ExtractRegions();
    	segm.SeparateRegions();
    	segm.SortRegions();

		RS232_cputs(motor1, "gb\n");

		int x1, x2, y1, y2;
        struct region *tempRegion=NULL;
        int bloobinates[5][4];
		if(segm.colors[ORANGE].list!=NULL){
            tempRegion = segm.colors[ORANGE].list;
		}
        for(int i = 0; i<6; i++){
            if(tempRegion == NULL) break;
            x1 = tempRegion->x1;
            y1 = tempRegion->y1;
            x2 = tempRegion->x2;
            y2 = tempRegion->y2;
            rectangle(img, Point(x1, y1), Point(x2, y2), Scalar(0,0,255));

            bloobinates[0][0] = x1;
            bloobinates[0][1] = x2;
            bloobinates[0][2] = y1;
            bloobinates[0][3] = y2;
            tempRegion = tempRegion->next;
        }

        imshow("aken", img);

        findBall(bloobinates[0][1]-bloobinates[0][0], bloobinates[0][3]-bloobinates[0][2]);


        if (waitKey(10) == 27) break;

    }


    close_serial();
    return 0;
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


unsigned char *serial_read(int id){
    unsigned char *buf = NULL;
    buf = (unsigned char*)malloc(15);
    memset(buf, '\0', 15);
    int n = RS232_PollComport(id, buf, 14);
    printf("%s %d\n",buf,n);
    if (n) {
        return buf;
    }
    else{
        //cout << "NEIN NEIN NEIN!";
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
    unsigned char *m1 = NULL;
	m1 = (unsigned char*)malloc(15);
	memset(m1, '\0', 15);
	m1 = serial_read(motor1);

	RS232_cputs(motor2, "?\n");
    unsigned char *m2 = NULL;
	m2 = (unsigned char*)malloc(15);
	memset(m1, '\0', 15);
	m2 = serial_read(motor2);

    RS232_cputs(coil, "?\n");
    unsigned char *c0 = NULL;
	c0 = (unsigned char*)malloc(15);
	memset(m1, '\0', 15);
	c0 = serial_read(coil);
    //RS232_cputs(motor2, "?\n");
    //unsigned char *m2 = serial_read(motor2);

    //printf("%s %s %s\n",m1,m2,c0);

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

void coil_boom(){
    RS232_cputs(coil, "k1000\n");
    sleep(0.1);

}

void coil_ping(){
    RS232_cputs(coil, "p\n");
}

void coil_charge(){
    RS232_cputs(coil, "c\n");
}

