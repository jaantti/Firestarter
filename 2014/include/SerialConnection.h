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
#include <unordered_map>

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
    void setDetectSerial(bool serial);
    void chargeCoil();
    void dischargeCoil();
    void pingCoil();
    void runDribbler();
    void stopDribbler();
    void closeSerial();
    
    
private:
    array<int, NR_OF_WHEELS + 1> serialDevice;
    bool detectSerial = true;
    void sendCommand(int comport, const char* command, unsigned char* answer);
    void sendCommand(int comport, const char* command);

};

#endif	/* SERIALCONNECTION_H */

