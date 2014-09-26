/* 
 * File:   Starter.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:22 PM
 */

#include "Starter.h"
#include "ImageProcessor.h"
#include "RobotLogic.h"
#include <iostream>

Starter::Starter() 
{
    
}

Starter::Starter(const Starter& orig) {
}

Starter::~Starter() {
}

bool Starter::init() {
    rController.init();
    //iProcessor.init();
    std::cout << "Initialization successful." << std::endl;
    return true;
}

bool Starter::start() {
    
    rController.driveRobot(50,PI/4.0,0);
    usleep(2000000);
    rController.driveRobot(50,2.0*PI - PI/(-4.0),0);
    //rController.driveRobot(50,PI, PI);
    
    return true;
}
