/* 
 * File:   SerialConnection.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "SerialConnection.h"
#include "rs232.h"
#include <iostream>


SerialConnection::SerialConnection() {
    motors.fill(-1);
    coil = -1;
}

SerialConnection::SerialConnection(const SerialConnection& orig) {
}

SerialConnection::~SerialConnection() {

}

bool SerialConnection::init() {
    cout << "init" << endl;
    for (int i = 5; i < 15; i++) {
        cout << "for" << endl;
        if (!RS232_OpenComport(i, 19200)) {
            unsigned char buf[100] = {0};
            sendCommand(i, "?\n", buf);
            cout << string((const char *)buf);
        }
    }
}

void SerialConnection::sendCommand(int comport, const char* command, unsigned char* answer) {
        
    RS232_cputs(comport, command);
    usleep(500000);
    RS232_PollComport(comport, answer, 100);    
}

