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
#include <cstdlib>

using namespace std;

SerialConnection::SerialConnection() {
    serialDevice.fill(-1);
}

SerialConnection::~SerialConnection() {
    stopDribbler();
      
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
                //cout << i << ":" << string((const char *) buf);
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
    
    //sendCommand(serialDevice[0], "fs0\n");
    //sendCommand(serialDevice[0], "pd0\n");
    
    usleep(3000000);
    return true;
}

void SerialConnection::sendCommand(int comport, const char* command, unsigned char* answer) {

    if(comport == -1) return;
    RS232_cputs(comport, command);
    usleep(10000);
    RS232_PollComport(comport, answer, 100);
}

void SerialConnection::sendCommand(int comport, const char* command) {
    if(comport == -1) return;
    RS232_cputs(comport, command);
    usleep(5000);
}

void SerialConnection::setSpeed(int motor, int speed) {
    if(motor == -1) return;
    char out[10] = {0};
    sprintf(out, "sd%d\n", speed);
    RS232_cputs(serialDevice[motor], (const char*) out);
    
}

void SerialConnection::kickBall(int power) {
    for(int i = 0; i < NR_OF_WHEELS + 1; i++){
        if (serialDevice[i] == -1) return;
    }
    for(int i = 0; i < NR_OF_WHEELS; i++){
        RS232_cputs(serialDevice[i+1], (const char*)("sd0\n"));
    }
    
    char out[10] = {0};
    sprintf(out, "k%d\n", power);
    RS232_cputs(serialDevice[0], (const char*) out);
    
}

bool SerialConnection::hasBall() {
    
    unsigned char answer[20] = {0};
    sendCommand(serialDevice[GET_BALL_BOARD_ID], "gb\n", answer);
    
    if (answer[3] == '1') {
        //cout << "I have a ball" << endl;
        return true;
    }
    return false;
    
}

char SerialConnection::getGoal() {
    
    unsigned char answer[20] = {0};
    sendCommand(serialDevice[GET_SWITCH_BOARD_ID], "s1\n", answer);
    
    if (answer[4] == '1') {
        cout << "Attacking blue" << endl;
        return 'B';
    } else if (answer[4] == '0') {
        cout << "Attacking yellow" << endl;
        return 'Y';
    }
    cout << "serial: getGoal: wat?" << endl;
    return 'X';
}

bool SerialConnection::getStart() {
    
    unsigned char answer[20] = {0};
    sendCommand(serialDevice[GET_SWITCH_BOARD_ID], "s4\n", answer);
    
    if (answer[4] == '1') {
        //cout << "GO" << endl;
        return true;
    }
    return false;
}

void SerialConnection::runDribbler() {
    if(serialDevice[0] == -1) return;
    RS232_cputs(serialDevice[0], (const char*)("tg\n"));
    //RS232_cputs(serialDevice[0], (const char*)("wl255\n"));
}

void SerialConnection::stopDribbler() {
    if(serialDevice[0] == -1) return;
    RS232_cputs(serialDevice[0], (const char*)("ts\n"));
    //RS232_cputs(serialDevice[0], (const char*)("wl0\n"));
}

void SerialConnection::pingCoil() {
    if(serialDevice[0] == -1) return;
    RS232_cputs(serialDevice[0], (const char*)("p\n"));
}

void SerialConnection::chargeCoil() {
    if(serialDevice[0] == -1) return;
    RS232_cputs(serialDevice[0], (const char*)("c\n"));
}

void SerialConnection::dischargeCoil() {
    if(serialDevice[0] == -1) return;
    RS232_cputs(serialDevice[0], (const char*)("d\n"));
}

void SerialConnection::setDetectSerial(bool serial) {
    detectSerial = serial;
}

void SerialConnection::closeSerial(){
    for(int i = 0; i < NR_OF_WHEELS + 1; i++){
        RS232_CloseComport(serialDevice[i]);
    }
}

vector<float> SerialConnection::getAllMotorSpeed(){
    
    for(int i = 0; i < NR_OF_WHEELS; i++){
        char resp[10] = {0};
        sendCommand(serialDevice[i], "s\n", (unsigned char *)resp);
        int spd = atoi(resp);
        float spd2 = spd * 0.052;
        motorSpeeds.push_back(spd2);
    }
    
    return motorSpeeds;
}

void SerialConnection::initSerialTime(unsigned long int initTime) {
    this->initialTime = initTime;
}

unsigned long int SerialConnection::getTimeSinceLastLoop(unsigned long int lastTime) {
    return lastTime-temp;
}
