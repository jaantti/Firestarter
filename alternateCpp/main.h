#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#define MIN_AREA 5000
#define CURRENT_GATE 1
#define BAUDRATE 115200
#define VASAK motor1
#define PAREM motor2

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
};

void coil_charge();
void coil_ping();
void coil_boom();
bool init_serial_dev();
void close_serial();
unsigned char *serial_read(int);
void findBall(float, float);
void findGate(double rel_pos_gate);
void write_spd(int write1, int write2);
void ball_timeout(blobs blobber, int last_y_size, int last_b_size, bool b_set, bool y_set, bool last_drive);
void drive_ball_timeout(blobs blobber, bool gate_select, bool last_drive);
void back_off();
char getBall();
blobs get_blobs(SEGMENTATION * segm);
void init_video();

#endif
