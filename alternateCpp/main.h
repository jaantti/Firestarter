#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#define MIN_AREA 100
#define CURRENT_GATE 1
#define BAUDRATE 115200
#define VASAK motor2
#define PAREM motor1
#define DEFEND(TOKEN) yellow ## _ ## TOKEN
#define ATTACK(TOKEN) blue ## _ ## TOKEN
#define MINGREEN 92160
#define MINBALL 2000
#define MINGOAL_YELLOW 1000
#define MINGOAL_BLUE 1000
#define MAX_SPD 50
#define SLOWER_BY 30

struct blobs{
    int orange_area;
    int orange_cen_x;
    int orange_cen_y;
    int blue_area;
    int blue_cen_x;
    int blue_cen_y;
    int yellow_area;
    int yellow_cen_x;
    int yellow_cen_y;
    int green_area;
    int green_cen_x;
    int green_cen_y;
    int black_area;
    int black_cen_x;
    int black_cen_y;
    int white_area;
    int white_cen_x;
    int white_cen_y;

    int total_green;
};

void coil_charge();
void coil_ping();
void coil_boom();
bool init_serial_dev();
void close_serial();
unsigned char *serial_read(int);
void findBall(float, float);
void findBigBall(float x, float y);
bool findGate(double rel_pos_gate);
void write_spd(int write1, int write2);
void ball_timeout();
void drive_ball_timeout(bool gate_select);
void back_off();
char getBall();
int get_blobs(SEGMENTATION * segm);

#endif
