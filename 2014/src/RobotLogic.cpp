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

    //Set the initial time for serial connection. Odometry purposes.
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long int tim1 = 1000000 * tv.tv_sec + tv.tv_usec;
    rController->initSerialTime(tim1);

    rController->stopDribbler();
    rController->driveRobot(0, 0, 0);
    setGoal();

    if (rController->getStart()) {
        rController->chargeCoil();
        rState = RobotState::FIND_BALL;
        usleep(250000);
        return;
    }

    //rController->dischargeCoil();
    usleep(100000);
}


//2 options : Robot drives to the closest ball, then locates the gate and kicks.
// OR : Look where there are more balls, drive there and proceed to shoot goals.

void RobotLogic::findBall() {

    ballState = getBallState();

    startCounter++;
    rController->stopDribbler();
    if (startCounter >= 5) {
        startCounter = 0;
        if (!(rController->getStart())) {
            cout << "STATE: IDLE" << endl;
            rState = RobotState::IDLE;
            return;
        }
    }

    if (rController->hasBall()) {
        cout << "STATE: FIND_GATE" << endl;
        rState = RobotState::FIND_GATE;
        return;
    }

    if (ballTimeoutLock) {
        cout << "STATE: BALL_TIMEOUT" << endl;
        rState = RobotState::BALL_TIMEOUT;
        return;
    }

    switch (ballState) {
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
            robotRotate();
            break;
    }
}
/*
 *Speed is based on distance - e^x , where maximum x is 5.3 for a speed of 222. Minimum at 0, where speed is 22 + e^0 = 23
 * The speed is mapped based on a distance from 0 to 5m to the range of [0, 5.3]. 
 */


//This function should handle the ball movement speed calculation based on distance.

int RobotLogic::calculateMoveSpeed(float distance) {
    if (distance > 5.0f) distance = 5.0f;
    float minSpeed=20.0f, maxSpeed=50.0f, maxSpeedDistance=1.5f;
        
    float speed = minSpeed + (maxSpeed-minSpeed)/maxSpeedDistance * distance;
    return Math::min(speed, maxSpeed);
}

//Generic drive to ball based on the ball's distance and angle.

void RobotLogic::driveBallsFront() {
    Ball ball = getFirstFrontBall();
    //std::cout << "DRIVIN TO FIRST BALL FRONT." << std::endl;
    //std::cout << " BALL COORDS : " << ball.getCen_x() << " and " << ball.getCen_y() << " AT DIST:" << ball.getDistance() << std::endl;
    float turnSpd = getAngle(ball.getCen_x())*0.8;
    float moveDir = ball.getAngle() /180.0 * PI * 0.8;
    //TODO : Replace len usage with distance.
    float moveSpd = calculateMoveSpeed(ball.getDistance());

    rController->driveRobot(moveSpd, moveDir, turnSpd);
}

//Drive to the closest rear ball until it is under 20cm, then rotate.

void RobotLogic::driveBallsRear() {
    Ball ball = getFirstRearBall();
    if (ball.getDistance() < 0.2f) {
        cout << "BALL AT REAR, TURNING" << endl;
        lockBallTurn();
        return;
    }

    float turnSpd = getAngle(ball.getCen_x())*0.8;
    float moveDir = ball.getAngle() / 180.0 * PI;
    moveDir = PI + ((moveDir - PI) * 0.8);
    
    if (moveDir >= PI) {
        moveDir -= 2*PI;
    }
    
    //TODO : Replace len usage with distance.
    float moveSpd = calculateMoveSpeed(ball.getDistance());

    rController->driveRobot((-1.0f*moveSpd), (moveDir + Math::MATHPI), (turnSpd));
}

void RobotLogic::ballsNotFound() {
    ballTimeoutCount++;
    rController->driveRobot(0, 0, 40);
    if (ballTimeoutCount > RobotConstants::ballTimeoutThresh) {
        ballTimeoutCount = 0;
        setRState(RobotState::BALL_TIMEOUT);
    }
}

void RobotLogic::robotRotate() {
    rController->turnAround(80);
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
        releaseBallDriveLocks();
        rState = RobotState::FIND_BALL;
        return;
    }

    if (gateTimeoutLock) {
        cout << "ENTERING GATE TIMEOUT" << endl;
        rState = RobotState::GATE_TIMEOUT;
        return;
    }

    gateState = getGateState();



    switch (gateState) {
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
            robotRotate();
            break;
    }
}

//Default aiming algorithm

void RobotLogic::gateVisibleFront() {
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
    aimThresh = gate_w * 0.2 + 5;
    float angleSpd = gateAngle * 0.5;
    float turn = 0;
    if (angleSpd < 0) turn = -3;
    if (angleSpd > 0) turn = 3;
    turnSpeed = angleSpd + turn;

    if (gate_x < CAM_W / 2 - aimThresh) rController->driveRobot(0, 0, turnSpeed);
    else if (gate_x > CAM_W / 2 + aimThresh) rController->driveRobot(0, 0, turnSpeed);
    else {
        cout << "STATE: KICK_BALL" << endl;
        rState = RobotState::KICK_BALL;
        return;
    }
}

//Rotate from whichever side is more beneficial, remember the direction of rotation

void RobotLogic::gateVisibleRear() {

}

//Drive a bit until

void RobotLogic::opposingGateFront() {

}

void RobotLogic::opposingGateRear() {

}

void RobotLogic::gateInvisible() {

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

