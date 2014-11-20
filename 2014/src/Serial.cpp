/* 
 * File:   Serial.cpp
 * Author: antti
 * 
 * Created on November 20, 2014, 12:02 PM
 */

#include "Serial.h"

Serial::Serial(int deviceId, unsigned int baudrate):io(), serial(NULL) {
    std::vector<std::string> device;
    FILE *in;
    char buffer[512];
    if ((in = popen("ls /dev/ttyACM*", "r"))) {
        while (fgets(buffer, sizeof (buffer), in)) {
            std::string name = (std::string) buffer;
            boost::trim(name);
            device.push_back(name);
        }
    }
    this->dev_id = deviceId;
    
    int i=0;
    while(i<device.size()){
        serial = new boost::asio::serial_port(io, device.at(i));
        serial->set_option(boost::asio::serial_port_base::baud_rate(baudrate));
        std::cout << " Currently working on :" << device.at(i) << std::endl;
        std::string response = readSerial("?\n");
        std::cout << " I got a response : " << response << std::endl;
        if(!response.length()==7){
            char comp = response.at(4);
            if(std::isdigit(comp)){
                if(compareDevice( comp-'0' )){
                    break;
                } else {
                    serial->close();
                    delete serial;
                    i++;
                }
            } else {
                serial->close();
                delete serial;
                continue;
            }
        } else {
            serial->close();
            delete serial;
            continue;
        }
    }
    std::cout << " I have matched " << device.at(i) << " against ID :" << dev_id;
}

Serial::~Serial() {
    serial->close();
    delete this->serial;
}

std::string Serial::readLine() {
    std::cout << " Reading line from serial port." << std::endl;
    char c;
    std::string result;
    while(true){
        boost::asio::read(*serial, boost::asio::buffer(&c, 1));
        std::cout << " Read : " << c << std::endl;
        switch(c){
            case '\n':
                return result;
            default:
                result+=c;
        }
    }
}

std::string Serial::readSerial(std::string command) {
    writeSerial(command);
    return readLine();
}

void Serial::writeSerial(std::string command) {
    boost::asio::write(*serial, boost::asio::buffer( command.c_str(), command.size() ) );
}

bool Serial::compareDevice(int deviceId) {
    if(this->dev_id == deviceId) return true;
    return false;
}


