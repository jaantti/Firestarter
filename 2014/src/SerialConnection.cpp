/* 
 * File:   SerialConnection.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */
#include "SerialConnection.h"

using namespace std;

SerialConnection::SerialConnection() {
    serialDevice.fill(-1);
}

SerialConnection::~SerialConnection() {
    stopDribbler();
      
}

bool SerialConnection::init() {
    //detectSerial = false;
    for (int j = 0; j < NR_OF_WHEELS+1; j++) {
        for (int i = 5; i < 15; i++) {
            //cout << "for" << endl;
            if (!RS232_OpenComport(i, 19200, "8N1")) {
                unsigned char buf[1300] = {0};
                sendCommand(i, "?\n", buf);
                //cout << i << ":" << string((const char *) buf);
                if (buf[0] == 'd' || buf[4] == '0') serialDevice[0] = i;
                else serialDevice[buf[4] - '0'] = i;
            }
        }
    }
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
    usleep(8000);
    RS232_PollComport(comport, answer, 100);
}

void SerialConnection::readAnswer(int comport, unsigned char* answer) {
    RS232_PollComport(comport, answer, 100);
}


void SerialConnection::sendCommand(int comport, const char* command) {
    if(comport == -1) return;
    RS232_cputs(comport, command);
}

void SerialConnection::setSpeed(int motor, int speed) {
    if(motor == -1) return;
    char out[1300] = {0};
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
    
    char out[1300] = {0};
    sprintf(out, "k%d\n", power);
    RS232_cputs(serialDevice[0], (const char*) out);
    
}

bool SerialConnection::hasBall() {
    
    unsigned char answer[1300] = {0};
    sendCommand(serialDevice[GET_BALL_BOARD_ID], "gb\n", answer);
    
    if (answer[3] == '1') {
        //cout << "I have a ball" << endl;
        return true;
    }
    return false;
    
}

char SerialConnection::getGoal() {
    
    unsigned char answer[1300] = {0};
    sendCommand(serialDevice[GET_SWITCH_BOARD_ID], "s1\n", answer);
    
    if (answer[4] == '1') {
        return 'B';
    } else if (answer[4] == '0') {
        return 'Y';
    }
    cout << "serial: getGoal: wat?" << endl;
    return 'X';
}

bool SerialConnection::getStart() {
    
    unsigned char answer[1300] = {0};
    sendCommand(serialDevice[GET_SWITCH_BOARD_ID], "s4\n", answer);
    
    if (answer[4] == '1') {
        //cout << "GO" << endl;
        return true;
    }
    return false;
}

Role SerialConnection::getRole() {
   
    unsigned char answer[1300] = {0};
    sendCommand(serialDevice[GET_SWITCH_BOARD_ID], "s2\n", answer);
    
    if (answer[4] == '0') {
        cout << "Attacking" << endl;
        return Role::rATTACK;
    } else if (answer[4] == '1') {
        cout << "Defending" << endl;
        return Role::rDEFEND;
    } else {
        cout << "getRole(): I should not be here" << endl;
        return Role::rATTACK;
    }
    
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
    motorSpeeds.clear();
    for(int i = 0; i<NR_OF_WHEELS; i++){
        
    }
    for(int i = 0; i < NR_OF_WHEELS; i++){
        sendCommand(serialDevice[i+1], "s\n");
        }
    
    usleep(8000);
    for(int i = 0; i < NR_OF_WHEELS; i++){
        char resp[1300] = {0};
        readAnswer(serialDevice[i+1], (unsigned char *) resp);
        int spd = atoi(&resp[3]);
        float spd2 = spd * 0.052083333f * Math::TWO_PI;
        motorSpeeds.push_back(spd2);
    }
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long int timeMicros = 1000000*tv.tv_sec + tv.tv_usec;
    
    timedif = timeMicros-initialTime;
    initialTime = timeMicros;
    
    return motorSpeeds;
}

void SerialConnection::initSerialTime(unsigned long int initTime) {
    this->initialTime = initTime;
}

unsigned long int SerialConnection::getTimeSinceLastLoop() {
    return timedif;
}
