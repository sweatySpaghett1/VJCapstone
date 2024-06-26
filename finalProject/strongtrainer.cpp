#include "strongtrainer.h"
//Must fix deletion of undesirables between strong classifiers.


//Constructor
strongFeature::strongFeature(float threshold, int numWeakClassifiers, float falsePosRate, 
float detectionRate, vector<weakFeature>* weakFeatures){
    this->threshold = threshold;
    this->numWeakClassifiers = numWeakClassifiers;
    this->falsePosRate = falsePosRate;
    this->detectionRate = detectionRate;
    this->weakFeatures = weakFeatures;    

}

//Printing out strongfeature for debugging
void strongFeature::printStrongFeature(){
    printf("Threshold: %f, numWeakClassifiers: %d, falsePosRate: %f, detectionRate: %f\n", this->threshold, this->numWeakClassifiers,
    this->falsePosRate, this->detectionRate);
    for(int t = 0; t < numWeakClassifiers; t++){
        (*weakFeatures)[t].printWeakFeature();
    }
}

//Turning into string for config files
string strongFeature::strongFeatureString(){
    string schnur = to_string(this->threshold) + " " + to_string(this->numWeakClassifiers) + "\n";
    for(int i = 0; i < this->numWeakClassifiers; i++){
        schnur += (*weakFeatures)[i].weakFeatureString();
    }
    return schnur;
}


float strongFeature::getThreshold(){
    return this->threshold;
}

float strongFeature::getDetectionRate(){
    return this->detectionRate;
}

float strongFeature::getFalsePosRate(){
    return this->falsePosRate;
}

int strongFeature::getNumWeakClassifiers(){
    return this->numWeakClassifiers;
}


vector<weakFeature>* strongFeature::getWeakFeatures(){
    return this->weakFeatures;
}


//Main strong feature training algorithm with cascading, this follows the 2004 paper's table 2 basically exactly, validation autobalances
vector<strongFeature>* automaticStrongTrainer(float maxAcceptedFalsePosRate, float minAcceptedDetectionRate, 
vector<image>* inputs, float targetFalsePosRate){
    //True training is 70% and validation set 30%
     
    vector<strongFeature>* cascadedClassifier = new vector<strongFeature>();
    //Hard set initial values
    float prevFalsePosRate = 1.0;  
    float prevDetectionRate = 1.0;
    float layerDetectionRate;
    float layerFalsePosRate;
    float cascadeFalsePosRate = 1.0;
    float cascadeDetectionRate = 1.0;
    float tempCascadeDR;
    float tempCascadeFP;
    int nPos = 485; //485 
    int nNeg = 3639; //3639
    float thresh;

    vector<image> currInputs = *inputs; 
    int i = 0;
    //While F_i > F_target (Creation of a single strong classifier per loop)
    while(cascadeFalsePosRate > targetFalsePosRate){
        vector<weakFeature>* bestFeatures = new vector<weakFeature>();
        int numWeak = 0;
        //i <- i + 1
        i++;
        //F_i = F_(i-1);
        tempCascadeFP = prevDetectionRate;

        float initNegWeight = 1/(2*(float)nNeg);
        float initPosWeight = 1/(2*(float)nPos);
        for(int p = 0; p < currInputs.size(); p++){
            if(currInputs[p].isFace == 0){
                currInputs[p].weight = initNegWeight;
            }
            else{
                currInputs[p].weight = initPosWeight;
            }
        }

        //Getting the true training and validation set
        vector<image> trueTrainingSet;
        vector<image> validationSet;

        int posCutoff = (nPos*7)/10;
        int negCutoff = (nNeg*7)/10;

        //[FACE trueTraining | Face Validation | Nonface trueTraining | Nonface Validation]
        //0                posCutoff          nPos               nPos + negCutoff        nNeg

        //Pushing positive onto trueTraining
        for(int k = 0; k < posCutoff; k++){
            trueTrainingSet.push_back(currInputs[k]);
        }
        //Pushing positive into validation
        for(int k = posCutoff; k < nPos; k++){
            validationSet.push_back(currInputs[k]);
        }
        //Pushing negative into trueTruetraining
        for(int k = nPos; k < nPos + negCutoff; k++){
            trueTrainingSet.push_back(currInputs[k]);
        }
       
        //Pushing negative onto validation
        for(int k = nPos + negCutoff; k < currInputs.size(); k++){
            validationSet.push_back(currInputs[k]);
        }
        //Check real quick integer math doesn't have any issues.
        if(validationSet.size() + trueTrainingSet.size() != currInputs.size()){
            perror("Training subsets are missing an element");
        }
        int testValPos = 0;
        int testValNeg = 0;
        for(int test = 0; test < validationSet.size(); test++){
            if(validationSet[test].isFace == 0){
                testValNeg++;
            }
            else{
                testValPos++;
            }
        }
        //END OF CREATION OF TRUE TRAINING AND VALIDATION SETS

        // F_i > f * F_(i-1)
        tempCascadeFP = cascadeFalsePosRate;
        while(tempCascadeFP > maxAcceptedFalsePosRate * prevFalsePosRate){
            vector<weakFeature>* bestFeature = weaktrainer(&trueTrainingSet, 1,negCutoff,posCutoff);
            printf("FEAT: ");
            (*bestFeature)[0].getFeat()->printFeature();
            
            numWeak++;
            bestFeatures->push_back(bestFeature->back());
            bestFeature->pop_back();
            printf("feat within features: ");
            (*bestFeatures)[0].getFeat()->printFeature();
            //Initial validation of data
            int truePositives = 0;
            int falsePositives = 0;
            int trueNegatives = 0;
            int falseNegatives = 0;
            for(int validationIndex = 0; validationIndex < validationSet.size(); validationIndex++){
                int result = strongClassify(bestFeatures, &(validationSet[validationIndex]), 0,0);
                //If classified nonface
                if(result == 0){
                    //If example actually is nonface
                    if(validationSet[validationIndex].isFace == 0){
                        trueNegatives++;
                    }
                    //If it's actually a face
                    else if(validationSet[validationIndex].isFace == 1){
                        falseNegatives++;
                    }
                }
                //If classified face
                else if(result == 1){
                    //If it's actually a nonface
                    if(validationSet[validationIndex].isFace == 0){
                        falsePositives++;
                    }
                    //If example actually is a face
                    else if(validationSet[validationIndex].isFace == 1){
                        truePositives++;
                    }
                }
                else{
                    perror("This literally shouldn't be possible.");
                }
            }
            int validationNegSize = nNeg - negCutoff;
            int validationPosSize = nPos - posCutoff;
            layerFalsePosRate = ((float)falsePositives)/((float)validationNegSize);
            layerDetectionRate = ((float)truePositives)/((float)validationPosSize);

            //Finally over here create strongtrainer 
            //get alpha
            float alphaSum = 0;
            for(int t = 0; t < bestFeatures->size();t++){
                alphaSum += (*bestFeatures)[t].getAlpha();
            }
            //Default thresh of alphaSum/2 from the bottom of table 1 in 2004 paper
            thresh = alphaSum/2.0;
            //Decrease threshold until detectionRate high enough
            tempCascadeDR = cascadeDetectionRate * layerDetectionRate;
            while((minAcceptedDetectionRate*prevDetectionRate) >= tempCascadeDR){
                //printf("DR too low: %f\n", tempCascadeDR);
                thresh -= 0.1;
                trueNegatives = 0;
                falseNegatives = 0;
                truePositives = 0;
                falsePositives = 0;
                //Going through classifying validation dataset with new thresh
                for(int validationIndex = 0; validationIndex < validationSet.size(); validationIndex++){
                    int result = strongClassify(bestFeatures, &(validationSet[validationIndex]), 1, thresh);
                    //If classified nonface
                    if(result == 0){
                        //If example actually is nonface
                        if(validationSet[validationIndex].isFace == 0){
                            trueNegatives++;
                        }
                        //If it's actually a face
                        else if(validationSet[validationIndex].isFace == 1){
                            falseNegatives++;
                        }
                    }
                    //If classified face
                    else if(result == 1){
                        //If it's actually a nonface
                        if(validationSet[validationIndex].isFace == 0){
                            falsePositives++;
                        }
                        //If example actually is a face
                        else if(validationSet[validationIndex].isFace == 1){
                            truePositives++;
                        }
                    }
                    else{
                        perror("This literally shouldn't be possible");
                    }
                }
                layerDetectionRate = ((float)truePositives)/((float)validationPosSize);
                layerFalsePosRate = ((float)falsePositives)/((float)validationNegSize);
                tempCascadeDR = cascadeDetectionRate * layerDetectionRate;
                printf("NEW LAYER DR: %f NEW LAYER FP: %f\n", layerDetectionRate, layerFalsePosRate);
            }
            tempCascadeFP = cascadeFalsePosRate * layerFalsePosRate;
            if(bestFeatures->back().getAlpha() > 8){
                break;
            }
        } //End of F_i > f*F_(i-1)
        //Actual creation of the strongFeature object
        strongFeature* strFeat = new strongFeature(thresh, numWeak, layerFalsePosRate, layerDetectionRate, bestFeatures);
        printf("Strong feature made, FP: %f, DR: %f\n", layerFalsePosRate, layerDetectionRate);
        cascadedClassifier->push_back(*strFeat);
        //TODO FIX THE DELETION, ez fix but slow. Still O(n) time and maintains locality.
        //Checking all non-faces and passing the FPs through to the next generation
        int nRemoved = 0;
        int currInputsSize = currInputs.size();
        for(int k = 0; k < currInputsSize; k++){
            //Only computing non_faces
            if(currInputs[k].isFace == 0){
                int strongFeatureResult = strongClassify(strFeat->getWeakFeatures(), &currInputs[k], 1, strFeat->getThreshold());
                //Removing true negatives
                /*
                if(strongFeatureResult == 0){
                    currInputs.erase(currInputs.begin() + k - nRemoved);
                    nNeg--;
                } 
                */  
            }
        }
        printf("Negatives passed: %d\n", nNeg);
        cascadeDetectionRate = tempCascadeDR;
        cascadeFalsePosRate = tempCascadeFP;
        printf("Casc DR: %f, Casc FP: %f\n", cascadeDetectionRate, cascadeFalsePosRate);
        prevDetectionRate = cascadeDetectionRate;
        prevFalsePosRate = cascadeFalsePosRate;
    }
    

    return cascadedClassifier;
}




//Return C(x), table showed at end of table 1
int strongClassify(vector<weakFeature>* bestFeatures, image* input, int adjusting, float adjustedThresh){
    float alphaSum = 0;
    float weakFeatSum = 0;
    for(int t = 0; t < bestFeatures->size(); t++){
        alphaSum += (*bestFeatures)[t].getAlpha(); //Could precompute this, but loop is still necessary for weakFeatSum
        weakFeatSum += ((*bestFeatures)[t].classify((*input).subWindow) * (*bestFeatures)[t].getAlpha());
    }   
    //alphaSum/2 is the initial strong threshold
    if(adjusting == 0){
        if(weakFeatSum >= (alphaSum/2.0)){
            input->classification = 1;
            return 1;
        }
        input->classification = 0;
        return 0;
    }
    //If non-initial(i.e the while loop making sure detectionRate is high enough)
    else{
        if(weakFeatSum >= adjustedThresh){
            input->classification = 1;
            return 1;
        }
        input->classification = 0;
        return 0;
    }
    
}




//Old design with manual number of strong and weaks, validation is present to show efficacy to designers, no autobalance
vector<strongFeature>* strongTrainer(int nStrongFeatures, float maxAcceptedFalsePosRate, float minAcceptedDetectionRate, 
vector<image>* inputs, vector<int>* nWeakFeatures){
    //True training is 70% and validation set 30%
     
    vector<strongFeature>* cascadedClassifier = new vector<strongFeature>();
    //Hard set initial values
    int nPos = 485; //485
    int nNeg = 3639; //3639
    vector<image> currInputs = *inputs; 
    for(int i = 1; i <= nStrongFeatures; i++){
        //Getting the true training and validation set
        vector<image> trueTrainingSet;
        vector<image> validationSet;

        float initNegWeight = 1/(2*(float)nNeg);
        float initPosWeight = 1/(2*(float)nPos);
        for(int p = 0; p < currInputs.size(); p++){
            if(currInputs[p].isFace == 0){
                currInputs[p].weight = initNegWeight;
            }
            else{
                currInputs[p].weight = initPosWeight;
            }
        }

        int posCutoff = (nPos*7)/10;
        int negCutoff = (nNeg*7)/10;

        //[FACE trueTraining | Face Validation | Nonface trueTraining | Nonface Validation]
        //0                posCutoff          nPos               nPos + negCutoff        nNeg

        //Pushing positive onto trueTraining
        for(int k = 0; k < posCutoff; k++){
            trueTrainingSet.push_back(currInputs[k]);
        }
        //Pushing positive into validation
        for(int k = posCutoff; k < nPos; k++){
            validationSet.push_back(currInputs[k]);
        }
        //Pushing negative into trueTruetraining
        for(int k = nPos; k < nPos + negCutoff; k++){
            trueTrainingSet.push_back(currInputs[k]);
        }
       
        //Pushing negative onto validation
        for(int k = nPos + negCutoff; k < currInputs.size(); k++){
            validationSet.push_back(currInputs[k]);
        }
        //Check real quick integer math doesn't have any issues
        if(validationSet.size() + trueTrainingSet.size() != currInputs.size()){
            perror("Training subsets are missing an element");
        }
        int testValPos = 0;
        int testValNeg = 0;
        for(int test = 0; test < validationSet.size(); test++){
            if(validationSet[test].isFace == 0){
                testValNeg++;
            }
            else{
                testValPos++;
            }
        }
        
        //Train t number of strong features, actually pass in &trueTrainingSet when time comes
        vector<weakFeature>* bestFeatures = weaktrainer(&trueTrainingSet,(*nWeakFeatures)[i-1],negCutoff,posCutoff);
        for(int weakIndex = 0; weakIndex < (*nWeakFeatures)[i-1]; weakIndex++){
            printf("FEAT: ");
            (*bestFeatures)[weakIndex].getFeat()->printFeature();
        }

        //Initial validation of data
        int truePositives = 0;
        int falsePositives = 0;
        int trueNegatives = 0;
        int falseNegatives = 0;
        for(int validationIndex = 0; validationIndex < validationSet.size(); validationIndex++){
            int result = strongClassify(bestFeatures, &(validationSet[validationIndex]), 0,0);
            //If classified nonface
            if(result == 0){
                //If example actually is nonface
                if(validationSet[validationIndex].isFace == 0){
                    trueNegatives++;
                }
                //If it's actually a face
                else if(validationSet[validationIndex].isFace == 1){
                    falseNegatives++;
                }
            }
            //If classified face
            else if(result == 1){
                //If it's actually a nonface
                if(validationSet[validationIndex].isFace == 0){
                    falsePositives++;
                }
                //If example actually is a face
                else if(validationSet[validationIndex].isFace == 1){
                    truePositives++;
                }
            }
            else{
                perror("This literally shouldn't be possible.");
            }
        }
        int validationNegSize = nNeg - negCutoff;
        int validationPosSize = nPos - posCutoff;
        float falsePosRate = ((float)falsePositives)/((float)validationNegSize);
        float detectionRate = ((float)truePositives)/((float)validationPosSize);
        printf("FalsePosRate: %f, detectionRate: %f\n", falsePosRate,detectionRate);    

        //Finally over here create strongtrainer 
        //get alpha
        float alphaSum = 0;
        for(int t = 0; t < bestFeatures->size();t++){
            alphaSum += (*bestFeatures)[t].getAlpha();
        }
        float thresh = alphaSum/2.0;

        //Actual creation of the strongFeature object
        strongFeature* strFeat = new strongFeature(thresh, (*nWeakFeatures)[i-1], falsePosRate, detectionRate, bestFeatures);
        cascadedClassifier->push_back(*strFeat);
        //Reset nPos and nNeg and find out their new values
        nPos = 0;
        nNeg = 0;
        int nRemoved = 0;
        //This loop will make the next strong classifier only consider things detected as faces
        //TODO FIX THIS
        for(int k = 0; k < currInputs.size(); k++){
            int strongFeatureResult = strongClassify(strFeat->getWeakFeatures(), &currInputs[k], 1, strFeat->getThreshold());
            //Removing things not deemed as faces
            if(strongFeatureResult == 0){
                currInputs.erase(currInputs.begin() + k - nRemoved);
                nRemoved++;
            }
            //Resetting actual values of nNeg and nPos for next strong classifier
            
        }
        for(int k = 0; k < currInputs.size(); k++){
            if(currInputs[k].isFace == 0){
                nNeg++;
            }
            else{
                nPos++;
            }
        }
    }

    return cascadedClassifier;
}