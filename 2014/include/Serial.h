/* 
 * File:   Serial.h
 * Author: antti
 *
 * Created on November 20, 2014, 12:02 PM
 */

#ifndef SERIAL_H
#define	SERIAL_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include "RobotConstants.h"
#include <boost/algorithm/string.hpp>

class Serial {
public:
    Serial(int deviceId, unsigned int baudrate);
    virtual ~Serial();
    int getDeviceId();
    void writeSerial(std::string command);
    std::string readSerial(std::string command);
private:
    boost::asio::io_service io;
    boost::asio::serial_port *serial;
    int dev_id;
    std::string port;
    std::string readLine();
    bool compareDevice(int deviceId);
};

#endif	/* SERIAL_H */

