/* 
 * File:   BlobDistanceCalculator.h
 * Author: antti
 *
 * Created on November 17, 2014, 1:03 PM
 */

#ifndef BLOBDISTANCECALCULATOR_H
#define	BLOBDISTANCECALCULATOR_H

#include "RobotConstants.h"
#include "ImagePostProcessor.h"
#include "Ball.h"
#include "YellowGate.h"
#include "BlueGate.h"
#include <vector>

using namespace std;
class ImagePostProcessor;

class BlobDistanceCalculator {
    
public:
    
    BlobDistanceCalculator(ImagePostProcessor* pProc);
    virtual ~BlobDistanceCalculator();
    void run();
    
    vector<Ball> getBalls();
    YellowGate getYellowGate();
    BlueGate getBlueGate();
    
private:
    ImagePostProcessor *pProcessor;
    
    blobs_processed blobsFront = {};
    blobs_processed blobsBack = {};
    
    big_blue_gate b_bGate;
    big_yellow_gate b_yGate;
    
    vector<Ball> balls; 
    
    YellowGate yGate;
    BlueGate bGate;
    
    float getFrontDistance(int y, int x);
    float getBackDistance(int y, int x);
    
    float getFrontAngle(int x);
    float getBackAngle(int x);
    
    void calcYellowGateDistance();
    void calcBlueGateDistance();
    
    void calcFrontBallDist(blobs_processed blobsFront);
    void calcBackBallDist(blobs_processed blobsBack);
    
    void sortBalls();
    
};

#endif	/* BLOBDISTANCECALCULATOR_H */

