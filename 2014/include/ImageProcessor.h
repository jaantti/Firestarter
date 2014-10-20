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

using namespace boost;

class ImageProcessor {
    mutex blobBackMutex;
    mutex blobFrontMutex;
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
    /**
     * Releases mutex lock, must be used after getBlobsFront() or code will be in a deadlock
     */
    void unlockFront();
    /**
     * Releases mutex lock, must be used after getBlobsBack() of code will be in a deadlock
     */
    void unlockBack();
private:
    // if switchCamers = true, then video0 is front, if false, video0 is back.
    char *front;
    char *back;
    bool switchCameras;
    bool chooseCameras();
    bool codeEnd = false;
    mutex frontLock;
    mutex backLock;
    void sendNetworkInfo();
    void processFrontCamera(char *cam);
    void processBackCamera( char *cam);
    void processOrange(bool front);
    void processYellow(bool front);
    void processBlue(bool front);
    void processGreens(bool front);
    
};

#endif	/* IMAGEPROCESSOR_H */

