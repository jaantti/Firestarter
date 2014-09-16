/* 
 * File:   Starter.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:22 PM
 */

#include "Starter.h"
#include <iostream>

Starter::Starter() {
}

Starter::Starter(const Starter& orig) {
}

Starter::~Starter() {
}

bool Starter::init(){
    std::cout << "Initialization successful." << std::endl;
    return true;
}

bool Starter::start(){
    
    return true;
}
