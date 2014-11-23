/* 
 * File:   Gate.cpp
 * Author: antti
 * 
 * Created on November 19, 2014, 10:51 AM
 */

#include "YellowGate.h"

YellowGate::YellowGate(RobotConstants::Direction dir) {
    this->dir = dir;
}


YellowGate::~YellowGate() {
}

void YellowGate::SetDistance(float distance) {
    this->distance = distance;
}

float YellowGate::GetDistance() const {
    return distance;
}

void YellowGate::SetAngle(float angle) {
    this->angle = angle;
}

float YellowGate::GetAngle() const {
    return angle;
}

RobotConstants::Direction YellowGate::GetDir() const {
    return dir;
}

void YellowGate::SetHeight(int height) {
    this->height = height;
}

int YellowGate::GetHeight() const {
    return height;
}

void YellowGate::SetWidth(int width) {
    this->width = width;
}

int YellowGate::GetWidth() const {
    return width;
}

void YellowGate::SetCen_y(int cen_y) {
    this->cen_y = cen_y;
}

int YellowGate::GetCen_y() const {
    return cen_y;
}

void YellowGate::SetCen_x(int cen_x) {
    this->cen_x = cen_x;
}

int YellowGate::GetCen_x() const {
    return cen_x;
}

void YellowGate::SetY2(int y2) {
    this->y2 = y2;
}

int YellowGate::GetY2() const {
    return y2;
}

void YellowGate::SetY1(int y1) {
    this->y1 = y1;
}

int YellowGate::GetY1() const {
    return y1;
}

void YellowGate::SetX2(int x2) {
    this->x2 = x2;
}

int YellowGate::GetX2() const {
    return x2;
}

void YellowGate::SetX1(int x1) {
    this->x1 = x1;
}

int YellowGate::GetX1() const {
    return x1;
}

