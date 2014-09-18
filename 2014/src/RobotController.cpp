/* 
 * File:   RobotController.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:23 PM
 */

#include "RobotController.h"
#include <iostream>

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


void RobotController::driveRobot(int spd, int angle) {
    if(NR_OF_WHEELS==3){
        
    }
}


bool RobotController::hasBall() {

}

void RobotController::kickBall(int str){
    
}


void RobotController::driveThree(int spd, int angle) {
    
    
   // int speed1 = spd * cos( 5*PI/6.0 - angle );
   // int speed2 = spd * cos( PI/6.0 - angle );
   // int speed3 = spd * cos( 9*PI/6.0 - angle );    
}

void RobotController::driveFour(int spd, int angle) {

}

