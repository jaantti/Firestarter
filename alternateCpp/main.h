#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#define MIN_AREA 8000
#define BAUDRATE 115200
#define VASAK motor2
#define PAREM motor1
#define DEFEND(TOKEN) blue ## _ ## TOKEN
#define ATTACK(TOKEN) yellow ## _ ## TOKEN
#define MINGREEN 76800
#define MINBALL 1
#define MINGOAL_YELLOW 1000
#define MINGOAL_BLUE 1000
#define MAX_SPD 75
#define SLOWER_BY 40
typedef unsigned char uchar;
struct blobs{
    int orange_area;
    int orange_cen_x;
    int orange_cen_y;
    int blue_area;
    int blue_cen_x;
    int blue_cen_y;
    int blue_x1;
    int blue_x2;
    int yellow_area;
    int yellow_cen_x;
    int yellow_cen_y;
    int yellow_x1;
    int yellow_x2;
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
void findBall(int max_spd, int slower_by);
bool findGate();
void write_spd(int write1, int write2);
void ball_timeout();
void drive_ball_timeout();
void goal_timeout();
void back_off();
char getBall();
int get_blobs(SEGMENTATION * segm);

#endif
