#ifndef feature_h
#define feature_h
#include<stdio.h>
#include<stdlib.h>
#include <vector>
#include <iostream>
#include "utils.h"
#include <cmath>
using namespace std;

enum featureType{
    A,B,C,D,E
};


class feature{
    public:
    feature(int x1, int y1, int x2, int y2, featureType);
    int areaCalc(vector<vector<int>>* integralImage, int x1, int y1, int x2, int y2);
    int getValue(vector<vector<int>>* integralImage);
    int getX1();
    int getY1();
    int getX2();
    int getY2();
    void printFeature();
    featureType getType();
    string featString();

    private:
    int height; //num rows
    int width; //num cols
    int coords[4]; //[x1,y1,x2,y2]
    featureType type;

};

#endif