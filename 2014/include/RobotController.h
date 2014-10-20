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

class RobotController {
    SerialConnection connection;
public:
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
    void detectSerial(bool serial);
    
private:
    void driveThree(float spd, float angle, float rotSpd);
    void driveFour(int spd, float angle);
};

#endif	/* ROBOTCONTROLLER_H */

