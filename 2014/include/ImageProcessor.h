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
#include "boost/thread.hpp"
#include <sys/time.h>
#include <unistd.h>

#include <stdlib.h>

using namespace boost;

class ImageProcessor {
public:
    blobs blob_data_front;
    blobs blob_data_back;
    Capture cap1;
    Capture cap2;
    SEGMENTATION segm; // front
    SEGMENTATION segm2; // back
    ImageProcessor();
    ImageProcessor(const ImageProcessor &obj);
    virtual ~ImageProcessor();
    void init();
    void runIprocessor();
    void runFrontCamera();
    void runBackCamera();
    void stopProcessor();
    /**
     * Blob info from front camera
     * @return Front camera blobs
     */
    blobs getBlobsFront();
    /**
     * Blob info form back camera
     * @return 
     */
    blobs getBlobsBack();
    
    void getWorkingBackFrame(uchar *dst);
    void getWorkingBackThresh(uchar *dst);
    
    void getWorkingFrontFrame(uchar *dst);
    void getWorkingFrontThresh(uchar *dst);
    
    
private:
    // if switchCamers = true, then video0 is front, if false, video0 is back.
    struct timeval start_front, start_back, end_front, end_back;
    char *front;
    char *back;
    bool switchCameras;
    bool chooseCameras();
    bool codeEnd = false;
    mutex iProcFrontLock;
    mutex iProcBackLock;
    
    uchar* workingFrontFrame;
    uchar* workingFrontThresh;
    uchar* workingBackFrame;
    uchar* workingBackThresh;
    
    mutex frontFrameLock;
    mutex backFrameLock;
    
    void sendNetworkInfo();
    void processFrontCamera(char *cam);
    void processBackCamera( char *cam);
    void processOrange(bool front);
    void processYellow(bool front);
    void processBlue(bool front);
    void processGreens(bool front);
    
};

#endif	/* IMAGEPROCESSOR_H */

