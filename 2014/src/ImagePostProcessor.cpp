/* 
 * File:   ImagePostProcessor.cpp
 * Author: antti
 * 
 * Created on November 4, 2014, 3:43 PM
 */

#include "capture.h"


#include "ImagePostProcessor.h"
#include "boost/thread.hpp"

//TODO:: Rewrite the system as threaded.
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

    //These will be only avalible in this thread.
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
    	backLock.lock();
    	blob_structure_back.oranges_postprocessed = 0;
    	backLock.unlock();
    	return;
    }
    int count = 0;
    for(int i = 0; i<size; i++){
        orange_blob blob = temp_vector.at(i);
        
        if(orangeFitsInBackList(blob.orange_cen_x, blob.orange_cen_y, blob.orange_w)){
            //Already fits in a defined box, discard by passing over
            continue;
        } else {
            
        	backLock.lock();
            blob_structure_back.o_ball.push_back(orange_ball());
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.orange_w/2;
            int x1, x2, y1, y2;
            x1 = blob.orange_cen_x-half_w*MAGIC;
            x2 = blob.orange_cen_x+half_w*MAGIC;
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
            backLock.unlock();
            count++;            
        }
    }
    backLock.lock();
    blob_structure_back.oranges_postprocessed = count;
    backLock.unlock();
}

void ImagePostProcessor::processOrangeBlobsFront(){
    std::vector<orange_blob> temp_vector = blob_container_front.o_blob;
    int size = temp_vector.size();
    if(size==0){
    	frontLock.lock();
        blob_structure_front.oranges_postprocessed = 0;
        frontLock.unlock();
        return;
    }
    //std::cout << "BALLS BEFORE : " << size << std::endl;
    int count = 0;
    for(int i = 0; i<size; i++){
        orange_blob blob = temp_vector.at(i);
        
        if(orangeFitsInFrontList(blob.orange_cen_x, blob.orange_cen_y, blob.orange_w)){
            //Already fits in a defined box, discard by passing over
            continue;
        } else {
            frontLock.lock();
            blob_structure_front.o_ball.push_back(orange_ball());
            
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.orange_w/2;
            int x1, x2, y1, y2;
            x1 = blob.orange_cen_x-half_w*MAGIC;
            x2 = blob.orange_cen_x+half_w*MAGIC;
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
            frontLock.unlock();
            count++;            
        }
    }
    frontLock.lock();
    blob_structure_front.oranges_postprocessed = count;
    frontLock.unlock();
    
}

void ImagePostProcessor::processBlueBlobsFront() {
    //Create temporary datastore for processing purposes
    std::vector<blue_blob> temp_vector = blob_container_front.b_blob;
    int size = temp_vector.size();
    
    if(size==0){
    	frontLock.lock();
    	blob_structure_front.blues_postprocessed = 0;
    	frontLock.unlock();
    	return;
    }

    // Initialize first gate vector, all other blobs will be placed according to
    // this one.
    int count = 0;
    blue_blob init = temp_vector.at(count);
    
    int x1, x2, y1, y2;
    
    //Calculate the x1, x2, y1, y2 based on centerpoint and width
    int half_w = init.blue_w/2;

    x1 = init.blue_cen_x-half_w;
    x2 = init.blue_cen_x+half_w;
    y1 = init.blue_cen_y-half_w;
    y2 = init.blue_cen_y+half_w;
    
    frontLock.lock();
    blob_structure_front.b_gate.push_back(blue_gate());
    frontLock.unlock();
    
    //Initialize the new blue gate.
    blue_gate gate = {};
    
    gate.blue_cen_x = init.blue_cen_x;
    gate.blue_cen_y = init.blue_cen_y;
    gate.blue_h = init.blue_w;
    gate.blue_x1 = x1;
    gate.blue_x2 = x2;
    gate.blue_y1 = y1;
    gate.blue_y2 = y2;
    
    
    // Iterate over the remaining blobs, placing them in the gate structure or discarding them if they do not qualify.
    for(int i = 1; i<size; i++){

        blue_blob blob = temp_vector.at(i);
        if(blob.blue_w < (gate.blue_h*0.33)){
        	break;
        }

        gate = mergeBlueGateStructure(gate, blob);
    }

    gate.blue_w = gate.blue_x2 - gate.blue_x1;
    gate.blue_cen_x = gate.blue_x2 - gate.blue_w/2;

    frontLock.lock();
    blob_structure_front.b_gate[0] = gate;
    blob_structure_front.blues_postprocessed = count;
    frontLock.unlock();
}

void ImagePostProcessor::processBlueBlobsBack() {
	//Create temporary datastore for processing purposes
	std::vector<blue_blob> temp_vector = blob_container_back.b_blob;
	int size = temp_vector.size();

	if(size==0){
		backLock.lock();
		blob_structure_back.blues_postprocessed = 0;
		backLock.unlock();
	    return;
	}

	// Initialize first gate vector, all other blobs will be placed according to
	// this one.
	int count = 0;
	blue_blob init = temp_vector.at(count);

	int x1, x2, y1, y2;

	    //Calculate the x1, x2, y1, y2 based on centerpoint and width
	int half_w = init.blue_w/2;

	x1 = init.blue_cen_x-half_w;
	x2 = init.blue_cen_x+half_w;
	y1 = init.blue_cen_y-half_w;
	y2 = init.blue_cen_y+half_w;

	frontLock.lock();
	blob_structure_back.b_gate.push_back(blue_gate());
	frontLock.unlock();

	//Initialize the new blue gate.
	blue_gate gate = {};

	gate.blue_cen_x = init.blue_cen_x;
	gate.blue_cen_y = init.blue_cen_y;
	gate.blue_h = init.blue_w;
	gate.blue_x1 = x1;
	gate.blue_x2 = x2;
	gate.blue_y1 = y1;
	gate.blue_y2 = y2;


	// Iterate over the remaining blobs, placing them in the gate structure or discarding them if they do not qualify.
	for(int i = 1; i<size; i++){

		blue_blob blob = temp_vector.at(i);
	    if(blob.blue_w < (gate.blue_h*0.33)){
	    	break;
	    }

	gate = mergeBlueGateStructure(gate, blob);
	}

	gate.blue_w = gate.blue_x2 - gate.blue_x1;
	gate.blue_cen_x = gate.blue_x2 - gate.blue_w/2;

	backLock.lock();
	blob_structure_back.b_gate[0] = gate;
	blob_structure_back.blues_postprocessed = count;
	backLock.unlock();
}

void ImagePostProcessor::processYellowBlobsFront(){
	//Create temporary datastore for processing purposes
		std::vector<yellow_blob> temp_vector = blob_container_front.y_blob;
		int size = temp_vector.size();

		if(size==0){
			frontLock.lock();
			blob_structure_front.yellows_postprocessed = 0;
			frontLock.unlock();
		    return;
		}

		// Initialize first gate vector, all other blobs will be placed according to
		// this one.
		int count = 0;
		yellow_blob init = temp_vector.at(count);

		int x1, x2, y1, y2;

		    //Calculate the x1, x2, y1, y2 based on centerpoint and width
		int half_w = init.yellow_w/2;

		x1 = init.yellow_cen_x-half_w;
		x2 = init.yellow_cen_x+half_w;
		y1 = init.yellow_cen_y-half_w;
		y2 = init.yellow_cen_y+half_w;

		frontLock.lock();
		blob_structure_front.y_gate.push_back(yellow_gate());
		frontLock.unlock();

		//Initialize the new blue gate.
		yellow_gate gate = {};

		gate.yellow_cen_x = init.yellow_cen_x;
		gate.yellow_cen_y = init.yellow_cen_y;
		gate.yellow_h = init.yellow_w;
		gate.yellow_x1 = x1;
		gate.yellow_x2 = x2;
		gate.yellow_y1 = y1;
		gate.yellow_y2 = y2;


		// Iterate over the remaining blobs, placing them in the gate structure or discarding them if they do not qualify.
		for(int i = 1; i<size; i++){

			yellow_blob blob = temp_vector.at(i);
		    if(blob.yellow_w < (gate.yellow_h*0.33)){
		    	break;
		    }

		gate = mergeYellowGateStructure(gate, blob);
		}

		gate.yellow_w = gate.yellow_x2 - gate.yellow_x1;
		gate.yellow_cen_x = gate.yellow_x2 - gate.yellow_w/2;

		frontLock.lock();
		blob_structure_front.y_gate[0] = gate;
		blob_structure_front.yellows_postprocessed = count;
		frontLock.unlock();
}

void ImagePostProcessor::processYellowBlobsBack(){
	//Create temporary datastore for processing purposes
		std::vector<yellow_blob> temp_vector = blob_container_back.y_blob;
		int size = temp_vector.size();

		if(size==0){
			backLock.lock();
			blob_structure_back.yellows_postprocessed = 0;
			backLock.unlock();
		    return;
		}

		// Initialize first gate vector, all other blobs will be placed according to
		// this one.
		int count = 0;
		yellow_blob init = temp_vector.at(count);

		int x1, x2, y1, y2;

		    //Calculate the x1, x2, y1, y2 based on centerpoint and width
		int half_w = init.yellow_w/2;

		x1 = init.yellow_cen_x-half_w;
		x2 = init.yellow_cen_x+half_w;
		y1 = init.yellow_cen_y-half_w;
		y2 = init.yellow_cen_y+half_w;

		backLock.lock();
		blob_structure_back.y_gate.push_back(yellow_gate());
		backLock.unlock();

		//Initialize the new blue gate.
		yellow_gate gate = {};

		gate.yellow_cen_x = init.yellow_cen_x;
		gate.yellow_cen_y = init.yellow_cen_y;
		gate.yellow_h = init.yellow_w;
		gate.yellow_x1 = x1;
		gate.yellow_x2 = x2;
		gate.yellow_y1 = y1;
		gate.yellow_y2 = y2;


		// Iterate over the remaining blobs, placing them in the gate structure or discarding them if they do not qualify.
		for(int i = 1; i<size; i++){

			yellow_blob blob = temp_vector.at(i);
		    if(blob.yellow_w < (gate.yellow_h*0.33)){
		    	break;
		    }

		gate = mergeYellowGateStructure(gate, blob);
		}

		gate.yellow_w = gate.yellow_x2 - gate.yellow_x1;
		gate.yellow_cen_x = gate.yellow_x2 - gate.yellow_w/2;

		backLock.lock();
		blob_structure_back.y_gate[0] = gate;
		blob_structure_back.yellows_postprocessed = count;
		backLock.unlock();
}

//These processes iterate over the existing processed list
// and find if the given blob data fits in one of the pre-defined spaces.

bool ImagePostProcessor::orangeFitsInFrontList(int x, int y, int w) {
	frontLock.lock();
	std::vector<orange_ball> ball_vector = blob_structure_front.o_ball;
	frontLock.unlock();

	int size = ball_vector.size();
    if(size==0){
        return false;
    }    

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
    
    return false;
}

bool ImagePostProcessor::orangeFitsInBackList(int x, int y, int w) {
	backLock.lock();
    std::vector<orange_ball> ball_vector = blob_structure_back.o_ball;
    backLock.unlock();

    int size = ball_vector.size();
    if(size==0){
        return false;
    }

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

//Merger logic : take the width of the first blob on y-scale, fit all the blue blobs that have their y-center
// situated between them into the new blob, while increasing x1 and x2 accordingly.
blue_gate ImagePostProcessor::mergeBlueGateStructure(blue_gate gate, blue_blob merger){

	int x1, x2, newx, y1, y2, newy;
	x1 = gate.blue_x1;
	x2 = gate.blue_x2;
	y1 = gate.blue_y1;
	y2 = gate.blue_y2;


	newx = merger.blue_cen_x;
	newy = merger.blue_cen_y;

	// If the new blob is outside the already existing x1-x2, and inside the existing y-zone, expand the existing gate.
	if( (newx < x1 || newx > x2) && (newy > y1 && newy < y2) ){
		gate = expandBlueGate(gate, merger);
	}

	return gate;
}

yellow_gate ImagePostProcessor::mergeYellowGateStructure(yellow_gate gate, yellow_blob merger){
	if(merger.yellow_w < (gate.yellow_h*0.33)){
			return gate;
		}
	int x1, x2, newx, y1, y2, newy;
	x1 = gate.yellow_x1;
	x2 = gate.yellow_x2;
	y1 = gate.yellow_y1;
	y2 = gate.yellow_y2;


	newx = merger.yellow_cen_x;
	newy = merger.yellow_cen_y;

	// If the new blob is outside the already existing x1-x2, and inside the existing y-zone, expand the existing gate.
	if( (newx < x1 || newx > x2) && (newy > y1 && newy < y2) ){
		gate = expandYellowGate(gate, merger);
	}

	return gate;
}

// Expander logic for both colors
blue_gate ImagePostProcessor::expandBlueGate(blue_gate gate, blue_blob expander){
	if(gate.blue_x2 < expander.blue_cen_x){
		gate.blue_x2 = expander.blue_cen_x;
	} else if(gate.blue_x1 > expander.blue_cen_x){
		gate.blue_x1 = expander.blue_cen_x;
	}
	return gate;
}

yellow_gate ImagePostProcessor::expandYellowGate(yellow_gate gate, yellow_blob expander){
	if(gate.yellow_x2 < expander.yellow_cen_x){
			gate.yellow_x2 = expander.yellow_cen_x;
		} else if(gate.yellow_x1 > expander.yellow_cen_x){
			gate.yellow_x1 = expander.yellow_cen_x;
		}
		return gate;

}

blobs_processed ImagePostProcessor::getBackSystem(){
	backLock.lock();
	return blob_structure_back;
}

blobs_processed ImagePostProcessor::getFrontSystem(){
	frontLock.lock();
	return blob_structure_front;
}

blue_gate ImagePostProcessor::getFrontBlue(){
	frontLock.lock();
	blue_gate blueGate = {};
	if(blob_structure_front.blues_postprocessed>0){
		blueGate = blob_structure_front.b_gate.at(0);
	}
	frontLock.unlock();
	return blueGate;
}

blue_gate ImagePostProcessor::getBackBlue(){
	backLock.lock();
	blue_gate blueGate= {};
	if(blob_structure_back.blues_postprocessed>0){
		blueGate = blob_structure_back.b_gate.at(0);
	}
	backLock.unlock();
	return blueGate;
}

yellow_gate ImagePostProcessor::getFrontYellow(){
	frontLock.lock();
	yellow_gate yellowGate = {};
	if(blob_structure_front.yellows_postprocessed>0){
		yellowGate = blob_structure_front.y_gate.at(0);
	}
	frontLock.unlock();
	return yellowGate;
}

yellow_gate ImagePostProcessor::getBackYellow(){
	backLock.lock();
	yellow_gate yellowGate = {};
	if(blob_structure_back.yellows_postprocessed>0){
		yellowGate = blob_structure_back.y_gate.at(0);
	}
	backLock.unlock();
	return yellowGate;
}

void ImagePostProcessor::lockBackSystem(){
	backLock.lock();
}

void ImagePostProcessor::lockFrontSystem(){
	frontLock.lock();
}

void ImagePostProcessor::unlockBackSystem(){
	backLock.unlock();
}

void ImagePostProcessor::unlockFrontSystem(){
	frontLock.unlock();
}
