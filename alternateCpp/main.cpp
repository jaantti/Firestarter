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
#include "main.h"
#include <thread>


using namespace cv;
using namespace std;

int motor1 = 24; //vasak
int motor2 = 25; //parem   24 /dev/ttyACM0, 25 /dev/ttyACM1
int coil = 2; // /dev/ttyACM2
int last_y_size=0, last_b_size=0; // for timeout function (ball)

double rel_pos = 0;
double last_gate_pos = 0;
bool last_ball_pos = false; // false for left

bool was_close = false;
bool ball_timeout_f = false;
// last_drive determines which way the robot last turned during timeout. false for left, true for right.
bool y_set=false, b_set=false, last_drive=false; //for timeout function (ball)
bool gate_select = false; // false for yellow, true for blue;
bool ball_kicked = false; //For experimental ball-lost-from-dribbler code
bool goal_timeout_f = false;
volatile bool last_ball_in = false;
bool ours = false;
bool theirs = false;
volatile bool ball_in = false;

blobs blob_data;

struct timeval start, end_time, start_goal;
long mtime, seconds, useconds, fps;



int main(){

    init_serial_dev();

    gettimeofday(&start, NULL);
    gettimeofday(&start_goal, NULL);
    coil_charge();

    SEGMENTATION segm(640, 480);
    thread t1(get_blobs, &segm);
    thread t2(getBall);


    while (true){
        coil_ping();
        usleep(100000);

        gettimeofday(&end_time, NULL);
        //cout << "Ball in " << ball_in << endl;
        if (
            blob_data.total_green > MINGREEN && blob_data.ATTACK(area) < 40000 && blob_data.DEFEND(area) < 40000) {
            if(!ball_in) {
                if(last_ball_in && ball_kicked ){
                    last_ball_in = false;
                    ball_kicked = false;
                } else if (last_ball_in) {
                    //backoffcodegoeshere
                    write_spd(0, 0);
                    usleep(10000);
                    write_spd(-30, -30);
                    usleep(500000);
                    last_ball_in = false;
                }
                if(!ball_timeout_f){
                    if(b_set && y_set){
                        b_set = false;
                        y_set = false;
                        last_y_size=0;
                        last_b_size=0;
                        cout<<"END TIMEOUT!"<<endl;
                        gettimeofday(&start, NULL);
                    }
                    if(blob_data.orange_area!=0){
                        gettimeofday(&start, NULL);
                        //cout<<"AREAZERO"<<endl;
                    }
                    else { // No balls currently visible
                        seconds  = end_time.tv_sec  - start.tv_sec;
                        useconds = end_time.tv_usec - start.tv_usec;


                        mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
                        cout<<mtime<<endl;
                        if(mtime>4000){
                            cout<< "STOP, TIMEOUTTIME" << endl;
                            //cout<<"Timeout!"<<mtime<<endl;
                            ball_timeout_f = true;
                            b_set = false;
                            y_set = false;
                            last_drive = false;
                        }
                    }
                    //cout << blob_data.orange_area << endl;
                    if (blob_data.orange_cen_y < 300) findBall(MAX_SPD, SLOWER_BY);
                    else findBall(30, 20);
                }
                else ball_timeout();
                gettimeofday(&start_goal, NULL); // Keep resetting goal_timeout when no ball in dribbler
            }
            else { // Ball in dribbler
                if(!goal_timeout_f){
                    //cout<<"Timeout!"<<mtime<<endl;
                    if (blob_data.ATTACK(area) != 0) {
                        gettimeofday(&start_goal, NULL);
                        goal_timeout_f = false;
                    }
                    else { // Own goal not in vision
                        seconds  = end_time.tv_sec  - start_goal.tv_sec;
                        useconds = end_time.tv_usec - start_goal.tv_usec;

                        cout<<mtime<<endl;
                        mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
                        if(mtime>4000){
                            cout<< "goalfinding timeout" << endl;
                            goal_timeout_f = true;
                        }
                    }
                    findGate();
                }
                else goal_timeout();
                gettimeofday(&start, NULL); // Keep resetting ball_timeout when ball is in dribbler
            }
        }
        else {
            cout << "Back off!" << endl;
            back_off();
        }
        // Last gate directions, true is right, false is left
        if (blob_data.ATTACK(cen_x) != 0 && blob_data.ATTACK(cen_x) < 320) theirs = false;
        else if (blob_data.ATTACK(cen_x)!= 0 && blob_data.ATTACK(cen_x)> 320) theirs = true;
        if (blob_data.DEFEND(cen_x) != 0 && blob_data.DEFEND(cen_x) < 320) ours = false;
        else if (blob_data.DEFEND(cen_x) != 0 && blob_data.DEFEND(cen_x) > 320) ours = true;
    }
    close_serial();
    return 0;
}

void findBall(int max_spd, int slower_by){

    rel_pos = (blob_data.orange_cen_x-320)/320.0;
    //cout << rel_pos << endl;
    if (rel_pos == -1) {
        if (last_ball_pos == true) {
            write_spd(-(int)(25),(int)(25));
            //cout << "going right" << endl;
        }
        else { //blob last seen left of center or not seen at all
            write_spd((int)(25), -(int)(25));
        }
    }
    else {
        if (rel_pos > 0) { //blob right of center
            write_spd(max_spd - (int)(rel_pos*slower_by), max_spd);
            last_ball_pos = true;
        }
        else if (rel_pos < 0) { //blob left of center
            write_spd(max_spd, max_spd + (int)(rel_pos*slower_by));
            last_ball_pos = false;
        }
        else { //blob exactly in the middle
            write_spd(max_spd, max_spd);
        }
    }
}

bool findGate(){
    double rel_pos_gate = (blob_data.ATTACK(cen_x)-320)/320.0;
    double left_edge = (blob_data.ATTACK(x1)-320)/320.0;
    double right_edge = (blob_data.ATTACK(x2)-320)/320.0;
    cout << left_edge << " " << right_edge << endl;
    if(rel_pos_gate == -1){
        if (ours == false) { //Own gate was last left vision on the left side of the screen
            write_spd(-(int)(25),(int)(25));
            //cout << "going right" << endl;
        }
        else { //blob last seen left of center or not seen at all
            write_spd((int)(25), -(int)(25));
        }
    }
    else {
        if (left_edge > -0.1) { //blob right of center
            write_spd((int)(-20*rel_pos_gate-5), (int)(20*rel_pos_gate+5));
        }
        else if (right_edge < 0.1) { //blob left of center
            write_spd((int)(-20*rel_pos_gate+5), (int)(20*rel_pos_gate-5));
        }
        else { //blob in the middle 10% of vision
            write_spd(0, 0);
            usleep(100000);
            double left_edge = (blob_data.ATTACK(x1)-320)/320.0;
            double right_edge = (blob_data.ATTACK(x2)-320)/320.0;
            if (left_edge < -0.1 && right_edge > 0.1) {
                coil_boom();
                cout << "BOOM!" << endl;
                return true;
            }
        }
    }
    return false;
}
void write_spd(int write1, int write2){
    stringstream ss1, ss2;

    //cout << write1 << " " << write2 << endl;

    ss1 << "sd" << write1 << "\n";
    ss2 << "sd" << -write2 << "\n";

    //cout << (unsigned char *)ss1.str().c_str() << " " << (unsigned char *)ss2.str().c_str() << endl;
    //RS232_SendBuf(VASAK, (unsigned char *)ss2.str().c_str(), sizeof(ss2.str()));
    //RS232_SendBuf(PAREM, (unsigned char *)ss1.str().c_str(), sizeof(ss1.str()));

    RS232_cputs(PAREM, ss1.str().c_str());
    RS232_cputs(VASAK, ss2.str().c_str());
}
//For determining the gate that is further away and driving towards it.
void ball_timeout(){
    bool drive_towards = false; // false for yellow, true for blue
    if(blob_data.orange_area!=0){
        ball_timeout_f=false;
        return;
    }
    if(blob_data.blue_area!=0 && !b_set){
        if(blob_data.blue_cen_x > 320){
            write_spd(-25, 25);
            if(last_b_size < blob_data.blue_area){
                last_b_size = blob_data.blue_area;
                last_drive = true;
            } else {
                b_set = true;
                cout<<"B SET!"<<endl;
                last_drive = true;
            }
        } else {
            write_spd(25, -25);
            if(last_b_size<blob_data.blue_area){
                last_b_size = blob_data.blue_area;
                last_drive = false;
            } else {
                b_set = true;
                cout<<"B SET!"<<endl;
                last_drive = false;
            }
        }
    } else if(blob_data.yellow_area!=0 && !y_set){
        if(blob_data.yellow_cen_x>320){
            write_spd(-25, 25);
            if(last_y_size<blob_data.yellow_area){
                last_y_size = blob_data.yellow_area;
                last_drive = true;
            } else {
                y_set = true;
                cout<<"Y SET!"<<endl;
                last_drive = true;
            }
        } else {
            write_spd(25, -25);
            if(last_y_size<blob_data.yellow_area){
                last_y_size = blob_data.yellow_area;
                last_drive = false;
            } else {
                y_set = true;
                cout<<"Y SET!"<<endl;
                last_drive = false;
            }
        }
    }
    if(b_set && y_set){
        if(last_y_size<last_b_size){
            gate_select = true; // blue = true, yellow = false
        } else {
            gate_select = false;
        }
        drive_ball_timeout();
    }

}
//drives the robot towards the gate that is further away. last_drive is used to determine which way the robot was turning
// to ensure a smooth transition.
void drive_ball_timeout(){
    int area = 0;
    double real_gate_pos=0;
    if(gate_select){
        if(blob_data.blue_area!=0){
            area = blob_data.blue_area;
            float x = blob_data.blue_cen_x;
            real_gate_pos = (x-320)/320.0;
            if (real_gate_pos > 0.05) { //blob right of center
                write_spd(MAX_SPD - (int)(real_gate_pos*SLOWER_BY), MAX_SPD);
            }
            else if (real_gate_pos < -0.05) { //blob left of center
                write_spd(MAX_SPD, MAX_SPD + (int)(real_gate_pos*SLOWER_BY));
            }
            else { //blob in the middle

                write_spd(MAX_SPD, MAX_SPD);
            }
            if(area>MIN_AREA){
                ball_timeout_f = false;
            }
        } else {
            //False left, true right
            if(last_drive){
                write_spd(MAX_SPD, 0);
            } else {
                write_spd(0, MAX_SPD);
            }
        }

    } else {
        if(blob_data.yellow_area!=0){
            area = blob_data.yellow_area;
            float x = blob_data.yellow_cen_x;
            real_gate_pos = (x-320)/320.0;
            if (real_gate_pos > 0.05) { //blob right of center
                write_spd(MAX_SPD - (int)(real_gate_pos*SLOWER_BY), MAX_SPD);
            }
            else if (real_gate_pos < -0.05) { //blob left of center
                write_spd(MAX_SPD, MAX_SPD + (int)(real_gate_pos*SLOWER_BY));
            }
            else { //blob in the middle
                write_spd(MAX_SPD, MAX_SPD);
            }
            if(area>MIN_AREA){
                ball_timeout_f = false;
            }
        } else {
            //False left, true right
            if(last_drive){
                write_spd(MAX_SPD, -MAX_SPD);
            } else {
                write_spd(-MAX_SPD, MAX_SPD);
            }
        }
    }
}

void goal_timeout(){
    double rel_pos_gate = (blob_data.DEFEND(cen_x)-320)/320.0;
    //cout << rel_pos_gate << endl;

    if(rel_pos_gate == -1){
        if (theirs == false) { //Their gate last left vision on the left side of the screen
            write_spd(-(int)(25),(int)(25));
            //cout << "going right" << endl;
        }
        else {
            write_spd((int)(25), -(int)(25));
        }
    }
    else {
        if (rel_pos_gate > 0) { //blob right of center
            write_spd(MAX_SPD - (int)(rel_pos_gate*SLOWER_BY), MAX_SPD);
        }
        else if (rel_pos_gate < 0) { //blob left of center
            write_spd(MAX_SPD, MAX_SPD + (int)(rel_pos_gate*SLOWER_BY));
        }
        else { //blob exactly in the middle
            write_spd(MAX_SPD, MAX_SPD);
        }
    }
    if (blob_data.DEFEND(area) > 10000 || blob_data.ATTACK(area) != 0){
        goal_timeout_f = false;
        gettimeofday(&start_goal, NULL);
    }
}

void back_off(){

    write_spd(0, 0);
    usleep(100000);
    write_spd(-MAX_SPD, -MAX_SPD);
    usleep(500000);
    if (!ball_in){
        write_spd(50, -50);
        usleep(500000);
        //while (blob_data.orange_area == 0 && blob_data.DEFEND(area) == 0 && blob_data.ATTACK(area) == 0);
    }
}

char getBall(){
    unsigned char read_buf[6];
    unsigned char write_buf[4] = "gb\n";
    while(true) {

        RS232_SendBuf(PAREM, write_buf, sizeof(write_buf));
        usleep(50000);
        RS232_PollComport(PAREM, read_buf, sizeof(read_buf));

        if (read_buf[0]=='<' && read_buf[3]=='1'){
            ball_in = true;
            last_ball_in = true;
            //cout<<"Ball in"<<endl;
        }
        else if (read_buf[0]=='<' && read_buf[3]=='0') {
            ball_in = false;
        }
        else {
            cout << read_buf << endl;
            RS232_PollComport(PAREM, read_buf, sizeof(read_buf));
        }
    }
    //return buf[3];
}

unsigned char *serial_read(int id){
    unsigned char *buf = NULL;
    buf = (unsigned char*)malloc(50);
    memset(buf, '\0', 15);
    int n = RS232_PollComport(id, buf, 49);
    //printf("%s %d\n",buf,n);
    if (n) {
        return buf;
    }
    else{
        //cout << "NEIN NEIN NEIN!";
        return NULL;
    }
}

bool init_serial_dev(){

    if(RS232_OpenComport(motor1, BAUDRATE))
    {
        cout << "Can not open /dev/ttyACM0\n";
        return(false);
    }
    if(RS232_OpenComport(motor2, BAUDRATE)){
        cout << "Can not open /dev/ttyACM1\n";
        return(false);
    }
    if(RS232_OpenComport(coil, BAUDRATE)){
        cout << "Can not open /dev/ttyACM2\n";
        return(false);
    }

    //clear buffer
    RS232_cputs(motor1, "?\n");
    RS232_cputs(motor2, "?\n");
    RS232_cputs(coil, "?\n");
    unsigned char temp[200];
    usleep(500000);
    RS232_PollComport(motor1, temp, 200);
    RS232_PollComport(motor2, temp, 200);
    RS232_PollComport(coil, temp, 200);

    RS232_cputs(motor1, "?\n");
    unsigned char *m1 = NULL;
	m1 = (unsigned char*)malloc(15);
	memset(m1, '\0', 15);
	usleep(500000);
	m1 = serial_read(motor1);

	RS232_cputs(motor2, "?\n");
    unsigned char *m2 = NULL;
	m2 = (unsigned char*)malloc(15);
	memset(m2, '\0', 15);
	usleep(500000);
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
                motor1 = 24;
                motor2 = 25;
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
                motor1 = 25;
                motor2 = 2;
                coil = 24;
            }
        }
    }
    else out_status = false;
    return out_status;
}

void close_serial(){
    write_spd(0, 0);
    usleep(500000);
    RS232_CloseComport(motor1);
    RS232_CloseComport(motor2);
    RS232_CloseComport(coil);
}

void coil_boom(){
    RS232_cputs(coil, "k1000\n");
    ball_kicked = true;
    usleep(100000);

}

void coil_ping(){
    RS232_cputs(coil, "p\n");
}

void coil_charge(){
    RS232_cputs(coil, "c\n");
}

int get_blobs(SEGMENTATION * segm){
    namedWindow("aken");

    VideoCapture capture(0);
	//capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//capture.set(CV_CAP_PROP_FPS, 60);

	unsigned char *data = NULL;
    Mat img, frame;
    int x;

    //int test = 0;
    while (true) {
        //test++;
        //cout << test << endl;

        capture >> img;
        cvtColor(img, frame, CV_BGR2YUV);
        data = frame.data;
        segm->readThresholds("conf");

        segm->thresholdImage(data);
        segm->EncodeRuns();
        segm->ConnectComponents();
        segm->ExtractRegions();
        segm->SeparateRegions();
        segm->SortRegions();

        struct region *tempRegion=NULL;
        if(segm->colors[BLACK].list!=NULL){
            tempRegion = segm->colors[BLACK].list;
            if (tempRegion->area > MINBALL) {
                blob_data.orange_area = tempRegion->area;
                blob_data.orange_cen_x = tempRegion->cen_x;
                blob_data.orange_cen_y = tempRegion->cen_y;
            }
            else {
                blob_data.orange_area = 0;
                blob_data.orange_cen_x = 0;
                blob_data.orange_cen_y = 0;
            }
            //tempRegion = tempRegion->next;
            //rectangle(img, Point(tempRegion->x1, tempRegion->y1), Point(tempRegion->x2, tempRegion->y2), Scalar(255,0,0), 2);
            //circle(img, Point(tempRegion->cen_x, tempRegion->cen_y), 5, Scalar(0,140,255), -1);
            while (tempRegion != NULL && tempRegion->area > MINBALL) {
                circle(img, Point(tempRegion->cen_x, tempRegion->cen_y), 5, Scalar(0,140,255), -1);
                tempRegion = tempRegion->next;
            }
        }
        else {
            blob_data.orange_area = 0;
            blob_data.orange_cen_x = 0;
            blob_data.orange_cen_y = 0;
        }
        if(segm->colors[YELLOW].list!=NULL){
            tempRegion = segm->colors[YELLOW].list;
            x = tempRegion->area;
            //cout << x << endl;
            if (x > MINGOAL_BLUE) {
                blob_data.blue_area = x;
                blob_data.blue_cen_x = tempRegion->cen_x;
                blob_data.blue_cen_y = tempRegion->cen_y;
                blob_data.blue_x1 = tempRegion->x1;
                blob_data.blue_x2 = tempRegion->x2;
                rectangle(img, Point(tempRegion->x1, tempRegion->y1), Point(tempRegion->x2, tempRegion->y2), Scalar(0, 255, 255), 2);
            }
            else {
                blob_data.blue_area = 0;
                blob_data.blue_cen_x = 0;
                blob_data.blue_cen_y = 0;
            }
            //tempRegion = tempRegion->next;

            //circle(img, Point(tempRegion->cen_x, tempRegion->cen_y), 5, Scalar(255,0,0), -1);
        }
        else {
            blob_data.blue_area = 0;
            blob_data.blue_cen_x = 0;
            blob_data.blue_cen_y = 0;
        }
        if(segm->colors[BLUE].list!=NULL){
            tempRegion = segm->colors[BLUE].list;
            x = tempRegion->area;
            if (x > MINGOAL_YELLOW){
                blob_data.yellow_area = x;
                blob_data.yellow_cen_x = tempRegion->cen_x;
                blob_data.yellow_cen_y = tempRegion->cen_y;
                blob_data.yellow_x1 = tempRegion->x1;
                blob_data.yellow_x2 = tempRegion->x2;
                rectangle(img, Point(tempRegion->x1, tempRegion->y1), Point(tempRegion->x2, tempRegion->y2), Scalar(255,0,0), 2);
            }
            else {
                blob_data.yellow_area = 0;
                blob_data.yellow_cen_x = 0;
                blob_data.yellow_cen_y = 0;
            }
            //tempRegion = tempRegion->next;

            //circle(img, Point(tempRegion->cen_x, tempRegion->cen_y), 5, Scalar(0,255,255), -1);
        }
        else {
            blob_data.yellow_area = 0;
            blob_data.yellow_cen_x = 0;
            blob_data.yellow_cen_y = 0;
        }
        if(segm->colors[ORANGE].list!=NULL){
            tempRegion = segm->colors[ORANGE].list;
            blob_data.green_area = tempRegion->area;
            blob_data.green_cen_x = tempRegion->cen_x;
            blob_data.green_cen_y = tempRegion->cen_y;
            blob_data.total_green = 0;

            //tempRegion = tempRegion->next;
            //rectangle(img, Point(tempRegion->x1, tempRegion->y1), Point(tempRegion->x2, tempRegion->y2), Scalar(0,0,0), 2);
            //circle(img, Point(tempRegion->cen_x, tempRegion->cen_y), 5, Scalar(0, 255, 0), -1);
            while (tempRegion != NULL) {
                blob_data.total_green += tempRegion->area;
                tempRegion = tempRegion->next;
            }
        }
        else {
            blob_data.green_area = 0;
            blob_data.green_cen_x = 0;
            blob_data.green_cen_y = 0;
        }
        if(segm->colors[GREEN].list!=NULL){
            tempRegion = segm->colors[GREEN].list;
            blob_data.black_area = tempRegion->area;
            blob_data.black_cen_x = tempRegion->cen_x;
            blob_data.black_cen_y = tempRegion->cen_y;
            //tempRegion = tempRegion->next;
            //rectangle(img, Point(tempRegion->x1, tempRegion->y1), Point(tempRegion->x2, tempRegion->y2), Scalar(255,0,0), 2);
            //circle(img, Point(tempRegion->cen_x, tempRegion->cen_y), 5, Scalar(0, 0, 0), -1);
        }
        else {
            blob_data.black_area = 0;
            blob_data.black_cen_x = 0;
            blob_data.black_cen_y = 0;
        }
        if(segm->colors[WHITE].list!=NULL){
            tempRegion = segm->colors[WHITE].list;
            blob_data.white_area = tempRegion->area;
            blob_data.white_cen_x = tempRegion->cen_x;
            blob_data.white_cen_y = tempRegion->cen_y;
            //tempRegion = tempRegion->next;
            //rectangle(img, Point(tempRegion->x1, tempRegion->y1), Point(tempRegion->x2, tempRegion->y2), Scalar(255,0,0), 2);
            //circle(img, Point(tempRegion->cen_x, tempRegion->cen_y), 5, Scalar(255, 255, 255), -1);
        }
        else {
            blob_data.white_area = 0;
            blob_data.white_cen_x = 0;
            blob_data.white_cen_y = 0;
        }
        imshow("aken", img);
        if (waitKey(10) == 27) break;
    }
    close_serial();
    exit(0);
    return(0);
}
