/* 
 * File:   Gate.h
 * Author: antti
 *
 * Created on November 19, 2014, 10:51 AM
 */

#ifndef BLUEGATE_H
#define	BLUEGATE_H

class BlueGate {
public:
    
    enum Direction{
        FRONT = 201,
        REAR = 202
    };
    
    BlueGate();
        
    virtual ~BlueGate();
    
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

