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

Starter::Starter() :pProcessor(&iProcessor), canvas1(RobotConstants::frontCamImgWinName, &iProcessor), canvas2(RobotConstants::backCamImgWinName, &iProcessor)
{
    rLogic = RobotLogic();
}

Starter::~Starter() {
}

bool Starter::init() {
    
    std::string t;
    std::cout << " Do you wish to start the image display system?(y/n)" << std::endl;
    std::cin >> t;
    while(true){
        if(t.compare("n")){
            canvas1.chooseCamera(true);
            canvas2.chooseCamera(false);
            break;
        } else if(t.compare("y")) {
            canvas1.switchOff();
            canvas2.switchOff();
            break;
        } else {
            std::cout << "Input not understood." << std::endl;
        }
    }
    canvas1.assignPostProcessor(&pProcessor);
    canvas2.assignPostProcessor(&pProcessor);
    
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
    //rController.driveRobot(50,PI/-2.0,0);
    //Runs robot logic
    while(!codeEnd){
        canvas1.refreshFrame();
        canvas2.refreshFrame();
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
