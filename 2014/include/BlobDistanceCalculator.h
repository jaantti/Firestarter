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
#include "Maths.h"
#include <vector>

using namespace std;
class ImagePostProcessor;

class BlobDistanceCalculator {
    
public:
    
    BlobDistanceCalculator(ImagePostProcessor* pProc);
    virtual ~BlobDistanceCalculator();
    void run();
    
    vector<Ball> getFrontBalls();
    vector<Ball> getBackBalls();   
    
private:
    ImagePostProcessor *pProcessor;
    
    blobs_processed blobsFront = {};
    blobs_processed blobsBack = {};
    
    vector<Ball> ballsFront;
    vector<Ball> ballsBack; 
    
    float getFrontDistance(int y, int x);
    float getBackDistance(int y, int x);
    
    float getFrontAngle(int x);
    float getBackAngle(int x);
    
    void calcFrontBallDist(blobs_processed blobsFront);
    void calcBackBallDist(blobs_processed blobsBack);
    
    void sortBalls(bool side);
    
    boost::mutex frontLock;
    boost::mutex backLock;

};

#endif	/* BLOBDISTANCECALCULATOR_H */

