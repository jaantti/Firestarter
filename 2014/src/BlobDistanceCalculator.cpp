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
    blobs_processed blobsFront = pProcessor->getUnlockedFront();
    blobs_processed blobsBack = pProcessor->getUnlockedBack();
    calcFrontBallDist(blobsFront);
    calcBackBallDist(blobsBack);
    sortBalls();
}

void BlobDistanceCalculator::calcFrontBallDist(blobs_processed blobsFront) {
    
    balls = {};
    vector<orange_ball> ballsF = blobsFront.o_ball;
    int size = ballsF.size();
    float tempDist = 0;
    float tempAngle = 0;
    
    for (int i = 0; i<size; i++) {
        orange_ball oBall = ballsF.at(i);
        tempDist = getFrontDistance(oBall.orange_cen_y, oBall.orange_cen_x);
        tempAngle = getFrontAngle(oBall.orange_cen_x);
        
        Ball tempBall(tempDist, tempAngle, oBall.orange_cen_x, oBall.orange_cen_y, (oBall.orange_x2-oBall.orange_x1), RobotConstants::Direction::FRONT );
        
        balls.push_back(tempBall);
        
    }    
}

void BlobDistanceCalculator::calcBackBallDist(blobs_processed blobsBack) {
            
    vector<orange_ball> ballsB = blobsBack.o_ball;
    int size = ballsB.size();
    float tempDist = 0;
    float tempAngle = 0;
    
    for (int i = 0; i<size; i++) {
        orange_ball oBall = ballsB.at(i);
        tempDist = getBackDistance(oBall.orange_cen_y, oBall.orange_cen_x);
        tempAngle = getBackAngle(oBall.orange_cen_x);
        
        Ball tempBall(tempDist, tempAngle, oBall.orange_cen_x, oBall.orange_cen_y, (oBall.orange_x2-oBall.orange_x1), RobotConstants::Direction::REAR);
        
        balls.push_back(tempBall);
        
    }
}

float BlobDistanceCalculator::getFrontDistance(int y, int x) {
    //y=a*x^b
    return ( F_CURVE_FIT_A * pow(y, F_CURVE_FIT_B) / cos(getFrontAngle(x) * PI / 180) ) -7.0f;
}

float BlobDistanceCalculator::getBackDistance(int y, int x) {
    //y=a*x^b
    
    return B_CURVE_FIT_A * pow(y, B_CURVE_FIT_B) / cos(getBackAngle(x)* PI / 180);
}

float BlobDistanceCalculator::getFrontAngle(int x) {
    return (((float)x - (float)CAM_W/2.0) / (float)CAM_W * (float)CAM_HFOV * -1.0);
}

float BlobDistanceCalculator::getBackAngle(int x) {
    return (((float)x - (float)CAM_W/2.0) / (float)CAM_W * (float)CAM_HFOV * -1.0) + 180;
}

void BlobDistanceCalculator::sortBalls() {
    sort(balls.begin(), balls.end());
}


vector <Ball> BlobDistanceCalculator::getBalls() {
    return this->balls;
}

BlueGate BlobDistanceCalculator::getBlueGate() {
    return this->bGate;
}

YellowGate BlobDistanceCalculator::getYellowGate() {
    return this->yGate;
}




