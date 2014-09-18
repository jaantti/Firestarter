/* 
 * File:   ImageProcessor.h
 * Author: antti
 *
 * Created on September 7, 2014, 12:24 PM
 */

#ifndef IMAGEPROCESSOR_H
#define	IMAGEPROCESSOR_H

#include "RobotConstants.h"
//#include "segmentation.h"

class ImageProcessor {
public:
    ImageProcessor();
    ImageProcessor(const ImageProcessor& orig);
    virtual ~ImageProcessor();
    void init();
private:
     //SEGMENTATION segm;
};

#endif	/* IMAGEPROCESSOR_H */

