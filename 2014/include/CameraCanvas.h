/* 
 * File:   CameraCanvas.h
 * Author: antti
 *
 * Created on November 13, 2014, 5:05 PM
 */

#ifndef CAMERACANVAS_H
#define	CAMERACANVAS_H

#include "ImageProcessor.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ImagePostProcessor.h"
#include "Ball.h"
#include "YellowGate.h"
#include "BlueGate.h"
#include <sstream>
#include <string>
#include <ctime>

class CameraCanvas {
public:
    CameraCanvas(std::string Name, ImageProcessor* iProcessor);
    void chooseCamera(bool front);
    void refreshFrame();
    void switchOff();
    void assignPostProcessor(ImagePostProcessor *pProcessor);
    
    CameraCanvas(const CameraCanvas& orig);
    virtual ~CameraCanvas();
private:
    std::string t_str;
    std::string canvasName;
    
    ImageProcessor *iProcessor;
    ImagePostProcessor *pProcessor;
    
    bool isSwitchedOn = true;
    bool isRGBMode = true;
    bool frontCamera = false;
    bool hasStreamInput = false;
    
    cv::Mat working_matrix;
    cv::Mat working_threshold;
    
    
    uchar* frame;
    uchar* thresholds;
    
    uchar* workingFrameRGB;
    uchar* workingFrameThresholds;
    
    void loadNewFrame();
    void processNewData();
    
    
    void pixel_conversion(uchar y, uchar u, uchar v, uchar *r, uchar *g, uchar *b);
    void convert_to_rgb();
    void convert_thresh_displayable();
    
    cv::Mat convertToCvMatrix();
    cv::Mat convertThresholdMatrix();    
    
    void waitForRealInput();
    
    void overlayObjects();
    
    void overlayText();
    
    void putText(Ball ball);
    
    void putText(YellowGate gate);
    
    void putText(BlueGate gate);
    
    //objectType - 0 : ball, 1:yellow gate, 2:blue gate 
    // dir : true - front, false : back
    void compileObjectString(float distance, float angle, int objectType, int xPoint, int yPoint);
    
};

#endif	/* CAMERACANVAS_H */

