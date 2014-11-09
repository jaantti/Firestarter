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
    ImagePostProcessor();
    ImagePostProcessor(ImageProcessor*  imageProcessor);
    void run();
    virtual ~ImagePostProcessor();
    blobs_processed blob_structure_front;
    blobs_processed blob_structure_back;
    
private:
    void loadBlobVectors();
    void processBlobVectors();
    
    void processOrangeBlobsFront();
    void processYellowBlobsFront();
    void processBlueBlobsFront();
    
    void processOrangeBlobsBack();
    void processYellowBlobsBack();
    void processBlueBlobsBack();
    
    bool orangeFitsInFrontList(int x, int y, int w);
    bool orangeFitsInBackList(int x, int y, int w);
    
    bool blueFitsInFrontList(int x, int y, int w);
    bool blueFitsInBackList(int x, int y, int w);
    
    bool yellowFitsInFrontList(int x, int y, int w);
    bool yellowFitsInBackList(int x, int y, int w);
    
    ImageProcessor* iProc;
    blobs blob_container_front;
    blobs blob_container_back;
    
    

};

#endif	/* IMAGEPOSTPROCESSOR_H */

