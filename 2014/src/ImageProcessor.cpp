/* 
 * File:   ImageProcessor.cpp
 * Author: antti
 * 
 * Created on September 7, 2014, 12:24 PM
 */

#include "ImageProcessor.h"
#include "RobotConstants.h"
#include <cstdlib>
#include <iostream>

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
    readThresholds("conf");
    int gw = cs.width;
    int gh = cs.height;
    start_capturing();
    segm.readThresholds("conf");
    int vx = 640, vy = 480, tx = 1279, ty = 298;
    video = new_window( "Video", vx, vy, gw, gh );
    while(true){
        frame = read_frame();
        if(frame){
            segm.thresholdImage(frame);
            segm.EncodeRuns();
            segm.ConnectComponents();
            segm.ExtractRegions();
            segm.SeparateRegions();
            segm.SortRegions();
            if( strcmp( "YUYV", cs.pix_fmt ) == 0 ) set_working_frame_yuyv( frame, gw, gh);
            fr = get_working_frame();
            
            if(segm.colors[ORANGE].list!=NULL){
                std::cout << " I SEE so so orange" << std::endl;
                struct region *tempRegion;
                tempRegion = segm.colors[ORANGE].list;
                std::cout << "Region area:" << tempRegion->area << " , centrX:" << tempRegion->cen_x << " , centrY: " << tempRegion->cen_y << std::endl;;
            }
            show_video(video, fr);
        }
    }
}
