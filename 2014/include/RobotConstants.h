/* 
 * File:   RobotConstants.h
 * Author: antti
 *
 * Created on September 7, 2014, 12:26 PM
 */

#ifndef ROBOTCONSTANTS_H
#define	ROBOTCONSTANTS_H

#include <vector>

#define NR_OF_WHEELS 4
#define CAMERA_PATH /dev/video1
#define CONF_PATH ../conf

#define CAM_W 640
#define CAM_H 480
#define CAM_HFOV 63
#define CAM_VFOV 48
#define REAR_CAM_HEIGTH 143 //in millimeters from ground to centre of lens
#define REAR_CAM_ANGLE 23 //angle between ground plane and camera central horizontal plane, camera is pointed downwards
#define FRONT_CAM_HEIGTH 154
#define FRONT_CAM_ANGLE 21

#define MAX_MOTOR_SPEED 250
#define GET_BALL_BOARD_ID 3
#define GET_SWITCH_BOARD_ID 2
#define MIN_BLOB_SIZE 3
#define MIN_GREEN_AREA 1000

#define SEG_BLACK 0
#define SEG_WHITE 1
#define SEG_GREEN 2
#define SEG_BLUE 3
#define SEG_YELLOW 4
#define SEG_ORANGE 5
#define SEG_NOCOLOR 6

//DO NOT TOUCH. MISSION CRITICAL SYSTEMS
#define MAGIC 1.15


struct orange_blob{
    int orange_area = 0;
    int orange_w = 0;
    int orange_cen_x = -1;
    int orange_cen_y = -1;
};

struct orange_ball{
    int orange_cen_x = -1;
    int orange_cen_y = -1;
    int orange_w = -1;
    int orange_h = -1;
    int orange_x1 = -1;
    int orange_x2 = -1;
    int orange_y1 = -1;
    int orange_y2 = -1;
};

struct yellow_blob{
    int yellow_area = 0;
    int yellow_w = 0;
    int yellow_cen_x = -1;
    int yellow_cen_y = -1;
};

struct yellow_gate{
    int yellow_cen_x = -1;
    int yellow_cen_y = -1;
    int yellow_w = -1;
    int yellow_h = -1;    
    int yellow_x1 = -1;
    int yellow_x2 = -1;
    int yellow_y1 = -1;
    int yellow_y2 = -1;
};

struct blue_blob{
    int blue_area = 0;
    int blue_w = 0;
    int blue_cen_x = -1;
    int blue_cen_y = -1;
    
};

struct blue_gate{
    int blue_cen_x = -1;
    int blue_cen_y = -1;
    int blue_w = -1;
    int blue_h = -1;
    int blue_x1 = -1;
    int blue_x2 = -1;
    int blue_y1 = -1;
    int blue_y2 = -1;
};

struct blobs{
    int oranges_processed = 0;
    std::vector<orange_blob> o_blob;
    
    int blues_processed = 0;
    std::vector<blue_blob> b_blob;
    
    int yellows_processed = 0;
    std::vector<yellow_blob> y_blob;
    
    int total_green = 0;
    int greens_processed = 0;
};

struct blobs_processed{
    int oranges_postprocessed = 0;
    std::vector<orange_ball> o_ball;
    
    int blues_postprocessed = 0;
    std::vector<blue_gate> b_gate;
    
    int yellows_postprocessed = 0;
    std::vector<yellow_gate> y_gate;
    
    int total_green = 0;
    int greens_processed = 0;
};

enum RobotState{
    FIND_BALL,
    FIND_GATE,
    BALL_TIMEOUT,
    GATE_TIMEOUT,
    NOT_GREEN,
    KICK_BALL
};

enum Goal{
    gYELLOW,
    gBLUE
};

enum Role{
    rATTACK,
    rDEFEND
};


#endif	/* ROBOTCONSTANTS_H */

