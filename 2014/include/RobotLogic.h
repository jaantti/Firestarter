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

class RobotLogic {

public:
    RobotLogic();
    virtual ~RobotLogic();
    /**
     * Initialize variables
     * @param rCont
     * @param iProc
     */
    void init(RobotController *rCont, ImageProcessor *iProc);
    
    /**
     * Run code
     * @param role Robot role
     * @param goal Color of the goal
     */
    void run(Role role, Goal goal);
private:
    void runAttack(Goal goal);
    void runDefend(Goal goal);
    RobotController *rController;
    ImageProcessor *iProcessor;
    void moveToBall(int x, int y);
    
};

#endif	/* ROBOTLOGIC_H */

