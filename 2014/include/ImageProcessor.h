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


class ImageProcessor {
public:
    blobs blob_data;
    SEGMENTATION segm;
    ImageProcessor();
    virtual ~ImageProcessor();
    void init();
private:
    void processOrange();
    void processYellow();
    void processBlue();
    void processGreens();
};

#endif	/* IMAGEPROCESSOR_H */

