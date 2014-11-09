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
#include <boost/algorithm/string.hpp>
#include "RobotConstants.h"

using namespace std;

SerialConnection::SerialConnection() {
    serialDevice.fill(-1);
}

SerialConnection::~SerialConnection() {

}

bool SerialConnection::init() {
    //cout << "init" << endl;

    //detectSerial = false;
    for (int j = 0; j < NR_OF_WHEELS; j++) {
        for (int i = 5; i < 15; i++) {
            //cout << "for" << endl;
            if (!RS232_OpenComport(i, 19200, "8N1")) {
                unsigned char buf[100] = {0};
                sendCommand(i, "?\n", buf);
                cout << i << ":" << string((const char *) buf);
                if (buf[0] == 'd' || buf[4] == '0') serialDevice[0] = i;
                else serialDevice[buf[4] - '0'] = i;
            }
        }
    }
    /*vector<string> device;
    FILE *in;
    char buffer[512];
    if ((in = popen("ls /dev/ttyACM*", "r"))) {
        while (fgets(buffer, sizeof (buffer), in)) {
            string name = (string) buffer;
            boost::trim(name);
            device.push_back(name);
        }
    }*/
    for(int i=0; i < NR_OF_WHEELS + 1; i++){
        cout << i << ":" << serialDevice[i] << endl;
    }
    usleep(10000000);
    return true;
}

void SerialConnection::sendCommand(int comport, const char* command, unsigned char* answer) {

    RS232_cputs(comport, command);
    usleep(100000);
    RS232_PollComport(comport, answer, 100);
}

void SerialConnection::sendCommand(int comport, const char* command) {
    RS232_cputs(comport, command);
    usleep(5000);
}

void SerialConnection::setSpeed(int motor, int speed) {
    char out[10] = {0};
    sprintf(out, "sd%d\n", speed);
    //cout << "speed:" << string(out) << endl;
    RS232_cputs(serialDevice[motor], (const char*) out);
    
}

void SerialConnection::kickBall(int power) {
    
    char out[10] = {0};
    sprintf(out, "k%d\n", power);
    //cout << "kick: " << string(out) << endl;
    RS232_cputs(serialDevice[0], (const char*) out);
     
}

bool SerialConnection::hasBall() {
    
    unsigned char answer[20] = {0};
    sendCommand(serialDevice[GET_BALL_BOARD_ID], "gb\n", answer);
    
    if (answer[3] == '1') {
        return true;
    }
    return false;
    
}

void SerialConnection::pingCoil() {
    RS232_cputs(serialDevice[0], (const char*)("p\n"));
}

void SerialConnection::chargeCoil() {
    RS232_cputs(serialDevice[0], (const char*)("c\n"));
}

void SerialConnection::setDetectSerial(bool serial) {
    detectSerial = serial;
}

