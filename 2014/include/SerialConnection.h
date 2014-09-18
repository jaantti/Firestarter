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

using namespace std;

class SerialConnection {
public:
    SerialConnection();
    SerialConnection(const SerialConnection& orig);
    virtual ~SerialConnection();
    bool init();
private:
    array<int, NR_OF_WHEELS> motors;
    int coil;
    void sendCommand(int comport, const char* command, unsigned char* answer);

};

#endif	/* SERIALCONNECTION_H */

