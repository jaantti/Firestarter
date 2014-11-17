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
    Ball();
    Ball(float distance, float angle, int cen_x, int cen_y);
    Ball(const Ball &other);
    virtual ~Ball();
        
    float getDistance();
    float getAngle();
    int getCenY();
    int getCenX();
    
private:
    float *distance;
    float *angle;
    int *cen_x;
    int *cen_y;
    
inline bool operator>(const Ball& other)
{
    return this->distance < other.distance;
}

inline bool operator<(const Ball& other)
{
    return this->distance > other.distance;
}

inline bool operator<=(const Ball& other)
{
    return !(this->distance < other.distance);
}

inline bool operator>=(const Ball& other)
{
    return !(this->distance < other.distance);
}
};

#endif	/* BALLOBJECT_H */

