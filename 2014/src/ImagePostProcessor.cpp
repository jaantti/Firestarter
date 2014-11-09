/* 
 * File:   ImagePostProcessor.cpp
 * Author: antti
 * 
 * Created on November 4, 2014, 3:43 PM
 */

#include "capture.h"


#include "ImagePostProcessor.h"

ImagePostProcessor::ImagePostProcessor(){
    
}

ImagePostProcessor::ImagePostProcessor(ImageProcessor* imageProcessor) {
    iProc = imageProcessor;
}


ImagePostProcessor::~ImagePostProcessor() {
}
void ImagePostProcessor::run(){
    loadBlobVectors();
    processBlobVectors();
}

void ImagePostProcessor::loadBlobVectors() {
    blob_structure_front = {};
    blob_structure_back = {};
    blob_container_front = {};
    blob_container_back = {};
    
    blob_container_front = iProc->getBlobsFront();
    iProc->unlockFront();
    blob_container_back = iProc->getBlobsBack();
    iProc->unlockBack();
            
}

void ImagePostProcessor::processBlobVectors() {
    processOrangeBlobsBack();
    processOrangeBlobsFront();
    processBlueBlobsFront();
    processBlueBlobsBack();
    processYellowBlobsFront();
    processYellowBlobsBack();
    
}

//The following methods process the lists of all colors from both cameras
//and populate the data structures with new data.
void ImagePostProcessor::processOrangeBlobsBack(){
    std::vector<orange_blob> temp_vector = blob_container_back.o_blob;
    int size = temp_vector.size();
    if(size==0){
       blob_structure_back.oranges_postprocessed = 0;
       return;
    }
    std::cout << "BALLS BEFORE : " << size << std::endl;
    int count = 0;
    for(int i = 0; i<size; i++){
        orange_blob blob = temp_vector.at(i);
        //std::cout << "Ball in judgement : X:" << blob.orange_cen_x << " ; Y:" << blob.orange_cen_y << " ; W:" << blob.orange_w << std::endl;
        
        if(orangeFitsInBackList(blob.orange_cen_x, blob.orange_cen_y, blob.orange_w)){
            //Already fits in a defined box, discard by passing over
            continue;
        } else {
            //std::cout << " Ball passed judgement." << std::endl;
            
            blob_structure_back.o_ball.push_back(orange_ball());
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.orange_w/2;
            int x1, x2, y1, y2;
            x1 = blob.orange_cen_x-half_w*1.15;
            x2 = blob.orange_cen_x+half_w*1.15;
            y1 = blob.orange_cen_y-half_w;
            y2 = blob.orange_cen_y+half_w;
            
            blob_structure_back.o_ball[count].orange_cen_x = blob.orange_cen_x;
            blob_structure_back.o_ball[count].orange_cen_y = blob.orange_cen_y;
            blob_structure_back.o_ball[count].orange_w = blob.orange_w*1.15;
            blob_structure_back.o_ball[count].orange_h = blob.orange_w;
            blob_structure_back.o_ball[count].orange_x1 = x1;
            blob_structure_back.o_ball[count].orange_x2 = x2;
            blob_structure_back.o_ball[count].orange_y1 = y1;
            blob_structure_back.o_ball[count].orange_y2 = y2;
            count++;            
        }
    }
    blob_structure_back.oranges_postprocessed = count;
    /*
    std::cout << " BALLS AFTER :" << count << std::endl;
    for(int i=0; i<count; i++){
        orange_ball ball = blob_structure_back.o_ball.at(i);
        
        std::cout << "BALL COORDS : (" << ball.orange_cen_x << " ; " << ball.orange_cen_y << ")" << std::endl;
        std::cout << "BALL WID :" << ball.orange_w << std::endl;
    }
     */
}

void ImagePostProcessor::processOrangeBlobsFront(){
    std::vector<orange_blob> temp_vector = blob_container_front.o_blob;
    int size = temp_vector.size();
    if(size==0){
       blob_structure_front.oranges_postprocessed = 0;
       return;
    }
    //std::cout << "BALLS BEFORE : " << size << std::endl;
    int count = 0;
    for(int i = 0; i<size; i++){
        orange_blob blob = temp_vector.at(i);
        //std::cout << "Ball in judgement : X:" << blob.orange_cen_x << " ; Y:" << blob.orange_cen_y << " ; W:" << blob.orange_w << std::endl;
        
        if(orangeFitsInFrontList(blob.orange_cen_x, blob.orange_cen_y, blob.orange_w)){
            //Already fits in a defined box, discard by passing over
            continue;
        } else {
            //std::cout << " Ball passed judgement." << std::endl;
            
            blob_structure_front.o_ball.push_back(orange_ball());
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.orange_w/2;
            int x1, x2, y1, y2;
            x1 = blob.orange_cen_x-half_w*1.15;
            x2 = blob.orange_cen_x+half_w*1.15;
            y1 = blob.orange_cen_y-half_w;
            y2 = blob.orange_cen_y+half_w;
            
            blob_structure_front.o_ball[count].orange_cen_x = blob.orange_cen_x;
            blob_structure_front.o_ball[count].orange_cen_y = blob.orange_cen_y;
            blob_structure_front.o_ball[count].orange_w = blob.orange_w*1.15;
            blob_structure_front.o_ball[count].orange_h = blob.orange_w;
            blob_structure_front.o_ball[count].orange_x1 = x1;
            blob_structure_front.o_ball[count].orange_x2 = x2;
            blob_structure_front.o_ball[count].orange_y1 = y1;
            blob_structure_front.o_ball[count].orange_y2 = y2;
            count++;            
        }
    }
    blob_structure_front.oranges_postprocessed = count;
    //std::cout << " BALLS AFTER :" << count << std::endl;
    /*
    
    for(int i=0; i<count; i++){
        orange_ball ball = blob_structure_front.o_ball.at(i);
        
        std::cout << "BALL COORDS : (" << ball.orange_cen_x << " ; " << ball.orange_cen_y << ")" << std::endl;
        std::cout << "BALL WID :" << ball.orange_w << std::endl;
    }
    */
}


void ImagePostProcessor::processBlueBlobsFront() {
    std::vector<blue_blob> temp_vector = blob_container_front.b_blob;
    int size = temp_vector.size();
    if(size==0){
       blob_structure_front.blues_postprocessed = 0;
       return;
    }
    int count = 0;
    for(int i = 0; i<size; i++){
        blue_blob blob = temp_vector.at(i);
        if(blueFitsInFrontList(blob.blue_cen_x, blob.blue_cen_y, blob.blue_w)){
            //Already exists, discard by passing over
            continue;
        } else {
            blob_structure_front.b_gate.push_back(blue_gate());
            
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.blue_w/2;
            int x1, x2, y1, y2;
            x1 = blob.blue_cen_x-half_w;
            x2 = blob.blue_cen_x+half_w;
            y1 = blob.blue_cen_y-half_w;
            y2 = blob.blue_cen_y+half_w;

            blob_structure_front.b_gate[count].blue_cen_x = blob.blue_cen_x;
            blob_structure_front.b_gate[count].blue_cen_y = blob.blue_cen_y;
            blob_structure_front.b_gate[count].blue_w = blob.blue_w;
            blob_structure_front.b_gate[count].blue_h = blob.blue_w;
            blob_structure_front.b_gate[count].blue_x1 = x1;
            blob_structure_front.b_gate[count].blue_x2 = x2;
            blob_structure_front.b_gate[count].blue_y1 = y1;
            blob_structure_front.b_gate[count].blue_y2 = x2;
            count++;            
        }
    }
    blob_structure_front.blues_postprocessed = count;
}


void ImagePostProcessor::processBlueBlobsBack() {
std::vector<blue_blob> temp_vector = blob_container_back.b_blob;
    int size = temp_vector.size();
    if(size==0){
       blob_structure_back.blues_postprocessed = 0;
       return;
    }
    int count = 0;
    for(int i = 0; i<size; i++){
        blue_blob blob = temp_vector.at(i);
        if(blueFitsInFrontList(blob.blue_cen_x, blob.blue_cen_y, blob.blue_w)){
            //Already exists, discard by passing over
            continue;
        } else {
            blob_structure_back.b_gate.push_back(blue_gate());
            
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.blue_w/2;
            int x1, x2, y1, y2;
            x1 = blob.blue_cen_x-half_w;
            x2 = blob.blue_cen_x+half_w;
            y1 = blob.blue_cen_y-half_w;
            y2 = blob.blue_cen_y+half_w;

            blob_structure_back.b_gate[count].blue_cen_x = blob.blue_cen_x;
            blob_structure_back.b_gate[count].blue_cen_y = blob.blue_cen_y;
            blob_structure_back.b_gate[count].blue_w = blob.blue_w;
            blob_structure_back.b_gate[count].blue_h = blob.blue_w;
            blob_structure_back.b_gate[count].blue_x1 = x1;
            blob_structure_back.b_gate[count].blue_x2 = x2;
            blob_structure_back.b_gate[count].blue_y1 = y1;
            blob_structure_back.b_gate[count].blue_y2 = x2;
            count++;            
        }
    }
    blob_structure_back.blues_postprocessed = count;
}


void ImagePostProcessor::processYellowBlobsFront(){
    std::vector<yellow_blob> temp_vector = blob_container_front.y_blob;
    int size = temp_vector.size();
    if(size==0){
       blob_structure_front.yellows_postprocessed = 0;
       return;
    }
    int count = 0;
    for(int i = 0; i<size; i++){
        yellow_blob blob = temp_vector.at(i);
        if(blueFitsInFrontList(blob.yellow_cen_x, blob.yellow_cen_y, blob.yellow_w)){
            //Already exists, discard by passing over
            continue;
        } else {
            blob_structure_front.y_gate.push_back(yellow_gate());
            
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.yellow_w/2;
            int x1, x2, y1, y2;
            x1 = blob.yellow_cen_x-half_w;
            x2 = blob.yellow_cen_x+half_w;
            y1 = blob.yellow_cen_y-half_w;
            y2 = blob.yellow_cen_y+half_w;

            blob_structure_front.y_gate[count].yellow_cen_x = blob.yellow_cen_x;
            blob_structure_front.y_gate[count].yellow_cen_y = blob.yellow_cen_y;
            blob_structure_front.y_gate[count].yellow_w = blob.yellow_w;
            blob_structure_front.y_gate[count].yellow_h = blob.yellow_w;
            blob_structure_front.y_gate[count].yellow_x1 = x1;
            blob_structure_front.y_gate[count].yellow_x2 = x2;
            blob_structure_front.y_gate[count].yellow_y1 = y1;
            blob_structure_front.y_gate[count].yellow_y2 = x2;
            count++;            
        }
    }
    blob_structure_front.yellows_postprocessed = count;
}


void ImagePostProcessor::processYellowBlobsBack(){
    std::vector<yellow_blob> temp_vector = blob_container_back.y_blob;
    int size = temp_vector.size();
    if(size==0){
       blob_structure_back.yellows_postprocessed = 0;
       return;
    }
    int count = 0;
    for(int i = 0; i<size; i++){
        yellow_blob blob = temp_vector.at(i);
        if(blueFitsInBackList(blob.yellow_cen_x, blob.yellow_cen_y, blob.yellow_w)){
            //Already exists, discard by passing over
            continue;
        } else {
            blob_structure_back.y_gate.push_back(yellow_gate());
            
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.yellow_w/2;
            int x1, x2, y1, y2;
            x1 = blob.yellow_cen_x-half_w;
            x2 = blob.yellow_cen_x+half_w;
            y1 = blob.yellow_cen_y-half_w;
            y2 = blob.yellow_cen_y+half_w;

            blob_structure_back.y_gate[count].yellow_cen_x = blob.yellow_cen_x;
            blob_structure_back.y_gate[count].yellow_cen_y = blob.yellow_cen_y;
            blob_structure_back.y_gate[count].yellow_w = blob.yellow_w;
            blob_structure_back.y_gate[count].yellow_h = blob.yellow_w;
            blob_structure_back.y_gate[count].yellow_x1 = x1;
            blob_structure_back.y_gate[count].yellow_x2 = x2;
            blob_structure_back.y_gate[count].yellow_y1 = y1;
            blob_structure_back.y_gate[count].yellow_y2 = x2;
            count++;            
        }
    }
    blob_structure_back.yellows_postprocessed = count;
}

//These processes iterate over the existing processed list
// and find if the given blob data fits in one of the pre-defined spaces.

bool ImagePostProcessor::orangeFitsInFrontList(int x, int y, int w) {
    int size = blob_structure_front.o_ball.size();
    if(size==0){
        return false;
    }    
    std::vector<orange_ball> ball_vector = blob_structure_front.o_ball;
    int j = 0;
    for(int i=0; i<size; i++){
        orange_ball ball = ball_vector.at(i);
        bool check1 = x>=ball.orange_x1 && x<=ball.orange_x2;
        bool check2 = y>=ball.orange_y1 && y<=ball.orange_y2;
        if(check1 && check2){
            return true;
        }
        j=i;
    }
    /*
    std::cout << " BALL PASSED." <<std::endl;
    for(int i=0; i<size; i++){
        orange_ball ballpass = ball_vector.at(i);
        std::cout  << " COMPARED AGAINST : (X1, X2, Y1, Y2, W) : " << ballpass.orange_x1 << " ; " << ballpass.orange_x2 << " ; " << ballpass.orange_y1 << " ; " << ballpass.orange_y2 << " ; " << ballpass.orange_w << std::endl;
    }
    //int fr;
    //std::cin >> fr;
    */    
    
    return false;
}

bool ImagePostProcessor::orangeFitsInBackList(int x, int y, int w) {
    int size = blob_structure_back.o_ball.size();
    if(size==0){
        return false;
    }
    std::vector<orange_ball> ball_vector = blob_structure_back.o_ball;
    for(int i=0; i<size; i++){
        orange_ball ball = ball_vector.at(i);
        bool check1 = x>=ball.orange_x1 && x<=ball.orange_x2;
        bool check2 = y>=ball.orange_y1 && y<=ball.orange_y2;
        if(check1 && check2){
            return true;
        }
        
    }
    
    return false;
}

bool ImagePostProcessor::blueFitsInFrontList(int x, int y, int w) {
    int size = blob_structure_front.b_gate.size();
    if(size==0){
        return false;
    }
    std::vector<blue_gate> gate_vector = blob_structure_front.b_gate;
    for(int i=0; i<size; i++){
        blue_gate gate = gate_vector.at(i);
        bool check1 = x>=gate.blue_x1 && x<=gate.blue_x2;
        bool check2 = y>=gate.blue_y1 && y<=gate.blue_y2;
        if(check1 && check2){
            return true;
        }
        
    }
    
    return false;
}

bool ImagePostProcessor::blueFitsInBackList(int x, int y, int w) {
    int size = blob_structure_back.b_gate.size();
    if(size==0){
        return false;
    }
    std::vector<blue_gate> gate_vector = blob_structure_back.b_gate;
    for(int i=0; i<size; i++){
        blue_gate gate =gate_vector.at(i);
        bool check1 = x>=gate.blue_x1 && x<=gate.blue_x2;
        bool check2 = y>=gate.blue_y1 && y<=gate.blue_y2;
        if(check1 && check2){
            return true;
        }
        
    }
    
    return false;
}

bool ImagePostProcessor::yellowFitsInFrontList(int x, int y, int w) {
    int size = blob_structure_front.y_gate.size();
    if(size==0){
        return false;
    }
    std::vector<yellow_gate> gate_vector = blob_structure_front.y_gate;
    for(int i=0; i<size; i++){
        yellow_gate gate = gate_vector.at(i);
        bool check1 = x>=gate.yellow_x1 && x<=gate.yellow_x2;
        bool check2 = y>=gate.yellow_y1 && y<=gate.yellow_y2;
        if(check1 && check2){
            return true;
        }
        
    }
    
    return false;
}

bool ImagePostProcessor::yellowFitsInBackList(int x, int y, int w) {
    int size = blob_structure_back.y_gate.size();
    if(size==0){
        return false;
    }
    std::vector<yellow_gate> gate_vector = blob_structure_back.y_gate;
    for(int i=0; i<size; i++){
        yellow_gate gate = gate_vector.at(i);
        bool check1 = x>=gate.yellow_x1 && x<=gate.yellow_x2;
        bool check2 = y>=gate.yellow_y1 && y<=gate.yellow_y2;
        if(check1 && check2){
            return true;
        }
        
    }
    
    return false;
}




