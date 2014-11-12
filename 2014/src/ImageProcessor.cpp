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
    
    switchCameras = chooseCameras();
    if(switchCameras){
        front = "/dev/video0";
        back = "/dev/video1";
    } else {
        front = "/dev/video1";
        back = "/dev/video0";
    }
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
    segm2.readThresholds("conf2");
    
    //If solution = true, means that video0 is the front camera and video1 is the back camera.
    //If solution = false, means that video1 is the front camera and video0 the back.
    bool solution = true;
    
    std::cout << "poop" << std::endl;
    
    while(true){
        frame1 = cap1.read_frame();
        frame2 = cap2.read_frame();
        if(frame1){
            segm.processImage(frame1);            
            if(segm.colors[SEG_ORANGE].list!=NULL){
                solution = true;
                std::cout << "Calibration found ! Video0 in front." << std::endl;
                break;
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
    return solution;
    
}

void ImageProcessor::processFrontCamera(char *cam) {
    uchar *frame = NULL;
    cap1.open_device( cam );
    cap1.init_device( cam );
    cap1.start_capturing();
    std::cout << " Successfully initialized Front camera " << cam << std::endl;
    segm.readThresholds("conf1");
 
    while(!codeEnd){
        frame = cap1.read_frame();
        if(frame){            
            segm.processImage(frame);            
            processOrange(true);
            processBlue(true);
            processYellow(true);
            processGreens(true);
            
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
    segm2.readThresholds("conf2");   
    
    while(!codeEnd){        
        frame = cap2.read_frame();
        if(frame){
            
            
            segm2.processImage(frame);            
            processOrange(false);
            processBlue(false);
            processYellow(false);
            processGreens(false);
            
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
                std::cout << orangeCounter+1 << std::endl;
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
        //std::cout << "I AM PROCESSING BLUE" << front << std::endl;
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
            frontLock.unlock();
        } else {
            backLock.lock();
            blob_data_back.b_blob.push_back(blue_blob());
            int len = tempRegion->y2 - tempRegion->y1;
            blob_data_back.b_blob[blueCounter].blue_w = len;
            blob_data_back.b_blob[blueCounter].blue_area = tempRegion->area;
            blob_data_back.b_blob[blueCounter].blue_cen_x = tempRegion->cen_x;
            blob_data_back.b_blob[blueCounter].blue_cen_y = tempRegion->cen_y;
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
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.b_blob.push_back(blue_blob());
                    int len = tempRegion->y2 - tempRegion->y1;
                    blob_data_back.b_blob[blueCounter].blue_w = len;
                    blob_data_back.b_blob[blueCounter].blue_area = tempRegion->area;
                    blob_data_back.b_blob[blueCounter].blue_cen_x = tempRegion->cen_x;
                    blob_data_back.b_blob[blueCounter].blue_cen_y = tempRegion->cen_y;
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
            frontLock.unlock();
        } else {
            backLock.lock();
            blob_data_back.y_blob.push_back(yellow_blob());
            int len = tempRegion->y2 - tempRegion->y1;
            blob_data_back.y_blob[yellowCounter].yellow_w = len;
            blob_data_back.y_blob[yellowCounter].yellow_area = tempRegion->area;
            blob_data_back.y_blob[yellowCounter].yellow_cen_x = tempRegion->cen_x;
            blob_data_back.y_blob[yellowCounter].yellow_cen_y = tempRegion->cen_y;
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
                    frontLock.unlock();
                } else {
                    backLock.lock();
                    blob_data_back.y_blob.push_back(yellow_blob());
                    int len = tempRegion->y2 - tempRegion->y1;
                    blob_data_back.y_blob[yellowCounter].yellow_w = len;
                    blob_data_back.y_blob[yellowCounter].yellow_area = tempRegion->area;
                    blob_data_back.y_blob[yellowCounter].yellow_cen_x = tempRegion->cen_x;
                    blob_data_back.y_blob[yellowCounter].yellow_cen_y = tempRegion->cen_y;
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
//Legacy image displaying code.
 /*
    //Imshow initialization block.    
    IMAGE_CONTEXT *video, *thresh;
    IMAGE_CONTEXT *thresholds;
    CAM_SETTINGS cs;
    SUPPORTED_SETTINGS ss;
    CAMERA_CONTROLS cam_ctl[100];
    char menu_names[32][100];
    int cam_ctl_c = 0;
    int menu_c = 0;    
    uchar **fr;
    cap2.get_camera_settings( &cs );
    cap2.get_camera_controls( cam_ctl, menu_names, &cam_ctl_c, &menu_c );
    cap2.get_supported_settings( cs, &ss );
    int gw = cs.width;
    int gh = cs.height;
    int vx = 640, vy = 480, tx = 1279, ty = 298;
    //video = cap2.new_window( "Video", vx, vy, gw, gh );
*/

