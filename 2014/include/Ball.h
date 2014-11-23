/* 
 * File:   BallObject.h
 * Author: antti
 *
 * Created on November 17, 2014, 2:00 PM
 */

#ifndef BALLOBJECT_H
#define	BALLOBJECT_H

class Ball {    
public:
    Ball(float distance, float angle, int cen_x, int cen_y, int len);
    Ball(const Ball &other);
    virtual ~Ball();
        
    float getDistance();
    float getAngle();
    
    bool comapare(Ball & b1, Ball & b2);
    int getCen_y();
    int getCen_x();
    
    int getLen();
    
    bool operator>(const Ball& other) const
    {
        return (distance < other.distance);
    }

    bool operator<(const Ball& other) const
    {
        return (distance > other.distance);
    }

    bool operator<=(const Ball& other) const
    {
        return !(distance < other.distance);
    }

    bool operator>=(const Ball& other) const
    {
        return !(distance < other.distance);
    }

private:
    
    float distance;
    float angle;
    int cen_x;
    int cen_y;
    int len;
 }; 

#endif	/* BALLOBJECT_H */

