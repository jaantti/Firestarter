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
#include <sys/time.h>

#define MIN_AREA 10000
#define CURRENT_GATE 1

using namespace cv;
using namespace std;

int baudrate = 115200;
int motor1 = 24; //parem
int motor2 = 25; //vasak   24 /dev/ttyACM0, 25 /dev/ttyACM1
int coil = 2; // /dev/ttyACM2

double rel_pos = 0;
double last_gate_pos = 0;
int rel_pos_ball = 0;
int count_ = 0;
int count_goal_find = 0;
int count_goal = 0;
int c = 0;
int switch_ = 0;
bool was_close = false;
bool ball_timeout_f = false;
//fb_list = [(255, 255), (255, 255), (255, 255), (255, 255)]
int fb[] = {0, 0};
int spd1 = 0;
int spd2 = 0;
bool ball_in = false;

struct timeval start, end;
long mtime, seconds, useconds, fps;


void coil_charge();
void coil_ping();
void coil_boom();
bool init_serial_dev();
void close_serial();
unsigned char *serial_read(int);
Mat thresholdedImg(Mat, int*);
vector<Point2f> findBlobCenter(Mat, double);
bool compareContourAreas (vector<Point>, vector<Point>);
void findBall(float, float);
void findGate(double rel_pos_gate);
void write_spd(int write1, int write2);
void ball_timeout(SEGMENTATION * segm, int last_y_size, int last_b_size, bool b_set, bool y_set, bool last_drive);
void drive_ball_timeout(SEGMENTATION * segm, bool gate_select, bool last_drive);
void back_off();
char getBall();


int main(){
    int last_yellow_size=0, last_blue_size=0; // for timeout function (ball)
    // last_drive determines which way the robot last turned during timeout. false for left, true for right.
    bool y_set=false, b_set=false, last_drive=false; //for timeout function (ball)
    int count = 0;
    init_serial_dev();

	namedWindow("aken");
    SEGMENTATION segm(640, 480);
    SEGMENTATION * segment = &segm;
    VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    Mat img;
    unsigned char *data = NULL;
    gettimeofday(&start, NULL);
    coil_charge();
    Mat frame;
    while (true){
        count = count+1;
        coil_ping();

        capture >> img;
        cvtColor(img, frame, CV_BGR2YUV);
        data = frame.data;
        segm.readThresholds("conf");

        segm.thresholdImage(data);
    	segm.EncodeRuns();
    	segm.ConnectComponents();
    	segm.ExtractRegions();
    	segm.SeparateRegions();
    	segm.SortRegions();
        gettimeofday(&end, NULL);
		RS232_cputs(motor1, "gb\n");
        double rel_pos_gate=0;
		int x1, x2, y1, y2;
        struct region *tempRegion=NULL;
        int bloobinates[2][5][2];
        if(!ball_timeout_f){
            if(b_set && y_set){
                b_set = false;
                y_set = false;
            }
            if(segm.colors[ORANGE].list!=NULL){
                tempRegion = segm.colors[ORANGE].list;
                bloobinates[0][0][0] = tempRegion->cen_x;
                bloobinates[0][0][1] = tempRegion->cen_y;
            //tempRegion = tempRegion->next;
                rectangle(img, Point(tempRegion->x1, tempRegion->y1), Point(tempRegion->x2, tempRegion->y2), Scalar(255,0,0), 2);
            //cout << tempRegion->area << endl;
                circle(img, Point(bloobinates[0][0][0], bloobinates[0][0][1]), 5, Scalar(255,0,255), -1);
                gettimeofday(&start, NULL);
            } else {
                gettimeofday(&end, NULL);
                seconds  = end.tv_sec  - start.tv_sec;
                useconds = end.tv_usec - start.tv_usec;

                mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
                if(mtime>2500){
                    ball_timeout_f = true;
                    b_set = false;
                    y_set = false;
                    last_drive = false;
                }
            }
            if (getBall()-'0') {
                findBall(bloobinates[0][0][0], bloobinates[0][0][1]);
            } else {
                // TODO if
                rel_pos_gate = (bloobinates[1][0][0]-320)/320;
                last_gate_pos= (bloobinates[1][0][0]-320)/320;
                findGate(rel_pos_gate);
            }
        } else {
            ball_timeout(segment, last_yellow_size, last_blue_size, b_set, y_set, last_drive);
        }


        imshow("aken", img);
        if (waitKey(10) == 27) break;
    }
    close_serial();
    return 0;
}

void findBall(float x, float y){

    int max_spd = 40, slower_by = 20;

    rel_pos = (x-320)/320;
    cout << rel_pos << endl;
    if (rel_pos > 0) { //blob right of center
        write_spd(max_spd - (int)(rel_pos*slower_by), max_spd);
    }
    else if (rel_pos < 0) { //blob left of center
        write_spd(max_spd, max_spd - (int)(rel_pos*slower_by));
    }
    else { //blob exactly in the middle
        write_spd(max_spd, max_spd);
    }
}

void findGate(double rel_pos_gate){
    int max_spd = 40, slower_by=20;
    if(rel_pos_gate==0){
        if (last_gate_pos && last_gate_pos > 0) { //blob last seen right of center
            write_spd(-max_spd, max_spd);
        }
        else { //blob last seen left of center or not seen at all
            write_spd(max_spd, -max_spd);
        }
    }
    else {
        if (rel_pos_gate > 0.05) { //blob right of center
            write_spd(max_spd - (int)(rel_pos_gate*slower_by), -max_spd);
        }
        else if (rel_pos_gate < 0.05) { //blob left of center
            write_spd(max_spd, -max_spd - (int)(rel_pos_gate*slower_by));
        }
        else { //blob in the middle 10% of vision
            usleep(100);
            coil_boom();
        }
    }
}
void write_spd(int write1, int write2){
    stringstream ss1, ss2;

    ss1 << "sd" << write1 << "\n";
    ss2 << "sd" << -write2 << "\n";

    RS232_cputs(motor1, ss1.str().c_str());
    RS232_cputs(motor2, ss2.str().c_str());
}
//For determining the gate that is further away and driving towards it.
void ball_timeout(SEGMENTATION * segm, int last_y_size, int last_b_size, bool b_set, bool y_set, bool last_drive){
    cout<< "STOP, TIMEOUTTIME" << endl;
    struct region *blue_gate, *yellow_gate;
    bool drive_towards = false; // false for yellow, true for blue
    int max_spd = 40;
    if(segm->colors[ORANGE].list!=NULL){
        ball_timeout_f=false;
        return;
    }
    if(segm->colors[BLUE].list!=NULL && !b_set){
        blue_gate = segm->colors[BLUE].list;
        if(blue_gate->cen_x>320){
            write_spd(max_spd, -max_spd);
            if(last_b_size<blue_gate->area){
                last_b_size = blue_gate->area;
                last_drive = true;
            } else {
                b_set = true;
                last_drive = true;
            }
        } else {
            write_spd(-max_spd, max_spd);
            if(last_b_size<blue_gate->area){
                last_b_size = blue_gate->area;
                last_drive = false;
            } else {
                b_set = true;
                last_drive = false;
            }
        }
    } else if(segm->colors[YELLOW].list!=NULL && !y_set){
        yellow_gate = segm->colors[YELLOW].list;
        if(yellow_gate->cen_x>320){
            write_spd(max_spd, -max_spd);
            if(last_y_size<yellow_gate->area){
                last_y_size = yellow_gate->area;
                last_drive = true;
            } else {
                y_set = true;
                last_drive = true;
            }
        } else {
            write_spd(-max_spd, max_spd);
            if(last_y_size<yellow_gate->area){
                last_y_size = yellow_gate->area;
                last_drive = false;
            } else {
                y_set = true;
                last_drive = false;
            }
        }
    }
    if(b_set && y_set){
        if(last_y_size<last_b_size){
            drive_towards = true; // blue = true, yellow = false
        }
        drive_ball_timeout(segm , drive_towards, last_drive);
    }

}
//drives the robot towards the gate that is further away. last_drive is used to determine which way the robot was turning
// to ensure a smooth transition.
void drive_ball_timeout(SEGMENTATION * segm, bool gate_select, bool last_drive){
    int gate = 1, real_gate_pos=0, area = 0;
    int max_spd = 80, slower_by = 20;
    struct region *gate_reg;
    if(gate_select){
        gate = 2;
    }


    if(segm->colors[gate].list!=NULL){
        gate_reg = segm->colors[gate].list;
        area = gate_reg->area;
        float x = gate_reg->cen_x;
        real_gate_pos = (x-320)/320;
        if (real_gate_pos > 0.05) { //blob right of center
            write_spd(max_spd - (int)(real_gate_pos*slower_by), max_spd);
        }
        else if (real_gate_pos < 0.05) { //blob left of center
            write_spd(max_spd, max_spd - (int)(real_gate_pos*slower_by));
        }
        else { //blob in the middle
            write_spd(max_spd, max_spd);
        }
        if(area>MIN_AREA){
            ball_timeout_f = false;
        }
    } else {
        //False left, true right
        if(last_drive){
            write_spd(max_spd, -max_spd);
        } else {
            write_spd(-max_spd, max_spd);
        }
    }

}

void back_off(){
    for(int i=0; i<2; i++){
        write_spd(-10,-10);
    }
}
char getBall(){
    unsigned char buf[11];
    RS232_PollComport(motor2, buf, 10);
    RS232_cputs(motor2, "gb\n");
    if (buf[3]) ball_in = true;
    else ball_in = false;
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
        return NULL;
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
	memset(m2, '\0', 15);
	m2 = serial_read(motor2);

    //RS232_cputs(motor2, "?\n");
    //unsigned char *m2 = serial_read(motor2);

    //printf("%s %s %s\n",m1,m2,c0);

    bool out_status = true;

    if(m1 && m2){
        if(m1[4]=='0'){
            if(m2[4]=='1'){
                coil = 2;
            }
            else{
                coil = 25;
                motor2 = 2;
            }
        }
        else if(m1[4] == '1'){
            if(m2[4] == '0'){
                motor1 = 25;
                motor2 = 24;
                coil = 2;
            }
            else{
                motor1 = 2;
                motor2 = 24;
                coil = 25;
            }
        }
        else{
            if(m2[4] == '1'){
                motor1 = 2;
                coil = 24;
            }
            else{
                motor1 = 2;
                motor2 = 25;
                coil = 24;
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

