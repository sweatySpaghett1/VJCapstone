#include "feature.h"
#include "weaktrainer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "strongtrainer.h"
#include <string>
#include <fstream>
#include <chrono>

using namespace chrono;

//If you're not Max(that's me!), you don't really need to look at this file. Cheers. 











//type, thresh, polarity, coords--- all separated by spaces
void configOutput(vector<strongFeature>* strongFeatures);

int main(int argc, char** argv){
    
    vector<struct image>* testInputs = readExamplesIn(485,3639);
    vector<int> numWeakFeatures{1}; 
    float maxAcceptedFPRate = .5; //Maximum accepted False Positive Rate per layer
    float minAcceptedDTRate = .995; //Minimum accepted Detection Rate per layer
    //Getting the strong features
    int numStrongFeatures = 1;
    auto start = high_resolution_clock::now();
    vector<strongFeature>* strongFeatures = automaticStrongTrainer(maxAcceptedFPRate, minAcceptedDTRate, testInputs, .0001);
    //vector<strongFeature>* strongFeatures = strongTrainer(numStrongFeatures, maxAcceptedFPRate, minAcceptedDTRate, testInputs, &numWeakFeatures);

    configOutput(strongFeatures);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end-start);
    cout << "Processing time: " << duration.count() << " seconds" << endl;
    float finalDetectionRate = 1;
    float finalFalsePosRate = 1;
    for(int i = 0; i < strongFeatures->size(); i++){
        finalDetectionRate *= (*strongFeatures)[i].getDetectionRate();
        finalFalsePosRate *= (*strongFeatures)[i].getFalsePosRate();
    }
    printf("Final DR: %f, Final FP: %f\n", finalDetectionRate, finalFalsePosRate);

    return 0;
}



//Outputs config files
void configOutput(vector<strongFeature>* strongFeatures){
    for(int i = 0; i < strongFeatures->size(); i++){
        string filename = "strong_test" + to_string(i) + ".txt";
        ofstream configFile;
        configFile.open(filename);
        configFile << (*strongFeatures)[i].strongFeatureString();
        configFile.close();
    }
}