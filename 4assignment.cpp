/*
CPSC 441: Assignment 4 Kathryn Lepine 
The program reads a file and RFID scans all the items in the scaned file or baskets. The program then
uses the adaptive tree walk protocol to find all the items in the basket from both the root and 
the leaf. We then print the collisions, idle, successes, total probes, success rate and time. 
This program references the website belows as well as my CPSC 441 Assignment 3 for method
to read files. 
//Resources
//https://www.programiz.com/cpp-programming/library-function/cmath/floor
//https://www.geeksforgeeks.org/measure-execution-time-function-cpp/
*/
//inmporting necessary libariries 
#include <iostream>
#include <chrono>
#include <limits.h>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <math.h>
#include <string.h>
#include <iomanip>
using namespace std;
using namespace std::chrono;

//creating global variables from idles from root and success from root 
int rootI;
int rootS;

//rootSearch function recusively calls itself to find the items in the baskets. Depending on items in the 
//current branch the function acts accordinly. Returns the number of collosions. 
int rootSearch(int items[], int low, int high){
    //find the mid point of the branch given 
    int mid = low + floor((high - low) / 2);
    //initilize the collector int
    int collector = 0;
    //iterates through branch to see if there is anything in the basket 
    for(int i = low; i <= high; i++){
        //if there is something in the branch basket then increase collectors by 1
        if(items[i]==1){
            collector++;
        //if there are 2 items in the branch basket then stop the searching we have a collision
        }if(collector > 1){
            break;
        }
    }
    //there is nothting in the branch basket we count this as an idle 
    if(collector == 0){
        rootI++; 
        return 0;
    }
    //there is one item in the branch basket we count this as a success 
    if(collector == 1){
        rootS++;
        return 0;
    }
    //there is more than one item in the branch basket we count this as a collision
    if(collector > 1){
        //recusively call function to look at both under branches, and increase collosion count by 1
        return rootSearch(items,low,mid)+rootSearch(items,mid+1,high)+1;
    }
    return 0; 
}

int main(){
    //set the parameter k
    int k = 10;
    //find the number of items avalible 
    int numItems = pow(2,k);
    //intitialize array to number of items 
    int items[numItems];
    //intitalize the entire basket to empty 
    for(int i = 0; i < numItems;i++){
        items[i]=0;
    }
    //read txt file  
    ifstream file("customer8.txt");
    //initialize input string 
    std::string str;
    //while loop to get one line at a time from text file 
    while (std::getline(file,str)){
        //get item from txt and convert to integer 
        int itemID = std::stoi(str);
        //put 1 to represent customer want this item in the position of the array 
        items[itemID] = 1;
    }
    //close the text file 
    file.close();
    //intitilize two varaibles for success and idles from the leaf search 
    int leafS = 0;
    int leafI = 0;
    //start timer before leaf serach 
    auto start = high_resolution_clock::now();
    //iterate through all leafs and see if there is something in basket 
    for(int i = 0; i < numItems; i++){
        //if item in basket then increase success
        if(items[i]==1){
            leafS++;
        }else{
            //otherwise increase idles 
            leafI++;
        }
    }
    //stop the timer 
    auto stop = high_resolution_clock::now();
    //get change in time 
    auto duration = duration_cast<microseconds>(stop-start);
    //calculute the success rate 
    double leafRate =  ((double)leafS / numItems)*100; 
    //output the results for leaf serach 
    cout << "Leaf level collosion: 0\n";
    cout << "Leaf level idle: " << leafI << "\n";
    cout << "Leaf level success: " << leafS << "\n";
    cout << "Leaf level total time slots: " << numItems << "\n";
    cout << "Leaf level success rate: " << leafRate << "%\n"; 
    cout << "Leaf level time: " << duration.count() << " microseconds\n\n";
    //intialize varalible for root collosion
    int rootC;
    //set global varables to 0 
    rootI = 0;
    rootS = 0;
    //start clock before executing function 
    start = high_resolution_clock::now();
    //call rootSearchh function 
    rootC = rootSearch(items, 0, numItems-1);
    //end clock after execution 
    stop = high_resolution_clock::now();
    //get change in time 
    duration = duration_cast<microseconds>(stop-start);
    //calculate the total time slots needed
    int rootTotal = rootC + rootS + rootI; 
    //calculate the success rate 
    double rootRate =  ((double)rootS / rootTotal)*100; 
    //output the results for root search 
    cout << "Root level collosion: " << rootC << "\n";
    cout << "Root level idle: " << rootI << "\n";
    cout << "Root level success: " << rootS << "\n";
    cout << "Root level total time slots: " << rootTotal << "\n";
    cout << "Root level success rate: " << rootRate << "%\n"; 
    cout << "Root level time: " << duration.count() << " microseconds\n\n";
    return 0; 
}