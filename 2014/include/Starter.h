/* 
 * File:   Starter.h
 * Author: antti
 *
 * Created on September 7, 2014, 12:22 PM
 */

#ifndef STARTER_H
#define	STARTER_H

#include "RobotController.h"
#include "NetworkConnector.h"
#include "RobotLogic.h"
#include "ImageProcessor.h"

class Starter {
private:
    RobotLogic rLogic;
    ImageProcessor iProcessor;

public:
    RobotController rController;
    NetworkConnector nConnector;
    
    Starter();
    Starter(const Starter& orig);
    virtual ~Starter();
    bool init();
    bool start();
};

#endif	/* STARTER_H */

