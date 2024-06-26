#include "feature.h"
using namespace std;


//Constructor
feature::feature(int x1, int y1, int x2, int y2, featureType type){
    this->width = x2-x1+1;
    this->height = y2-y1+1;
    this->type = type;
    this->coords[0] = x1;
    this->coords[1] = y1;
    this->coords[2] = x2;
    this->coords[3] = y2;
}
//Printing out for debugging
void feature::printFeature(){
    printf("Feature Coords: [%d, %d] [%d, %d]; Type: %d\n", this->coords[0], this->coords[1], this->coords[2], this->coords[3], (int)this->type);
}
//string for config files
string feature::featString(){
    string schnur = to_string(this->coords[0]) + " " + to_string(this->coords[1]) + " " + to_string(this->coords[2]) + " " 
    + to_string(this->coords[3]) + " " + to_string(this->type) + "\n";
    return schnur;
}

int feature::getX1(){
    return this->coords[0];
}

int feature::getY1(){
    return this->coords[1];
}

int feature::getX2(){
    return this->coords[2];
}

int feature::getY2(){
    return this->coords[3];
}
featureType feature::getType(){
    return this->type;
}

int feature::getValue(vector<vector<int>>* integralImage){
    int x1 = this->getX1();
    int y1 = this->getY1();
    int x2 = this->getX2();
    int y2 = this->getY2();
    featureType type = this->getType();
    int width = x2 - x1 + 1; //For a more conventional understanding of width with repsonse to indices
    int height = y2 - y1 + 1;
    int blackVal= 0;
    int border = 0; //for A and B
    int borderone = 0;
    int bordertwo = 0;
    int whiteVal = 0;
    this->width = width;
    this->height = height;
    this->type = type;
    if(height < 1 || width < 1){
        throw runtime_error("Invalid coords.");
    }
    //White minus black
    switch(type){
        case A:
            //Split by horizontal axis; black over white
            //Border is upper index of split, aka the index of beginning of white
            border = y2 - (height/2);
            blackVal = areaCalc(integralImage, x1, y1,x2,border);
            whiteVal = areaCalc(integralImage, x1, border+1, x2, y2);
            return whiteVal - blackVal;
        case B:
            //Split by vertical axis; white|black
            border = x2 -(width/2);
            blackVal = areaCalc(integralImage, border + 1, y1, x2,y2);
            whiteVal = areaCalc(integralImage, x1, y1, border, y2);
            return whiteVal - blackVal;
        case C:
            //Hamburger black/white/black
            borderone = y2 - (height*2)/3;
            bordertwo = y2 - (height/3);
            blackVal += areaCalc(integralImage, x1, y1, x2, borderone);
            whiteVal += areaCalc(integralImage, x1, borderone+1, x2, bordertwo);
            blackVal += areaCalc(integralImage, x1, bordertwo+1, x2, y2 );
            return whiteVal - blackVal;
        case D:
            //Split by vertical axes black|white|black
            borderone = x2 - (width)*2/3;
            bordertwo = x2 - (width/3);
            blackVal+= areaCalc(integralImage,x1,y1,borderone,y2);
            whiteVal += areaCalc(integralImage, borderone+1, y1, bordertwo, y2);
            blackVal += areaCalc(integralImage, bordertwo+1, y1, x2,y2);
            return whiteVal - blackVal;
        case E:
            //It's the hard one. 
            //White|Black
            //Black|White
            borderone = x2 - width/2; //The vertical axis
            bordertwo = y2 - height/2; //The horizontal axis
            whiteVal += areaCalc(integralImage, x1, y1, borderone, bordertwo);
            blackVal += areaCalc(integralImage, borderone+1, y1, x2, bordertwo);
            blackVal += areaCalc(integralImage, x1, bordertwo+1, borderone, y2);
            whiteVal += areaCalc(integralImage, borderone + 1, bordertwo+1, x2, y2);
            return whiteVal - blackVal;
        default:
            throw runtime_error("Invalid type.");
            break;
    }
}

int feature::areaCalc(vector<vector<int>>* integralImage,int x1, int y1, int x2, int y2){
    //[which row(y)][which col(x)]
    int area = 0;
    //Bottom right corner
    area += (*integralImage)[y2][x2];
    //Top right
    if(y1 > 0){
        area -= (*integralImage)[y1- 1][x2];
    }
    //Bottom left
    if(x1 > 0){
        area-= (*integralImage)[y2][x1-1];
    }
    //Top left
    if(x1 > 0 && y1 > 0){
        area += (*integralImage)[y1-1][x1-1];
    }
    //Bottom right corner val subtracted by the top right value and the bottom left value, then added with the top left 
    return area;
}


