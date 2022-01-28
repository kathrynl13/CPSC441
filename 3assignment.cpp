//CPSC 441 Fall 2021: Assignment 3
//The programs reads topology file canada.txt and creates of adjacency matrix of the graph
//the program then runs dijkstras algorithm and finds the shortest path from YCC to 
//any other airport city on the text file. 
//To complie "g++ -o 3assignment.cpp" and run "./3assignment"
//Make sure that text file "canada.txt" is in same folder when running 
//This code heavily references the implementation of dijkstras algorithm from:
//https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/
//Other references used are: 
//http://www.fredosaurus.com/notes-cpp/io/readtextfile.html
//https://stackoverflow.com/questions/13035674/how-to-read-a-file-line-by-line-or-a-whole-text-file-at-once
//https://java2blog.com/split-string-space-cpp/
//https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/

//importing necessary modules 
#include <iostream>
//for use of INT_MAX representing position infinity
using namespace std;
#include <limits.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <string.h>
#include <iomanip>
using namespace std;

//defining number of vertecies in graph 
#define VERTEX 21
 
//pathHelper prints the shortest path of the alogorithm 
//pathHelper takes in previous array, the position in array, the places array, and a counter
//if a value is found in preious array at the postion then recursive call function and 
//print the airport city of the position 
//count represents the number of hops and is returned by the function 
int pathHelper(int previous[], int i, string places[], int count){
    //if previous is empty then just return the number of hops 
    if (previous[i] == -1){
        return count;
    }else{
        //recusively call pathHelper to find next airport city on path 
        count = pathHelper(previous, previous[i], places, count);
        //print the airport city on the path 
        cout << "-->" << places[i]; 
        //increase the number of hops by one
        return count+1;
    }
}

//printer function prints all of the output for the shortest path 
//this includes destination, cost, shortest path, and hops in tabular form 
//funtion takes in distance array, previous array, and places array 
void printer(int distance[], int previous[], string places[]){
    //initialize count, aka hops 
    int count = 0;
    //initialize start to be YYC
    string start = "YYC";
    //print header titiles ensuring they are in line 
    cout << "\n\nDestination"
        << setw(10) << "Cost"
        << setw(20) << "Shortest Path"
        << setw(40) << "Hops";

    //iterate through all places and print cost, shortest path, and hops 
    for (int i = 1; i < VERTEX; i++){  
        //only show the output for YYZ, YQB, YOW the three CAN places 
        //comment out the if statement to see only three CAN places 
        if (places[i] == "YYZ" || places[i] == "YQB" ||places[i] == "YOW"){
            //print the places, shortest distance
            cout << "\nYCC-->" << places[i] << setw(10) <<  distance[i] << setw(10) << start;
            //print the shortest path 
            count = pathHelper(previous, i, places, 0);
            //print the hops and algin 
            cout << setw(50-(count*(6)))  << count;
            //set hops to 0 for next iteration 
            count = 0;
        }
    }
    //format output 
    cout << endl;
    cout << endl;
}

//minimumDistance function finds the minimum distance from looking at nodes we have not serached yet
int minimumDistance(int distance[], bool tested[]){
    //initilize min to position infinity 
    int min = INT_MAX;
    //initialize currentMin
    int currentMin;
    //iterate through all nodes 
    for (int i = 0; i < VERTEX; i++){
        //if the node we are looking at distance is less than the min and we have not serached node yet then 
        if (distance[i] <= min && tested[i] == false){
            //set the distance of the node to the new min
            min = distance[i];
            //set the currentMin/return value to the index node position 
            currentMin = i;
        }
    }
    //return the index node position of the minimum distance 
    return currentMin;
}

//algorithm is an implementation of dijkstras algorithm
void algorithm(int graph[VERTEX][VERTEX], int start, string places[]){
    //initialize arrays to be size of number of nodes 
    //distance is the distance from start to each node 
    int distance[VERTEX];
    //tested represented if we have search the node before 
    //serached = true, unsearched = false 
    bool tested[VERTEX];
    //previous tracks the previous nodes on the shortest path 
    int previous[VERTEX];
    //initialize all arrays to starting values 
    for (int i = 0; i < VERTEX; i++){
        //start distance is position infinity for all
        distance[i] = INT_MAX;
        //start tested is false, aka unsearched for all
        tested[i] = false;
        //previous is to -1 
        previous[i] = -1;
    }
    //set the starting node to zero, in this case it is YCC 
    distance[start] = 0;
    //iterate through all nodes 
    for (int i = 0; i < (VERTEX - 1); i++){
        //calculate the current minimum distance 
        int x = minimumDistance(distance, tested);
        //set the minimum distance index node to searched 
        tested[x] = true;
        //iterate through all nodes 
        for (int j = 0; j < VERTEX; j++){
            //if current index node position is not in tested, is in the graph matrix, and there is a shorter distance path 
            if (!tested[j] && graph[x][j] && distance[x]!=INT_MAX && distance[x] + graph[x][j] < distance[j]){
                //undate the distance to be the distance of the minimum distance position plus the current node we are seraching 
                distance[j] = distance[x] + graph[x][j];
                //update the previous to be index of the node in mimimum distance position 
                previous[j] = x;
            }
        }
    }
    //call function printer to print the output of the shortest path 
    printer(distance, previous, places);
}


//main function reads the files canada.txt and converts to graph a adejacency matrix 
//main function then calls the algorithm to run and print 
int main(){
    //intitial indec to 0, index will be used to create places array 
    int index = 0;
    //initialize places array to keep track of all cities 
    string places[VERTEX];
    //initialize graph as adjancey matrx, set all values to 0
    int graph[VERTEX][VERTEX]={0};
    //initialize counters i and j
    int i, j;
    //print the graph, to check graph is empty 
    for(i = 0; i < VERTEX; i++) {
        for(j = 0; j < VERTEX; j++) {
            cout << graph[i][j] << " ";
    }
        cout << endl;
    }
    //open file canada.txt to read 
    ifstream file("canada.txt");
    //initialize input string str 
    std::string str;
    //while loop to get one line at a time from text file 
    while (std::getline(file,str)){
        //cout << str << endl;
        //sting buffer to split strings 
        istringstream ss(str);
        //initialize strings to hold different values of each input line 
        string area1;
        string area2;
        string dis; 
        //split first city of input line into area1
        getline(ss, area1, ' ');
        //initilize ints to keeps index numbers 
        int area1index;
        int area2index;
        //initilize tracker bool values to false 
        bool update = false;
        //check if area1 is in the places array
        for (int i = 0; i < VERTEX; i++){
            //if area1 is already in the places array 
            if (places[i] == area1){
                //update tracker bool value to true 
                update = true; 
                //update the index of area1
                area1index = i; 
            }
        }
        //if area1 not in places array then add to places array
        if (update == false){
            //add to next avalible position in places array then increase index 
            places[index++] = area1;
            //get the position where area1 was stored 
            int temp = index-1;
            area1index = temp;
        }
        //reset bool tracker to false 
        update = false;
        //split second city of input line into area2
        getline(ss, area2, ' ');
        //check if area2 is in the places array 
        for (int j = 0; j < VERTEX; j++){
            //if area2 is already in the places array 
            if (places[j] == area2){
                //update tracker bool value to true 
                update = true; 
                //update the index of area2
                area2index = j;
            }
        }
        //if the area2 not in places array then add to places 
        if (update == false){
            //add to next avalible position in places array then increase index 
            places[index++] = area2;
            //get the position where area2 was stored 
            int temp = index -1;
            area2index = temp;
        }
        //split distance of input line into dis 
        getline(ss, dis, ' ');
        //convert dis from a string to a int 
        int disint = std::stoi(dis);
        //add the distance to the respective positions in the adjacency matrix graph 
        //since matrix needs to be added twice 
        graph[area2index][area1index] = disint; 
        graph[area1index][area2index] = disint; 
    }
    //close the text file 
    file.close();
    //print the places array, to see the positions in the matrix 
    for (int i = 0; i < VERTEX; i++){
        cout << i << ": "<< places[i] << endl;
    }
    //print the graph adjanceny matrix 
    for(i = 0; i < VERTEX; i++) {
        for(j = 0; j < VERTEX; j++) {
            cout << graph[i][j] << " ";
    }
    //format output 
    cout << endl;
    }
    //call the graph algorithm to find the shortest path for all respective nodes 
    algorithm(graph, 0, places);
    return 0;
}