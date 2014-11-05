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
    
    bool isGreen(blobs blobsFront, blobs blobsBack);
    
    void findBall(blobs blobsFront, blobs blobsBack);
    void ballTimeout(blobs blobsFront, blobs blobsBack);
    void findGate(blobs blobsFront, blobs blobsBack);
    void gateTimeout(blobs blobsFront, blobs blobsBack);
    void kickBall(blobs blobsFront, blobs blobsBack);
    void notGreen(blobs blobsFront, blobs blobsBack);    
    
};

#endif	/* ROBOTLOGIC_H */

