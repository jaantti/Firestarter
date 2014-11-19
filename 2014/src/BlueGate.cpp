/* 
 * File:   BlueGate.cpp
 * Author: antti
 * 
 * Created on November 19, 2014, 11:31 AM
 */

#include "BlueGate.h"

BlueGate::BlueGate() {
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



BlueGate::~BlueGate() {
    delete this->x1;
    delete this->x2;
    delete this->y1;
    delete this->y2;
    
    delete this->cen_x;
    delete this->cen_y;
    
    delete this->width;
    delete this->height;
    
    delete this->dir;
    
    delete this->angle;
    delete this->distance;
}

