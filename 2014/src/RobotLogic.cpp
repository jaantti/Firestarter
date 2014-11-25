/* 
 * File:   RobotLogic.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

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
        runAttack(deltaTime);
    }
    if (role == Role::rDEFEND) {
        runDefend(deltaTime);
    }
}

void RobotLogic::runAttack(float dt) {

    loadOperationalData();
    countBallData();
    
    switch (rState) {
        case RobotState::IDLE:
            cout << "IDLE" << endl;
            idle();
            break;
        case RobotState::FIND_BALL:
            cout << "FIND_BALL" << endl;
            findBall();
            lastState = RobotState::FIND_BALL;
            break;
        case RobotState::BALL_TIMEOUT:
            cout << "BALL_TIMEOUT" << endl;
            ballTimeout();
            lastState = RobotState::BALL_TIMEOUT;
            break;
        case RobotState::FIND_GATE:
            cout << "FIND_GATE" << endl;
            findGate();
            lastState = RobotState::FIND_GATE;
            break;
        case RobotState::GATE_TIMEOUT:
            cout << "GATE_TIMEOUT" << endl;
            gateTimeout();
            lastState = RobotState::GATE_TIMEOUT;
            break;
        case RobotState::KICK_BALL:
            cout << "KICK_BALL" << endl;
            kickBall();
            lastState = RobotState::KICK_BALL;
            break;
        case RobotState::NOT_GREEN:
            cout << "NOT_GREEN" << endl;
            notGreen();
            lastState = RobotState::NOT_GREEN;
            break;
        case RobotState::STALLING:
            cout << "STALLING" << endl;
            stalled();
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

void RobotLogic::runDefend(float dt) {

}

void RobotLogic::setRState(RobotState state) {
    rState = state;
}

bool RobotLogic::isGreen() {
    if(rController->getDriveDir()==DriveDirection::FRONT){
        if (greens.front_green < MIN_GREEN_AREA) return false;
        else return true; 
    } else if(rController->getDriveDir()==DriveDirection::REAR){
        if (greens.back_green < MIN_GREEN_AREA) return false;
        else return true;
    }
    return true;
}

void RobotLogic::idle() {
    
    //Set the initial time for serial connection. Odometry purposes.
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long int tim1 = 1000000 * tv.tv_sec + tv.tv_usec;
    rController->initSerialTime(tim1);

    rController->stopDribbler();
    rController->driveRobot(0,0,0);
    setGoal();
    
    if (rController->getStart()) {
        rController->chargeCoil();
        rState = RobotState::FIND_BALL;
        usleep(250000);
        return;
    }
    
    rController->dischargeCoil();
    usleep(1000000);
}


//2 options : Robot drives to the closest ball, then locates the gate and kicks.
// OR : Look where there are more balls, drive there and proceed to shoot goals.
void RobotLogic::findBall() {
    
    ballState = getBallState();
    
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
    
    if (ballTimeout) {
        cout << "STATE: BALL_TIMEOUT" << endl;
        rState = RobotState::BALL_TIMEOUT;
        return;
    }
    
    switch(ballState){
        case BallFindState::BALL_FRONT:
            //Drive to ball, pick it up, shoot for the moon.
            driveBallsFront();
            break;
        case BallFindState::BALL_REAR:
            //Drive until the ball is closer than a threshold, then turn and pick it up.
            driveBallsRear();
            break;
        case BallFindState::BALL_NOT_FOUND:
            //Turn for a bit, then enter timeout
            ballsNotFound();
            break;
        case BallFindState::ROBOT_ROTATE:
            //The robot should use this state to turn around and reinitialize the state.
            robotRotate(false);
            break;
    }        
}
/*
 *Speed is based on distance - e^x , where maximum x is 5.3 for a speed of 222. Minimum at 0, where speed is 22 + e^0 = 23
 * The speed is mapped based on a distance from 0 to 5m to the range of [0, 5.3]. 
 */



int RobotLogic::calculateMoveSpeed(float distance) {
    if(distance>5.0f) distance = 5.0f;
        
    float speedExponent = distance+0.3; 
    int speedBase = (int)Math::pow(RobotConstants::Euler, speedExponent);
    return (22 + speedBase);
}


void RobotLogic::driveBallsFront() {
    Ball ball = getFirstFrontBall();    
    
    int turnSpd = getAngle(ball.getCen_x())*0.8;
    int moveDir = ball.getAngle()/180*PI*0.8;
    //TODO : Replace len usage with distance.
    int moveSpd = calculateMoveSpeed(ball.getDistance());
        
    rController->driveRobot(moveSpd, moveDir, turnSpd);   
}

void RobotLogic::driveBallsRear() {
    Ball ball = getFirstRearBall();    
    
    if(ball.getDistance()<0.33f) {
        lockBallTurn();
        return;
    }
    
    int turnSpd = getAngle(ball.getCen_x())*0.8;
    int moveDir = ball.getAngle()/180*PI*0.8;
    //TODO : Replace len usage with distance.
    int moveSpd = calculateMoveSpeed(ball.getDistance());
       
    rController->driveRobot( (-1.0*moveSpd), (-1.0*moveDir), (-1.0*turnSpd) );   
}

void RobotLogic::ballsNotFound() {
    //TODO : Drive around in a circle, increase timeout counter.
    ballTimeoutCount++;
    rController->driveRobot(0, 0, 40);
    if(ballTimeoutCount>RobotConstants::ballTimeoutThresh){
        setRState(RobotState::BALL_TIMEOUT);
    }
}

void RobotLogic::robotRotate() {
    rController->turnAround(80);    
    releaseBallDriveLocks();
}

//Neither camera saw balls for 1 second.
//This function should keep the robot in timeout state until both gates can be seen.
void RobotLogic::ballTimeout() {
    //TODO : Some smart logamathingie
}


void RobotLogic::findGate() {

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
    
    if(gateTimeout){
        cout << "ENTERING GATE TIMEOUT" << endl;
        rState = RobotState::GATE_TIMEOUT;
        return;
    }
    
    gateState = getGateState();
    
    
    
    switch(gateState){
        //Aim for the gate and shoot.
        case GateFindState::GATE_VISIBLE_FRONT:
            gateVisibleFront();
            break;            
        //Rotate, shoot.
        case GateFindState::GATE_VISIBLE_REAR:
            gateVisibleRear();
            break;
        //Relocate to a better position, rotate until gate visible, shoot.
        case GateFindState::OPPOSING_GATE_FRONT:
            opposingGateFront();
            break;
        //Relocate to a better position, rotate, shoot..
        case GateFindState::OPPOSING_GATE_REAR:
            opposingGateRear();
            break;
        //Turn until a gate is found, otherwise enter a timeout. (RobotConstants::gateTimeout)
        case GateFindState::GATE_INVISIBLE:
            gateInvisible();
            break;
        //The system has decided that it must rotate to reach an optimal aiming solution. Rotation = ~180 deg.
        case GateFindState::GATE_ROTATE:
            robotRotate(true);
            break;
    }
    
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


//Drive the robot closer to own goal.
void RobotLogic::gateTimeout() {

}

void RobotLogic::kickBall() {
    rController->stopDribbler();
    rController->kickBall(2000);
    usleep(16667);
    rState = RobotState::FIND_BALL;
}

void RobotLogic::notGreen() {
    //TODO : something smart
    rController->driveRobot(0,0,100);
    cout << "STATE: FIND_BALL" << endl;
    rState = RobotState::FIND_BALL;
}

void RobotLogic::stalled() {
    rController->driveReverse();
    usleep(200000);
    rState = lastState;
    
}

void RobotLogic::loadOperationalData() {
    this->balls = pProcessor->getBalls();
    this->bGate = pProcessor->getBlueGate();
    this->yGate = pProcessor->getYellowGate();
    this->greens = pProcessor->getGreen();
}

BallFindState RobotLogic::getBallState() {
    if(ball_front_drive && hasBallsFront()){
        return BallFindState::BALL_FRONT;
    } else if(ball_rear_drive && hasBallsRear()) {
        return BallFindState::BALL_REAR;
    }
    
    //If neither lock is currently active, look for a new one (or keep rotating until 180 deg)
    releaseBallDriveLocks();
    if(ball_rear_turn){
        return BallFindState::ROBOT_ROTATE;
    }
    
    if(balls.size()>0){
        if(balls.at(0).getDir() == RobotConstants::Direction::FRONT){
            lockFrontBallDrive();
            return BallFindState::BALL_FRONT;
        } else {
            lockRearBallDrive();
            return BallFindState::BALL_REAR;
        }
    } else {
        releaseBallTurnLock();
        return BallFindState::BALL_NOT_FOUND
    }
    
}


GateFindState RobotLogic::getGateState() {
    
    if(gate_rear_turn){
        return GateFindState::GATE_ROTATE;
    }
        
    if(goal == Goal::gBLUE){
        if(bGate.GetDir()==RobotConstants::Direction::FRONT){
            return GateFindState::GATE_VISIBLE_FRONT;
        } else if (bGate.GetDir()==RobotConstants::Direction::REAR){
            return GateFindState::GATE_VISIBLE_REAR;
        } else if (yGate.GetDir()==RobotConstants::Direction::FRONT){
            return GateFindState::OPPOSING_GATE_FRONT;
        } else if (yGate.GetDir()==RobotConstants::Direction::REAR){
            return GateFindState::OPPOSING_GATE_REAR;
        } else {
            return GateFindState::GATE_INVISIBLE;
        }
    } else {
        if(yGate.GetDir()==RobotConstants::Direction::FRONT){
            return GateFindState::GATE_VISIBLE_FRONT;
        } else if (yGate.GetDir()==RobotConstants::Direction::REAR){
            return GateFindState::GATE_VISIBLE_REAR;
        } else if (bGate.GetDir()==RobotConstants::Direction::FRONT){
            return GateFindState::OPPOSING_GATE_FRONT;
        } else if (bGate.GetDir()==RobotConstants::Direction::REAR){
            return GateFindState::OPPOSING_GATE_REAR;
        } else {
            return GateFindState::GATE_INVISIBLE;
        }
    }
}

void RobotLogic::lockBallTurn() {
    ball_rear_turn = true;
}

void RobotLogic::lockFrontBallDrive() {
    ball_front_drive = true;
}

void RobotLogic::lockRearBallDrive() {
    ball_rear_drive = true;
}

void RobotLogic::lockGateTurn() {
    gate_rear_turn = true;
}

void RobotLogic::releaseBallDriveLocks() {
    ball_rear_drive = false;
    ball_front_drive = false;
}

void RobotLogic::releaseBallTurnLock() {
    ball_rear_turn = false;
}

void RobotLogic::releaseGateTurnLock() {
    gate_rear_turn = false;
}

bool RobotLogic::hasBallsFront() {
    for(Ball ball : balls){
        if (ball.getDir()==RobotConstants::Direction::FRONT) return true;
    }
    return false;
}

bool RobotLogic::hasBallsRear() {
    for(Ball ball : balls){
        if (ball.getDir()==RobotConstants::Direction::REAR) return true;
    }
    return false;
}

Ball RobotLogic::getFirstFrontBall() {
    for(Ball ball : balls){
        if(ball.getDir()==RobotConstants::Direction::FRONT) return ball;
    }
}

Ball RobotLogic::getFirstRearBall() {
    for(Ball ball : balls){
        if(ball.getDir()==RobotConstants::Direction::REAR) return ball;
    }
}

