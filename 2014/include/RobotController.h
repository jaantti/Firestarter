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
    RobotController(const RobotController& orig);
    ~RobotController();
    void init();
    bool hasBall();
    void kickBall(int str);
    void driveRobot(int spd, float angle);
    
private:
    void driveThree(int spd, float angle);
    void driveFour(int spd, float angle);
};

#endif	/* ROBOTCONTROLLER_H */

