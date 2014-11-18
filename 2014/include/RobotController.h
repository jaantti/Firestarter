/* 
 * File:   RobotController.h
 * Author: not me
 *
 * Created on September 7, 2014, 12:23 PM
 */

#ifndef ROBOTCONTROLLER_H
#define	ROBOTCONTROLLER_H

#define PI 3.14159265

#include "RobotConstants.h"
#include "SerialConnection.h"
#include <vector>

class RobotController {
    SerialConnection connection;
public:

    struct StallComparator {
    	int motorId = -1;
    	float wantedSpeed = 0.0f;
    	float realSpeed = 0.0f;
    };

    RobotController();
    ~RobotController();
    /**
     * Initialize variables and serial connection
     */
    void init();
    /**
     * Checks if ball is in dribbler
     * @return true if ball in dribbler
     */
    bool hasBall();
    /**
     * Kick ball
     * @param str Kick with strength str
     */
    void kickBall(int str);
    /**
     * Drive three-wheeled robot
     * @param spd Moving speed
     * @param angle Moving angle
     * @param rotSpd Rotation speed
     */
    void driveRobot(float spd, float angle, float rotSpd);
    /**
     * Drive robot
     * @param spd Moving speed
     * @param angle Moving angle
     * @param rotSpd Rotation speed
     */
    void stallStep();

    void detectSerial(bool serial);   
    void pingCoil();    
    void chargeCoil();    
    void dischargeCoil();    
    void runDribbler();    
    void stopDribbler();    
    char getAttackedGoal();    
    bool getStart();    
    void closeSerial(){connection.closeSerial();}
    vector<float> getAllMotorSpeeds();
    
    void initSerialTime(unsigned long timeInMicros);
    unsigned long timeSinceLastLoop();
    
    
private:
    std::vector<StallComparator> stallings;
    unsigned long timeSinceLastSerial = 0;
    void driveThree(float spd, float angle, float rotSpd);
    void driveFour(float spd, float angle, float rotSpd);
};

#endif	/* ROBOTCONTROLLER_H */

