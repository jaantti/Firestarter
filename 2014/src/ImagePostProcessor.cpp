/* 
 * File:   ImagePostProcessor.cpp
 * Author: antti
 * 
 * Created on November 4, 2014, 3:43 PM
 */

#include "capture.h"


#include "ImagePostProcessor.h"

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
       blob_structure_back.oranges_postprocessed = 0;
       return;
    }
    //std::cout << "BALLS BEFORE : " << size << std::endl;
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
    //Create temporary datastore for processing purposes
    std::vector<blue_blob> temp_vector = blob_container_front.b_blob;
    int size = temp_vector.size();
    
    if(size==0){
       blob_structure_front.blues_postprocessed = 0;
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
    

    blob_structure_front.b_gate.push_back(blue_gate());
    
    
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

    blob_structure_front.b_gate[0] = gate;
    blob_structure_front.blues_postprocessed = count;
}

void ImagePostProcessor::processBlueBlobsBack() {
	//Create temporary datastore for processing purposes
	std::vector<blue_blob> temp_vector = blob_container_back.b_blob;
	int size = temp_vector.size();

	if(size==0){
		blob_structure_back.blues_postprocessed = 0;
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


	blob_structure_front.b_gate.push_back(blue_gate());


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

	blob_structure_back.b_gate[0] = gate;
	blob_structure_back.blues_postprocessed = count;
}

void ImagePostProcessor::processYellowBlobsFront(){
	//Create temporary datastore for processing purposes
		std::vector<yellow_blob> temp_vector = blob_container_front.y_blob;
		int size = temp_vector.size();

		if(size==0){
			blob_structure_front.yellows_postprocessed = 0;
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


		blob_structure_front.y_gate.push_back(yellow_gate());


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

		blob_structure_front.y_gate[0] = gate;
		blob_structure_front.yellows_postprocessed = count;
}

void ImagePostProcessor::processYellowBlobsBack(){
	//Create temporary datastore for processing purposes
		std::vector<yellow_blob> temp_vector = blob_container_back.y_blob;
		int size = temp_vector.size();

		if(size==0){
			blob_structure_back.yellows_postprocessed = 0;
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


		blob_structure_back.y_gate.push_back(yellow_gate());


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

		blob_structure_back.y_gate[0] = gate;
		blob_structure_back.yellows_postprocessed = count;
}

//These processes iterate over the existing processed list
// and find if the given blob data fits in one of the pre-defined spaces.

bool ImagePostProcessor::orangeFitsInFrontList(int x, int y, int w) {
	std::vector<orange_ball> ball_vector = blob_structure_front.o_ball;

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
    std::vector<orange_ball> ball_vector = blob_structure_back.o_ball;

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
