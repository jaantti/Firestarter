/* 
 * File:   SerialConnection.h
 * Author: antti
 *
 * Created on September 7, 2014, 12:24 PM
 */

#ifndef SERIALCONNECTION_H
#define	SERIALCONNECTION_H

#include "RobotConstants.h"
#include <array>
#include <vector>
#include <unordered_map>
#include "rs232.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "Maths.h"
#include <cstdlib>
#include <sys/time.h>

using namespace std;

class SerialConnection {
public:
    SerialConnection();
    virtual ~SerialConnection();
    /**
     * Coil controller must have id 0, motor id's must start from 1
     * @return 
     */
    bool init();
    void setSpeed(int motor, int speed);
    void kickBall(int power);
    bool hasBall();
    bool getStart();
    char getGoal();
    Role getRole();
    void setDetectSerial(bool serial);
    void chargeCoil();
    void dischargeCoil();
    void pingCoil();
    void runDribbler();
    void stopDribbler();
    void closeSerial();
    vector<float> getAllMotorSpeed();
    
    void initSerialTime(unsigned long int initTime);
    unsigned long int getTimeSinceLastLoop();
    
    
private:
    unsigned long int initialTime;
    unsigned long int temp;
    unsigned long int timedif;
    
    array<int, NR_OF_WHEELS + 1> serialDevice;
    vector<float> motorSpeeds;
    bool detectSerial = true;
    void sendCommand(int comport, const char* command, unsigned char* answer);
    void sendCommand(int comport, const char* command);
    void readAnswer(int comport, unsigned char* answer);

};

#endif	/* SERIALCONNETION_H */

