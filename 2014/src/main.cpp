/* 
 * File:   main.cpp
 * Author: antti
 *
 * Created on September 7, 2014, 12:21 PM
 */

#include <cstdlib>
#include "Starter.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    Starter starter = Starter();
    starter.init();
    starter.start();
    return 0;
}

