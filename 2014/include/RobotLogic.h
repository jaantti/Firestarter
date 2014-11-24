/* 
 * File:   RobotLogic.h
 * Author: antti
 *
 * Created on September 7, 2014, 12:24 PM
 */

#ifndef ROBOTLOGIC_H
#define	ROBOTLOGIC_H


#include "RobotConstants.h"
#include "ImageProcessor.h"
#include "RobotController.h"
#include "ImagePostProcessor.h"
#include "ParticleFilterLocalizer.h"
#include "Odometer.h"
#include "OdometerLocalizer.h"

class RobotLogic {

public:
    RobotLogic();
    RobotLogic(Goal gl);
    void setGoal();
    float getAngle(int x_coor);
    virtual ~RobotLogic();
    /**
     * Initialize variables
     * @param rCont
     * @param iProc
     */
    void init(RobotController *rCont, ImageProcessor *iProc);
    void init(RobotController *rCont, ImagePostProcessor *pProc);
    
    void loadOdometer(Odometer *odometer);
    void loadOdometryLocalizer(OdometerLocalizer *odoLocalizer);
    void loadParticleFilterLocalizer(ParticleFilterLocalizer *localizer);
    void setPosition(float x, float y, float orientation);
    
    void setInitialTime(unsigned long int time);
    unsigned long timeSinceLastSerial();
    /**
     * Run code
     * @param role Robot role
     */
    void run(Role role, float deltaTime);
private:

    float posX = 0.0f;
    float posY = 0.0f;
    float orientation = 0.0f;

    Odometer *odometer;
    ParticleFilterLocalizer *localizer;
    OdometerLocalizer *odometryLocalizer;

    Goal goal;
    int startCounter=0;
    void runAttack(float dt);
    void runDefend(float dt);
    
    RobotController *rController;
    ImageProcessor *iProcessor;
    ImagePostProcessor *pProcessor;
    
    void moveToBall(int x, int y);
    void setRState(RobotState state);
    RobotState rState = RobotState::FIND_BALL;
    RobotState lastState = RobotState::IDLE;
    
    bool isGreen(blobs_processed blobsFront, blobs_processed blobsBack);
    
    void idle();
    void findBall(blobs_processed blobsFront, blobs_processed blobsBack);
    void ballTimeout(blobs_processed blobsFront, blobs_processed blobsBack);
    void findGate(blobs_processed blobsFront, blobs_processed blobsBack);
    void gateTimeout(blobs_processed blobsFront, blobs_processed blobsBack);
    void kickBall(blobs_processed blobsFront, blobs_processed blobsBack);
    void notGreen(blobs_processed blobsFront, blobs_processed blobsBack);
    void stalled(blobs_processed blobsFront, blobs_processed blobsBack);
        
};

#endif	/* ROBOTLOGIC_H */

