/* 
 * File:   CameraCanvas.cpp
 * Author: antti
 * 
 * Created on November 13, 2014, 5:05 PM
 */

#include "CameraCanvas.h"

CameraCanvas::CameraCanvas(std::string Name, ImageProcessor* iProcessor){
    std::cout << " Creating canvas " << canvasName << "." << std::endl;
    this->canvasName = Name;
    this->iProcessor = iProcessor;
    
    frame = (uchar*)malloc(RobotConstants::frameSize);
    thresholds = (uchar*)malloc(RobotConstants::frameSize/2);
    
    workingFrameRGB = (uchar*)malloc(640*480*3);
    workingFrameThresholds = (uchar*)malloc(640*480*3);
    
    std::stringstream stream;
    stream << this->canvasName << "_Threshold";
    t_str = stream.str();
    std::cout << " Canvas " << canvasName << " created." << std::endl;
}

CameraCanvas::CameraCanvas(const CameraCanvas& orig) {
}

CameraCanvas::~CameraCanvas() {
}

void CameraCanvas::assignPostProcessor(ImagePostProcessor* pProcessor) {
    this->pProcessor = pProcessor;
}


void CameraCanvas::chooseCamera(bool front){
    this->frontCamera = front;
    if(front){
        std::cout << " Canvas " << canvasName << " represents front camera." << std::endl;
    } else {
        std::cout << " Canvas " << canvasName << " represents back camera." << std::endl;        
    }
}

void CameraCanvas::waitForRealInput(){
    cv::Mat initImg;
    initImg = cv::imread(RobotConstants::initFile, CV_LOAD_IMAGE_COLOR);
    while(true){
        cv::imshow(this->canvasName, initImg);
        if(frontCamera){
            iProcessor->getWorkingFrontFrame(frame);
        } else {
            iProcessor->getWorkingBackFrame(frame);
        }
        int pixel_real = 0;
        for(int i=0; i<RobotConstants::frameSize; i++)
        {
            if(frame[i]){
                pixel_real++;
            }
            if(pixel_real>100){
                break;
            }
        }
        if(pixel_real>100) break;
        
        if(cv::waitKey(30) >=0){
            break;
        }
    }
    std::cout << " Stream for " << this->canvasName << " has been received." << std::endl;
    hasStreamInput = true;
    
}

void CameraCanvas::refreshFrame(){
    if(!isSwitchedOn) return;
    if(!hasStreamInput) waitForRealInput();
    loadNewFrame();
    processNewData();   
    
    cv::imshow(this->canvasName, working_matrix);
    cv::imshow(t_str, working_threshold);
    cv::waitKey(1);
    usleep(15);
}
//Processes the raw data into something that we can display.
void CameraCanvas::processNewData() {
    if(isRGBMode) convert_to_rgb();
    
    working_matrix = convertToCvMatrix();
    overlayObjects();
    overlayText();
    convert_thresh_displayable();
    
    working_threshold = convertThresholdMatrix();    
}

void CameraCanvas::loadNewFrame() {
    if(frontCamera){
        iProcessor->getWorkingFrontFrame(frame);
        iProcessor->getWorkingFrontThresh(thresholds);
    } else {
        iProcessor->getWorkingBackFrame(frame);
        iProcessor->getWorkingBackThresh(thresholds);
    }
}

void CameraCanvas::convert_thresh_displayable() {
    int green_r = 0, green_g = 255, green_b = 0;
    int orange_r = 255, orange_g = 128, orange_b = 0;
    int yellow_r = 255, yellow_g = 255, yellow_b = 0;
    int blue_r = 0, blue_g = 0, blue_b = 255;
    int white_r = 255, white_g = 255, white_b = 255;
    int black_r = 20, black_g = 20, black_b = 20;
    int nocolor_r = 0, nocolor_g = 0, nocolor_b = 0;
    
    int work_count = 0;
    for(int i = 0; i<RobotConstants::frameSize/2; i++){
        if( (int)thresholds[i] == 0 ){
            workingFrameThresholds[work_count] = black_b;
            workingFrameThresholds[work_count+1] = black_g;
            workingFrameThresholds[work_count+2] = black_r;
        }
        if( (int)thresholds[i] == 1 ){
            workingFrameThresholds[work_count] = white_b;
            workingFrameThresholds[work_count+1] = white_g;
            workingFrameThresholds[work_count+2] = white_r;
        }
        if( (int)thresholds[i] == 2 ){
            workingFrameThresholds[work_count] = green_b;
            workingFrameThresholds[work_count+1] = green_g;
            workingFrameThresholds[work_count+2] = green_r;
        }
        if( (int)thresholds[i] == 3 ){
            workingFrameThresholds[work_count] = blue_b;
            workingFrameThresholds[work_count+1] = blue_g;
            workingFrameThresholds[work_count+2] = blue_r;
        }
        if( (int)thresholds[i] == 4 ){
            workingFrameThresholds[work_count] = yellow_b;
            workingFrameThresholds[work_count+1] = yellow_g;
            workingFrameThresholds[work_count+2] = yellow_r;
        }
        if( (int)thresholds[i] == 5 ){
            workingFrameThresholds[work_count] = orange_b;
            workingFrameThresholds[work_count+1] = orange_g;
            workingFrameThresholds[work_count+2] = orange_r;
        }
        if( (int)thresholds[i] == 6 ){
            workingFrameThresholds[work_count] = nocolor_b;
            workingFrameThresholds[work_count+1] = nocolor_g;
            workingFrameThresholds[work_count+2] = nocolor_r;
        }
        
        work_count += 3;
    }
    
}

void CameraCanvas::convert_to_rgb() {
    int rgb_count = 0;
    for(int i=0; i<RobotConstants::frameSize; i+=4){
        uchar y, u, y2, v;
        y = frame[i];
        u = frame[i+1];
        y2 = frame[i+2];
        v = frame[i+3];
        
        pixel_conversion(y, u, v, &workingFrameRGB[rgb_count], &workingFrameRGB[rgb_count+1], &workingFrameRGB[rgb_count+2]);
        pixel_conversion(y2, u, v, &workingFrameRGB[rgb_count+3], &workingFrameRGB[rgb_count+4], &workingFrameRGB[rgb_count+5]);        
        rgb_count+=6;
    }
    
}

void CameraCanvas::pixel_conversion(uchar y, uchar u, uchar v, uchar* r, uchar* g, uchar* b) {
    int amp = 255;
	double R, G, B;

	//conversion equations
	B = amp * ( 0.004565*y + 0.000001*u + 0.006250*v - 0.872 );
	G = amp * ( 0.004565*y - 0.001542*u - 0.003183*v + 0.531 );
	R = amp * ( 0.004565*y + 0.007935*u - 1.088 );

	//R, G and B must be in the range from 0 to 255
	if( R < 0 ) R = 0;
	if( G < 0 ) G = 0;
	if( B < 0 ) B = 0;

	if( R > 255 ) R = 255;
	if( G > 255 ) G = 255;
	if( B > 255 ) B = 255;

	*r = (uchar) R;
	*g = (uchar) G;
	*b = (uchar) B;
}

cv::Mat CameraCanvas::convertToCvMatrix() {
    
    cv::Mat mat = cv::Mat(480, 640, CV_8UC3, workingFrameRGB, cv::Mat::AUTO_STEP);
    
    return mat;
}

cv::Mat CameraCanvas::convertThresholdMatrix() {
   cv::Mat mat = cv::Mat(480, 640, CV_8UC3, workingFrameThresholds, cv::Mat::AUTO_STEP);
   return mat;
}

void CameraCanvas::switchOff() {
    isSwitchedOn = false;
}

void CameraCanvas::overlayObjects() {
    std::vector<Ball> balls = pProcessor->getBalls();
    for(int i=0; i<balls.size(); i++){
        Ball ball = balls.at(i);
        if(this->frontCamera && ball.getDir()==RobotConstants::Direction::FRONT){
            cv::circle(working_matrix, cv::Point(ball.getCen_x(), ball.getCen_y()), ball.getLen()/2, cv::Scalar(0, 128, 255), 3, 8, 0);
        }
        if(!this->frontCamera && ball.getDir()==RobotConstants::Direction::REAR){
            cv::circle(working_matrix, cv::Point(ball.getCen_x(), ball.getCen_y()), ball.getLen()/2, cv::Scalar(0, 128, 255), 3, 8, 0);
        }
    }
    YellowGate gateY = pProcessor->getYellowGate();
    BlueGate gateB = pProcessor->getBlueGate();
    //cout << " Yellow gate direction :" << gateY.GetDir() << std::endl;
    //cout << " Blue gate direction :" << gateB.GetDir() << std::endl;
    if(gateB.GetCen_x()!=-1 && gateB.GetDir()==RobotConstants::Direction::FRONT && this->frontCamera){
        cv::rectangle(working_matrix, cv::Point(gateB.GetX1(), gateB.GetY1()), cv::Point(gateB.GetX2(), gateB.GetY2()), cv::Scalar(255,0,0), 3, 8);
    }
    
    if(gateB.GetCen_x()!=-1 && gateB.GetDir()==RobotConstants::Direction::REAR && !this->frontCamera){
        cv::rectangle(working_matrix, cv::Point(gateB.GetX1(), gateB.GetY1()), cv::Point(gateB.GetX2(), gateB.GetY2()), cv::Scalar(255,0,0), 3, 8);
    }
    
    if(gateY.GetCen_x()!=-1 && gateY.GetDir()==RobotConstants::Direction::FRONT && this->frontCamera){
        cv::rectangle(working_matrix, cv::Point(gateY.GetX1(), gateY.GetY1()), cv::Point(gateY.GetX2(), gateY.GetY2()), cv::Scalar(0,255,255), 3, 8);
    }
    
    if(gateY.GetCen_x()!=-1 && gateY.GetDir()==RobotConstants::Direction::REAR && !this->frontCamera){
        cv::rectangle(working_matrix, cv::Point(gateY.GetX1(), gateY.GetY1()), cv::Point(gateY.GetX2(), gateY.GetY2()), cv::Scalar(0,255,255), 3, 8);
    }
}

void CameraCanvas::overlayText() {
    std::vector<Ball> balls;
    balls = pProcessor->getBalls();
    for (Ball ball : balls){
        if(this->frontCamera && ball.getDir()==RobotConstants::Direction::FRONT){
            putText(ball);
        }
        if(!this->frontCamera && ball.getDir()==RobotConstants::Direction::REAR){
            putText(ball);
        }
    }
    putText(pProcessor->getYellowGate());
    putText(pProcessor->getBlueGate());
}

void CameraCanvas::putText(Ball ball){    
    int halflen = ball.getLen()/2;
    int xCoord = 0, yCoord = 0;
    if(ball.getCen_x()>600){
        xCoord = ball.getCen_x()-halflen;
    } else {
        xCoord = ball.getCen_x()+halflen;
    }
    yCoord = ball.getCen_y()-halflen;
    compileObjectString(ball.getDistance(), ball.getAngle(), 0, xCoord, yCoord);
   
}

void CameraCanvas::putText(YellowGate gate){ 
    if(gate.GetDir()==RobotConstants::Direction::FRONT && frontCamera){
        compileObjectString(gate.GetDistance(), gate.GetAngle(), 1, gate.GetCen_x(), gate.GetCen_y());
    } else if (gate.GetDir()==RobotConstants::Direction::REAR && !frontCamera){
        compileObjectString(gate.GetDistance(), gate.GetAngle(), 1, gate.GetCen_x(), gate.GetCen_y());
    }
}

void CameraCanvas::putText(BlueGate gate){
    if(gate.GetDir()==RobotConstants::Direction::FRONT && frontCamera){
        compileObjectString(gate.GetDistance(), gate.GetAngle(), 1, gate.GetCen_x(), gate.GetCen_y());
    } else if (gate.GetDir()==RobotConstants::Direction::REAR && !frontCamera){
        compileObjectString(gate.GetDistance(), gate.GetAngle(), 1, gate.GetCen_x(), gate.GetCen_y());
    }    
}

//TODO : Rework it into putText functions.
void CameraCanvas::compileObjectString(float distance, float angle, int objectType, int xPoint, int yPoint) {
    std::stringstream distanceStream;
    std::stringstream angleStream;
    std::stringstream objectStream;
    distanceStream << "Distance :" << distance;
    angleStream << "Angle :" << angle;
    objectStream << " Object :";
    if(objectType==0) objectStream << "Ball";
    else if(objectType==1) objectStream << "YellowGate";
    else objectStream << "BlueGate";
    
    cv::putText(working_matrix, distanceStream.str(), cv::Point(xPoint, yPoint) , cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
    cv::putText(working_matrix, angleStream.str(), cv::Point(xPoint, yPoint+13) , cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
    cv::putText(working_matrix, objectStream.str(), cv::Point(xPoint, yPoint+26) , cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);

    
}
