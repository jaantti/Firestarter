/* 
 * File:   RobotLogic.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "RobotLogic.h"
#include <iostream>

using namespace std;

RobotLogic::RobotLogic(){
    
}

RobotLogic::RobotLogic(Goal gl) {
    goal = gl;
}

RobotLogic::~RobotLogic() {
}

void RobotLogic::init(RobotController* rCont, ImageProcessor* iProc) {
    rController = rCont;
    iProcessor = iProc;
}

void RobotLogic::init(RobotController* rCont, ImagePostProcessor* pProc) {
    rController = rCont;
    pProcessor = pProc;
}


void RobotLogic::run(Role role) {
    if (role == rATTACK) {
        runAttack();
    }
    if (role == rDEFEND) {
        runDefend();
    }
}

void RobotLogic::runAttack() {
    //cout << "before mutex" << endl;
    blobs blobsFront = iProcessor->getBlobsFront();
    iProcessor->unlockFront();
    blobs blobsBack = iProcessor->getBlobsBack();
    iProcessor->unlockBack();
    
    rController->pingCoil();
    
    switch (rState) {
                
        case RobotState::FIND_BALL:
            //cout << "FIND_BALL";
            findBall(blobsFront, blobsBack);
            break;
        case RobotState::BALL_TIMEOUT:
            //cout << "BALL_TIMEOUT";
            ballTimeout(blobsFront, blobsBack);
            break;
        case RobotState::FIND_GATE:
            //cout << "FIND_GATE";
            findGate(blobsFront, blobsBack);
            break;
        case RobotState::GATE_TIMEOUT:
            //cout << "GATE_TIMEOUT";
            gateTimeout(blobsFront, blobsBack);
            break;
        case RobotState::KICK_BALL:
            //cout << "KICK_BALL";
            kickBall(blobsFront, blobsBack);
            break;
        case RobotState::NOT_GREEN:
            //cout << "NOT_GREEN";
            notGreen(blobsFront, blobsBack);
            break;
    }
    
    // See osa tundub olevat ebavajalik - Taavi
    
    if (false && blobsFront.oranges_processed > 0) {
        //cout << "orange list size: " << blobsFront.o_blob.size() << endl;
        orange_blob oBlob = blobsFront.o_blob.at(0);
        //cout << "orange area front: " << oBlob.orange_area << "\n";
        if (oBlob.orange_area > 10) {
            if (oBlob.orange_cen_x > CAM_W / 2) std::cout << "front: turn right\n";
            if (oBlob.orange_cen_x <= CAM_W / 2) std::cout << "front: turn left\n";
            moveToBall(oBlob.orange_cen_x, oBlob.orange_cen_y);
        }

    } else {
        //rController->driveRobot(0, 0, 0);
    }
    if (false && blobsBack.oranges_processed > 0) {
        orange_blob oBlob = blobsBack.o_blob.at(0);
        cout << "orange area back: " << oBlob.orange_area << "\n";
        if (oBlob.orange_area > 10) {
            if (oBlob.orange_cen_x > CAM_W / 2) std::cout << "back: turn right\n";
            if (oBlob.orange_cen_x <= CAM_W / 2) std::cout << "back: turn left\n";
        }

    }
}

void RobotLogic::runDefend() {

}

void RobotLogic::moveToBall(int x, int y) {
    if (x < CAM_W / 2 - 50) rController->driveRobot(30, 0, 10);
    else if (x > CAM_W / 2 + 50) rController->driveRobot(30, 0, -10);
    else rController->driveRobot(30, 0, 0);
}

void RobotLogic::setRState(RobotState state) {
    rState = state;
}

bool RobotLogic::isGreen(blobs blobsFront, blobs blobsBack) {
    if (blobsFront.total_green < MIN_GREEN_AREA) return false;
    else return true;
}

void RobotLogic::findBall(blobs blobsFront, blobs blobsBack) {
    
    if(blobsFront.oranges_processed>0){
        orange_blob oBlob = blobsFront.o_blob.at(0);
        int x = oBlob.orange_cen_x;
        
        if (oBlob.orange_area > 15) {
             if (x < CAM_W / 2 - 40) {
                 //cout << "LEFT" << "\n";
                 rController->driveRobot(30, 0, 15);
             }
            else if (x > CAM_W / 2 + 40) {
                //cout << "RIGHT" << "\n";
                rController->driveRobot(30, 0, -15);
                }
            else rController->driveRobot(30, 0, 0);
        } else {
            rController->driveRobot(0,0,20);
        }
        
     
        
        if (rController->hasBall()) rState = RobotState::FIND_GATE;
        if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
        if (false) rState = RobotState::BALL_TIMEOUT;
    }
    
}

void RobotLogic::ballTimeout(blobs blobsFront, blobs blobsBack) {

}

void RobotLogic::findGate(blobs blobsFront, blobs blobsBack) {
    int goalX, goalY;
    
    if (goal == Goal::gBLUE) {
        if (blobsFront.blues_processed > 0) {
            goalX = blobsFront.b_blob.at(0).blue_cen_x;
            goalY = blobsFront.b_blob.at(0).blue_cen_y;
        }else{
            //rState = RobotState.GATE_TIMEOUT;
            rController->driveRobot(0,0,10);
            return;
        }
    }
    if (goal == Goal::gYELLOW) {
        if(blobsFront.yellows_processed > 0){
            goalX = blobsFront.y_blob.at(0).yellow_cen_x;
            goalX = blobsFront.y_blob.at(0).yellow_cen_x;
        }else{
            //rState = RobotState.GATE_TIMEOUT;
            rController->driveRobot(0,0,10);
            return;
        }
    }
    if (goalX < 310) rController->driveRobot(0, 0, 20);
    else if (goalX > 330) rController->driveRobot(0, 0, -20);
    else {       
        rState = RobotState::KICK_BALL;
    }
    
    if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
    
}

void RobotLogic::gateTimeout(blobs blobsFront, blobs blobsBack) {

    if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
}

void RobotLogic::kickBall(blobs blobsFront, blobs blobsBack) {
    cout << "KICKBALL" << "\n";
    rController->kickBall(1000);
    rState = RobotState::FIND_BALL;
    if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
}

void RobotLogic::notGreen(blobs blobsFront, blobs blobsBack) {
    //Do something smart
    rState = RobotState::FIND_BALL;    
}