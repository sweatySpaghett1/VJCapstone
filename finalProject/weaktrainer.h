#ifndef weaktrainer_h
#define weaktrainer_h
#include "feature.h"




class weakFeature{
    public:
    //In case we don't have thresh or polarity figured out
    weakFeature(feature* feat);
    //When we do, I guess?
    weakFeature(feature* feat, float threshold, int polarity);
    float optimalThresh(vector<image>*, int nNeg, int nPos,float TNeg, float TPos);
    int classify(vector<vector<int>>* integralImage);
    void printWeakFeature();
    float getThreshold();
    int getPolarity();
    feature* getFeat();
    float getAlpha();
    string weakFeatureString();

    void setAlpha(float alph);
    private:
    //A -1 polarity means it's negative, a 1 polarity means it's positive, 0 is an error
    int polarity;
    float threshold;
    feature *feat; 
    float alpha;
};


//Actual trainer of nFeatures amount of features

//Before being called, the inputs go through and get the feature values for the respective feature 
vector<weakFeature>* weaktrainer(vector<image>* inputs, int nFeatures, int nNeg, int nPos);
//This will produce a vector containing all possible features
vector<feature>* featureSetGenerator();


vector<feature>* testFeatureSetGenerator();

#endif