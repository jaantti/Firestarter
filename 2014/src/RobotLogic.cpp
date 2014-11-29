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

void RobotLogic::loadOdometer(Odometer *odometer) {
    this->odometer = odometer;
}

void RobotLogic::loadOdometryLocalizer(OdometerLocalizer *odometerLocalizer) {
    this->odometryLocalizer = odometerLocalizer;
}

void RobotLogic::setPosition(float x, float y, float orientation) {
    this->odometryLocalizer->setPosition(x, y, orientation);
    this->localizer->setPosition(x, y, orientation);
    this->posX = x;
    this->posY = y;
    this->orientation = Math::floatModulus(orientation, Math::TWO_PI);
}

void RobotLogic::loadParticleFilterLocalizer(ParticleFilterLocalizer *localizer) {
    this->localizer = localizer;
}

void RobotLogic::setGoal() {
    char gate = rController->getAttackedGoal();
    if (gate == 'B') {
        goal = Goal::gBLUE;
    } else if (gate == 'Y') {
        goal = Goal::gYELLOW;
    } else {
        cout << "robotLogic: setGoal: wat?" << endl;
    }
}

float RobotLogic::getAngle(int x_coor) {
    return (((float) x_coor - (float) CAM_W / 2.0) / (float) CAM_W * (float) CAM_HFOV*-1.0);
}

void RobotLogic::run(float deltaTime) {
    if (role == Role::rATTACK) {
        runAttack(deltaTime);
    }
    if (role == Role::rDEFEND) {
        runDefend(deltaTime);
    }
}

void RobotLogic::runAttack(float dt) {

    loadOperationalData();

    switch (rState) {
        case RobotState::IDLE:
            cout << "IDLE: ATTACK" << endl;
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
    loadOperationalData();
    switch (rState) {
        case RobotState::IDLE:
            cout << "IDLE: DEFENSE" << endl;
            idle();
            break;
        case RobotState::FIND_BALL:
            cout << "FIND_BALL" << endl;
            rState = RobotState::DEFEND_INITIAL;
            break;
        case RobotState::DEFEND_INITIAL:
            cout << "DEFEND_INITIAL" << endl;
            defendInitial();
            break;
        case RobotState::DEFEND_BALL:
            cout << "DEFEND_BALL" << endl;
            defendBall();
            break;
        case RobotState::DEFEND_FINDGATE:
            cout << "DEFEND_FINDGATE" << endl;
            defendFindGate();
            break;
        case RobotState::DEFEND_SCAN:
            cout << "DEFEND_SCAN" << endl;
            defendScan();
            break;
        case RobotState::DEFEND_KICK:
            cout << "DEFEND_KICK" << endl;
            defendKick();
            break;

    }

}

void RobotLogic::setRState(RobotState state) {
    rState = state;
}

void RobotLogic::driveToGate() {
    long driveTime = 100000;
    rController->driveRobot(50, 0, 0);
    usleep(driveTime);
    rController->driveRobot(50, PI / 2.0, 0);
    usleep(driveTime * 7);
    rState = RobotState::DEFEND_FINDGATE;
}

void RobotLogic::defendBall() {
    startCounter++;
    if (startCounter >= 5) {
        startCounter = 0;
        if (!(rController->getStart())) {
            cout << "STATE: IDLE" << endl;
            rState = RobotState::IDLE;
            return;
        }
    }
    
    rController->stopDribbler();
    
    if (rController->hasBall()) {
        cout << "STATE: DEFEND_KICK" << endl;
        rState = RobotState::DEFEND_KICK;
        return;
    }
    
    gateState = getDefendGateState();
    ballState = getBallState();
    
    if ((gateState != GateFindState::OPPOSING_GATE_REAR) && 
            (gateState != GateFindState::OPPOSING_REAR_OTHER_FRONT)) {
        cout << "STATE: DEFEND_FINDGATE" << endl;
        rState = RobotState::DEFEND_FINDGATE;
        return;
    }
    if (ballState != BallFindState::BALL_FRONT) {
        cout << "STATE: DEFEND_SCAN" << endl;
        rState = RobotState::DEFEND_SCAN;
        return;
    }
    
    float gateAngle, ballAngle, gateDistance, ballDistance;
    Ball ball = getFirstFrontBall();
    
    ballAngle = ball.getAngle() / 180.0 * PI;
    ballDistance = ball.getDistance();
    
    if (ballDistance < 0.3) {
        rController->runDribbler();
    }
    
    if (goal == Goal::gBLUE) {
        gateAngle = yGate.GetAngle() / 180.0 * PI;
        gateDistance = yGate.GetDistance();
    } else {
        gateAngle = bGate.GetAngle() / 180.0 * PI;
        gateDistance = bGate.GetDistance();
    }
    
    float moveDir, rotSpd, moveSpd;
    
    moveDir = (PI - gateAngle) + ballAngle;
    rotSpd = (gateAngle - PI) + ballAngle;
    
    if (moveDir > 0.04) {
        moveSpd = Math::abs((float)(MAX_MOTOR_SPEED) * moveDir / PI)*3;
        moveDir = PI / 2.0;
    } else if (moveDir < -0.04) {
        moveSpd = Math::abs((float)(MAX_MOTOR_SPEED) * moveDir / PI)*3;
        moveDir = PI / -2.0;
    } else {
        moveSpd = 0.0;
    }
    
    rController->driveRobot(moveSpd, moveDir, rotSpd);
}

void RobotLogic::defendFindGate() {
    startCounter++;
    if (startCounter >= 5) {
        startCounter = 0;
        if (!(rController->getStart())) {
            cout << "STATE: IDLE" << endl;
            rState = RobotState::IDLE;
            return;
        }
    }
    
    if (rController->hasBall()) {
        cout << "STATE: DEFEND_KICK" << endl;
        rState = RobotState::DEFEND_KICK;
        return;
    }
    
    rController->stopDribbler();
    
    gateState = getDefendGateState();
    
    
    switch (gateState) {
            //Aim for the gate and shoot.
        case GateFindState::GATE_VISIBLE_FRONT:
            cout << "gate visible front" << endl;
            rController->driveRobot(0, 0, 30);
            break;
            //Rotate, shoot.
        case GateFindState::GATE_VISIBLE_REAR:
            cout << "gate visible rear" << endl;
            rController->driveRobot(0, 0, 30);
            break;
            //Relocate to a better position, rotate until gate visible, shoot.
        case GateFindState::OPPOSING_GATE_FRONT:
            cout << "opposing gate visible front" << endl;
            rController->turnAround(180, 100);
            break;
            //Relocate to a better position, rotate, shoot..
        case GateFindState::OPPOSING_GATE_REAR:
            cout << "opposing gate visible rear" << endl;
            cout << "STATE: DEFEND_SCAN" << endl;
            rState = RobotState::DEFEND_SCAN;
            return;
            break;
        case GateFindState::OPPOSING_REAR_OTHER_FRONT:
            cout << "opposing gate visible rear, other front" << endl;
            cout << "STATE: DEFEND_SCAN" << endl;
            rState = RobotState::DEFEND_SCAN;
            return;
            break;
            //Turn until a gate is found, otherwise enter a timeout. (RobotConstants::gateTimeout)
        case GateFindState::GATE_INVISIBLE:
            cout << "gate invisible" << endl;
            rController->driveRobot(0, 0, 30);
            break;
        case GateFindState::GATE_ROTATE:
            cout << "gate rotate" << endl;
            rController->driveRobot(0, 0, 30);
            releaseGateTurnLock();
            break;
    }
    
    
    
}

void RobotLogic::defendKick() {
    startingOppositeDistance = 0.0f;
    rController->stopDribbler();
    rController->kickBall(2000);
    usleep(16667);
    rState = RobotState::DEFEND_FINDGATE;
    
}

void RobotLogic::defendInitial() {
    startCounter++;
    if (startCounter >= 5) {
        startCounter = 0;
        if (!(rController->getStart())) {
            rState = RobotState::IDLE;
            return;
        }
    }
    
    if (rController->hasBall()) {
        cout << "STATE: DEFEND_KICK" << endl;
        rState = RobotState::DEFEND_KICK;
        return;
    }
    
    rController->stopDribbler();
    
    releaseGateTurnLock();
    gateState = getDefendGateState();
    
    if ((gateState == GateFindState::OPPOSING_GATE_REAR) || 
            (gateState == GateFindState::OPPOSING_REAR_OTHER_FRONT)) {
        cout << "STATE: DEFEND_FINDGATE" << endl;
        rState = RobotState::DEFEND_FINDGATE;
        return;
    }
    
    rController->driveRobot(60, PI/2.4, 0);
}

void RobotLogic::defendScan() {
    startCounter++;
    if (startCounter >= 5) {
        startCounter = 0;
        if (!(rController->getStart())) {
            rState = RobotState::IDLE;
            return;
        }
    }
    
    rController->stopDribbler();
    
    if (rController->hasBall()) {
        cout << "STATE: DEFEND_KICK" << endl;
        rState = RobotState::DEFEND_KICK;
        return;
    }
    
    releaseBallDriveLocks();
    ballTimeoutLock = false;
    
    gateState = getDefendGateState();
    ballState = getBallState();
    
    if ((gateState != GateFindState::OPPOSING_GATE_REAR) && 
            (gateState != GateFindState::OPPOSING_REAR_OTHER_FRONT)) {
        cout << "STATE: DEFEND_FINDGATE" << endl;
        rState = RobotState::DEFEND_FINDGATE;
        return;
    }
    if (ballState == BallFindState::BALL_FRONT) {
        cout << "STATE: DEFEND_BALL" << endl;
        rState = RobotState::DEFEND_BALL;
        return;
    }
    
    float gateAngle, gateDistance, attackingGateAngle;
    
    if (goal == Goal::gBLUE) {
        gateAngle = yGate.GetAngle();
        gateDistance = yGate.GetDistance();
        attackingGateAngle = bGate.GetAngle();
    } else {
        gateAngle = bGate.GetAngle();
        gateDistance = bGate.GetDistance();
        attackingGateAngle = yGate.GetAngle();
    }
    
    if (gateAngle < 165) {
        defendScanDir = 1.0;
    } else if (gateAngle > 195) {
        defendScanDir = -1.0;
    }
    
    float moveDir, rotSpd, moveSpd;
    
    if (gateState == GateFindState::OPPOSING_GATE_REAR) {
        moveDir = ((gateDistance - DEFEND_GATE_DISTANCE) + (PI / 2.0)) * defendScanDir;
        moveSpd = 70.0;
        rotSpd = (gateAngle / 180.0 * PI - PI) * 0.15;
    }
    if (gateState == GateFindState::OPPOSING_REAR_OTHER_FRONT) {
        moveDir = ((gateDistance - DEFEND_GATE_DISTANCE) + (PI / 2.0)) * defendScanDir;
        moveSpd = 70.0;
        rotSpd = (attackingGateAngle / 180.0 * PI) * 1.2;
    }
    
    rController->driveRobot(moveSpd, moveDir, rotSpd);
}

void RobotLogic::defend() {
    
    int defendX = -1;
    float defendAngle = -1;
    float defendDistance = -1;
    
    int attackX = -1;
    float attackDistance = -1;
    float attackAngle = -1;
    
    int angleTolerance = 15;
    
    startCounter++;
    if (startCounter >= 5) {
        startCounter = 0;
        if (!(rController->getStart())) {
            rState = RobotState::IDLE;
            return;
        }
    }
    
    if (goal == Goal::gBLUE) {
        defendX = yGate.GetCen_x();
        defendDistance = yGate.GetDistance();
        defendAngle = yGate.GetAngle();
        attackX = bGate.GetCen_x();
        attackDistance = bGate.GetDistance();
        attackAngle = bGate.GetAngle();
        
    } else if (goal == Goal::gYELLOW) {
        defendX = bGate.GetCen_x();
        defendDistance = bGate.GetDistance();
        defendAngle = bGate.GetAngle();
        
        attackX = yGate.GetCen_x();
        attackDistance = yGate.GetDistance();
        attackAngle = yGate.GetAngle();
        
    } else {
        cout << "defend(): I shouldn't be here" << endl;
        return;
    }
    
    if ((hasBallsFront() || hasBallsRear()) && false) {
        //Kaitse palli vastu
    } else {
        //Väravate vahele keskele minemine
        if (Math::abs(attackAngle) > angleTolerance) {
            if (attackAngle > 0) {
                rController->driveRobot(30, 1.5 * PI, -15);
            } else {
                rController->driveRobot(30, PI/2.0, 15);
            }
            
        } else if (Math::abs(defendAngle) > angleTolerance && false) {
            if (defendAngle > 0) {
                rController->driveRobot(30, PI/2.0, 15);
            } else {
                rController->driveRobot(30, 1.5 * PI, -15);
            }
        }
    }
    
    
}

bool RobotLogic::isGreen() {
    if (rController->getDriveDir() == DriveDirection::FRONT) {
        if (greens.front_green < MIN_GREEN_AREA) return false;
        else return true;
    } else if (rController->getDriveDir() == DriveDirection::REAR) {
        if (greens.back_green < MIN_GREEN_AREA) return false;
        else return true;
    }
    return true;
}

void RobotLogic::idle() {
    
    startingOppositeDistance = 0.0f;
    //Set the initial time for serial connection. Odometry purposes.
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long int tim1 = 1000000 * tv.tv_sec + tv.tv_usec;
    rController->initSerialTime(tim1);
    role = rController->getRole();
    rController->stopDribbler();
    rController->driveRobot(0, 0, 0);
    setGoal();
    setRole();

    if (rController->getStart()) {
        rController->chargeCoil();
        rState = RobotState::FIND_BALL;
        usleep(50000);
        return;
    }

    //rController->dischargeCoil();
    usleep(100000);
}


//2 options : Robot drives to the closest ball, then locates the gate and kicks.
// OR : Look where there are more balls, drive there and proceed to shoot goals.

void RobotLogic::findBall() {

    ballState = getBallState();
    
    startingOppositeDistance = 0.0f;
    startCounter++;
    if (startCounter >= 5) {
        startCounter = 0;
        if (!(rController->getStart())) {
            rState = RobotState::IDLE;
            return;
        }
    }

    if (rController->hasBall()) {
        rState = RobotState::FIND_GATE;
        return;
    }

    if (ballTimeoutLock) {
        ballTimeoutLock = false;
        rController->stopDribbler();
        rState = RobotState::BALL_TIMEOUT;
        return;
    }

    switch (ballState) {
        case BallFindState::BALL_FRONT:
            cout << " Driving to ball in front." << std::endl;
            //Drive to ball, pick it up, shoot for the moon.
            driveBallsFront();
            break;
        case BallFindState::BALL_REAR:
            //Drive until the ball is closer than a threshold, then turn and pick it up.
            cout << " Driving to ball in rear." << std::endl;
            driveBallsRear();
            break;
        case BallFindState::BALL_NOT_FOUND:
            std::cout << " Balls not found state." << std::endl;
            //Turn for a bit, then enter timeout
            ballsNotFound();
            break;
        case BallFindState::ROBOT_ROTATE:
            std::cout << " Doing robot rotation." << std::endl;
            //The robot should use this state to turn around and reinitialize the state.
            robotRotate(180, 80);
            break;
    }
}
/*
 *Speed is based on distance - e^x , where maximum x is 5.3 for a speed of 222. Minimum at 0, where speed is 22 + e^0 = 23
 * The speed is mapped based on a distance from 0 to 5m to the range of [0, 5.3]. 
 */


//This function should handle the ball movement speed calculation based on distance.

int RobotLogic::calculateMoveSpeed(float distance, float min, float max, float thresh) {
        
    float speed = min + (max-min)/thresh * distance;
    return Math::min(speed, max);
}

//Generic drive to ball based on the ball's distance and angle.

void RobotLogic::driveBallsFront() {
    Ball ball = getFirstFrontBall();
    if(ball.getDistance()<0.33f){
        rController->runDribbler();
    } else {
        rController->stopDribbler();
    }
    robotDriveWrapperFront(ball.getCen_x(), ball.getAngle(), ball.getDistance(), 30, 160, 1.0f);
    
}


//Drive to the closest rear ball until it is under 20cm, then rotate.

void RobotLogic::driveBallsRear() {
    Ball ball = getFirstRearBall();
    if (ball.getDistance() < 0.5f) {
        cout << "BALL AT REAR, TURNING" << endl;
        lockBallTurn();
        return;
    }
    
    robotDriveWrapperRear(ball.getCen_x(), ball.getAngle(), ball.getDistance(), 35, 160, 1.0f);
}

void RobotLogic::robotDriveWrapperFront(int cen_x, float angle, float distance, float minSpd, float maxSpd, float speedThreshold){
    float turnSpd = getAngle(cen_x)*0.8;
    float moveDir = angle /180.0 * PI * 0.8;
    float moveSpd = calculateMoveSpeed(distance, minSpd, maxSpd, speedThreshold);
    rotationSpeedSaver = turnSpd;
    
    rController->driveRobot(moveSpd, moveDir, turnSpd);
}

void RobotLogic::robotDriveWrapperRear(int cen_x, float angle, float distance, float minSpd, float maxSpd, float speedThreshold) {
    float turnSpd = getAngle(cen_x)*0.8;
    float moveDir = angle / 180.0 * PI;
    moveDir = PI + ((moveDir - PI) * 0.8);
    if (moveDir >= PI) {
        moveDir -= 2*PI;
    }
    float moveSpd = calculateMoveSpeed(distance, minSpd, maxSpd, speedThreshold);
    rotationSpeedSaver = turnSpd;
    rController->driveRobot((-1.0f*moveSpd), (moveDir + Math::MATHPI), (turnSpd));
}


void RobotLogic::ballsNotFound() {
    ballTimeoutCount++;
    if(rotationSpeedSaver>0 && (rotationSpeedSaver>20 || rotationSpeedSaver<-20)){
    	rController->driveRobot(0, 0, rotationSpeedSaver);
    } else {
    	rController->driveRobot(0, 0, 35);
    	rotationSpeedSaver = 35;
    }
    if (ballTimeoutCount > RobotConstants::ballTimeoutThresh) {
        ballTimeoutCount = 0;
        setRState(RobotState::BALL_TIMEOUT);
    }
}

void RobotLogic::robotRotate(int angle, int spd) {
    rController->turnAround(angle, spd);
    releaseBallDriveLocks();
    releaseBallTurnLock();
    releaseGateTurnLock();
}

//Neither camera saw balls for 1 second.
//This function should keep the robot in timeout state until both gates can be seen.

void RobotLogic::ballTimeout() {
    //TODO : Some smart logamathingie
    rState = RobotState::FIND_BALL;
}

void RobotLogic::findGate() {

    startCounter++;
    if (startCounter >= 5) {
        startCounter = 0;
        if (!(rController->getStart())) {
            rState = RobotState::IDLE;
            return;
        }
    }

    if (!rController->hasBall()) {
        rController->stopDribbler();
        releaseBallDriveLocks();
        rState = RobotState::FIND_BALL;
        return;
    }

    if (gateTimeoutLock) {
        cout << "ENTERING GATE TIMEOUT" << endl;
        gateTimeoutLock = false;
        rState = RobotState::GATE_TIMEOUT;
        return;
    }

    gateState = getGateState();



    switch (gateState) {
            //Aim for the gate and shoot.
        case GateFindState::GATE_VISIBLE_FRONT:
            std::cout << " GATE VISIBLE FRONT." << std::endl;
            gateVisibleFront();
            break;
            //Rotate, shoot.
        case GateFindState::GATE_VISIBLE_REAR:
            std::cout << " GATE VISIBLE REAR." << std::endl;
            gateVisibleRear();
            break;
            //Relocate to a better position, rotate until gate visible, shoot.
        case GateFindState::OPPOSING_GATE_FRONT:
            std::cout << " OPPOSING GATE VISIBLE FRONT." << std::endl;
            opposingGateFront();
            break;
            //Relocate to a better position, rotate, shoot..
        case GateFindState::OPPOSING_GATE_REAR:
            std::cout << " OPPOSING GATE VISIBLE REAR." << std::endl;
            opposingGateRear();
            break;
            //Turn until a gate is found, otherwise enter a timeout. (RobotConstants::gateTimeout)
        case GateFindState::GATE_INVISIBLE:
            std::cout << " GATE INVISIBLE " << std::endl;
            gateInvisible();
            break;
            //The system has decided that it must rotate to reach an optimal aiming solution. Rotation = ~180 deg.
        case GateFindState::GATE_ROTATE:
            robotRotate(180, 60);
            break;
    }
}

//Default aiming algorithm

void RobotLogic::gateVisibleFront() {
    startingOppositeDistance = 0.0f;
    int aimThresh = -1;
    int turnSpeed = -1;

    float gateAngle, gateDistance;
    int gate_x, gate_y, gate_w;

    if (goal == Goal::gBLUE) {
        gateAngle = bGate.GetAngle();
        gateDistance = bGate.GetDistance();
        gate_x = bGate.GetCen_x();
        gate_y = bGate.GetCen_y();
        gate_w = bGate.GetWidth();
    } else {
        gateAngle = yGate.GetAngle();
        gateDistance = yGate.GetDistance();
        gate_x = yGate.GetCen_x();
        gate_y = yGate.GetCen_y();
        gate_w = yGate.GetWidth();
    }
    aimThresh = gate_w * 0.15;
    float angleSpd = gateAngle * 0.5;
    float turn = 0;
    if (angleSpd < 0) turn = -7;
    if (angleSpd > 0) turn = 7;
    turnSpeed = angleSpd + turn;

    if (gate_x < CAM_W / 2 - aimThresh) rController->driveRobot(0, 0, turnSpeed);
    else if (gate_x > CAM_W / 2 + aimThresh) rController->driveRobot(0, 0, turnSpeed);
    else {
        rController->driveRobot(0, 0, 0);
        cout << "STATE: KICK_BALL" << endl;
        rState = RobotState::KICK_BALL;
        return;
    }
}

//Rotate from whichever side is more beneficial, remember the direction of rotation

void RobotLogic::gateVisibleRear() {
    startingOppositeDistance = 0.0f;
    lockGateTurn();
    return;
}

//Drive a bit until the opposing gate reaches some threshold or 

void RobotLogic::opposingGateFront() {
    float distance, angle;
    int cen_x;
    if(goal == Goal::gBLUE){
        distance = yGate.GetDistance();
        angle = yGate.GetAngle();
        cen_x = yGate.GetCen_x();
    } else {
        distance = bGate.GetDistance();
        angle = bGate.GetAngle();
        cen_x = bGate.GetCen_x();
    }
    if(distance>2.0f){
        robotDriveWrapperFront(cen_x, angle, distance, 10, 150, 2.5);
    } else if(rotationSpeedSaver>=30 || rotationSpeedSaver<=-30){
        rController->driveRobot(0, 0, rotationSpeedSaver);
    } else {
    	rController->driveRobot(0, 0, 40);
    }
}

void RobotLogic::opposingGateRear() {
    float distance, angle;
    int cen_x;
    if(goal == Goal::gBLUE){
        distance = yGate.GetDistance();
        angle = yGate.GetAngle();
        cen_x = yGate.GetCen_x();
    } else {
        distance = bGate.GetDistance();
        angle = bGate.GetAngle();
        cen_x = bGate.GetCen_x();
    }
    
    std::cout << " OPPOSING GATE DISTANCE :" << distance << std::endl;
    if(distance>2.0f){
        robotDriveWrapperRear(cen_x, angle, distance, 10, 150, 2.5);
    }  else if(rotationSpeedSaver>=30 || rotationSpeedSaver<=-30){
        rController->driveRobot(0, 0, rotationSpeedSaver);
    } else {
    	rController->driveRobot(0, 0, 40);
    }
    rState = RobotState::FIND_GATE;
}

void RobotLogic::gateInvisible() {
    startingOppositeDistance = 0.0f;
    gateTimeoutCount++;
    rController->driveRobot(0,0,-50);
    /*
    if (gateTimeoutCount > RobotConstants::ballTimeoutThresh) {
        gateTimeoutCount = 0;
        setRState(RobotState::GATE_TIMEOUT);
    }
    */
}


//Drive the robot closer to own goal.

void RobotLogic::gateTimeout() {
    startingOppositeDistance = 0.0f;
    //TODO : SOMETING
    setRState(RobotState::FIND_GATE);
}

void RobotLogic::kickBall() {
    startingOppositeDistance = 0.0f;
    rController->stopDribbler();
    rController->kickBall(2000);
    usleep(16667);
    rState = RobotState::FIND_BALL;
}

void RobotLogic::notGreen() {
    startingOppositeDistance = 0.0f;
    //TODO : something smart
    rController->driveRobot(0, 0, 100);
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
    if (ball_rear_turn) {
        return BallFindState::ROBOT_ROTATE;
    }
    
    if (ball_front_drive && hasBallsFront()) {
        return BallFindState::BALL_FRONT;
    } else if (ball_rear_drive && hasBallsRear()) {
        return BallFindState::BALL_REAR;
    }

    //If neither lock is currently active, look for a new one (or keep rotating until 180 deg)
    releaseBallDriveLocks();
    
    if (balls.size() > 0) {
        if (balls.at(0).getDir() == RobotConstants::Direction::FRONT) {
            lockFrontBallDrive();
            return BallFindState::BALL_FRONT;
        } else {
            lockRearBallDrive();
            return BallFindState::BALL_REAR;
        }
    } else {
        releaseBallTurnLock();
        return BallFindState::BALL_NOT_FOUND;
    }

}

GateFindState RobotLogic::getGateState() {

    if (gate_rear_turn) {
        return GateFindState::GATE_ROTATE;
    }

    if (goal == Goal::gBLUE) {
        if (bGate.GetDir() == RobotConstants::Direction::FRONT) {
            return GateFindState::GATE_VISIBLE_FRONT;
        } else if (bGate.GetDir() == RobotConstants::Direction::REAR) {
            return GateFindState::GATE_VISIBLE_REAR;
        } else if (yGate.GetDir() == RobotConstants::Direction::FRONT) {
            return GateFindState::OPPOSING_GATE_FRONT;
        } else if (yGate.GetDir() == RobotConstants::Direction::REAR) {
            return GateFindState::OPPOSING_GATE_REAR;
        } else {
            return GateFindState::GATE_INVISIBLE;
        }
    } else {
        if (yGate.GetDir() == RobotConstants::Direction::FRONT) {
            return GateFindState::GATE_VISIBLE_FRONT;
        } else if (yGate.GetDir() == RobotConstants::Direction::REAR) {
            return GateFindState::GATE_VISIBLE_REAR;
        } else if (bGate.GetDir() == RobotConstants::Direction::FRONT) {
            return GateFindState::OPPOSING_GATE_FRONT;
        } else if (bGate.GetDir() == RobotConstants::Direction::REAR) {
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
    std::cout << " Ball driver locks released." << std::endl;
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
    for (Ball ball : balls) {
        if (ball.getDir() == RobotConstants::Direction::FRONT) return true;
    }
    return false;
}

bool RobotLogic::hasBallsRear() {
    for (Ball ball : balls) {
        if (ball.getDir() == RobotConstants::Direction::REAR) return true;
    }
    return false;
}

Ball RobotLogic::getFirstFrontBall() {
    for (Ball ball : balls) {
        if (ball.getDir() == RobotConstants::Direction::FRONT) return ball;
    }
}

Ball RobotLogic::getFirstRearBall() {
    for (Ball ball : balls) {
        if (ball.getDir() == RobotConstants::Direction::REAR) return ball;
    }
}

void RobotLogic::setRole() {
    role = rController->getRole();
}

GateFindState RobotLogic::getDefendGateState() {
    if (gate_rear_turn) {
        return GateFindState::GATE_ROTATE;
    }

    if (goal == Goal::gBLUE) {
        if ((yGate.GetDir() == RobotConstants::Direction::REAR) && 
                (bGate.GetDir() == RobotConstants::Direction::FRONT)) {
            return GateFindState::OPPOSING_REAR_OTHER_FRONT;
        } else if (yGate.GetDir() == RobotConstants::Direction::REAR) {
            return GateFindState::OPPOSING_GATE_REAR;
        } else if (yGate.GetDir() == RobotConstants::Direction::FRONT) {
            return GateFindState::OPPOSING_GATE_FRONT;
        } else if (bGate.GetDir() == RobotConstants::Direction::REAR) {
            return GateFindState::GATE_VISIBLE_REAR;
        } else if (bGate.GetDir() == RobotConstants::Direction::FRONT) {
            return GateFindState::GATE_VISIBLE_FRONT;
        } else {
            return GateFindState::GATE_INVISIBLE;
        }
    } else {
        if ((bGate.GetDir() == RobotConstants::Direction::REAR) && 
                (yGate.GetDir() == RobotConstants::Direction::FRONT)) {
            return GateFindState::OPPOSING_REAR_OTHER_FRONT;
        } else if (bGate.GetDir() == RobotConstants::Direction::REAR) {
            return GateFindState::OPPOSING_GATE_REAR;
        } else if (bGate.GetDir() == RobotConstants::Direction::FRONT) {
            return GateFindState::OPPOSING_GATE_FRONT;
        } else if (yGate.GetDir() == RobotConstants::Direction::REAR) {
            return GateFindState::GATE_VISIBLE_REAR;
        } else if (yGate.GetDir() == RobotConstants::Direction::FRONT) {
            return GateFindState::GATE_VISIBLE_FRONT;
        } else {
            return GateFindState::GATE_INVISIBLE;
        }
    }
}
