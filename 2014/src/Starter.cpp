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
    
    iProcessor.runIprocessor();
    //rController.driveRobot(50,PI/-2.0,0);
    //rController.driveRobot(50,PI, PI);
    while(true){
        //Stall loop.
    }
    return true;
}
