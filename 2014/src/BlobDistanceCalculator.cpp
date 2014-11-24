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
    b_bGate = pProcessor->getUnlockedBlue();
    b_yGate = pProcessor->getUnlockedYellow();
    calcFrontBallDist(blobsFront);
    calcBackBallDist(blobsBack);
    calcYellowGateDistance();
    calcBlueGateDistance();
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
    
    return (B_CURVE_FIT_A * pow(y, B_CURVE_FIT_B) / cos(getBackAngle(x)* PI / 180))*-1.0f;
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

void BlobDistanceCalculator::calcYellowGateDistance() {
    if(b_yGate.yellow_cen_x>0){
        if(b_yGate.direction){
            yGate.SetDir(RobotConstants::Direction::FRONT);
            yGate.SetCen_x(b_yGate.yellow_cen_x);
            yGate.SetCen_y(b_yGate.yellow_cen_y);
            yGate.SetHeight(b_yGate.yellow_h);
            yGate.SetWidth(b_yGate.yellow_w);
            yGate.SetX1(b_yGate.yellow_x1);
            yGate.SetX2(b_yGate.yellow_x2);
            yGate.SetY1(b_yGate.yellow_y1);
            yGate.SetY2(b_yGate.yellow_y2);
            float dist = getFrontDistance(b_yGate.yellow_y2, b_yGate.yellow_cen_x);
            float angle = getFrontAngle(b_yGate.yellow_cen_x);
            yGate.SetAngle(angle);
            yGate.SetDistance(dist/100.0f);
            
        } else {
            yGate.SetDir(RobotConstants::Direction::FRONT);
            yGate.SetCen_x(b_yGate.yellow_cen_x);
            yGate.SetCen_y(b_yGate.yellow_cen_y);
            yGate.SetHeight(b_yGate.yellow_h);
            yGate.SetWidth(b_yGate.yellow_w);
            yGate.SetX1(b_yGate.yellow_x1);
            yGate.SetX2(b_yGate.yellow_x2);
            yGate.SetY1(b_yGate.yellow_y1);
            yGate.SetY2(b_yGate.yellow_y2);
            float dist = getFrontDistance(b_yGate.yellow_y2, b_yGate.yellow_cen_x);
            float angle = getFrontAngle(b_yGate.yellow_cen_x);
            yGate.SetAngle(angle);
            yGate.SetDistance(dist/100.0f);
        }
        
        
    } else {
        yGate.SetDir(RobotConstants::Direction::UNDEFINED);
    }
}

void BlobDistanceCalculator::calcBlueGateDistance() {
    if(b_bGate.blue_cen_x>0){
        if(b_bGate.direction){
            bGate.SetDir(RobotConstants::Direction::FRONT);
            bGate.SetCen_x(b_bGate.blue_cen_x);
            bGate.SetCen_y(b_bGate.blue_cen_y);
            bGate.SetHeight(b_bGate.blue_h);
            bGate.SetWidth(b_bGate.blue_w);
            bGate.SetX1(b_bGate.blue_x1);
            bGate.SetX2(b_bGate.blue_x2);
            bGate.SetY1(b_bGate.blue_y1);
            bGate.SetY2(b_bGate.blue_y2);
            float dist = getFrontDistance(b_bGate.blue_y2, b_bGate.blue_cen_x);
            float angle = getFrontAngle(b_bGate.blue_cen_x);
            bGate.SetAngle(angle);
            bGate.SetDistance(dist/100.0f);
            
        } else {
            bGate.SetDir(RobotConstants::Direction::REAR);
            bGate.SetCen_x(b_bGate.blue_cen_x);
            bGate.SetCen_y(b_bGate.blue_cen_y);
            bGate.SetHeight(b_bGate.blue_h);
            bGate.SetWidth(b_bGate.blue_w);
            bGate.SetX1(b_bGate.blue_x1);
            bGate.SetX2(b_bGate.blue_x2);
            bGate.SetY1(b_bGate.blue_y1);
            bGate.SetY2(b_bGate.blue_y2);
            float dist = getBackDistance(b_bGate.blue_y2, b_bGate.blue_cen_x);
            float angle = getBackAngle(b_bGate.blue_cen_x);
            bGate.SetAngle(angle);
            bGate.SetDistance(dist/100.0f);
        }
        
        
    } else {
        bGate.SetDir(RobotConstants::Direction::UNDEFINED);
    }
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




