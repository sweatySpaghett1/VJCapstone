#include "utils.h"



using namespace std;
//Getting the integralImage
vector<vector<int>>* integralImage(vector<vector<int>>* originalImage, int nRows, int nCols){
    vector<vector<int>>* integralImage = new vector<vector<int>>(nRows, vector<int>(nCols,0));
    for(int i = 0; i < nCols; i++){
        for(int j = 0; j < nRows; j++){
            if(i == 0 && j == 0){
                (*integralImage)[i][j] = (*originalImage)[i][j];
            }
            //First row but not 0,0
            else if(i == 0){
                (*integralImage)[i][j] = (*originalImage)[i][j] + (*integralImage)[i][j-1];
            }
            //First col but not 0,0
            else if(j == 0){
                (*integralImage)[i][j] = (*originalImage)[i][j] + (*integralImage)[i-1][j];
            }
            //Not on an edge
            else{
                (*integralImage)[i][j] = (*integralImage)[i-1][j] + (*integralImage)[i][j-1] + (*originalImage)[i][j] - (*integralImage)[i-1][j-1];
            }
        }
    }
    return integralImage;
}

//Printing out the grid
void printGrid(vector<vector<int>>* image, int nRows, int nCols){
    for(int i = 0; i < nCols; i++ ){
        for(int j = 0; j < nRows; j++){
            printf("%d ", (*image)[i][j]);
        }
        printf("\n");
    }    
}



//Reading in all the txt files and making into images that are respectivelly labeled face or non_face and then added to vector
vector<image>* readExamplesIn(int numFaceExamples, int numNonFaceExamples){
    vector<image>* inputs = new vector<image>();

    //Roots(chage this if need be, make sure file only containts .txt files)
    string faceRoot = "finalData/training/face/";
    string nonFaceRoot = "finalData/training/non_face/";
    for(int n = 1; n <= numFaceExamples; n++){
        vector<vector<int>> pixels;
        string path;
        //work to get correct path made
            if(n < 10){
                path = faceRoot + "000" + to_string(n) + ".txt";
            }
            else if(n < 100){
                path = faceRoot + "00" + to_string(n) + ".txt";
            }
            else if(n < 1000){
                path = faceRoot + "0" + to_string(n) + ".txt";
            }
            else{
                perror("you're out of range.");
            }
        

        //Opening path and checking existence
        ifstream inputFile(path);
        if (!inputFile){
            perror("File not found.\n");
        } 

        //Initialize currLine
        string currLine;
        //Going through file line by line
        while(getline(inputFile, currLine)){
            vector<int> currRow;
            string numString = "";
            //Goes through and seeks out the numbers of each line, turns them into ints, and pushes them into the currRow vector
            for(int i = 0; i <= currLine.length(); i++){
                //End of number
                if(currLine[i] == ',' || i == currLine.length()){
                    currRow.push_back(stoi(numString));
                    numString = "";
                }
                else{
                    numString = numString.append(to_string(currLine[i] - 48));
                }    

            }
            //Get to the next row
            pixels.push_back(currRow);
            
        }
        //Close up so another file can be opened  
        inputFile.close();
        vector<vector<int>>* II = integralImage(&pixels, 24, 24);
        //Image with a face set to yes, because this is the yes faces
        struct image example = {II, 0, 1};
        //Push onto inputs
        inputs->push_back(example);



    }


    //Face code ^
    //---------------------I'm really dumb, and this is the simplest way I think without brain hurty.------------------------
    //-------------I guess I could pass and return a pointer in two separate functions but I'm not going to.-----------------
    //Non-face coe v

    for(int n = 1; n <= numNonFaceExamples; n++){
        vector<vector<int>> pixels;
        string path;
        //work to get correct path made
        
            if(n < 10){
                path = nonFaceRoot + "000" + to_string(n) + ".txt";
            }
            else if(n < 100){
                path = nonFaceRoot + "00" + to_string(n) + ".txt";
            }
            else if(n < 1000){
                path = nonFaceRoot + "0" + to_string(n) + ".txt";
            }
            else{
                path = nonFaceRoot + to_string(n) + ".txt";
            }
        
        //cout << path << endl;

        //Opening path and checking existence
        ifstream inputFile(path);
        if (!inputFile){
            perror("File not found.\n");
        } 

        //Initialize currLine
        string currLine;
        //Going through file line by line
        while(getline(inputFile, currLine)){
            vector<int> currRow;
            

            string numString = "";
            //Goes through and seeks out the numbers of each line, turns them into ints, and pushes them into the currRow vector
            for(int i = 0; i <= currLine.length(); i++){
                //printf("%c", currLine[i]);
                //End of number
                if(currLine[i] == ',' || i == currLine.length()){
                    currRow.push_back(stoi(numString));
                    numString = "";
                }
                else{
                    numString = numString.append(to_string(currLine[i] - 48));
                }    

            }
            //Get to the next row
            pixels.push_back(currRow);            
        }
        //Close up so another file can be opened  
        inputFile.close();
        vector<vector<int>>* II = integralImage(&pixels, 24, 24);
        //Image with a face set to no, because this is the non faces
        struct image example = {II, 0, 0};
        //Push onto inputs
        inputs->push_back(example);
    }


    return inputs;
}
