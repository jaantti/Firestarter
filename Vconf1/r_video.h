#ifndef R_VIDEO_H_INCLUDED
#define R_VIDEO_H_INCLUDED

#define ORANGE 0
#define YELLOW 1
#define BLUE 2
#define GREEN 3
#define WHITE 4
#define BLACK 5

#define WIDTH 640
#define HEIGHT 480


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>


typedef unsigned char uchar;


void detect_gate(int *x, int *y, int *x1, int *y1, int *x2, int *y2, int color);
void draw_gate_lines(void);
void draw_rect(int x, int y, int width, int height);
void draw_dot(int x, int y);
bool check_for_line(int x, int y);
void detect_ball(int *x, int *y);
int find_ball_area(int x, int y, int *green_c);
int find_gate_area(int x, int y, int color );
int getColor(int x, int y);

void set_working_frame_yuyv(uchar *frame, int width, int height);
uchar **get_working_frame(void);
void get_pixel(int pos_x, int pos_y, int *y, int *u, int *v);
void black_ch(void);
void readThresholds(char path[100]);
void get_real_coord(int *x, int *y);
void r_video_cleanup(void);


#endif
