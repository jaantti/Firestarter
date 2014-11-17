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

Starter::Starter() :pProcessor(&iProcessor), canvas1(RobotConstants::frontCamImgWinName, &iProcessor),
					canvas2(RobotConstants::backCamImgWinName, &iProcessor), odometer(NULL),  localizer(NULL)
{
    rLogic = RobotLogic();
}

Starter::~Starter() {
}

bool Starter::init() {
    
    char t;
    std::cout << " Do you wish to start the image display system?(y/n)" << std::endl;
    std::cin >> t;
    if(t=='y'){
    	canvas1.chooseCamera(true);
        canvas2.chooseCamera(false);
    } else {
        canvas1.switchOff();
        canvas2.switchOff();
    }

    canvas1.assignPostProcessor(&pProcessor);
    canvas2.assignPostProcessor(&pProcessor);
    
    setupOdometer();
    setupParticleFilter();

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
    //Runs robot logic
    while(!codeEnd){
        canvas1.refreshFrame();
        canvas2.refreshFrame();
        rLogic.run(Role::rATTACK);
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

void Starter::setupOdometer() {
	odometer = new Odometer(
		RobotConstants::angleMotor1,
		RobotConstants::angleMotor2,
		RobotConstants::angleMotor3,
		RobotConstants::angleMotor4,
		RobotConstants::wheelOffset,
		RobotConstants::wheelRadius
	);
}


void Starter::setupParticleFilter(){
	localizer = new ParticleFilterLocalizer();

	localizer->addLandmark(
		"yellow-center",
		0.0f,
		Config::fieldHeight / 2.0f
	);

	localizer->addLandmark(
		"blue-center",
		RobotConstants::fieldWidth,
		RobotConstants::fieldHeight / 2.0f
	);
}

void Starter::codeEndListener(){
    int i;
    std::cin >> i;
    codeEnd = true;
}
