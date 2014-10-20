/* 
 * File:   RobotLogic.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "RobotLogic.h"
#include <iostream>

using namespace std;

RobotLogic::RobotLogic() {
}

RobotLogic::~RobotLogic() {
}

void RobotLogic::init(RobotController* rCont, ImageProcessor* iProc) {
    rController = rCont;
    iProcessor = iProc;
}

void RobotLogic::run(Role role, Goal goal){
    if(role == rATTACK){
        runAttack(goal);
    }
    if(role == rDEFEND){
        runDefend(goal);
    }
}

void RobotLogic::runAttack(Goal goal){
    //cout << "before mutex" << endl;
    blobs blobsFront = iProcessor->getBlobsFront();
    iProcessor->unlockFront();
    blobs blobsBack = iProcessor->getBlobsBack();
    iProcessor->unlockBack();
    //cout << "after mutex" << endl;
    //cout << "Oranges processed: " << blobsFront.oranges_processed << endl;
    if(blobsFront.oranges_processed > 0){
        //cout << "orange list size: " << blobsFront.o_blob.size() << endl;
        orange_blob oBlob = blobsFront.o_blob.at(0);
        cout << "orange area front: " << oBlob.orange_area << "\n";
        if (oBlob.orange_area > 20){
            if(oBlob.orange_cen_x > 320) std::cout << "front: turn right\n";
            if(oBlob.orange_cen_x <= 320) std::cout << "front: turn left\n";
            moveToBall(oBlob.orange_cen_x, oBlob.orange_cen_y);
        }
        
    }else{
        rController->driveRobot(0, 0, 0);
    }
    if(false && blobsBack.oranges_processed > 0){
        orange_blob oBlob = blobsBack.o_blob.at(0);
        cout << "orange area back: " << oBlob.orange_area << "\n";
        if (oBlob.orange_area > 20){
            if(oBlob.orange_cen_x > 320) std::cout << "back: turn right\n";
            if(oBlob.orange_cen_x <= 320) std::cout << "back: turn left\n";
        }
        
    }
}

void RobotLogic::runDefend(Goal goal){
    
}

void RobotLogic::moveToBall(int x, int y){
    if(x < 280) rController->driveRobot(20, 0, 20);
    else if(x > 360) rController->driveRobot(20, 0, -20);
    else rController->driveRobot(20, 0, 0);
}
