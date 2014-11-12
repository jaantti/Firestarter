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
#include <boost/thread.hpp>

class ImagePostProcessor {
public:
    
    ImagePostProcessor(ImageProcessor*  imageProcessor);
    ImagePostProcessor(const ImagePostProcessor &iProc);
    void run();
    virtual ~ImagePostProcessor();

    void stopProcessor();
    
    void lockBackSystem();
    void lockFrontSystem();

    blobs_processed getFrontSystem();
    blobs_processed getBackSystem();

    void unlockBackSystem();
    void unlockFrontSystem();

    blue_gate getFrontBlue();
    blue_gate getBackBlue();

    yellow_gate getFrontYellow();
    yellow_gate getBackYellow();

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
    
    blue_gate mergeBlueGateStructure(blue_gate gate, blue_blob merger);

    yellow_gate mergeYellowGateStructure(yellow_gate gate, yellow_blob merger);

    yellow_gate expandYellowGate(yellow_gate gate, yellow_blob expander);

    blue_gate expandBlueGate(blue_gate gate, blue_blob expander);
    
    blobs_processed blob_structure_front;
    blobs_processed blob_structure_back;

    ImageProcessor* iProc;
    blobs blob_container_front;
    blobs blob_container_back;
    bool codeEnd = false;
    
    boost::mutex frontLock;
    boost::mutex backLock;
    

};

#endif	/* IMAGEPOSTPROCESSOR_H */

