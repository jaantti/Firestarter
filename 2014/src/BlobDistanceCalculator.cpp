/* 
 * File:   BlobDIstanceCalculator.cpp
 * Author: antti
 * 
 * Created on November 17, 2014, 1:03 PM
 */

#include "BlobDistanceCalculator.h"

using namespace std;

BlobDistanceCalculator::BlobDistanceCalculator(ImagePostProcessor* pProc) {
    
    pProcessor = pProc;
}

BlobDistanceCalculator::~BlobDistanceCalculator() {
}


void BlobDistanceCalculator::run() {
    blobs_processed blobsFront = pProcessor->getFrontSystem();
    blobs_processed blobsBack = pProcessor->getBackSystem();

}

void BlobDistanceCalculator::calcFrontBallDist(blobs_processed blobsFront) {
    
    frontLock.lock();
    ballsFront = {};
    frontLock.unlock();
        
    vector<orange_ball> balls = blobsFront.o_ball;
    int size = balls.size();
    float tempDist = 0;
    float tempAngle = 0;
    
    for (int i = 0; i<size; i++) {
        orange_ball oBall = balls.at(i);
        tempDist = getFrontDistance(oBall.orange_cen_y);
        tempAngle = getFrontAngle(oBall.orange_cen_x);
        
        Ball tempBall = Ball(tempDist, tempAngle, oBall.orange_cen_x, oBall.orange_cen_y);
        
        frontLock.lock();
        ballsFront.push_back(tempBall);
        frontLock.unlock();
        
    }
    
}

void BlobDistanceCalculator::calcBackBallDist(blobs_processed blobsBack) {

}

float BlobDistanceCalculator::getFrontDistance(int y) {
    //y=a*x^b
    return F_CURVE_FIT_A * pow(y, F_CURVE_FIT_B);
}

float BlobDistanceCalculator::getFrontAngle(int x) {
    return (((float)x - (float)CAM_W/2.0) / (float)CAM_W * (float)CAM_HFOV * -1.0);
}

float BlobDistanceCalculator::getBackAngle(int x) {
    return (((float)x - (float)CAM_W/2.0) / (float)CAM_W * (float)CAM_HFOV * -1.0) + 180;
}

vector <Ball> BlobDistanceCalculator::getFrontBalls() {
    frontLock.lock();
    vector<Ball> temp = ballsFront;
    frontLock.unlock();
    
    return temp;
}

vector <Ball> BlobDistanceCalculator::getBackBalls() {
    backLock.lock();
    vector<Ball> temp = ballsBack;
    backLock.unlock();
    
    return temp;
}





