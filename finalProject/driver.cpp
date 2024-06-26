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
using namespace std;

void configOutput(vector<strongFeature>* strongFeatures);

//Main driver
int main(int argc, char** argv){
    //Reading in all the txts as image structs
    vector<struct image>* testInputs = readExamplesIn(485,3639);
    vector<strongFeature>* strongFeatures = new vector<strongFeature>();
    bool manual = 0; //1 for manual input, 0 for automatic

    vector<int> numWeakFeatures{2,3,5,7,9}; //Only used in manual
    int numStrongFeatures = 5;  //Only used in manual

    float targetFP = .005; //Only used in automatic

    float maxAcceptedFPRate = .5; //Maximum accepted False Positive Rate
    float minAcceptedDTRate = .99; //Minimum accepted Detection Rate
    //Getting the strong features
    
    if(manual){
        strongFeatures = strongTrainer(numStrongFeatures, maxAcceptedFPRate, minAcceptedDTRate, testInputs, &numWeakFeatures);
    }
    else{
        strongFeatures = automaticStrongTrainer(maxAcceptedFPRate, minAcceptedDTRate, testInputs, targetFP);
    }
    //Print out the final detection rate and false positive rate
    //I'm still keeping this in main driver because I think it helps
    float finalDetectionRate = 1;
    float finalFalsePosRate = 1;
    for(int i = 0; i < strongFeatures->size(); i++){
        finalDetectionRate *= (*strongFeatures)[i].getDetectionRate();
        finalFalsePosRate *= (*strongFeatures)[i].getFalsePosRate();
    }
    printf("Final DR: %f, Final FP: %f\n", finalDetectionRate, finalFalsePosRate);
    //Puts out config files
    configOutput(strongFeatures);
    return 0;
}

//Outputs config files
void configOutput(vector<strongFeature>* strongFeatures){
    for(int i = 0; i < strongFeatures->size(); i++){
        string filename = "strong" + to_string(i) + ".txt";
        ofstream configFile;
        configFile.open(filename);
        configFile << (*strongFeatures)[i].strongFeatureString();
        configFile.close();
    }
}