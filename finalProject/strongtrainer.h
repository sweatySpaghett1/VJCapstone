#ifndef strongtrainer_h
#define strongtrainer_h
#include "weaktrainer.h"


class strongFeature{
    public:
    strongFeature(vector<weakFeature>* weakFeatures);
    strongFeature(float threshold, int numWeakClassifier, float falsePosRate, float detectionRate, vector<weakFeature>* weakFeatures);
    void printStrongFeature();


    float getThreshold();
    int getNumWeakClassifiers();
    float getFalsePosRate();
    float getDetectionRate();
    vector<weakFeature>* getWeakFeatures();
    string strongFeatureString();
    
    private:
    float threshold;
    int numWeakClassifiers;
    float falsePosRate;
    float detectionRate;
    vector<weakFeature>* weakFeatures;

};
//maximum accepted layer false positive rate, minimum accepted layer detection rate, vector of inputs, target overall false positive rate
vector<strongFeature>* automaticStrongTrainer(float maxAcceptedFalsePosRate, float minAcceptedDetectionRate, 
vector<image>* inputs, float targetFalsePosRate);

//number of strong features, maximum accepted layer false postive rate, minumum accepted layer detection rate, vector of inputs, 
//vector of desired num of weak trainers per strong feature(aka: t)
vector<strongFeature>* strongTrainer(int nStrongFeatures, float maxAcceptedFalsePosRate, float minAcceptedDetectionRate, 
vector<image>* inputs, vector<int>* nWeakFeatures);


int strongClassify(vector<weakFeature>* bestFeatures, image* input, int adjusting, float adjustedThresh);


#endif

