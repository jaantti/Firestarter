/* 
 * File:   RobotController.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:23 PM
 */

#include "RobotController.h"
#include "SerialConnection.h"
#include <iostream>
#include <cmath>

using namespace std;

RobotController::RobotController() {
}

RobotController::RobotController(const RobotController& orig) {
}

RobotController::~RobotController() {
}

void RobotController::init() {
    cout << "RC init start" << endl;
    connection = SerialConnection();
    connection.init();
    cout << "RC init end" << endl;
}

void RobotController::driveRobot(int spd, float angle) {
    if (NR_OF_WHEELS == 3) {
        driveThree(spd, angle);
    }
}

bool RobotController::hasBall() {

}

void RobotController::kickBall(int str) {

}

void RobotController::driveThree(int spd, float angle) {


    int speed0 = spd * cos(5 * PI / 6.0 - angle);
    int speed1 = spd * cos(PI / 6.0 - angle);
    int speed2 = spd * cos(9 * PI / 6.0 - angle);

    connection.setSpeed(0, speed0);
    connection.setSpeed(1, speed1);
    connection.setSpeed(2, speed2);
}

void RobotController::driveFour(int spd, float angle) {

}

