/* 
 * File:   ImageProcessor.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
    : segm(CAM_W, CAM_H) 
{

}

ImageProcessor::~ImageProcessor() {
}

void ImageProcessor::init() {
    
    IMAGE_CONTEXT *video, *thresh;
    IMAGE_CONTEXT *thresholds;
    CAM_SETTINGS cs;
    SUPPORTED_SETTINGS ss;
    CAMERA_CONTROLS cam_ctl[100];
    char menu_names[32][100];
    int cam_ctl_c = 0;
    int menu_c = 0;
    
    open_device( "/dev/video0" );
    init_device( "/dev/video0" );
    
    uchar **fr;
    uchar *frame = NULL;
    get_camera_settings( &cs );
    get_camera_controls( cam_ctl, menu_names, &cam_ctl_c, &menu_c );
    get_supported_settings( cs, &ss );
    //readThresholds("conf");
    int gw = cs.width;
    int gh = cs.height;
    start_capturing();
    segm.readThresholds("conf");    
    int vx = 640, vy = 480, tx = 1279, ty = 298;
    video = new_window( "Video", vx, vy, gw, gh );
    while(true){
        frame = read_frame();
        
        if(frame){
            segm.processImage( frame );
            
            if( strcmp( "YUYV", cs.pix_fmt ) == 0 ) set_working_frame_yuyv( frame, gw, gh);
            fr = get_working_frame();
            processOrange();
            processBlue();
            processYellow();
            processGreens();
            show_video(video, fr);
        }
    }
}

void ImageProcessor::processOrange() {
    if(segm.colors[SEG_ORANGE].list!=NULL){                
        int orangeCounter = 0;
        struct region *tempRegion;
        tempRegion = segm.colors[SEG_ORANGE].list;
                 
        blob_data.o_blob.push_back(orange_blob());
        blob_data.o_blob[orangeCounter].orange_area = tempRegion->area;
        blob_data.o_blob[orangeCounter].orange_cen_x = tempRegion->cen_x;
        blob_data.o_blob[orangeCounter].orange_cen_y = tempRegion->cen_y;
                
        while(true){
            if(tempRegion->next!=NULL){
                orangeCounter++;
                tempRegion = tempRegion->next;      
                      
                blob_data.o_blob.push_back(orange_blob());
                blob_data.o_blob[orangeCounter].orange_area = tempRegion->area;
                blob_data.o_blob[orangeCounter].orange_cen_x = tempRegion->cen_x;
                blob_data.o_blob[orangeCounter].orange_cen_y = tempRegion->cen_y;
                        
            } else {
                blob_data.oranges_processed = orangeCounter+1;
                break;
            }
        }
    }
}

void ImageProcessor::processBlue() {
    if(segm.colors[SEG_BLUE].list!=NULL){                
        int blueCounter = 0;
        struct region *tempRegion;
        tempRegion = segm.colors[SEG_BLUE].list;
                 
        blob_data.b_blob.push_back(blue_blob());
        blob_data.b_blob[blueCounter].blue_area = tempRegion->area;
        blob_data.b_blob[blueCounter].blue_cen_x = tempRegion->cen_x;
        blob_data.b_blob[blueCounter].blue_cen_y = tempRegion->cen_y;
                
        while(true){
            if(tempRegion->next!=NULL){
                blueCounter++;
                tempRegion = tempRegion->next;      
                      
                blob_data.b_blob.push_back(blue_blob());
                blob_data.b_blob[blueCounter].blue_area = tempRegion->area;
                blob_data.b_blob[blueCounter].blue_cen_x = tempRegion->cen_x;
                blob_data.b_blob[blueCounter].blue_cen_y = tempRegion->cen_y;
                        
            } else {
                blob_data.blues_processed = blueCounter+1;
                break;
            }
        }
    }
}

void ImageProcessor::processYellow() {
    if(segm.colors[SEG_YELLOW].list!=NULL){                
        int yellowCounter = 0;
        struct region *tempRegion;
        tempRegion = segm.colors[SEG_YELLOW].list;
                 
        blob_data.y_blob.push_back(yellow_blob());
        blob_data.y_blob[yellowCounter].yellow_area = tempRegion->area;
        blob_data.y_blob[yellowCounter].yellow_cen_x = tempRegion->cen_x;
        blob_data.y_blob[yellowCounter].yellow_cen_y = tempRegion->cen_y;
                
        while(true){
            if(tempRegion->next!=NULL){
                yellowCounter++;
                tempRegion = tempRegion->next;      
                      
                blob_data.y_blob.push_back(yellow_blob());
                blob_data.y_blob[yellowCounter].yellow_area = tempRegion->area;
                blob_data.y_blob[yellowCounter].yellow_cen_x = tempRegion->cen_x;
                blob_data.y_blob[yellowCounter].yellow_cen_y = tempRegion->cen_y;
                        
            } else {
                blob_data.yellows_processed = yellowCounter+1;
                break;
            }
        }
    }
}


void ImageProcessor::processGreens() {
    if(segm.colors[SEG_GREEN].list!=NULL){
        struct region *tempRegion;
        int total_green = 0;
        tempRegion = segm.colors[SEG_GREEN].list;
        int greens_processed = 0;
        greens_processed++;
        total_green+=tempRegion->area;
        while(true){
            if(tempRegion->next!=NULL){
                tempRegion = tempRegion->next;
                total_green+=tempRegion->area;
                greens_processed++;
            } else {
                blob_data.total_green = total_green;
                blob_data.greens_processed = greens_processed;
                break;
            }
        }
    }
}



