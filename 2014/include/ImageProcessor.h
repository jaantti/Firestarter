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
#include <cstdlib>
#include <iostream>
#include <thread>

class ImageProcessor {
public:
    blobs blob_data_front;
    blobs blob_data_back;
    Capture cap1;
    Capture cap2;
    SEGMENTATION segm; // front
    SEGMENTATION segm2; // back
    ImageProcessor();
    virtual ~ImageProcessor();
    void init();
    void runIprocessor();
private:
    // if switchCamers = true, then video0 is front, if false, video0 is back.
    char *front;
    char *back;
    bool switchCameras;
    bool chooseCameras();
    void sendNetworkInfo();
    void processFrontCamera(char *cam);
    void processBackCamera( char *cam);
    void processOrange(bool front);
    void processYellow(bool front);
    void processBlue(bool front);
    void processGreens(bool front);
};

#endif	/* IMAGEPROCESSOR_H */

