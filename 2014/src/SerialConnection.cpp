/* 
 * File:   SerialConnection.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "SerialConnection.h"
#include "rs232.h"
#include <iostream>
#include <stdlib.h>



SerialConnection::SerialConnection() {
    motors.fill(-1);
    coil = -1;
}

SerialConnection::~SerialConnection() {

}

bool SerialConnection::init() {
    //cout << "init" << endl;
    for (int i = 5; i < 15; i++) {
        //cout << "for" << endl;
        if (!RS232_OpenComport(i, 19200)) {
            unsigned char buf[100] = {0};
            sendCommand(i, "?\n", buf);
            cout << i << ":"<< string((const char *)buf);
            motors[buf[4]-'0' - 1] = i;
        }
    }
    cout << "motor array" << endl;
    for(int i = 0; i < motors.size(); i++){
        cout << i << ";" << motors[i] << endl;
    }
    return true;
}

void SerialConnection::sendCommand(int comport, const char* command, unsigned char* answer) {
        
    RS232_cputs(comport, command);
    usleep(500000);
    RS232_PollComport(comport, answer, 100);    
}

void SerialConnection::sendCommand(int comport, const char* command){
    RS232_cputs(comport, command);
    usleep(5000);
}

void SerialConnection::setSpeed(int motor, int speed){
    char out[10] = {0};
    sprintf(out, "sd%d\n", speed);
    cout << "speed:" << string(out) << endl;
    RS232_cputs(motors[motor], (const char*)out);
}



