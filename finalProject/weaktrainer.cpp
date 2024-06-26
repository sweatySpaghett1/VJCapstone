#include "weaktrainer.h"
#include <algorithm>

//The whole "weaktrainer" title might be a misnomer. This is closer to, well, a single strong classifier trainer. 
//THERE WILL BE NO CASCADING HERE

//Comparison for std::sort on image vectors
bool compareImages(image i1, image i2){
    return i1.value < i2.value;
}

//Constructor 1(only one actually used)
weakFeature::weakFeature(feature* feat){
    this->feat = feat;
}
//Constructor 2(never used?)
weakFeature::weakFeature(feature* feat, float threshold, int polarity){
   this->feat = feat;
   this->polarity = polarity;
   this->threshold = threshold;
}
//Print for debugging
void weakFeature::printWeakFeature(){
    printf("Threshold: %f, Polarity %d, Feat: ", this->threshold, this->polarity);
    this->feat->printFeature();
}

//String for config files
string weakFeature::weakFeatureString(){
    string schnur = to_string(this->threshold) + " " + to_string(this->polarity) + " " + to_string(this->alpha) + "\n" + this->feat->featString();
    return schnur;    
}


float weakFeature::getThreshold(){
    return this->threshold;
}
int weakFeature::getPolarity(){
    return this->polarity;
}

feature* weakFeature::getFeat(){
    return this->feat;
}

float weakFeature::getAlpha(){
    return this->alpha;
}

void weakFeature::setAlpha(float alph){
    this->alpha = alph;
}

//Returns the error of the feature and sets its threshold and polarity to get minimum error for the feature
float weakFeature::optimalThresh(vector<image>* inputs, int nNeg, int nPos, float TNeg, float TPos){
    int inputCount = nNeg + nPos;
    float sPos[inputCount]; //Sum of positive(face) weights below current
    float sNeg[inputCount]; //Sum of negative(non face) weights below current
    struct image localInputs[inputCount];
    //Initializing new localInputs
    for(int i = 0; i < inputCount; i++){
        localInputs[i].value = (*inputs)[i].value;
        localInputs[i].isFace = (*inputs)[i].isFace;
        localInputs[i].weight = (*inputs)[i].weight;
    }
    //Sort inputs by their value
    std::sort(localInputs, localInputs + inputCount, compareImages);
    float currSPos = 0;
    float currSNeg = 0;

    //The threshold to be returned is the minimum error
    float currError = 1;
    float minError = 1;
    //Loop that gets the respective sPos and sNeg and does error calculations
    //(Acting as noninclusive of current weight, if that's correct or not, unknown)
    for(int i = 0; i < inputCount; i++){
        sPos[i] = currSPos;
        sNeg[i] = currSNeg;

        //The two cases of slide 27
        float error1 = sPos[i] + (TNeg - sNeg[i]); 
        float error2 = sNeg[i] + (TPos - sPos[i]);
        int localPolarity = 0;

        //If the negative error portion is less than positive(therefore better)
        if(error1 < error2){
            currError = error1;
            localPolarity = -1;
        }
        //If the positive error portion is less than negative(therefore better)
        else{
            currError = error2;
            localPolarity = 1;
        }
        //If the current error is better than minError and should thus become it.
        if(minError > currError){
            minError = currError;
            this->threshold = localInputs[i].value;
            this->polarity = localPolarity;
        }
        //Updating the sNeg and sPos
        if(localInputs[i].isFace == 0){
            currSNeg += localInputs[i].weight;
        }
        else{
            currSPos += localInputs[i].weight;
        }
    }
    return minError;
}

//0 means not face, 1 means face
int weakFeature::classify(vector<vector<int>>* integralImage){
    int value = this->feat->getValue(integralImage);
    if(this->polarity == -1){
        if(this->threshold > value){
            return 0;
        }
        else{
            return 1;
        }
    }
    else if(this->polarity == 1){
        if(this-> threshold > value){
            return 1;
        }
        else{
            return 0;
        }
    }
    perror("Polarity was flagged as 0(non 1/-1), something went wrong");
    return -1;
}





//This will produce a vector containing all possible features
vector<feature>* featureSetGenerator(){
    vector<feature>* allFeats = new vector<feature>();
    //First two layers of loop go through all 24^2 coords of the area and use them as top right corner coords.
    for(int TRY = 0; TRY < 24; TRY++){
        for(int TRX = 0; TRX < 24; TRX++){
            for(int BLY = TRY; BLY < 24; BLY++ ){
                for(int BLX = TRX; BLX < 24; BLX++){
                    //If its height is even(A)
                    if((BLY - TRY + 1) % 2 == 0){
                        feature* featA = new feature(TRX,TRY,BLX,BLY,A);
                        allFeats->push_back(*featA);
                    }
                    //If its height is divisble by 3(C)
                    if((BLY - TRY + 1) % 3 == 0){
                        feature* featC = new feature(TRX,TRY,BLX,BLY,C);
                        allFeats->push_back(*featC);
                    }
                    //If its width is even(B)
                    if((BLX - TRX + 1) % 2 == 0){
                        feature* featB = new feature(TRX,TRY,BLX,BLY,B);
                        allFeats->push_back(*featB);
                    }
                    //If its width is divisble by 3(D)
                    if((BLX- TRX +1 ) % 3 == 0){
                        feature* featD = new feature(TRX,TRY,BLX,BLY,D);
                        allFeats->push_back(*featD);
                    }
                    //If both height and width divisible by 2(E)
                    if(((BLX- TRX + 1) % 2 == 0) && ((BLY - TRY + 1) % 2 == 0)){
                        feature* featE = new feature(TRX,TRY,BLX,BLY,E);
                        allFeats->push_back(*featE);
                    }       
                }
            }
        }
    }
    return allFeats;
}




//Actual trainer of nFeatures amount of features
//Before being called, the inputs go through and get the feature values for the respective feature 
vector<weakFeature>* weaktrainer(vector<image>* inputs, int nFeatures, int nNeg, int nPos){
    int totalInputs = nNeg + nPos;
    vector<feature>* allFeatures = featureSetGenerator();
    vector<weakFeature>* chosen = new vector<weakFeature>();
    float beta = 0; 
    //Save some computing time in optimalThresh()
    
    weakFeature* currWeakFeat = nullptr;
    weakFeature* best = nullptr;
    int test = 0;
    for(int t = 0; t < nFeatures; t++){
        //Calculating total weight of current iteration
        float totalWeight = 0;
        for(int j = 0; j < totalInputs; j++){
            totalWeight += (*inputs)[j].weight;
        }
        //Normalizing weights(Step 1)
        for(int j = 0; j < totalInputs; j++){
            (*inputs)[j].weight = (*inputs)[j].weight/totalWeight;
        }
        float TNeg = 0;
        float TPos = 0;
        for(int i = 0; i < totalInputs; i++){
            if((*inputs)[i].isFace == 0){
                TNeg += (*inputs)[i].weight;
            }
            else{
                TPos += (*inputs)[i].weight;
            }
        }

        //Select best weak classifier by min error(step 2)
        float currError;
        float minError = MAXFLOAT;
        for(int i = 0; i < (*allFeatures).size(); i++){
            currWeakFeat = new weakFeature(&(*allFeatures)[i]);
            //Just to show user it's working
            if(i % 10000 == 0){
                printf("%d\n", i);
            }
            //Calculating the values for this feature on every input
            for(int j = 0; j < totalInputs; j++){
                (*inputs)[j].value = ((*allFeatures)[i]).getValue((*inputs)[j].subWindow);;
            }
            
            currError = currWeakFeat->optimalThresh(inputs, nNeg, nPos, TNeg, TPos);
            if(currError < minError){
                //delete best; //Could help space efficiency?
                minError = currError;
                best = currWeakFeat;
            }
            else{
                //delete currWeakFeat; //Could help space efficiency?
            }
        }

        //I guess this is part 3? Pulling out the best feature
        if(best){
            chosen->push_back(*best);
            delete best;
        }

        //Part 4, reweight
        beta = minError/ (1.0-minError);
        printf("BETA: %f\n", beta);
        for(int j = 0; j < totalInputs; j++){
            int isFace = ((*chosen)[t]).classify(((*inputs)[j]).subWindow);
            float exponent = 1 - isFace;
            (*inputs)[j].weight = (*inputs)[j].weight * pow(beta, exponent);
        }
        //Calculation and assignment of alpha value
        float alpha = log(1.0/beta);
        (*chosen)[t].setAlpha(alpha);
        printf("ALPHA: %f\n", (*chosen)[t].getAlpha());

    }
    return chosen;
}
