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
#include "BlobDistanceCalculator.h"
#include "YellowGate.h"
#include "BlueGate.h"
#include "Ball.h"
#include <boost/thread.hpp>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
class BlobDistanceCalculator;
class ImagePostProcessor {
public:
    
    
    ImagePostProcessor(ImageProcessor*  imageProcessor);
    ImagePostProcessor(const ImagePostProcessor &iProc);
    void run();
    virtual ~ImagePostProcessor();

    void stopProcessor();
    
    //To be decommissioned once new system is in place.
    blobs_processed getUnlockedFront();
    blobs_processed getUnlockedBack();
    
    big_yellow_gate getUnlockedYellow();
    big_blue_gate getUnlockedBlue();

    std::vector<Ball> getBalls();
    
    YellowGate getYellowGate();
    BlueGate getBlueGate();

    GreenContainer getGreen();
private:
    BlobDistanceCalculator* calculator;
    
    void loadBlobVectors();
    void processBlobVectors();
    
    void processOrangeBlobsFront();
    void processYellowBlobsFront();
    void processBlueBlobsFront();
    
    void processOrangeBlobsBack();
    void processYellowBlobsBack();
    void processBlueBlobsBack();
    
    void runGateDecisionSystem();
    
    void getCalculatorCalculations();
    
    // 0 - no gates 1 - blue gate, 2 - yellow gate, 3 - both gates
    int getFrontGates();
    int getBackGates();

    void eliminateFalseGates(int front, int back);
    void assignBigGates();

    bool orangeFitsInFrontList(int x, int y, int w);
    bool orangeFitsInBackList(int x, int y, int w);
    
    blue_gate mergeBlueGateStructure(blue_gate gate, blue_blob merger);

    yellow_gate mergeYellowGateStructure(yellow_gate gate, yellow_blob merger);

    yellow_gate expandYellowGate(yellow_gate gate, yellow_blob expander);

    blue_gate expandBlueGate(blue_gate gate, blue_blob expander);
    
    big_blue_gate biggestBlueGate = {};
    big_yellow_gate biggestYellowGate = {};

    blobs_processed blob_structure_front;
    blobs_processed blob_structure_back;

    blobs_processed temp_holder_front;
    blobs_processed temp_holder_back;
    
    YellowGate yGate;
    BlueGate bGate;
    std::vector<Ball> balls;
    GreenContainer green = {};
    
    ImageProcessor* iProc;
    blobs blob_container_front;
    blobs blob_container_back;
    bool codeEnd = false;
    
    boost::mutex greenLock;
    
    boost::mutex ballLock;
    
    boost::mutex yGateLock;
    boost::mutex bGateLock;

};

#endif	/* IMAGEPOSTPROCESSOR_H */

