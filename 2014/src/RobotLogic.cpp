/* 
 * File:   RobotLogic.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "OdometerLocalizer.h"


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

void RobotLogic::setInitialTime(long unsigned int time) {
    rController->initSerialTime(time);
}

unsigned long RobotLogic::timeSinceLastSerial(long unsigned int lastSerial) {
    return rController->timeSinceLastLoop(lastSerial);
}


void RobotLogic::init(RobotController* rCont, ImageProcessor* iProc) {
    rController = rCont;
    iProcessor = iProc;
}

void RobotLogic::init(RobotController* rCont, ImagePostProcessor* pProc) {
    rController = rCont;
    pProcessor = pProc;
}

void RobotLogic::loadOdometer(Odometer *odometer){
	this->odometer = odometer;
}

void RobotLogic::loadOdometryLocalizer(OdometerLocalizer *odometerLocalizer){
    this->odometryLocalizer = odometerLocalizer;
}

void RobotLogic::setPosition(float x, float y, float orientation){
    this->odometryLocalizer->setPosition(x, y, orientation);
    this->localizer->setPosition(x, y, orientation);
    this->posX = x;
    this->posY = y;
    this->orientation = Math::floatModulus(orientation, Math::TWO_PI);
}

void RobotLogic::loadParticleFilterLocalizer(ParticleFilterLocalizer *localizer){
	this->localizer = localizer;
}

void RobotLogic::setGoal() {
    char gate = rController->getAttackedGoal();
    if (gate=='B') {
        goal = Goal::gBLUE;
    }
    else if (gate=='Y') {
        goal = Goal::gYELLOW;
    }
    else {
        cout << "robotLogic: setGoal: wat?" << endl;
    }
}

float RobotLogic::getAngle(int x_coor) {
	return (((float)x_coor-(float)CAM_W/2.0)/(float)CAM_W*(float)CAM_HFOV*-1.0);
}

void RobotLogic::run(Role role, float deltaTime) {
    if (role == Role::rATTACK) {
        runAttack();
    }
    if (role == Role::rDEFEND) {
        runDefend();
    }
}

void RobotLogic::runAttack() {

    blobs_processed blobsFront = pProcessor->getFrontSystem();
    pProcessor->unlockFrontSystem();
    blobs_processed blobsBack = pProcessor->getBackSystem();
    pProcessor->unlockBackSystem();
    
    switch (rState) {
        case RobotState::IDLE:
            cout << "IDLE" << endl;
            idle();
            break;
        case RobotState::FIND_BALL:
            //cout << "FIND_BALL" << endl;
            findBall(blobsFront, blobsBack);
            break;
        case RobotState::BALL_TIMEOUT:
            cout << "BALL_TIMEOUT" << endl;
            ballTimeout(blobsFront, blobsBack);
            break;
        case RobotState::FIND_GATE:
            //cout << "FIND_GATE" << endl;
            findGate(blobsFront, blobsBack);
            break;
        case RobotState::GATE_TIMEOUT:
            cout << "GATE_TIMEOUT" << endl;
            gateTimeout(blobsFront, blobsBack);
            break;
        case RobotState::KICK_BALL:
            cout << "KICK_BALL" << endl;
            kickBall(blobsFront, blobsBack);
            break;
        case RobotState::NOT_GREEN:
            cout << "NOT_GREEN" << endl;
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
    cout << "total green area: " << blobsFront.total_green << endl;
    if (blobsFront.total_green < MIN_GREEN_AREA) return false;
    else return true; 
}

void RobotLogic::idle() {
    rController->stopDribbler();
    rController->driveRobot(0,0,0);
    rController->dischargeCoil();
    setGoal();
    if (rController->getStart()) {
        rController->chargeCoil();
        cout << "STATE: FIND_BALL" << endl;
        rState = RobotState::FIND_BALL;
        return;
    }
    usleep(500000);
}

void RobotLogic::findBall(blobs_processed blobsFront, blobs_processed blobsBack) {
    //std::cout << " Finding ball nemo." << std::endl;
    //vector<float> speeds = rController->getAllMotorSpeeds();
    //cout << "motor speeds (rad/s):" << endl;
    //for (float i : speeds){
    //    cout << i << endl;
    //}
    startCounter++;
    rController->stopDribbler();
    if (startCounter>=5) {
        startCounter=0;
        if (!(rController->getStart())) {
            cout << "STATE: IDLE" << endl;
            rState = RobotState::IDLE;
            return;
        }
    }
    if (rController->hasBall()){
        cout << "STATE: FIND_GATE" << endl;
        rState = RobotState::FIND_GATE;
        //setGoal();
        return;        
    }
    /*    
    if (!isGreen(blobsFront, blobsBack)) {
        cout << "STATE: NOT_GREEN" << endl;
        rState = RobotState::NOT_GREEN;
        return;
    }
    */
    if (false) {
        cout << "STATE: BALL_TIMEOUT" << endl;
        rState = RobotState::BALL_TIMEOUT;
        return;
    }
    if (blobsFront.o_ball.size() > 0) {

        orange_ball oBall = blobsFront.o_ball.at(0);

        int x = oBall.orange_cen_x;
        int y = oBall.orange_cen_y;
        
        //Fancy formula
        //int turnSpd = (CAM_W/2.0 - x)*0.07;
        //cout << "ball x:" << x << endl;
        //cout << "ball y:" << y << endl;
        //cout << "---------------" << endl;
        //cout << "ball angle:" << getAngle(x) << endl;
        int turnSpd = getAngle(x)*0.8;
        int moveDir = getAngle(x)/180*PI*0.8;
        int moveSpd = 15 + 800/oBall.orange_w;
        
        /*
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
        */
        if (oBall.orange_w > 5) {
            rController->driveRobot(moveSpd, moveDir, turnSpd);
        } else {
            rController->driveRobot(0, 0, 15);
        }
        
        //If ball is near, turn on dribbler
        if (oBall.orange_w > 30) {
            rController->runDribbler();
        } else {
            //rController->stopDribbler();
        }
    } else {
        rController->driveRobot(0, 0, 15);
    }
    
}

void RobotLogic::ballTimeout(blobs_processed blobsFront, blobs_processed blobsBack) {

}

void RobotLogic::findGate(blobs_processed blobsFront, blobs_processed blobsBack) {
    startCounter++;
    if (startCounter>=5) {
        startCounter=0;
        if (!(rController->getStart())) {
            cout << "STATE: IDLE" << endl;
            rState = RobotState::IDLE;
            return;
        }
    }
    
    if(!rController->hasBall()) {
        cout << "STATE: FIND_BALL" << endl;
        rState = RobotState::FIND_BALL;
        return;
    }
    rController->runDribbler();
    //setGoal();
    int aimThresh = -1;
    int turnSpeed = -1;
    int goalX = -2, goalY;
    //cout << "blue size:" << blobsFront.b_gate.size() << endl;
    if(blobsFront.b_gate.size() > 0)
        cout << "blue size: " << blobsFront.b_gate.at(0).blue_w << endl;

    if (goal == Goal::gBLUE) {
        if (blobsFront.b_gate.size() > 0) {
            if (blobsFront.b_gate.at(0).blue_w > MIN_GATE_WIDTH) {
                goalX = blobsFront.b_gate.at(0).blue_cen_x;
                cout << "blue x: " << goalX << endl;
                goalY = blobsFront.b_gate.at(0).blue_cen_y;
                aimThresh = blobsFront.b_gate.at(0).blue_w * 0.1+5;
                turnSpeed = getAngle(goalX)*0.8+3;
                //turnSpeed = (CAM_W/2.0 - blobsFront.b_gate.at(0).blue_cen_x)*0.05 + 5;
            }
            
        } else {
            //rState = RobotState.GATE_TIMEOUT;
            rController->driveRobot(0, 0, 15);
            return;
        }
    }
    
    if (goal == Goal::gYELLOW) {
        if (blobsFront.y_gate.size() > 0) {
            cout << "yellow size: " << blobsFront.y_gate.at(0).yellow_w << endl;
            if (blobsFront.y_gate.at(0).yellow_w > MIN_GATE_WIDTH) {
                goalX = blobsFront.y_gate.at(0).yellow_cen_x;
                cout << "yellow x: " << goalX << endl;
                goalY = blobsFront.y_gate.at(0).yellow_cen_y;
                aimThresh = blobsFront.y_gate.at(0).yellow_w * 0.1+5;
                turnSpeed = getAngle(goalX)*0.8+3;
                //turnSpeed = (CAM_W/2.0 - blobsFront.y_gate.at(0).yellow_cen_x)*0.05 + 5;
            }
            
        } else {
            //rState = RobotState.GATE_TIMEOUT;
            rController->driveRobot(0, 0, 15);
            return;
        }
    }
    //cout << "goalX:" << goalX << endl;
    if (goalX == -2) rController->driveRobot(0, 0, 15);
    else if (goalX < CAM_W/2 - aimThresh) rController->driveRobot(0, 0, turnSpeed);
    else if (goalX > CAM_W/2 + aimThresh) rController->driveRobot(0, 0, turnSpeed);
    else {
        cout << "STATE: KICK_BALL" << endl;
        rState = RobotState::KICK_BALL;
        return;
    }
    //rController->stopDribbler();
}

void RobotLogic::gateTimeout(blobs_processed blobsFront, blobs_processed blobsBack) {

    //if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
}

void RobotLogic::kickBall(blobs_processed blobsFront, blobs_processed blobsBack) {
    cout << "!!!KICKBALL!!!" << "\n";
    rController->stopDribbler();
    rController->kickBall(770);
    cout << "STATE: FIND_BALL" << endl;
    rState = RobotState::FIND_BALL;
    
    //if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
}

void RobotLogic::notGreen(blobs_processed blobsFront, blobs_processed blobsBack) {
    //TODO : something smart
    rController->driveRobot(0,0,100);
    cout << "STATE: FIND_BALL" << endl;
    rState = RobotState::FIND_BALL;
}
