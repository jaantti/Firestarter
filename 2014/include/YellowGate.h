/* 
 * File:   Gate.h
 * Author: antti
 *
 * Created on November 19, 2014, 10:51 AM
 */

#ifndef YELLOWGATE_H
#define	YELLOWGATE_H

class YellowGate {
public:
    
    enum Direction{
        FRONT = 201,
        REAR = 202
    };
    
    YellowGate();
        
    virtual ~YellowGate();
    
    int *x1;
    int *x2;
    int *y1;
    int *y2;
    
    int *cen_x;
    int *cen_y;
    
    int *width;
    int *height;
    
    Direction *dir;
    
    float *angle;
    float *distance;
    
    
private:   

};

#endif	/* GATE_H */

