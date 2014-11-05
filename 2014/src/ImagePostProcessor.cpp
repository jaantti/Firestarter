/* 
 * File:   ImagePostProcessor.cpp
 * Author: antti
 * 
 * Created on November 4, 2014, 3:43 PM
 */

#include "ImagePostProcessor.h"

ImagePostProcessor::ImagePostProcessor(ImageProcessor* imageProcessor) {
    iProc = imageProcessor;
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
    
    
}

void ImagePostProcessor::processOrangeBlobs(){
    
    
}

void ImagePostProcessor::processBlueBlobs() {

    
}

void ImagePostProcessor::processYellowBlobs(){
    
}


ImagePostProcessor::~ImagePostProcessor() {
}


