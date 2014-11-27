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
    canvas2(RobotConstants::backCamImgWinName, &iProcessor), odometer(NULL),  localizer(NULL), odometryLocalizer(NULL)
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
    setupOdometryLocalizer();
    
    
    rController.init();
    
    iProcessor.init();
    rLogic.init(&rController, &pProcessor);
    rLogic.setGoal();
    rLogic.loadOdometer(odometer);
    rLogic.loadOdometryLocalizer(odometryLocalizer);
    rLogic.loadParticleFilterLocalizer(localizer);
    
    std::cout << "Initialization successful." << std::endl;
    return true;
}

bool Starter::start() {
    
    boost::thread iFrontThread(&ImageProcessor::runFrontCamera, &iProcessor);
    usleep(500000);
    boost::thread iBackThread(&ImageProcessor::runBackCamera, &iProcessor);
    usleep(500000);
    boost::thread pProcTread(&ImagePostProcessor::run, &pProcessor);
    boost::thread codeEndThread(&Starter::codeEndListener, this);
    
    usleep(1000000);
    
    //Runs robot logic
    struct timeval tv, tv2;
    gettimeofday(&tv, NULL);
    unsigned long int tim1 = 1000000 * tv.tv_sec + tv.tv_usec;
    rLogic.setInitialTime(tim1);
    unsigned long int tim2;
    
    unsigned long int serialDif;
    
    //TODO : Measure time for main loop AND measure time between serial communications.
    while(!codeEnd){
        gettimeofday(&tv, NULL);
        tim1 = 1000000 * tv.tv_sec + tv.tv_usec;
        canvas1.refreshFrame();
        canvas2.refreshFrame();        
        float f = translateMicrosToSec(rLogic.timeSinceLastSerial());
        rLogic.run(Role::rATTACK, f);
        
        gettimeofday(&tv2, NULL);
        tim2 = 1000000 * tv2.tv_sec + tv2.tv_usec;
        unsigned long int timeDif = tim2-tim1;
        sleepForDifference(translateMicrosToSec(timeDif));
    }
    
    pProcessor.stopProcessor();
    iProcessor.stopProcessor();

    codeEndThread.join();
    pProcTread.join();
    iFrontThread.join();
    iBackThread.join();

    rController.stopDribbler();
    rController.driveRobot(0,0,0);
    rController.closeSerial();
    
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

void Starter::setupOdometryLocalizer() {
    odometryLocalizer = new OdometerLocalizer();
}


void Starter::setupParticleFilter(){
	localizer = new ParticleFilterLocalizer();

	localizer->addLandmark(
		"yellow-center",
		0.0f,
		RobotConstants::fieldHeight / 2.0f
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

float Starter::translateMicrosToSec(unsigned long timeDiffMicros) {
    float f = (float)timeDiffMicros / 1000000.0;
    return f;
}

void Starter::sleepForDifference(float f){
    if(f>RobotConstants::minimumDeltaT) return;
    int dif = ( (int) 1000000 * RobotConstants::minimumDeltaT) - ( (int) 1000000*f);
    usleep(dif);
    
    
}
