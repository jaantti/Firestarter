/* 
 * File:   BlueGate.cpp
 * Author: antti
 * 
 * Created on November 19, 2014, 11:31 AM
 */

#include "BlueGate.h"

BlueGate::BlueGate(RobotConstants::Direction dir) {
    this->dir = dir;
}



BlueGate::~BlueGate() {
   
}

void BlueGate::SetDistance(float distance) {
    this->distance = distance;
}

float BlueGate::GetDistance() const {
    return distance;
}

void BlueGate::SetAngle(float angle) {
    this->angle = angle;
}

float BlueGate::GetAngle() const {
    return angle;
}

RobotConstants::Direction BlueGate::GetDir() const {
    return dir;
}

void BlueGate::SetHeight(int height) {
    this->height = height;
}

int BlueGate::GetHeight() const {
    return height;
}

void BlueGate::SetWidth(int width) {
    this->width = width;
}

int BlueGate::GetWidth() const {
    return width;
}

void BlueGate::SetCen_y(int cen_y) {
    this->cen_y = cen_y;
}

int BlueGate::GetCen_y() const {
    return cen_y;
}

void BlueGate::SetCen_x(int cen_x) {
    this->cen_x = cen_x;
}

int BlueGate::GetCen_x() const {
    return cen_x;
}

void BlueGate::SetY2(int y2) {
    this->y2 = y2;
}

int BlueGate::GetY2() const {
    return y2;
}

void BlueGate::SetY1(int y1) {
    this->y1 = y1;
}

int BlueGate::GetY1() const {
    return y1;
}

void BlueGate::SetX2(int x2) {
    this->x2 = x2;
}

int BlueGate::GetX2() const {
    return x2;
}

void BlueGate::SetX1(int x1) {
    this->x1 = x1;
}

int BlueGate::GetX1() const {
    return x1;
}

