/* 
 * File:   ImageProcessor.h
 * Author: antti
 *
 * Created on September 7, 2014, 12:24 PM
 */

#ifndef IMAGEPROCESSOR_H
#define	IMAGEPROCESSOR_H

#include "RobotConstants.h"
#include "segmentation.h"
#include "capture.h"
#include "r_video.h"
#include <stdio.h>

class ImageProcessor {
public:
    SEGMENTATION segm;
    ImageProcessor();
    virtual ~ImageProcessor();
    void init();
};

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

#endif	/* IMAGEPROCESSOR_H */

