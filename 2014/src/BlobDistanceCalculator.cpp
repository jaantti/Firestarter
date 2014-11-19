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
    calcFrontBallDist(blobsFront);
    calcBackBallDist(blobsBack);
    sortBalls(true);
    sortBalls(false);

}

void BlobDistanceCalculator::calcFrontBallDist(blobs_processed blobsFront) {
    
    ballsFront = {};
        
    vector<orange_ball> balls = blobsFront.o_ball;
    int size = balls.size();
    float tempDist = 0;
    float tempAngle = 0;
    
    for (int i = 0; i<size; i++) {
        orange_ball oBall = balls.at(i);
        tempDist = getFrontDistance(oBall.orange_cen_y);
        tempAngle = getFrontAngle(oBall.orange_cen_x);
        
        Ball tempBall(tempDist, tempAngle, oBall.orange_cen_x, oBall.orange_cen_y);
        
        ballsFront.push_back(tempBall);
        
    }    
}

void BlobDistanceCalculator::calcBackBallDist(blobs_processed blobsBack) {
    
    ballsBack = {};
        
    vector<orange_ball> balls = blobsBack.o_ball;
    int size = balls.size();
    float tempDist = 0;
    float tempAngle = 0;
    
    for (int i = 0; i<size; i++) {
        orange_ball oBall = balls.at(i);
        tempDist = getBackDistance(oBall.orange_cen_y);
        tempAngle = getBackAngle(oBall.orange_cen_x);
        
        Ball tempBall(tempDist, tempAngle, oBall.orange_cen_x, oBall.orange_cen_y);
        
        ballsBack.push_back(tempBall);
        
    }
}

float BlobDistanceCalculator::getFrontDistance(int y) {
    //y=a*x^b
    return F_CURVE_FIT_A * pow(y, F_CURVE_FIT_B);
}

float BlobDistanceCalculator::getBackDistance(int y) {
    //y=a*x^b
    return B_CURVE_FIT_A * pow(y, B_CURVE_FIT_B);
}

float BlobDistanceCalculator::getFrontAngle(int x) {
    return (((float)x - (float)CAM_W/2.0) / (float)CAM_W * (float)CAM_HFOV * -1.0);
}

float BlobDistanceCalculator::getBackAngle(int x) {
    return (((float)x - (float)CAM_W/2.0) / (float)CAM_W * (float)CAM_HFOV * -1.0) + 180;
}

void BlobDistanceCalculator::sortBalls(bool side) {
    
    if(side) {
        sort(ballsFront.begin(), ballsFront.end());
    } else {
        sort(ballsBack.begin(), ballsBack.end());
    }
}


vector <Ball> BlobDistanceCalculator::getFrontBalls() {
    frontLock.lock();
    vector<Ball> temp = ballsFront;
    for(int i = 0; i<temp.size(); i++){
        Ball ball = temp.at(i);
        cout << "Ball distance : " << ball.getDistance() << " ball angle : " << ball.getAngle() << " BallX:" << ball.getCen_x() << " BallY:" << ball.getCen_y() << endl;
    }
    frontLock.unlock();
    
    return temp;
}

vector <Ball> BlobDistanceCalculator::getBackBalls() {
    backLock.lock();
    vector<Ball> temp = ballsBack;
    backLock.unlock();
    
    return temp;
}





