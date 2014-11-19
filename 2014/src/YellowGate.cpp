/* 
 * File:   Gate.cpp
 * Author: antti
 * 
 * Created on November 19, 2014, 10:51 AM
 */

#include "YellowGate.h"

YellowGate::YellowGate() {
    this->x1 = new int;
    this->x2 = new int;
    this->y1 = new int;
    this->y2 = new int;
    
    this->cen_x = new int;
    this->cen_y = new int;
    
    this->width = new int;
    this->height = new int;
    
    this->dir = new Direction;
    
    this->angle = new float;
    this->distance = new float;
}


YellowGate::~YellowGate() {
}

