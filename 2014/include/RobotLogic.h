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
#include "Maths.h"

class RobotLogic {

public:
    RobotLogic();
    RobotLogic(Goal gl);
    void setGoal();
    void setRole();
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

    int wantedGateRotation = 0;
    float startingOppositeDistance = 0.0f;
    int wantedRotateSpeed = 0;
    
    Odometer *odometer;
    ParticleFilterLocalizer *localizer;
    OdometerLocalizer *odometryLocalizer;

    Goal goal;
    Role role = Role::rATTACK;
    int startCounter=0;
    void runAttack(float dt);
    void runDefend(float dt);
    
    RobotController *rController;
    ImageProcessor *iProcessor;
    ImagePostProcessor *pProcessor;
    
    std::vector<Ball> balls;
    YellowGate yGate;
    BlueGate bGate;
    GreenContainer greens;
    
    int ballsFront = 0;
    int ballsBack = 0;
    
    void moveToBall(int x, int y);
    void setRState(RobotState state);
    RobotState rState = RobotState::FIND_BALL;
    RobotState lastState = RobotState::IDLE;
    
    BallFindState ballState;
    GateFindState gateState;
    TimeoutGateDir timeoutGateDir;
    
    bool gate_rear_turn;
    bool ball_rear_turn;
    bool ball_rear_drive;
    bool ball_front_drive;
    
    bool ballTimeoutLock = false;
    bool gateTimeoutLock = false;
    
    int ballTimeoutCount = 0;
    int gateTimeoutCount = 0;
    
    bool isGreen();
    
    void idle();
    void findBall();
    void ballTimeout();
    void findGate();
    void gateTimeout();
    void kickBall();
    void notGreen();
    void stalled();
    void defend();
    void driveToGate();
    
    GateFindState getGateState();
    
    BallFindState getBallState();
    
    void countBallData();   
    void loadOperationalData();
    
    void lockFrontBallDrive();
    void lockRearBallDrive();
    void releaseBallDriveLocks();
    
    void lockGateTurn();
    void releaseGateTurnLock();
    
    void lockBallTurn();
    void releaseBallTurnLock();
    
    bool hasBallsFront();
    bool hasBallsRear();
    
    Ball getFirstFrontBall();
    Ball getFirstRearBall();
    
    void gateVisibleFront();
    void gateVisibleRear();
    void opposingGateFront();
    void opposingGateRear();
    void gateInvisible();
    
    void driveBallsFront();
    void driveBallsRear();
    void ballsNotFound();
    
    //Rotates the robot 180 degrees.
    void robotRotate(int angle, int spd);
    
    int calculateMoveSpeed(float distance, float min, float max, float thresh);
    
    void robotDriveWrapperFront(int cen_x, float angle, float distance, float minSpd, float maxSpd, float speedThreshold);
    void robotDriveWrapperRear(int cen_x, float angle, float distance, float minSpd, float maxSpd, float speedThreshold);
    
};

#endif	/* ROBOTLOGIC_H */

