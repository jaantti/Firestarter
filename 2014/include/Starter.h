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
#include "ImagePostProcessor.h"
#include "CameraCanvas.h"
#include "ParticleFilterLocalizer.h"
#include "Odometer.h"
#include "OdometerLocalizer.h"

class Starter {
private:
    

    Odometer *odometer;
    ParticleFilterLocalizer *localizer;
    OdometerLocalizer *odometryLocalizer;
    
    CameraCanvas canvas1;
    CameraCanvas canvas2;
    
    RobotLogic rLogic;
    ImageProcessor iProcessor;
    ImagePostProcessor pProcessor;
    bool codeEnd = false;
    /**
     * Waits for keypress to end code
     */
    void codeEndListener();

public:
    RobotController rController;
    NetworkConnector nConnector;    
    
    void setupParticleFilter();
    void setupOdometer();
    void setupOdometryLocalizer();

    Starter();
    virtual ~Starter();
    bool init();
    bool start();
};

#endif	/* STARTER_H */

