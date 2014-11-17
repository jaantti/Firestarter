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
    if (NR_OF_WHEELS == 3) {
        driveThree(spd, angle, rotSpd);
    }
    
    if (NR_OF_WHEELS == 4) {
        driveFour(spd, angle, rotSpd);
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
    
    for(int i = 0; i < 2; i++){
        if(abs(motorSpeeds[i]) < abs(motorSpeeds[i+1])) max = i+1;
    }
    
    if(abs(motorSpeeds[max]) > MAX_MOTOR_SPEED){
        int maxSpeed = motorSpeeds[max];
        for(int i = 0; i < 3; i++){
            motorSpeeds[i] = (motorSpeeds[i] / (float) maxSpeed) * MAX_MOTOR_SPEED;
        }
    }
    for(int i = 0; i < 3; i++){
        connection.setSpeed(i+1, motorSpeeds[i]);
    }   
}

void RobotController::driveFour(float spd, float angle, float rotSpd) {
    
    int speed0 = spd * sin(angle - PI / 4.0) + rotSpd;
    int speed1 = spd * -sin(angle + PI / 4.0) + rotSpd;
    int speed2 = spd * -sin(angle - PI / 4.0) + rotSpd;
    int speed3 = spd * sin(angle + PI / 4.0) + rotSpd;
    
    int motorSpeeds[4] = {speed0, speed1, speed2, speed3};
        
    int max = 1;
    
    for(int i = 0; i < 3; i++){
        if(abs(motorSpeeds[i]) < abs(motorSpeeds[i+1])) max = i+1;
    }
    
    if(abs(motorSpeeds[max]) > MAX_MOTOR_SPEED){
        int maxSpeed = motorSpeeds[max];
        for(int i = 0; i < 4; i++){
            motorSpeeds[i] = (motorSpeeds[i] / (float) maxSpeed) * MAX_MOTOR_SPEED;
        }
    }
    for(int i = 0; i < 4; i++){
        connection.setSpeed(i+1, motorSpeeds[i]);
    }
}

void RobotController::detectSerial(bool serial){
    connection.setDetectSerial(serial);
}

vector<float> RobotController::getAllMotorSpeeds(){
    return connection.getAllMotorSpeed();
}