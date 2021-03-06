/* 
 * File:   ImagePostProcessor.cpp
 * Author: antti
 * 
 * Created on November 4, 2014, 3:43 PM
 */

#include "ImagePostProcessor.h"

ImagePostProcessor::ImagePostProcessor(ImageProcessor* imageProcessor) : calculator(NULL) {
    calculator = new BlobDistanceCalculator(this);
    std::cout << " I AM THE CONSTRUCTOR " << std::endl;
    iProc = imageProcessor;
    blob_structure_back = {};
    blob_structure_front = {};
}

ImagePostProcessor::~ImagePostProcessor() {
    delete this->calculator;
}

void ImagePostProcessor::run() {
    std::cout << " Starting ImagePostProcessor." << std::endl;
    while (!codeEnd) {
        loadBlobVectors();
        processBlobVectors();

        calculator->run();
        getCalculatorCalculations();

        usleep(14000);

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

    greenLock.try_lock();
    green.back_green = blob_container_back.total_green;
    green.front_green = blob_container_front.total_green;
    greenLock.unlock();
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

void ImagePostProcessor::processOrangeBlobsBack() {
    std::vector<orange_blob> temp_vector = blob_container_back.o_blob;
    int size = temp_vector.size();
    if (size == 0) {
        blob_structure_back.o_ball.clear();
        blob_structure_back.oranges_postprocessed = 0;
        return;
    }
    int count = 0;
    for (int i = 0; i < size; i++) {
        orange_blob blob = temp_vector.at(i);

        if (orangeFitsInBackList(blob.orange_cen_x, blob.orange_cen_y, blob.orange_w)) {
            //Already fits in a defined box, discard by passing over
            continue;
        } else {

            temp_holder_back.o_ball.push_back(orange_ball());
            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.orange_w / 2;
            int x1, x2, y1, y2;
            x1 = blob.orange_cen_x - half_w;
            x2 = blob.orange_cen_x + half_w;
            y1 = blob.orange_cen_y - half_w;
            y2 = blob.orange_cen_y + half_w;

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
    blob_structure_back.o_ball.clear();
    blob_structure_back.o_ball = temp_holder_back.o_ball;
    blob_structure_back.oranges_postprocessed = count;
}

void ImagePostProcessor::processOrangeBlobsFront() {
    std::vector<orange_blob> temp_vector = blob_container_front.o_blob;
    int size = temp_vector.size();

    if (size == 0) {
        blob_structure_front.o_ball.clear();
        blob_structure_front.oranges_postprocessed = 0;
        return;
    }
    int count = 0;
    for (int i = 0; i < size; i++) {
        orange_blob blob = temp_vector.at(i);

        if (orangeFitsInFrontList(blob.orange_cen_x, blob.orange_cen_y, blob.orange_w)) {
            //Already fits in a defined box, discard by passing over
            continue;
        } else {
            temp_holder_front.o_ball.push_back(orange_ball());

            //Calculate the x1, x2, y1, y2 based on centerpoint and width
            int half_w = blob.orange_w / 2;
            int x1, x2, y1, y2;
            x1 = blob.orange_cen_x - half_w;
            x2 = blob.orange_cen_x + half_w;
            y1 = blob.orange_cen_y - half_w;
            y2 = blob.orange_cen_y + half_w;

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
    blob_structure_front.o_ball.clear();
    blob_structure_front.o_ball = temp_holder_front.o_ball;
    blob_structure_front.oranges_postprocessed = count;

}

void ImagePostProcessor::processBlueBlobsFront() {
    //Create temporary datastore for processing purposes
    std::vector<blue_blob> temp_vector = blob_container_front.b_blob;
    int size = temp_vector.size();
    if (size == 0) {
        blob_structure_front.b_gate.clear();
        blob_structure_front.blues_postprocessed = 0;
        return;
    }

    // Initialize first gate vector, all other blobs will be placed according to
    // this one.
    int count = 0;
    blue_blob init;
    while (true) {
        init = temp_vector.at(count);
        if (init.blue_cen_y < CAM_H / 2) break;
        else {
            count++;
        }
        if (count == size) {
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
    for (int i = 1; i < size; i++) {

        blue_blob blob = temp_vector.at(i);
        if (blob.blue_w < (gate.blue_h * 0.2)) {
            break;
        }

        gate = mergeBlueGateStructure(gate, blob);
    }

    gate.blue_w = gate.blue_x2 - gate.blue_x1;
    gate.blue_cen_x = gate.blue_x2 - gate.blue_w / 2;

    blob_structure_front.b_gate.clear();
    blob_structure_front.b_gate.push_back(blue_gate());
    if (gate.blue_h * gate.blue_w > MIN_GATE_SIZE) {
        blob_structure_front.b_gate[0] = gate;
        blob_structure_front.blues_postprocessed = 1;
    } else {
        blob_structure_front.blues_postprocessed = 0;
    }
}

void ImagePostProcessor::processBlueBlobsBack() {
    //Create temporary datastore for processing purposes
    std::vector<blue_blob> temp_vector = blob_container_back.b_blob;
    int size = temp_vector.size();

    if (size == 0) {
        blob_structure_back.b_gate.clear();
        blob_structure_back.blues_postprocessed = 0;
        return;
    }

    // Initialize first gate vector, all other blobs will be placed according to
    // this one.
    int count = 0;
    blue_blob init;
    while (true) {
        init = temp_vector.at(count);
        if (init.blue_cen_y < CAM_H / 2) break;
        else {
            count++;
        }
        if (count == size) {
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
    for (int i = 1; i < size; i++) {

        blue_blob blob = temp_vector.at(i);
        if (blob.blue_w < (gate.blue_h * 0.33)) {
            break;
        }

        gate = mergeBlueGateStructure(gate, blob);
    }

    gate.blue_w = gate.blue_x2 - gate.blue_x1;
    gate.blue_cen_x = gate.blue_x2 - gate.blue_w / 2;

    blob_structure_back.b_gate.clear();
    blob_structure_back.b_gate.push_back(blue_gate());
    if (gate.blue_h * gate.blue_w > MIN_GATE_SIZE) {
        blob_structure_back.b_gate[0] = gate;
        blob_structure_back.blues_postprocessed = 1;
    } else {
        blob_structure_back.blues_postprocessed = 0;
    }
}

void ImagePostProcessor::processYellowBlobsFront() {
    //Create temporary datastore for processing purposes
    std::vector<yellow_blob> temp_vector = blob_container_front.y_blob;
    int size = temp_vector.size();
    if (size == 0) {
        blob_structure_front.y_gate.clear();
        blob_structure_front.yellows_postprocessed = 0;
        return;
    }

    // Initialize first gate vector, all other blobs will be placed according to
    // this one.
    int count = 0;
    yellow_blob init;

    while (true) {
        init = temp_vector.at(count);
        if (init.yellow_cen_y < CAM_H / 2) break;
        else {
            count++;
        }
        if (count == size) {
            return;
        }

    }



    int x1, x2, y1, y2;

    //Calculate the x1, x2, y1, y2 based on centerpoint and width
    int half_w = init.yellow_w / 2;

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

    // Iterate over the remaining blobs, placing them in the gate structure or discarding them if they do not qualify.
    for (int i = 1; i < size; i++) {
        yellow_blob blob = temp_vector.at(i);
        if (blob.yellow_w < (gate.yellow_h * 0.33)) {
            break;
        }

        gate = mergeYellowGateStructure(gate, blob);
    }

    gate.yellow_w = gate.yellow_x2 - gate.yellow_x1;
    gate.yellow_cen_x = gate.yellow_x2 - gate.yellow_w / 2;

    blob_structure_front.y_gate.clear();
    blob_structure_front.y_gate.push_back(yellow_gate());
    if (gate.yellow_h * gate.yellow_w > MIN_GATE_SIZE) {
        blob_structure_front.y_gate[0] = gate;
        blob_structure_front.yellows_postprocessed = 1;
    } else {
        blob_structure_front.yellows_postprocessed = 0;
    }
}

void ImagePostProcessor::processYellowBlobsBack() {
    //Create temporary datastore for processing purposes
    std::vector<yellow_blob> temp_vector = blob_container_back.y_blob;
    int size = temp_vector.size();

    if (size == 0) {
        blob_structure_back.y_gate.clear();
        blob_structure_back.yellows_postprocessed = 0;
        return;
    }

    // Initialize first gate vector, all other blobs will be placed according to
    // this one.
    int count = 0;
    yellow_blob init;
    while (true) {
        init = temp_vector.at(count);
        if (init.yellow_cen_y < CAM_H / 2) break;
        else {
            count++;
        }
        if (count == size) {
            return;
        }
    }

    int x1, x2, y1, y2;

    //Calculate the x1, x2, y1, y2 based on centerpoint and width
    int half_w = init.yellow_w / 2;

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

    // Iterate over the remaining blobs, placing them in the gate structure or discarding them if they do not qualify.
    for (int i = 1; i < size; i++) {

        yellow_blob blob = temp_vector.at(i);
        if (blob.yellow_w < (gate.yellow_h * 0.33)) {
            break;
        }

        gate = mergeYellowGateStructure(gate, blob);
    }

    gate.yellow_w = gate.yellow_x2 - gate.yellow_x1;
    gate.yellow_cen_x = gate.yellow_x2 - gate.yellow_w / 2;


    blob_structure_back.y_gate.clear();
    blob_structure_back.y_gate.push_back(yellow_gate());
    if (gate.yellow_h * gate.yellow_w > MIN_GATE_SIZE) {
        blob_structure_back.y_gate[0] = gate;
        blob_structure_back.yellows_postprocessed = 1;
    } else {
        blob_structure_back.yellows_postprocessed = 0;
    }
}

//These processes iterate over the existing processed list
// and find if the given blob data fits in one of the pre-defined spaces.

bool ImagePostProcessor::orangeFitsInFrontList(int x, int y, int w) {

    std::vector<orange_ball> ball_vector = temp_holder_front.o_ball;

    int size = ball_vector.size();
    if (size == 0) {
        return false;
    }

    for (int i = 0; i < size; i++) {
        orange_ball ball = ball_vector.at(i);
        bool check1 = x >= ball.orange_x1 && x <= ball.orange_x2;
        bool check2 = y >= ball.orange_y1 && y <= ball.orange_y2;
        if (check1 && check2) {
            return true;
        }
    }

    return false;
}

bool ImagePostProcessor::orangeFitsInBackList(int x, int y, int w) {

    std::vector<orange_ball> ball_vector = temp_holder_back.o_ball;

    int size = ball_vector.size();
    if (size == 0) {
        return false;
    }

    for (int i = 0; i < size; i++) {
        orange_ball ball = ball_vector.at(i);
        bool check1 = x >= ball.orange_x1 && x <= ball.orange_x2;
        bool check2 = y >= ball.orange_y1 && y <= ball.orange_y2;
        if (check1 && check2) {
            return true;
        }

    }

    return false;
}

//Merger logic : take the width of the first blob on y-scale, fit all the blue blobs that have their y-center
// situated between them into the new blob, while increasing x1 and x2 accordingly.

blue_gate ImagePostProcessor::mergeBlueGateStructure(blue_gate gate, blue_blob merger) {

    int x1, x2, newx, y1, y2, newy;
    x1 = gate.blue_x1;
    x2 = gate.blue_x2;
    y1 = gate.blue_y1;
    y2 = gate.blue_y2;

    newx = merger.blue_cen_x;
    newy = merger.blue_cen_y;

    // If the new blob is outside the already existing x1-x2, and inside the existing y-zone, expand the existing gate.
    if ((newx < x1 || newx > x2) && (newy > y1 && newy < y2)) {
        gate = expandBlueGate(gate, merger);
    }

    return gate;
}

yellow_gate ImagePostProcessor::mergeYellowGateStructure(yellow_gate gate, yellow_blob merger) {
    if (merger.yellow_w < (gate.yellow_h * 0.33)) {
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
    if ((newx < x1 || newx > x2) && (newy > y1 && newy < y2)) {
        gate = expandYellowGate(gate, merger);
    }

    return gate;
}

// Expander logic for both colors

blue_gate ImagePostProcessor::expandBlueGate(blue_gate gate, blue_blob expander) {
    if (gate.blue_x2 < expander.blue_cen_x) {
        gate.blue_x2 = expander.blue_cen_x;
    } else if (gate.blue_x1 > expander.blue_cen_x) {
        gate.blue_x1 = expander.blue_cen_x;
    }
    if (gate.blue_y2 < expander.blue_y2 && expander.blue_y2 < CAM_H / 2) {
        gate.blue_y2 = expander.blue_y2;
    }
    if (gate.blue_y1 > expander.blue_y1) {
        gate.blue_y1 = expander.blue_y1;
    }
    return gate;
}

yellow_gate ImagePostProcessor::expandYellowGate(yellow_gate gate, yellow_blob expander) {
    if (gate.yellow_x2 < expander.yellow_cen_x) {
        gate.yellow_x2 = expander.yellow_cen_x;
    } else if (gate.yellow_x1 > expander.yellow_cen_x) {
        gate.yellow_x1 = expander.yellow_cen_x;
    }
    if (gate.yellow_y2 < expander.yellow_y2 && expander.yellow_y2 < CAM_H / 2) {
        gate.yellow_y2 = expander.yellow_y2;
    }
    if (gate.yellow_y1 > expander.yellow_y1) {
        gate.yellow_y1 = expander.yellow_y1;
    }

    return gate;

}

void ImagePostProcessor::runGateDecisionSystem() {
    int front, back;
    front = getFrontGates();
    back = getBackGates();
    eliminateFalseGates(front, back);
    assignBigGates();
}

int ImagePostProcessor::getFrontGates() {
    if (blob_structure_front.blues_postprocessed > 0 && blob_structure_front.yellows_postprocessed > 0) return 3;
    if (blob_structure_front.yellows_postprocessed > 0) return 2;
    if (blob_structure_front.blues_postprocessed > 0) return 1;
    else return 0;
}

int ImagePostProcessor::getBackGates() {
    if (blob_structure_back.blues_postprocessed > 0 && blob_structure_back.yellows_postprocessed > 0) return 3;
    if (blob_structure_back.yellows_postprocessed > 0) return 2;
    if (blob_structure_back.blues_postprocessed > 0) return 1;
    else return 0;
}

void ImagePostProcessor::eliminateFalseGates(int front, int back) {
    if (front == 3) {
        int gates = 2;
        yellow_gate f_y_g = blob_structure_front.y_gate.at(0);
        blue_gate f_b_g = blob_structure_front.b_gate.at(0);
        if (f_y_g.yellow_h * f_y_g.yellow_w < MIN_GATE_SIZE) {
            gates--;
            f_y_g = {};
            blob_structure_front.yellows_postprocessed = 0;
            blob_structure_front.y_gate.at(0) = f_y_g;
        }
        if (f_b_g.blue_h * f_b_g.blue_w < MIN_GATE_SIZE) {
            gates--;
            blob_structure_front.blues_postprocessed = 0;
            f_b_g = {};
            blob_structure_front.b_gate.at(0) = f_b_g;
        }
        //Eliminate the smaller gate.
        if (gates == 2) {
            if (f_b_g.blue_h * f_b_g.blue_w < f_y_g.yellow_h * f_y_g.yellow_w) {
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

    if (back == 3) {
        int gates = 2;
        yellow_gate b_y_g = blob_structure_back.y_gate.at(0);
        blue_gate b_b_g = blob_structure_back.b_gate.at(0);
        if (b_y_g.yellow_h * b_y_g.yellow_w < MIN_GATE_SIZE) {
            gates--;
            b_y_g = {};
            blob_structure_back.yellows_postprocessed = 0;
            blob_structure_back.y_gate.at(0) = b_y_g;
        }
        if (b_b_g.blue_h * b_b_g.blue_w < MIN_GATE_SIZE) {
            gates--;
            b_b_g = {};
            blob_structure_back.blues_postprocessed = 0;
            blob_structure_back.b_gate.at(0) = b_b_g;
        }
        //Eliminate the smaller gate.
        if (gates == 2) {
            if (b_b_g.blue_h * b_b_g.blue_w > b_y_g.yellow_h * b_y_g.yellow_w) {
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

void ImagePostProcessor::assignBigGates() {
    biggestBlueGate = {};
    biggestYellowGate = {};
    if (blob_structure_front.blues_postprocessed > 0) {
        biggestBlueGate = {};
        blue_gate gate_t = blob_structure_front.b_gate.at(0);
        biggestBlueGate.blue_cen_x = gate_t.blue_cen_x;
        biggestBlueGate.blue_cen_y = gate_t.blue_cen_y;
        biggestBlueGate.blue_x1 = gate_t.blue_x1;
        biggestBlueGate.blue_x2 = gate_t.blue_x2;
        biggestBlueGate.blue_y1 = gate_t.blue_y1;
        biggestBlueGate.blue_y2 = gate_t.blue_y2;
        biggestBlueGate.blue_h = gate_t.blue_h;
        biggestBlueGate.blue_w = gate_t.blue_w;
        biggestBlueGate.direction = true;
    } else if (blob_structure_front.yellows_postprocessed > 0) {
        biggestYellowGate = {};
        yellow_gate gate_t = blob_structure_front.y_gate.at(0);
        biggestYellowGate.yellow_cen_x = gate_t.yellow_cen_x;
        biggestYellowGate.yellow_cen_y = gate_t.yellow_cen_y;
        biggestYellowGate.yellow_x1 = gate_t.yellow_x1;
        biggestYellowGate.yellow_x2 = gate_t.yellow_x2;
        biggestYellowGate.yellow_y1 = gate_t.yellow_y1;
        biggestYellowGate.yellow_y2 = gate_t.yellow_y2;
        biggestYellowGate.yellow_h = gate_t.yellow_h;
        biggestYellowGate.yellow_w = gate_t.yellow_w;
        biggestYellowGate.direction = true;
    }
    if (blob_structure_back.blues_postprocessed > 0) {
        biggestBlueGate = {};
        blue_gate gate_t = blob_structure_back.b_gate.at(0);
        biggestBlueGate.blue_cen_x = gate_t.blue_cen_x;
        biggestBlueGate.blue_cen_y = gate_t.blue_cen_y;
        biggestBlueGate.blue_x1 = gate_t.blue_x1;
        biggestBlueGate.blue_x2 = gate_t.blue_x2;
        biggestBlueGate.blue_y1 = gate_t.blue_y1;
        biggestBlueGate.blue_y2 = gate_t.blue_y2;
        biggestBlueGate.blue_h = gate_t.blue_h;
        biggestBlueGate.blue_w = gate_t.blue_w;
        biggestBlueGate.direction = false;
    } else if (blob_structure_back.yellows_postprocessed > 0) {
        biggestYellowGate = {};
        yellow_gate gate_t = blob_structure_back.y_gate.at(0);
        biggestYellowGate.yellow_cen_x = gate_t.yellow_cen_x;
        biggestYellowGate.yellow_cen_y = gate_t.yellow_cen_y;
        biggestYellowGate.yellow_x1 = gate_t.yellow_x1;
        biggestYellowGate.yellow_x2 = gate_t.yellow_x2;
        biggestYellowGate.yellow_y1 = gate_t.yellow_y1;
        biggestYellowGate.yellow_y2 = gate_t.yellow_y2;
        biggestYellowGate.yellow_h = gate_t.yellow_h;
        biggestYellowGate.yellow_w = gate_t.yellow_w;
        biggestYellowGate.direction = false;
    }

}

blobs_processed ImagePostProcessor::getUnlockedFront() {
    blobs_processed front = blob_structure_front;
    return front;
}

blobs_processed ImagePostProcessor::getUnlockedBack() {
    blobs_processed back = blob_structure_back;
    return back;
}

big_blue_gate ImagePostProcessor::getUnlockedBlue() {
    return biggestBlueGate;
}

big_yellow_gate ImagePostProcessor::getUnlockedYellow() {
    return biggestYellowGate;
}

void ImagePostProcessor::getCalculatorCalculations() {
    this->balls = calculator->getBalls();
    this->bGate = calculator->getBlueGate();
    this->yGate = calculator->getYellowGate();
}

std::vector<Ball> ImagePostProcessor::getBalls() {
    ballLock.lock();
    std::vector<Ball> temp = this->balls;
    ballLock.unlock();
    return temp;
}

YellowGate ImagePostProcessor::getYellowGate() {
    yGateLock.lock();
    YellowGate temp = this->yGate;
    yGateLock.unlock();
    return temp;
}

BlueGate ImagePostProcessor::getBlueGate() {
    bGateLock.lock();
    BlueGate temp = this->bGate;
    bGateLock.unlock();
    return temp;
}

GreenContainer ImagePostProcessor::getGreen() {
    greenLock.lock();
    GreenContainer temp = green;
    greenLock.unlock();
    return green;
}
