/* 
 * File:   ImagePostProcessor.h
 * Author: antti
 *
 * Created on November 4, 2014, 3:43 PM
 */

#ifndef IMAGEPOSTPROCESSOR_H
#define	IMAGEPOSTPROCESSOR_H

#include "RobotConstants.h"
#include "ImageProcessor.h"
#include "RobotConstants.h"

class ImagePostProcessor {
public:
    ImagePostProcessor(ImageProcessor*  imageProcessor);
    void run();
    virtual ~ImagePostProcessor();
    blobs_processed blob_structure_front;
    blobs_processed blob_structure_back;
    
private:
    void loadBlobVectors();
    void processBlueBlobs();
    void processOrangeBlobs();
    void processYellowBlobs();
    void processBlobVectors();
    ImageProcessor* iProc;
    blobs blob_container_front;
    blobs blob_container_back;
    
    

};

#endif	/* IMAGEPOSTPROCESSOR_H */

