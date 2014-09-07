/* 
 * File:   SerialConnection.h
 * Author: antti
 *
 * Created on September 7, 2014, 12:24 PM
 */

#ifndef SERIALCONNECTION_H
#define	SERIALCONNECTION_H

#include "RobotConstants.h"

class SerialConnection {
public:
    SerialConnection();
    SerialConnection(const SerialConnection& orig);
    virtual ~SerialConnection();
    bool init();
private:

};

#endif	/* SERIALCONNECTION_H */

