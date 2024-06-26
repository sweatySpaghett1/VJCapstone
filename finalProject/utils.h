#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>


using namespace std;
//As in the example images, (xi,yi)
struct image{
    vector<vector<int>>* subWindow; //Integral Image.
    int value; //The return of the feature over the window
    int isFace; //1 if subWindow contains face, else 0
    float weight;
    int classification;
};


void printGrid(vector<vector<int>>* image, int nRows, int nCols);

vector<vector<int>>* integralImage(vector<vector<int>>* originalImage, int nRows, int nCols);
vector<image>* readExamplesIn(int numFaceExamples, int numNonFaceExamples);




#endif //UTILS_H
