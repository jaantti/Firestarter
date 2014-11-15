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
#include "boost/thread.hpp"

Starter::Starter() :pProcessor(&iProcessor)
{
    rLogic = RobotLogic();
}

Starter::~Starter() {
}

bool Starter::init() {
    
    rController.init();
    iProcessor.init();
    rLogic.init(&rController, &pProcessor);
    rLogic.setGoal();
    std::cout << "Initialization successful." << std::endl;
    return true;
}

bool Starter::start() {
            
    boost::thread iFrontThread(&ImageProcessor::runFrontCamera, &iProcessor);
    boost::thread iBackThread(&ImageProcessor::runBackCamera, &iProcessor);
    boost::thread pProcTread(&ImagePostProcessor::run, &pProcessor);
    boost::thread codeEndThread(&Starter::codeEndListener, this);    
    
    rController.driveRobot(50,PI/-2.0,0);
    //Runs robot logic
    while(!codeEnd){
        rLogic.run(Role::rATTACK);
        usleep(1000);        
    }
    
    pProcessor.stopProcessor();
    iProcessor.stopProcessor();
    codeEndThread.join();
    pProcTread.join();
    iFrontThread.join();
    iBackThread.join();
    rController.closeSerial();
    rController.driveRobot(0,0,0);

    return true;
}

void Starter::codeEndListener(){
    int i;
    std::cin >> i;
    codeEnd = true;
}
