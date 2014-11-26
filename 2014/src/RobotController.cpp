/* 
 * File:   RobotController.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:23 PM
 */

#include "RobotController.h"
#include "SerialConnection.h"
#include "Maths.h"
#include <iostream>
#include <cmath>

using namespace std;

RobotController::RobotController() {
    for (int i = 0; i < 4; i++) {
        stallings.push_back(StallComparator());
    }
}

RobotController::~RobotController() {
}

void RobotController::init() {
    cout << "RC init start" << endl;
    connection = SerialConnection();
    connection.init();
    cout << "RC init end" << endl;
}

void RobotController::driveRobot(float spd, float angle, float rotSpd) {
    pingCoil();
    lastSpeed = spd;
    lastAngle = angle;
    lastRotationSpeed = rotSpd;
    if (NR_OF_WHEELS == 3) {
        driveThree(spd, angle, rotSpd);
    }

    if (NR_OF_WHEELS == 4) {
        driveFour(spd, angle, rotSpd);
    }
}

void RobotController::driveReverse() {
    pingCoil();
    if (NR_OF_WHEELS == 3) {
        driveThree((lastSpeed*-1.0f), (lastAngle*-1.0f), (lastRotationSpeed*-1.0f));
    }

    if (NR_OF_WHEELS == 4) {
        driveFour((lastSpeed*-1.0f), (lastAngle*-1.0f), (lastRotationSpeed*-1.0f));
    }
}

bool RobotController::hasBall() {
    return connection.hasBall();
}

void RobotController::chargeCoil() {
    connection.chargeCoil();
}

void RobotController::dischargeCoil() {
    connection.dischargeCoil();
}

void RobotController::pingCoil() {
    connection.pingCoil();
}

void RobotController::kickBall(int str) {
    connection.kickBall(str);
}

void RobotController::runDribbler() {
    connection.runDribbler();
}

void RobotController::stopDribbler() {
    connection.stopDribbler();
}

char RobotController::getAttackedGoal() {
    return connection.getGoal();
}

bool RobotController::getStart() {
    return connection.getStart();
}

void RobotController::driveThree(float spd, float angle, float rotSpd) {

    int speed0 = spd * cos(5.0 * PI / 6.0 - angle) + rotSpd;
    int speed1 = spd * cos(PI / 6.0 - angle) + rotSpd;
    int speed2 = spd * cos(3.0 * PI / 2.0 - angle) + rotSpd;

    int motorSpeeds[3] = {speed0, speed1, speed2};

    int max = 1;

    for (int i = 0; i < 2; i++) {
        if (abs(motorSpeeds[i]) < abs(motorSpeeds[i + 1])) max = i + 1;
    }

    if (abs(motorSpeeds[max]) > MAX_MOTOR_SPEED) {
        int maxSpeed = motorSpeeds[max];
        for (int i = 0; i < 3; i++) {
            motorSpeeds[i] = (motorSpeeds[i] / (float) maxSpeed) * MAX_MOTOR_SPEED;
        }
    }

    for (int i = 0; i < 3; i++) {
        connection.setSpeed(i + 1, motorSpeeds[i]);
    }
}

void RobotController::driveFour(float spd, float angle, float rotSpd) {
    stallings.clear();
    int speed0 = spd * sin(angle - PI / 4.0) + rotSpd;
    int speed1 = spd * -sin(angle + PI / 4.0) + rotSpd;
    int speed2 = spd * -sin(angle - PI / 4.0) + rotSpd;
    int speed3 = spd * sin(angle + PI / 4.0) + rotSpd;

    int motorSpeeds[4] = {speed0, speed1, speed2, speed3};

    int max = 1;

    for (int i = 0; i < 3; i++) {
        if (abs(motorSpeeds[i]) < abs(motorSpeeds[i + 1])) max = i + 1;
    }

    if (abs(motorSpeeds[max]) > MAX_MOTOR_SPEED) {
        int maxSpeed = motorSpeeds[max];
        for (int i = 0; i < 4; i++) {
            motorSpeeds[i] = (motorSpeeds[i] / (float) maxSpeed) * MAX_MOTOR_SPEED;
        }
    }
    for (int i = 0; i < 4; i++) {
        connection.setSpeed(i + 1, motorSpeeds[i]);
        StallComparator comp = {};
        comp.motorId = i + 1;
        comp.wantedSpeed = motorSpeeds[i] * 0.05208333f * Math::TWO_PI;
        stallings.push_back(comp);
    }
}

void RobotController::detectSerial(bool serial) {
    connection.setDetectSerial(serial);
}

vector<float> RobotController::getAllMotorSpeeds() {
    vector<float> speeds = connection.getAllMotorSpeed();
    for (int i = 0; i < speeds.size(); i++) {
        stallings.at(i).realSpeed = speeds.at(i);
    }
    stallStep();
    return speeds;
}

void RobotController::stallStep() {
    for (int i = 0; i < 4; i++) {
        StallComparator comp = stallings.at(i);
        if (Math::abs(comp.wantedSpeed) / Math::abs(comp.realSpeed) > 1.7f && Math::abs(comp.wantedSpeed) > Math::MATHPI) {
            stallCounters[i]++;
        } else {
            stallCounters[i] = 0;
        }
    }
}

bool RobotController::isStalled() {
    for (int i = 0; i < 4; i++) {
        if (stallCounters[i] > RobotConstants::stallThreshold) {
            std::cout << " The robot has stalled." << std::endl;
            return true;
        }
    }
    return false;
}

void RobotController::initSerialTime(unsigned long int timeInMicros) {
    connection.initSerialTime(timeInMicros);
}

unsigned long RobotController::timeSinceLastLoop() {
    connection.getTimeSinceLastLoop();
}

DriveDirection RobotController::getDriveDir() {
    return this->dir;
}

void RobotController::setDriveDirection(DriveDirection dir) {
    this->dir = dir;
}

void RobotController::CalculateDriveDirection(float spd, float angle, float rotSpd) {

}

void RobotController::turnAround(int spd) {
    driveRobot(0, 0, spd);
    float turnTime = 37.1 / spd * 1000000.0 * RobotConstants::magicTurnConstant;
    usleep(turnTime);
    driveRobot(0, 0, 0);
}

