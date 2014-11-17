/* 
 * File:   BallObject.cpp
 * Author: antti
 * 
 * Created on November 17, 2014, 2:00 PM
 */

#include "Ball.h"

Ball::Ball() {
}

Ball::Ball(float distance, float angle, int cen_x, int cen_y) {
    this->distance = new float;
    this->angle = new float;
    this->cen_x = new int;
    this->cen_y = new int;
    
    *this->distance = distance;
    *this->angle = angle;
    *this->cen_x = cen_x;
    *this->cen_y = cen_y;
    
}

Ball::Ball(const Ball &other){
    this->distance = new float;
    this->angle = new float;
    this->cen_x = new int;
    this->cen_y = new int;
    
    
    *distance = *other.distance;
    *angle = *other.angle;
    *cen_x = *other.cen_x;
    *cen_y = *other.cen_y;
}

Ball::~Ball() {
    delete distance;
    delete angle;
    delete cen_x;
    delete cen_y;
}

