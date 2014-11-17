/* 
 * File:   OdometerLocalizer.h
 * Author: antti
 *
 * Created on November 17, 2014, 10:36 AM
 */

#ifndef ODOMETERLOCALIZER_H
#define	ODOMETERLOCALIZER_H

#include "Localizer.h"

class OdometerLocalizer : public Localizer {

    public:
    
        OdometerLocalizer();
    
        void move(float velocityX, float velocityY, float omega, float dt);
        
        private:
        

};

#endif	/* ODOMETERLOCALIZER_H */
