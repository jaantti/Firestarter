/* 
 * File:   ImageProcessor.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "ImageProcessor.h"
#include "boost/thread.hpp"

using namespace boost;

ImageProcessor::ImageProcessor() :segm(CAM_W, CAM_H), segm2(CAM_W, CAM_H)
{

}

ImageProcessor::~ImageProcessor() {
}

void ImageProcessor::init() {
    
    std::system("./config.sh");
    
    workingFrontFrame = (uchar*)calloc(RobotConstants::frameSize, 1);
    workingBackFrame = (uchar*)calloc(RobotConstants::frameSize, 1);
    
    workingFrontThresh = (uchar*)calloc(RobotConstants::frameSize/2, 1);
    workingBackThresh = (uchar*)calloc(RobotConstants::frameSize/2, 1);
    
    switchCameras = chooseCameras();
    if(switchCameras){
        front = "/dev/video0";
        back = "/dev/video1";
    } else {
        front = "/dev/video1";
        back = "/dev/video0";
    }
    std::cout << " Front camera chosen. Waiting" << std::endl;
    usleep(1000000);
}

void ImageProcessor::runIprocessor() {
    boost::thread frontThread  (&ImageProcessor::processFrontCamera, this,  front);
    boost::thread backThread (&ImageProcessor::processBackCamera, this,  back);
    frontThread.join();
    backThread.join();
}

void ImageProcessor::runFrontCamera(){
    processFrontCamera(front);
}

void ImageProcessor::runBackCamera(){
    processBackCamera(back);
}


bool ImageProcessor::chooseCameras() {
    // TODO : Implement logic for choosing which camera is which.
    // if switchCamers = true, then video0 is front, if false, video0 is back.
    uchar *frame1 = NULL;
    uchar *frame2 = NULL;
    
    cap1.open_device("/dev/video0");
    cap1.init_device("/dev/video0");
    cap1.start_capturing();
    
    cap2.open_device("/dev/video1");
    cap2.init_device("/dev/video1");
    cap2.start_capturing();
    
    segm.readThresholds("conf1");
    segm2.readThresholds("conf1");
    
    //If solution = true, means that video0 is the front camera and video1 is the back camera.
    //If solution = false, means that video1 is the front camera and video0 the back.
    bool solution = true;
        
    while(true){
        frame1 = cap1.read_frame();
        frame2 = cap2.read_frame();
        if(frame1){
            segm.processImage(frame1);            
            if(segm.colors[SEG_ORANGE].list!=NULL){
                std::cout << "orange size:" << segm.colors[SEG_ORANGE].list[0].area << std::endl;
                if(segm.colors[SEG_ORANGE].list[0].area>20) {
                    solution = true;
                    std::cout << "Calibration found ! Video0 in front." << std::endl;
                    break;
                }
            }
        }
        if(frame2){
            segm2.processImage(frame2);            
            if(segm2.colors[SEG_ORANGE].list!=NULL){
                solution = false;
                std::cout << "Calibration found ! Video1 in front." << std::endl;
                break;
            }           
        }
        std::cout << "Waiting for calibration..." << std::endl;
    }
   
    cap2.stop_capturing();
    cap2.uninit_device();
    cap2.close_device();
    
    
    cap1.stop_capturing();
    cap1.uninit_device();
    cap1.close_device();
    usleep(1000000);
    std::cout << " Capture stopped successfully" << std::endl;
    return solution;
    
}

void ImageProcessor::processFrontCamera(char *cam) {
    uchar *frame = NULL;
    cap1.open_device( cam );
    cap1.init_device( cam );
    cap1.start_capturing();
    std::cout << " Successfully initialized Front camera " << cam << std::endl;
    segm.readThresholds("conf1");
    int tim = 0;
    while(!codeEnd){
        frame = cap1.read_frame();
        if(frame){
            std::clock_t begin = std::clock();
            segm.processImage(frame);
            
            lockFrontFrame();            
            memcpy(workingFrontFrame, frame, RobotConstants::frameSize);
            memcpy(workingFrontThresh, segm.th_data, RobotConstants::frameSize/2);
            unlockFrontFrame();
            
            processOrange(true);
            processBlue(true);
            processYellow(true);
            processGreens(true);            
            std::clock_t end = std::clock();
            tim = (int)(double(end - begin) / CLOCKS_PER_SEC * 1000);
        }
    }
    cap1.close_device();
}

void ImageProcessor::processBackCamera(char *cam) {
    uchar *frame = NULL;
    cap2.open_device( cam );
    cap2.init_device( cam );
    cap2.start_capturing();
    std::cout << " Successfully initialized Back camera " << cam << std::endl;
    segm2.readThresholds("conf1");   
    int tim = 0;
    while(!codeEnd){        
        frame = cap2.read_frame();
        if(frame){
            
            std::clock_t begin = std::clock();
            segm2.processImage(frame);
            
            lockBackFrame();
            memcpy(workingBackFrame, frame, RobotConstants::frameSize);
            memcpy(workingBackThresh, segm2.th_data, RobotConstants::frameSize/2);
            unlockBackFrame();
            
            processOrange(false);
            processBlue(false);
            processYellow(false);
            processGreens(false);            
            std::clock_t end = std::clock();
            tim = (int)(double(end - begin) / CLOCKS_PER_SEC * 1000);
        }  
    }    
    cap2.close_device();
}

void ImageProcessor::processOrange(bool front) {
    bool seg_exist_selector;
    if(front){
        frontLock.lock();
        blob_data_front = {};
        frontLock.unlock();
        if(segm.colors[SEG_ORANGE].list!=NULL){
            seg_exist_selector = true;
        } else {
            seg_exist_selector = false;
        }
    } else {
        backLock.lock();
        blob_data_back = {};
        backLock.unlock();
        if(segm2.colors[SEG_ORANGE].list!=NULL){
            seg_exist_selector = true;
        } else {
            seg_exist_selector = false;
        }
    }    
    if(seg_exist_selector){
        int orangeCounter = 0;
        struct region *tempRegion;
        if(front){
            tempRegion = segm.colors[SEG_ORANGE].list;
        } else {
            tempRegion = segm2.colors[SEG_ORANGE].list;
        }
        
        int len_check = tempRegion->y2 - tempRegion->y1;
        if(len_check < MIN_BLOB_SIZE){
            return;
        }
        
        if(front){
            frontLock.lock();
            blob_data_front.o_blob.push_back(orange_blob());
            int len = tempRegion->y2 - tempRegion->y1;
            blob_data_front.o_blob[orangeCounter].orange_w = len;
            blob_data_front.o_blob[orangeCounter].orange_area = tempRegion->area;
            blob_data_front.o_blob[orangeCounter].orange_cen_x = tempRegion->cen_x;
            blob_data_front.o_blob[orangeCounter].orange_cen_y = tempRegion->cen_y;
            frontLock.unlock();
            
        } else {
            backLock.lock();
            blob_data_back.o_blob.push_back(orange_blob());
            int len = tempRegion->y2 - tempRegion->y1;
            blob_data_back.o_blob[orangeCounter].orange_w = len;
            blob_data_back.o_blob[orangeCounter].orange_area = tempRegion->area;
            blob_data_back.o_blob[orangeCounter].orange_cen_x = tempRegion->cen_x;
            blob_data_back.o_blob[orangeCounter].orange_cen_y = tempRegion->cen_y;
            backLock.unlock();
        }
        
                
        while(true){
            if(tempRegion->next!=NULL){
                orangeCounter++;
                tempRegion = tempRegion->next;  
                
                len_check = tempRegion->y2 - tempRegion->y1;
                if(len_check < MIN_BLOB_SIZE){
                    if(front){
                        frontLock.lock();
                        blob_data_front.oranges_processed = orangeCounter+1;
                        frontLock.unlock();
                    } else {
                        backLock.lock();
                        blob_data_back.oranges_processed = orangeCounter+1;
                        backLock.unlock();
                    }
                    break;
                }
                
                
                if(front){
                    frontLock.lock();
                    blob_data_front.o_blob.push_back(orange_blob());
                    int len = tempRegion->y2 - tempRegion->y1;
                    blob_data_front.o_blob[orangeCounter].orange_w = len;
                    blob_data_front.o_blob[orangeCounter].orange_area = tempRegion->area;
                    blob_data_front.o_blob[orangeCounter].orange_cen_x = tempRegion->cen_x;
                    blob_data_front.o_blob[orangeCounter].orange_cen_y = tempRegion->cen_y;
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.o_blob.push_back(orange_blob());
                    int len = tempRegion->y2 - tempRegion->y1;
                    blob_data_back.o_blob[orangeCounter].orange_w = len;
                    blob_data_back.o_blob[orangeCounter].orange_area = tempRegion->area;
                    blob_data_back.o_blob[orangeCounter].orange_cen_x = tempRegion->cen_x;
                    blob_data_back.o_blob[orangeCounter].orange_cen_y = tempRegion->cen_y;
                    backLock.unlock();
                }
            } else {
                if(front){
                    frontLock.lock();
                    blob_data_front.oranges_processed = orangeCounter+1;
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.oranges_processed = orangeCounter+1;
                    backLock.unlock();
                }
                break;
            }
        }        
    }
}

void ImageProcessor::processBlue(bool front) {
    bool seg_exist_selector;
    if(front){
        if(segm.colors[SEG_BLUE].list!=NULL){
            seg_exist_selector = true;
        } else {
            seg_exist_selector = false;
        }
    } else {
        if(segm2.colors[SEG_BLUE].list!=NULL){
            seg_exist_selector = true;
        } else {
            seg_exist_selector = false;
        }
    }
    if(seg_exist_selector){
        int blueCounter = 0;
        struct region *tempRegion;
        if(front){
            tempRegion = segm.colors[SEG_BLUE].list;
        } else {
            tempRegion = segm2.colors[SEG_BLUE].list;
        }
        
        if(tempRegion->area < MIN_BLOB_SIZE){
            return;
        }
        
        if(front){
            frontLock.lock();
            blob_data_front.b_blob.push_back(blue_blob());
            int len = tempRegion->y2 - tempRegion->y1;
            blob_data_front.b_blob[blueCounter].blue_w = len;
            blob_data_front.b_blob[blueCounter].blue_area = tempRegion->area;
            blob_data_front.b_blob[blueCounter].blue_cen_x = tempRegion->cen_x;
            blob_data_front.b_blob[blueCounter].blue_cen_y = tempRegion->cen_y;
            blob_data_front.b_blob[blueCounter].blue_x1 = tempRegion->x1;
            blob_data_front.b_blob[blueCounter].blue_x2 = tempRegion->x2;
            blob_data_front.b_blob[blueCounter].blue_y1 = tempRegion->y1;
            blob_data_front.b_blob[blueCounter].blue_y2 = tempRegion->y2;
            frontLock.unlock();
        } else {
            backLock.lock();
            blob_data_back.b_blob.push_back(blue_blob());
            int len = tempRegion->y2 - tempRegion->y1;
            blob_data_back.b_blob[blueCounter].blue_w = len;
            blob_data_back.b_blob[blueCounter].blue_area = tempRegion->area;
            blob_data_back.b_blob[blueCounter].blue_cen_x = tempRegion->cen_x;
            blob_data_back.b_blob[blueCounter].blue_cen_y = tempRegion->cen_y;
            blob_data_back.b_blob[blueCounter].blue_x1 = tempRegion->x1;
            blob_data_back.b_blob[blueCounter].blue_x2 = tempRegion->x2;
            blob_data_back.b_blob[blueCounter].blue_y1 = tempRegion->y1;
            blob_data_back.b_blob[blueCounter].blue_y2 = tempRegion->y2;
            backLock.unlock();
               
        }
        while(true){
            if(tempRegion->next!=NULL){
                blueCounter++;
                tempRegion = tempRegion->next;
                
                if(tempRegion->area < MIN_BLOB_SIZE){
                    break;
                }
                
                if(front){
                    frontLock.lock();
                    blob_data_front.b_blob.push_back(blue_blob());
                    int len = tempRegion->y2 - tempRegion->y1;
                    blob_data_front.b_blob[blueCounter].blue_w = len;
                    blob_data_front.b_blob[blueCounter].blue_area = tempRegion->area;
                    blob_data_front.b_blob[blueCounter].blue_cen_x = tempRegion->cen_x;
                    blob_data_front.b_blob[blueCounter].blue_cen_y = tempRegion->cen_y;
                    blob_data_front.b_blob[blueCounter].blue_x1 = tempRegion->x1;
                    blob_data_front.b_blob[blueCounter].blue_x2 = tempRegion->x2;
                    blob_data_front.b_blob[blueCounter].blue_y1 = tempRegion->y1;
                    blob_data_front.b_blob[blueCounter].blue_y2 = tempRegion->y2;
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.b_blob.push_back(blue_blob());
                    int len = tempRegion->y2 - tempRegion->y1;
                    blob_data_back.b_blob[blueCounter].blue_w = len;
                    blob_data_back.b_blob[blueCounter].blue_area = tempRegion->area;
                    blob_data_back.b_blob[blueCounter].blue_cen_x = tempRegion->cen_x;
                    blob_data_back.b_blob[blueCounter].blue_cen_y = tempRegion->cen_y;
                    blob_data_back.b_blob[blueCounter].blue_x1 = tempRegion->x1;
                    blob_data_back.b_blob[blueCounter].blue_x2 = tempRegion->x2;
                    blob_data_back.b_blob[blueCounter].blue_y1 = tempRegion->y1;
                    blob_data_back.b_blob[blueCounter].blue_y2 = tempRegion->y2;
                    backLock.unlock();
                }
                                        
            } else {
                if(front){
                    frontLock.lock();
                    blob_data_front.blues_processed = blueCounter+1;
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.blues_processed = blueCounter+1;
                    backLock.unlock();
                }
                break;
            }
        }
    }
}

void ImageProcessor::processYellow(bool front) {
    bool seg_exist_selector;
    if(front){
        if(segm.colors[SEG_YELLOW].list!=NULL){
            seg_exist_selector = true;
        } else {
            seg_exist_selector = false;
        }
    } else {
        if(segm2.colors[SEG_YELLOW].list!=NULL){
            seg_exist_selector = true;
        } else {
            seg_exist_selector = false;
        }
    }
    if(seg_exist_selector){
        int yellowCounter = 0;
        struct region *tempRegion;
        if(front){
            tempRegion = segm.colors[SEG_YELLOW].list;
        } else {
            tempRegion = segm2.colors[SEG_YELLOW].list;
        }
        
        if(tempRegion->area < MIN_BLOB_SIZE){
            return;
        }
        
        if(front){
            frontLock.lock();
            blob_data_front.y_blob.push_back(yellow_blob());
            int len = tempRegion->y2 - tempRegion->y1;
            blob_data_front.y_blob[yellowCounter].yellow_w = len;
            blob_data_front.y_blob[yellowCounter].yellow_area = tempRegion->area;
            blob_data_front.y_blob[yellowCounter].yellow_cen_x = tempRegion->cen_x;
            blob_data_front.y_blob[yellowCounter].yellow_cen_y = tempRegion->cen_y;
            blob_data_front.y_blob[yellowCounter].yellow_x1 = tempRegion->x1;
            blob_data_front.y_blob[yellowCounter].yellow_x2 = tempRegion->x2;
            blob_data_front.y_blob[yellowCounter].yellow_y1 = tempRegion->y1;
            blob_data_front.y_blob[yellowCounter].yellow_y2 = tempRegion->y2;
            frontLock.unlock();
        } else {
            backLock.lock();
            blob_data_back.y_blob.push_back(yellow_blob());
            int len = tempRegion->y2 - tempRegion->y1;
            blob_data_back.y_blob[yellowCounter].yellow_w = len;
            blob_data_back.y_blob[yellowCounter].yellow_area = tempRegion->area;
            blob_data_back.y_blob[yellowCounter].yellow_cen_x = tempRegion->cen_x;
            blob_data_back.y_blob[yellowCounter].yellow_cen_y = tempRegion->cen_y;
            blob_data_back.y_blob[yellowCounter].yellow_x1 = tempRegion->x1;
            blob_data_back.y_blob[yellowCounter].yellow_x2 = tempRegion->x2;
            blob_data_back.y_blob[yellowCounter].yellow_y1 = tempRegion->y1;
            blob_data_back.y_blob[yellowCounter].yellow_y2 = tempRegion->y2;
            backLock.unlock();
        }
        
                
        while(true){
            if(tempRegion->next!=NULL){
                yellowCounter++;
                tempRegion = tempRegion->next;
                
                if(tempRegion->area < MIN_BLOB_SIZE){
                    break;
                }
                
                if(front){
                    frontLock.lock();
                    blob_data_front.y_blob.push_back(yellow_blob());
                    int len = tempRegion->y2 - tempRegion->y1;
                    blob_data_front.y_blob[yellowCounter].yellow_w = len;
                    blob_data_front.y_blob[yellowCounter].yellow_area = tempRegion->area;
                    blob_data_front.y_blob[yellowCounter].yellow_cen_x = tempRegion->cen_x;
                    blob_data_front.y_blob[yellowCounter].yellow_cen_y = tempRegion->cen_y;
                    blob_data_front.y_blob[yellowCounter].yellow_x1 = tempRegion->x1;
                    blob_data_front.y_blob[yellowCounter].yellow_x2 = tempRegion->x2;
                    blob_data_front.y_blob[yellowCounter].yellow_y1 = tempRegion->y1;
                    blob_data_front.y_blob[yellowCounter].yellow_y2 = tempRegion->y2;
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.y_blob.push_back(yellow_blob());
                    int len = tempRegion->y2 - tempRegion->y1;
                    blob_data_back.y_blob[yellowCounter].yellow_w = len;
                    blob_data_back.y_blob[yellowCounter].yellow_area = tempRegion->area;
                    blob_data_back.y_blob[yellowCounter].yellow_cen_x = tempRegion->cen_x;
                    blob_data_back.y_blob[yellowCounter].yellow_cen_y = tempRegion->cen_y;
                    blob_data_back.y_blob[yellowCounter].yellow_x1 = tempRegion->x1;
                    blob_data_back.y_blob[yellowCounter].yellow_x2 = tempRegion->x2;
                    blob_data_back.y_blob[yellowCounter].yellow_y1 = tempRegion->y1;
                    blob_data_back.y_blob[yellowCounter].yellow_y2 = tempRegion->y2;
                    backLock.unlock();
                }
                                        
            } else {
                if(front){
                    frontLock.lock();
                    blob_data_front.yellows_processed = yellowCounter+1;
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.yellows_processed = yellowCounter+1;
                    backLock.unlock();
                }
                break;
            }
        }
    }
}

void ImageProcessor::processGreens(bool front) {
    bool seg_exist_selector;
    if(front){
        if(segm.colors[SEG_GREEN].list!=NULL){
            seg_exist_selector = true;
        } else {
            seg_exist_selector = false;
        }
    } else {
        if(segm2.colors[SEG_GREEN].list!=NULL){
            seg_exist_selector = true;
        } else {
            seg_exist_selector = false;
        }
    }
    if(seg_exist_selector){
        struct region *tempRegion;
        int total_green = 0;
        if(front){
            tempRegion = segm.colors[SEG_GREEN].list;
        } else {
            tempRegion = segm2.colors[SEG_GREEN].list;
        }
        int greens_processed = 0;
        greens_processed++;
        total_green+=tempRegion->area;
        while(true){
            if(tempRegion->next!=NULL){
                tempRegion = tempRegion->next;
                total_green+=tempRegion->area;
                greens_processed++;
            } else {
                if(front){
                    frontLock.lock();
                    blob_data_front.total_green = total_green;
                    blob_data_front.greens_processed = greens_processed;
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.total_green = total_green;
                    blob_data_back.greens_processed = greens_processed;
                    backLock.unlock();
                }                
                break;
            }
        }
    }
}

blobs ImageProcessor::getBlobsFront(){
    //lock_guard<mutex> guard(blobFrontMutex);
    frontLock.lock();
    return blob_data_front;
}

blobs ImageProcessor::getBlobsBack(){
    //lock_guard<mutex> guard(blobBackMutex);
    backLock.lock();    
    return blob_data_back;
}

void ImageProcessor::unlockFront(){
    frontLock.unlock();
}

void ImageProcessor::unlockBack(){
    backLock.unlock();
}

void ImageProcessor::stopProcessor(){
    codeEnd = true;
}

void ImageProcessor::lockBackFrame() {
    backFrameLock.lock();
}


void ImageProcessor::unlockBackFrame(){
    backFrameLock.unlock();
}

void ImageProcessor::lockFrontFrame() {
    frontFrameLock.lock();
}

void ImageProcessor::unlockFrontFrame() {
    frontFrameLock.unlock();
}

uchar* ImageProcessor::getWorkingBackFrame() {
    return this->workingBackFrame;
}


uchar* ImageProcessor::getWorkingBackThresh() {
    return this->workingBackThresh;
}

uchar* ImageProcessor::getWorkingFrontFrame() {
    return this->workingFrontFrame;
}

uchar* ImageProcessor::getWorkingFrontThresh() {
    return this->workingFrontThresh;
}

