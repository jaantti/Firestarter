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

class RobotLogic {

public:
    RobotLogic();
    RobotLogic(Goal gl);
    virtual ~RobotLogic();
    /**
     * Initialize variables
     * @param rCont
     * @param iProc
     */
    void init(RobotController *rCont, ImageProcessor *iProc);
    void init(RobotController *rCont, ImagePostProcessor *pProc);
    
    /**
     * Run code
     * @param role Robot role
     */
    void run(Role role);
private:
    Goal goal;
    void runAttack();
    void runDefend();
    RobotController *rController;
    ImageProcessor *iProcessor;
    ImagePostProcessor *pProcessor;
    void moveToBall(int x, int y);
    void setRState(RobotState state);
    RobotState rState = RobotState::FIND_BALL;
    
    bool isGreen(blobs_processed blobsFront, blobs_processed blobsBack);
    
    void findBall(blobs_processed blobsFront, blobs_processed blobsBack);
    void ballTimeout(blobs_processed blobsFront, blobs_processed blobsBack);
    void findGate(blobs_processed blobsFront, blobs_processed blobsBack);
    void gateTimeout(blobs_processed blobsFront, blobs_processed blobsBack);
    void kickBall(blobs_processed blobsFront, blobs_processed blobsBack);
    void notGreen(blobs_processed blobsFront, blobs_processed blobsBack);    
    
};

#endif	/* ROBOTLOGIC_H */

