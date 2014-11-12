/* 
 * File:   RobotLogic.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "RobotLogic.h"
#include <iostream>

using namespace std;

RobotLogic::RobotLogic() {

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

void RobotLogic::setGoal() {
    if (rController->getAttackedGoal()=='B') {
        goal = Goal::gBLUE;
    }
    else if (rController->getAttackedGoal()=='Y') {
        goal = Goal::gYELLOW;
    }
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

    blobs_processed blobsFront = pProcessor->getFrontSystem();
    pProcessor->unlockFrontSystem();
    blobs_processed blobsBack = pProcessor->getBackSystem();
    pProcessor->unlockBackSystem();

    switch (rState) {

        case RobotState::FIND_BALL:
            //cout << "FIND_BALL" << endl;
            findBall(blobsFront, blobsBack);
            break;
        case RobotState::BALL_TIMEOUT:
            //cout << "BALL_TIMEOUT" << endl;
            ballTimeout(blobsFront, blobsBack);
            break;
        case RobotState::FIND_GATE:
            //cout << "FIND_GATE" << endl;
            findGate(blobsFront, blobsBack);
            break;
        case RobotState::GATE_TIMEOUT:
            //cout << "GATE_TIMEOUT" << endl;
            gateTimeout(blobsFront, blobsBack);
            break;
        case RobotState::KICK_BALL:
            //cout << "KICK_BALL" << endl;
            kickBall(blobsFront, blobsBack);
            break;
        case RobotState::NOT_GREEN:
            //cout << "NOT_GREEN" << endl;
            notGreen(blobsFront, blobsBack);
            break;
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

bool RobotLogic::isGreen(blobs_processed blobsFront, blobs_processed blobsBack) {
    if (blobsFront.total_green < MIN_GREEN_AREA) return false;
    else return true; 
}

void RobotLogic::findBall(blobs_processed blobsFront, blobs_processed blobsBack) {
    std::cout << " Finding ball nemo." << std::endl;
    
    if (rController->hasBall()) rState = RobotState::FIND_GATE;
    if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
    if (false) rState = RobotState::BALL_TIMEOUT;
    
    if (blobsFront.o_ball.size() > 0) {

        orange_ball oBall = blobsFront.o_ball.at(0);

        int x = oBall.orange_cen_x;
        
        //Fancy formula
        int turnSpd = (CAM_W/2.0 - x + 30)*0.07;
        int moveSpd = 5 + 800/oBall.orange_w;
        
        if (oBall.orange_w > 5) {
            if (x < CAM_W / 2 - 10) {
                //cout << "LEFT" << "\n";
                rController->driveRobot(moveSpd, 0, turnSpd);
            } else if (x > CAM_W / 2 + 10) {
                //cout << "RIGHT" << "\n";
                rController->driveRobot(moveSpd, 0, turnSpd);
            } else rController->driveRobot(40, 0, 0);
        } else {
            rController->driveRobot(0, 0, 20);
        }

        //If ball is near, turn on dribbler
        if (oBall.orange_w > 50) {
            rController->runDribbler();
        } else {
            //rController->stopDribbler();
        }
    } else {
        rController->driveRobot(0, 0, 20);
    }
    
}

void RobotLogic::ballTimeout(blobs_processed blobsFront, blobs_processed blobsBack) {

}

void RobotLogic::findGate(blobs_processed blobsFront, blobs_processed blobsBack) {
    if(!rController->hasBall()) rState = RobotState::FIND_BALL;
    rController->runDribbler();
    
    int goalX = -2, goalY;
    cout << "blue size:" << blobsFront.b_gate.size() << endl;
    if(blobsFront.b_gate.size() > 0)
        cout << "goal size: " << blobsFront.b_gate.at(0).blue_w << endl;

    if (goal == Goal::gBLUE) {
        if (blobsFront.b_gate.size() > 0) {
            if (blobsFront.b_gate.at(0).blue_w > 30) {
                goalX = blobsFront.b_gate.at(0).blue_cen_x;
                goalY = blobsFront.b_gate.at(0).blue_cen_y;
            }
            
        } else {
            //rState = RobotState.GATE_TIMEOUT;
            rController->driveRobot(0, 0, 10);
            return;
        }
    }
    
    if (goal == Goal::gYELLOW) {
        if (blobsFront.y_gate.size() > 0) {
            if (blobsFront.y_gate.at(0).yellow_w > 20) {
                goalX = blobsFront.y_gate.at(0).yellow_cen_x;
                goalY = blobsFront.y_gate.at(0).yellow_cen_y;
            }
            
        } else {
            //rState = RobotState.GATE_TIMEOUT;
            rController->driveRobot(0, 0, 10);
            return;
        }
    }
    //cout << "goalX:" << goalX << endl;
    int aimTresh = blobsFront.b_gate.at(0).blue_w * 0.2 - 10;
    int turnSpd = (CAM_W/2.0 - blobsFront.b_gate.at(0).blue_cen_x)*0.05 + 5;
    if (goalX == -2) rController->driveRobot(0, 0, 10);
    else if (goalX < 320 - aimTresh) rController->driveRobot(0, 0, turnSpd);
    else if (goalX > 320 + aimTresh) rController->driveRobot(0, 0, turnSpd);
    else {
        rState = RobotState::KICK_BALL;
    }
    rController->stopDribbler();
}

void RobotLogic::gateTimeout(blobs_processed blobsFront, blobs_processed blobsBack) {

    //if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
}

void RobotLogic::kickBall(blobs_processed blobsFront, blobs_processed blobsBack) {
    cout << "!!!KICKBALL!!!" << "\n";
    rController->kickBall(2000);
    rState = RobotState::FIND_BALL;
    rController->stopDribbler();
    //if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
}

void RobotLogic::notGreen(blobs_processed blobsFront, blobs_processed blobsBack) {
    //TODO : something smart
    rState = RobotState::FIND_BALL;
}