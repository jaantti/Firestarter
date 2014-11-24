/* 
 * File:   RobotLogic.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "OdometerLocalizer.h"
#include "RobotLogic.h"
#include <iostream>


// IMPORTANT : 
// this system is being refactored to use new inputs
// from imagePostProcessor : Ball and YellowGate / BlueGate structures.
//
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

unsigned long RobotLogic::timeSinceLastSerial() {
    return rController->timeSinceLastLoop();
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
        runAttack(dt);
    }
    if (role == Role::rDEFEND) {
        runDefend(dt);
    }
}

void RobotLogic::runAttack(float dt) {

    blobs_processed blobsFront = pProcessor->getFrontSystem();
    blobs_processed blobsBack = pProcessor->getBackSystem();
    
    switch (rState) {
        case RobotState::IDLE:
            cout << "IDLE" << endl;
            idle();
            break;
        case RobotState::FIND_BALL:
            //cout << "FIND_BALL" << endl;
            findBall(blobsFront, blobsBack);
            lastState = RobotState::FIND_BALL;
            break;
        case RobotState::BALL_TIMEOUT:
            cout << "BALL_TIMEOUT" << endl;
            ballTimeout(blobsFront, blobsBack);
            lastState = RobotState::BALL_TIMEOUT;
            break;
        case RobotState::FIND_GATE:
            cout << "FIND_GATE" << endl;
            findGate(blobsFront, blobsBack);
            lastState = RobotState::FIND_GATE;
            break;
        case RobotState::GATE_TIMEOUT:
            cout << "GATE_TIMEOUT" << endl;
            gateTimeout(blobsFront, blobsBack);
            lastState = RobotState::GATE_TIMEOUT;
            break;
        case RobotState::KICK_BALL:
            cout << "KICK_BALL" << endl;
            kickBall(blobsFront, blobsBack);
            lastState = RobotState::KICK_BALL;
            break;
        case RobotState::NOT_GREEN:
            cout << "NOT_GREEN" << endl;
            notGreen(blobsFront, blobsBack);
            lastState = RobotState::NOT_GREEN;
            break;
        case RobotState::STALLING:
            cout << "STALLING" << endl;
            stalled(blobsFront, blobsBack);
            lastState = RobotState::STALLING;
            break;
    }
    //Handle odometry, localization.
    /*
    vector<float> speeds = rController->getAllMotorSpeeds();
    Odometer::Movement movement = odometer->calculateMovement(speeds.at(0), speeds.at(3), speeds.at(1), speeds.at(2));
    odometryLocalizer->move(movement.velocityX, movement.velocityY, movement.omega, dt);
    Localizer->move(movement.velocityX, movement.velocityY, movement.omega, dt);
    std::cout << " The robot is stalled : " << rController->isStalled() << std::endl;
    */
    
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

void RobotLogic::idle() {
    struct timeval tv, tv2;
    gettimeofday(&tv, NULL);
    unsigned long int tim1 = 1000000 * tv.tv_sec + tv.tv_usec;
    rController->initSerialTime(tim1);

    rController->stopDribbler();
    rController->driveRobot(0,0,0);
    setGoal();
    //cout << " GetStart status : " << rController->getStart() << " . " << std::endl;
    if (rController->getStart()) {
        rController->chargeCoil();
        rState = RobotState::FIND_BALL;
        usleep(250000);
        return;
    }
    rController->dischargeCoil();
    usleep(1000000);
}

void RobotLogic::findBall(blobs_processed blobsFront, blobs_processed blobsBack) {
    //isGreen(blobsFront, blobsBack);    
    if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
    
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
        return;        
    }
    
    if (false) {
        cout << "STATE: BALL_TIMEOUT" << endl;
        rState = RobotState::BALL_TIMEOUT;
        return;
    }
    if (blobsFront.o_ball.size() > 0) {

        orange_ball oBall = blobsFront.o_ball.at(0);

        int x = oBall.orange_cen_x;
        int y = oBall.orange_cen_y;
        
        int turnSpd = getAngle(x)*0.8;
        int moveDir = getAngle(x)/180*PI*0.8;
        int moveSpd = 22 + 1600/oBall.orange_w;
        
        if (oBall.orange_w > 5) {
            rController->driveRobot(moveSpd, moveDir, turnSpd);
        } else {
            rController->driveRobot(0, 0, 30);
        }
        
        //If ball is near, turn on dribbler
        if (oBall.orange_w > 15) {
            rController->runDribbler();
        } else {
            //rController->stopDribbler();
        }
    } else {
        rController->driveRobot(0, 0, 30);
    }
    
}

void RobotLogic::ballTimeout(blobs_processed blobsFront, blobs_processed blobsBack) {

}

void RobotLogic::findGate(blobs_processed blobsFront, blobs_processed blobsBack) {
    startCounter++;
    if (startCounter>=5) {
        startCounter=0;
        if (!(rController->getStart())) {
            rState = RobotState::IDLE;
            return;
        }
    }
    
    if(!rController->hasBall()) {
        rState = RobotState::FIND_BALL;
        return;
    }
    rController->runDribbler();
    int aimThresh = -1;
    int turnSpeed = -1;
    int goalX = -2, goalY;
    if(blobsFront.b_gate.size() > 0)

    if (goal == Goal::gBLUE) {
        if (blobsFront.b_gate.size() > 0) {
            if (blobsFront.b_gate.at(0).blue_w > MIN_GATE_WIDTH) {
                goalX = blobsFront.b_gate.at(0).blue_cen_x;
                goalY = blobsFront.b_gate.at(0).blue_cen_y;
                aimThresh = blobsFront.b_gate.at(0).blue_w * 0.2+5;
                float angleSpd = getAngle(goalX)*0.5;
                float turn = 0;
                if (angleSpd < 0) turn = -3;
                if (angleSpd > 0) turn = 3;
                turnSpeed = angleSpd+turn;
            }
            
        } else {
            //rState = RobotState.GATE_TIMEOUT;
            rController->driveRobot(0, 0, 30);
            return;
        }
    }
    
    if (goal == Goal::gYELLOW) {
        if (blobsFront.y_gate.size() > 0) {
            if (blobsFront.y_gate.at(0).yellow_w > MIN_GATE_WIDTH) {
                goalX = blobsFront.y_gate.at(0).yellow_cen_x;
                goalY = blobsFront.y_gate.at(0).yellow_cen_y;
                aimThresh = blobsFront.y_gate.at(0).yellow_w * 0.2+5;
                float angleSpd = getAngle(goalX)*0.5;
                float turn = 0;
                if (angleSpd < 0) turn = -3;
                if (angleSpd > 0) turn = 3;
                turnSpeed = angleSpd+turn;
            }
            
        } else {
            //rState = RobotState.GATE_TIMEOUT;
            rController->driveRobot(0, 0, 30);
            return;
        }
    }
    if (goalX == -2) rController->driveRobot(0, 0, 30);
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
    rController->stopDribbler();
    rController->kickBall(2000);
    usleep(16667);
    rState = RobotState::FIND_BALL;
    
    //if (!isGreen(blobsFront, blobsBack)) rState = RobotState::NOT_GREEN;
}

void RobotLogic::notGreen(blobs_processed blobsFront, blobs_processed blobsBack) {
    //TODO : something smart
    rController->driveRobot(0,0,100);
    cout << "STATE: FIND_BALL" << endl;
    rState = RobotState::FIND_BALL;
}

void RobotLogic::stalled(blobs_processed blobsFront, blobs_processed blobsBack) {
    rController->driveReverse();
    usleep(16667);
    rState = lastState;
    
}
