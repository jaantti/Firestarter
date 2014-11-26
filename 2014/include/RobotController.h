/* 
 * File:   RobotController.h
 * Author: not me
 *
 * Created on September 7, 2014, 12:23 PM
 */

#ifndef ROBOTCONTROLLER_H
#define	ROBOTCONTROLLER_H

#include "RobotConstants.h"
#include "SerialConnection.h"
#include <vector>
#include <stdlib.h>

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

    bool isStalled();
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
    void driveReverse();
        
    DriveDirection getDriveDir();
    void turnAround(int angle, int spd);
    
    float getLastRotation(){ return lastRotationSpeed; }
private:
    int stallCounters[4] = {0,0,0,0};
    
    float lastSpeed = 0.0f;
    float lastAngle = 0.0f;
    float lastRotationSpeed = 0.0f;
    DriveDirection dir;
    
    void stallStep();
    std::vector<StallComparator> stallings;
    unsigned long timeSinceLastSerial = 0;
    void driveThree(float spd, float angle, float rotSpd);
    void driveFour(float spd, float angle, float rotSpd);
    void CalculateDriveDirection(float spd, float angle, float rotSpd);
    
    void setDriveDirection(DriveDirection dir);
    DriveDirection getDriveDirection();
    
};

#endif	/* ROBOTCONTROLLER_H */

