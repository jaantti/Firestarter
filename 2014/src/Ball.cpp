/* 
 * File:   BallObject.cpp
 * Author: antti
 * 
 * Created on November 17, 2014, 2:00 PM
 */

#include "Ball.h"


Ball::Ball(float distance, float angle, int cen_x, int cen_y, int len, RobotConstants::Direction dir) {
    this->dir = dir;
    this->distance = distance;
    this->angle = angle;
    this->cen_x = cen_x;
    this->cen_y = cen_y;
    this->len = len;
}

Ball::Ball(const Ball &other){    
    distance = other.distance;
    angle = other.angle;
    cen_x = other.cen_x;
    cen_y = other.cen_y;
    len = other.len;
    dir = other.dir;
}

Ball::~Ball() {
}

bool Ball::comapare(Ball & b1, Ball & b2) {
    return b1.getDistance() < b2.getDistance();
}

int Ball::getCen_y() {
    return cen_y;
}

int Ball::getCen_x() {
    return cen_x;
}

float Ball::getDistance() {
    return distance;
}

float Ball::getAngle() {
    return angle;
}

int Ball::getLen() {
    return len;
}

RobotConstants::Direction Ball::getDir(){
    return dir;
}

