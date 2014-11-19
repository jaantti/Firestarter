/* 
 * File:   ImagePostProcessor.cpp
 * Author: antti
 * 
 * Created on November 4, 2014, 3:43 PM
 */

#include "ImagePostProcessor.h"

ImagePostProcessor::ImagePostProcessor(ImageProcessor* imageProcessor):calculator(NULL)
{
    calculator = new BlobDistanceCalculator(this);
    std::cout << " I AM THE CONSTRUCTOR " << std::endl;
    iProc = imageProcessor;
    
    backLock.lock();
    blob_structure_back = {};
    backLock.unlock();
    
    frontLock.lock();
    blob_structure_front = {};
    frontLock.unlock();
}


ImagePostProcessor::~ImagePostProcessor() {
    delete this->calculator;
}


void ImagePostProcessor::run(){
    std::cout << " Starting ImagePostProcessor." << std::endl;
    while(!codeEnd){
        loadBlobVectors();
        processBlobVectors();
        calculator->run();
        
        usleep(15000);
        
    }
}


void ImagePostProcessor::stopProcessor() {
    codeEnd = true;
}


void ImagePostProcessor::loadBlobVectors() {

    temp_holder_back = {};
    
    temp_holder_front = {};
        
    //These will be only avalible in this thread.
    blob_container_front = {};

    blob_container_back = {};
    
    blob_container_front = iProc->getBlobsFront();
    blob_container_back = iProc->getBlobsBack();
       
    backLock.lock();
    blob_structure_back.total_green = blob_container_back.total_green;
    backLock.unlock();
    
    frontLock.try_lock();
    
    //frontLock.lock();
    blob_structure_front.total_green = blob_container_front.total_green;
    frontLock.unlock();
}

void ImagePostProcessor::processBlobVectors() {
    processOrangeBlobsBack();
    processOrangeBlobsFront();
    processBlueBlobsFront();
    processBlueBlobsBack();
    processYellowBlobsFront();
    processYellowBlobsBack();
    runGateDecisionSystem();
    
    
}

//The following methods process the lists of all colors from both cameras
//and populate the data structures with new data.
void ImagePostProcessor::processOrangeBlobsBack(){
    std::vector<orange_blob> temp_vector = blob_container_back.o_blob;
    int size = temp_vector.size();
    if(size==0){
        backLock.lock();
        blob_structure_back.o_ball.clear();
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
            
            temp_holder_back.o_ball.push_back(orange_ball());
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.orange_w/2;
            int x1, x2, y1, y2;
            x1 = blob.orange_cen_x-half_w;
            x2 = blob.orange_cen_x+half_w;
            y1 = blob.orange_cen_y-half_w;
            y2 = blob.orange_cen_y+half_w;
            
            temp_holder_back.o_ball[count].orange_cen_x = blob.orange_cen_x;
            temp_holder_back.o_ball[count].orange_cen_y = blob.orange_cen_y;
            temp_holder_back.o_ball[count].orange_w = blob.orange_w;
            temp_holder_back.o_ball[count].orange_h = blob.orange_w;
            temp_holder_back.o_ball[count].orange_x1 = x1;
            temp_holder_back.o_ball[count].orange_x2 = x2;
            temp_holder_back.o_ball[count].orange_y1 = y1;
            temp_holder_back.o_ball[count].orange_y2 = y2;
            count++;            
        }
    }
    backLock.lock();
    blob_structure_back.o_ball.clear();
    blob_structure_back.o_ball = temp_holder_back.o_ball;
    blob_structure_back.oranges_postprocessed = count;
    backLock.unlock();
}

void ImagePostProcessor::processOrangeBlobsFront(){
    std::vector<orange_blob> temp_vector = blob_container_front.o_blob;
    int size = temp_vector.size();
    
    if(size==0){
    	frontLock.lock();
        blob_structure_front.o_ball.clear();
        blob_structure_front.oranges_postprocessed = 0;
        frontLock.unlock();
        return;
    }
    int count = 0;
    for(int i = 0; i<size; i++){
        orange_blob blob = temp_vector.at(i);
        
        if(orangeFitsInFrontList(blob.orange_cen_x, blob.orange_cen_y, blob.orange_w)){
            //Already fits in a defined box, discard by passing over
            continue;
        } else {
            temp_holder_front.o_ball.push_back(orange_ball());
            
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.orange_w/2;
            int x1, x2, y1, y2;
            x1 = blob.orange_cen_x-half_w;
            x2 = blob.orange_cen_x+half_w;
            y1 = blob.orange_cen_y-half_w;
            y2 = blob.orange_cen_y+half_w;
            
            temp_holder_front.o_ball[count].orange_cen_x = blob.orange_cen_x;
            temp_holder_front.o_ball[count].orange_cen_y = blob.orange_cen_y;
            temp_holder_front.o_ball[count].orange_w = blob.orange_w;
            temp_holder_front.o_ball[count].orange_h = blob.orange_w;
            temp_holder_front.o_ball[count].orange_x1 = x1;
            temp_holder_front.o_ball[count].orange_x2 = x2;
            temp_holder_front.o_ball[count].orange_y1 = y1;
            temp_holder_front.o_ball[count].orange_y2 = y2;
            count++;            
        }
    }
    
    /*
    for(int i = 0; i<temp_holder_front.o_ball.size(); i++){
        orange_ball ball = temp_holder_front.o_ball.at(i);
        std::cout << " BALL X:" << ball.orange_cen_x << " ; BALL Y:" << ball.orange_cen_y << " BALL WIDTH" << ball.orange_w << std::endl;
    }
    */
    frontLock.lock();
    blob_structure_front.o_ball.clear();
    blob_structure_front.o_ball = temp_holder_front.o_ball;
    blob_structure_front.oranges_postprocessed = count;
    frontLock.unlock();
    
}

void ImagePostProcessor::processBlueBlobsFront() {
    //Create temporary datastore for processing purposes
    std::vector<blue_blob> temp_vector = blob_container_front.b_blob;
    int size = temp_vector.size();
    if(size==0){
    	frontLock.lock();
        blob_structure_front.b_gate.clear();
    	blob_structure_front.blues_postprocessed = 0;
    	frontLock.unlock();
    	return;
    }

    // Initialize first gate vector, all other blobs will be placed according to
    // this one.
    int count = 0;
    blue_blob init;
    while(true){
    	init = temp_vector.at(count);
    	if(init.blue_cen_y<CAM_H/2) break;
    	else {
    	    count++;
    	}
        if(count==size){
            return;
        }
    }
    int x1, x2, y1, y2;
    
    //Calculate the x1, x2, y1, y2 based on centerpoint and width
    x1 = init.blue_x1;
    x2 = init.blue_x2;
    y1 = init.blue_y1;
    y2 = init.blue_y2;

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
        if(blob.blue_w < (gate.blue_h*0.2)){
        	break;
        }

        gate = mergeBlueGateStructure(gate, blob);
    }

    gate.blue_w = gate.blue_x2 - gate.blue_x1;
    gate.blue_cen_x = gate.blue_x2 - gate.blue_w/2;
       
    
    frontLock.lock();
    blob_structure_front.b_gate.clear();
    blob_structure_front.b_gate.push_back(blue_gate());
    blob_structure_front.b_gate[0] = gate;
    blob_structure_front.blues_postprocessed = 1;
    frontLock.unlock();
}

void ImagePostProcessor::processBlueBlobsBack() {
	//Create temporary datastore for processing purposes
	std::vector<blue_blob> temp_vector = blob_container_back.b_blob;
	int size = temp_vector.size();

	if(size==0){
            backLock.lock();
            blob_structure_back.b_gate.clear();
            blob_structure_back.blues_postprocessed = 0;
            backLock.unlock();
	    return;
	}

	// Initialize first gate vector, all other blobs will be placed according to
	// this one.
	int count = 0;
	blue_blob init;
	while(true){
            init = temp_vector.at(count);
            if(init.blue_cen_y<CAM_H/2) break;
            else {
                count++;
            }
            if(count==size){
                return;
            }
        }
	
	int x1, x2, y1, y2;

	//Calculate the x1, x2, y1, y2 based on centerpoint and width
	
        x1 = init.blue_x1;
	x2 = init.blue_x2;
	y1 = init.blue_y1;
	y2 = init.blue_y2;

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
        blob_structure_back.b_gate.clear();
        blob_structure_back.b_gate.push_back(blue_gate());
	blob_structure_back.b_gate[0] = gate;
	blob_structure_back.blues_postprocessed = 1;
	backLock.unlock();
}

void ImagePostProcessor::processYellowBlobsFront(){
	//Create temporary datastore for processing purposes
	std::vector<yellow_blob> temp_vector = blob_container_front.y_blob;
	int size = temp_vector.size();    
	if(size==0){
            frontLock.lock();
            blob_structure_front.y_gate.clear();
            blob_structure_front.yellows_postprocessed = 0;
            frontLock.unlock();
	return;
	}

	// Initialize first gate vector, all other blobs will be placed according to
	// this one.
	int count = 0;
	yellow_blob init;
        //std::cout << " Beginning size : " << size << std::endl;
        
	while(true){
            init = temp_vector.at(count);
            if(init.yellow_cen_y<CAM_H/2) break;
            else {
		count++;
            }
	if(count==size){
		return;
	}

        }
	
        

	int x1, x2, y1, y2;

	//Calculate the x1, x2, y1, y2 based on centerpoint and width
	int half_w = init.yellow_w/2;

	x1 = init.yellow_x1;
	x2 = init.yellow_x2;
	y1 = init.yellow_y1;
	y2 = init.yellow_y2;

	//Initialize the new blue gate.
	yellow_gate gate = {};

	gate.yellow_cen_x = init.yellow_cen_x;
	gate.yellow_cen_y = init.yellow_cen_y;
	gate.yellow_h = init.yellow_w;
	gate.yellow_x1 = x1;
	gate.yellow_x2 = x2;
	gate.yellow_y1 = y1;
	gate.yellow_y2 = y2;

        /*
        std::cout << " Initial data for merging : " <<std::endl<<
                " x1 : " << x1 << " y1 : " << y1 << " x2 : " << x2 << " y2 : " << y2 << std::endl <<
                " cen_x : " << gate.yellow_cen_x << " cen_y : " << gate.yellow_cen_y << " height :" << gate.yellow_h << std::endl;                
        */
        
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

        //std::cout << " Yellow gate X :" << gate.yellow_cen_x << " ; Y :" << gate.yellow_cen_y << " WIDTH :" << gate.yellow_w << " HEIGHT :" << gate.yellow_h << std::endl;
                
        frontLock.lock();
        blob_structure_front.y_gate.clear();
        blob_structure_front.y_gate.push_back(yellow_gate());
	blob_structure_front.y_gate[0] = gate;
	blob_structure_front.yellows_postprocessed = 1;
	frontLock.unlock();
}

void ImagePostProcessor::processYellowBlobsBack(){
	//Create temporary datastore for processing purposes
		std::vector<yellow_blob> temp_vector = blob_container_back.y_blob;
		int size = temp_vector.size();

        if(size==0){
            backLock.lock();
            blob_structure_back.y_gate.clear();
            blob_structure_back.yellows_postprocessed = 0;
            backLock.unlock();
		    return;
		}

		// Initialize first gate vector, all other blobs will be placed according to
		// this one.
        int count = 0;
        yellow_blob init;
        while(true){
            init = temp_vector.at(count);
            if(init.yellow_cen_y<CAM_H/2) break;
        else {
            count++;
        }
            if(count==size){
        	return;
            }
        }

		int x1, x2, y1, y2;

		    //Calculate the x1, x2, y1, y2 based on centerpoint and width
		int half_w = init.yellow_w/2;

		x1 = init.yellow_cen_x-half_w;
		x2 = init.yellow_cen_x+half_w;
		y1 = init.yellow_cen_y-half_w;
		y2 = init.yellow_cen_y+half_w;

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
        blob_structure_back.y_gate.clear();
        blob_structure_back.y_gate.push_back(yellow_gate());
	blob_structure_back.y_gate[0] = gate;
	blob_structure_back.yellows_postprocessed = 1;
	backLock.unlock();
}

//These processes iterate over the existing processed list
// and find if the given blob data fits in one of the pre-defined spaces.

bool ImagePostProcessor::orangeFitsInFrontList(int x, int y, int w) {
    
    std::vector<orange_ball> ball_vector = temp_holder_front.o_ball;

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

bool ImagePostProcessor::orangeFitsInBackList(int x, int y, int w) {
    
    std::vector<orange_ball> ball_vector = temp_holder_back.o_ball;

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
        if(gate.blue_y2 < expander.blue_y2 && expander.blue_y2 < CAM_H/2){
            gate.blue_y2 = expander.blue_y2;
        }
        if(gate.blue_y1 > expander.blue_y1){
            gate.blue_y1 = expander.blue_y1;
        }
	return gate;
}

yellow_gate ImagePostProcessor::expandYellowGate(yellow_gate gate, yellow_blob expander){
	if(gate.yellow_x2 < expander.yellow_cen_x){
            gate.yellow_x2 = expander.yellow_cen_x;
        } else if(gate.yellow_x1 > expander.yellow_cen_x){
            gate.yellow_x1 = expander.yellow_cen_x;
	}
        if(gate.yellow_y2 < expander.yellow_y2 && expander.yellow_y2 < CAM_H/2){
            gate.yellow_y2 = expander.yellow_y2;
        }
        if(gate.yellow_y1 > expander.yellow_y1){
            gate.yellow_y1 = expander.yellow_y1;
        }
        
	return gate;

}

void ImagePostProcessor::runGateDecisionSystem(){
	int front, back;
	frontLock.lock();
	backLock.lock();

	front = getFrontGates();
	back = getBackGates();
	eliminateFalseGates(front, back);
	assignBigGates();

	frontLock.unlock();
	backLock.unlock();

}

int ImagePostProcessor::getFrontGates(){
	if(blob_structure_front.blues_postprocessed>0 && blob_structure_front.yellows_postprocessed>0) return 3;
	if(blob_structure_front.yellows_postprocessed>0) return 2;
	if(blob_structure_front.blues_postprocessed>0) return 1;
	else return 0;
}

int ImagePostProcessor::getBackGates(){
	if(blob_structure_back.blues_postprocessed>0 && blob_structure_back.yellows_postprocessed>0) return 3;
	if(blob_structure_back.yellows_postprocessed>0) return 2;
	if(blob_structure_back.blues_postprocessed>0) return 1;
	else return 0;
}

void ImagePostProcessor::eliminateFalseGates(int front, int back)
{	if(front==3){
		int gates = 2;
		yellow_gate f_y_g = blob_structure_front.y_gate.at(0);
		blue_gate f_b_g = blob_structure_front.b_gate.at(0);
		if(f_y_g.yellow_h * f_y_g.yellow_w < MIN_GATE_SIZE){
			gates--;
			f_y_g = {};
			blob_structure_front.yellows_postprocessed = 0;
                        blob_structure_front.y_gate.at(0) = f_y_g;
		}
		if(f_b_g.blue_h * f_b_g.blue_w < MIN_GATE_SIZE){
			gates--;
			blob_structure_front.blues_postprocessed = 0;
			f_b_g = {};
                        blob_structure_front.b_gate.at(0) = f_b_g;
		}
		//Eliminate the smaller gate.
		if(gates==2){
			if(f_b_g.blue_h * f_b_g.blue_w > f_y_g.yellow_h * f_y_g.yellow_w){
				blob_structure_front.blues_postprocessed = 0;
				f_b_g = {};
                                blob_structure_front.b_gate.at(0) = f_b_g;
			} else {
				blob_structure_front.yellows_postprocessed = 0;
				f_y_g = {};
                                blob_structure_front.y_gate.at(0) = f_y_g;
			}
		}
	}

	if(back==3){
		int gates = 2;
		yellow_gate b_y_g = blob_structure_back.y_gate.at(0);
		blue_gate b_b_g = blob_structure_back.b_gate.at(0);
		if(b_y_g.yellow_h * b_y_g.yellow_w < MIN_GATE_SIZE){
			gates--;
			b_y_g = {};
			blob_structure_back.yellows_postprocessed = 0;
                        blob_structure_back.y_gate.at(0) = b_y_g;
		}
		if(b_b_g.blue_h * b_b_g.blue_w < MIN_GATE_SIZE){
			gates--;
			b_b_g = {};
                        blob_structure_back.blues_postprocessed = 0;
			blob_structure_back.b_gate.at(0) = b_b_g;
		}
		//Eliminate the smaller gate.
		if(gates==2){
			if(b_b_g.blue_h * b_b_g.blue_w > b_y_g.yellow_h * b_y_g.yellow_w){
				blob_structure_back.yellows_postprocessed = 0;
				b_y_g = {};
                                blob_structure_back.y_gate.at(0) = b_y_g;
			} else {
				blob_structure_back.blues_postprocessed = 0;
				b_b_g = {};
                                blob_structure_back.b_gate.at(0) = b_b_g;
			}
		}
	}
}

void ImagePostProcessor::assignBigGates(){
	bGateLock.lock();
	yGateLock.lock();
	if(blob_structure_front.blues_postprocessed>0){
		biggestBlueGate = {};
		blue_gate gate_t = blob_structure_front.b_gate.at(0);
		biggestBlueGate.blue_cen_x = gate_t.blue_cen_x;
		biggestBlueGate.blue_cen_y = gate_t.blue_cen_y;
		biggestBlueGate.blue_h = gate_t.blue_h;
		biggestBlueGate.blue_w = gate_t.blue_w;
		biggestBlueGate.direction = true;
	} else if(blob_structure_front.yellows_postprocessed>0){
		biggestYellowGate = {};
		yellow_gate gate_t = blob_structure_front.y_gate.at(0);
		biggestYellowGate.yellow_cen_x = gate_t.yellow_cen_x;
		biggestYellowGate.yellow_cen_y = gate_t.yellow_cen_y;
		biggestYellowGate.yellow_h = gate_t.yellow_h;
		biggestYellowGate.yellow_w = gate_t.yellow_w;
		biggestYellowGate.direction = true;
	}
	if(blob_structure_back.blues_postprocessed>0){
		biggestBlueGate = {};
		blue_gate gate_t = blob_structure_back.b_gate.at(0);
		biggestBlueGate.blue_cen_x = gate_t.blue_cen_x;
		biggestBlueGate.blue_cen_y = gate_t.blue_cen_y;
		biggestBlueGate.blue_h = gate_t.blue_h;
		biggestBlueGate.blue_w = gate_t.blue_w;
		biggestBlueGate.direction = false;
	} else if(blob_structure_back.yellows_postprocessed>0){
		biggestYellowGate = {};
		yellow_gate gate_t = blob_structure_back.y_gate.at(0);
		biggestYellowGate.yellow_cen_x = gate_t.yellow_cen_x;
		biggestYellowGate.yellow_cen_y = gate_t.yellow_cen_y;
		biggestYellowGate.yellow_h = gate_t.yellow_h;
		biggestYellowGate.yellow_w = gate_t.yellow_w;
		biggestYellowGate.direction = false;
	}
	yGateLock.unlock();
	bGateLock.unlock();

}

blobs_processed ImagePostProcessor::getBackSystem(){
    blobs_processed temp;
    backLock.lock();
    temp = blob_structure_back;
    backLock.unlock();
    return temp;
}

blobs_processed ImagePostProcessor::getFrontSystem(){
    blobs_processed temp;
    frontLock.lock();
    temp = blob_structure_front;
    frontLock.unlock();
    return temp;
}

big_blue_gate ImagePostProcessor::getBiggestBlue(){
	big_blue_gate gate = {};
	bGateLock.lock();
	gate = biggestBlueGate;
	bGateLock.unlock();
	return gate;
}

big_yellow_gate ImagePostProcessor::getBiggestYellow(){
	big_yellow_gate gate = {};
	yGateLock.lock();
	gate = biggestYellowGate;
	yGateLock.unlock();
	return gate;
}

