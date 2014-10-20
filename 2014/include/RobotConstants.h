/* 
 * File:   RobotConstants.h
 * Author: antti
 *
 * Created on September 7, 2014, 12:26 PM
 */

#ifndef ROBOTCONSTANTS_H
#define	ROBOTCONSTANTS_H

#include <vector>

#define NR_OF_WHEELS 3
#define CAMERA_PATH /dev/video1
#define CONF_PATH ../conf
#define CAM_W 640
#define CAM_H 480
#define MAX_MOTOR_SPEED 190
#define GET_BALL_BOARD_ID 1

#define SEG_BLACK 0
#define SEG_WHITE 1
#define SEG_GREEN 2
#define SEG_BLUE 3
#define SEG_YELLOW 4
#define SEG_ORANGE 5
#define SEG_NOCOLOR 6



struct orange_blob{
    int orange_area = 0;
    int orange_cen_x = -1;
    int orange_cen_y = -1;
};

struct yellow_blob{
    int yellow_area = 0;
    int yellow_cen_x = -1;
    int yellow_cen_y = -1;
};

struct blue_blob{
    int blue_area = 0;
    int blue_cen_x = -1;
    int blue_cen_y = -1;
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

enum RobotStates{
    //TODO populate states
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

