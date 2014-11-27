/* 
 * File:   Gate.h
 * Author: antti
 *
 * Created on November 19, 2014, 10:51 AM
 */

#ifndef YELLOWGATE_H
#define	YELLOWGATE_H
#include "RobotConstants.h"

class YellowGate{
public:
    
    
    YellowGate();
        
    virtual ~YellowGate();
    void SetDistance(float distance);
    float GetDistance() const;
    void SetAngle(float angle);
    float GetAngle() const;
    void SetDir(RobotConstants::Direction dir);
    RobotConstants::Direction GetDir() const;
    void SetHeight(int height);
    int GetHeight() const;
    void SetWidth(int width);
    int GetWidth() const;
    void SetCen_y(int cen_y);
    int GetCen_y() const;
    void SetCen_x(int cen_x);
    int GetCen_x() const;
    void SetY2(int y2);
    int GetY2() const;
    void SetY1(int y1);
    int GetY1() const;
    void SetX2(int x2);
    int GetX2() const;
    void SetX1(int x1);
    int GetX1() const;
    
    int x1 =-1;
    int x2 =-1;
    int y1 =-1;
    int y2 =-1;
    
    int cen_x =-1;
    int cen_y =-1;
    
    int width =-1;
    int height =-1;
    
    RobotConstants::Direction dir;
    
    float angle = 0.0f;
    float distance = -1;
    
    
private:   

};

#endif	/* GATE_H */

