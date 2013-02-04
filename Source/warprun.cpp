/*
 * Onur Yilmaz
 * CENG 315
 * HW #4
 *
 */

/*
 * The Push-Relabel Algorithm is implemented in this homework
 * Steps of the algorithm is taken from the lecture notes:
 * URL: http://theory.stanford.edu/~trevisan/cs261/lecture12.pdf
 *
 */

// Headers
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

// Definitions
#define MAX 2000
#define BIGM 1000

using namespace std;

// Mario related variables
long int livesOfMario;
long int numberOfLevels;
long int numberOfWarpGates;
long int endingLives;

// Matrices and lists
int CapacityMatrix[MAX][MAX] = {0};
int FlowMatrix[MAX][MAX] = {0};
int ExcessList[MAX] = {0};
int SeenList[MAX] = {0};
int LabelList[MAX] = {0};
int ExtraLife[MAX] = {0};
string LevelNames[MAX];
long int Limit[MAX] = {0};

// Function declarations
void SendFromTo(int, int);
void ChangeLabel(int);
void CheckNode(int);
void PushRelabelAlgorithm();

// Main function for Push-Relabel Algorithm
void PushRelabelAlgorithm() {

    // Global variables
    extern int FlowMatrix[MAX][MAX];
    extern long int numberOfLevels;
    extern long int endingLives;
    extern int ExcessList[MAX];

    // Initialization
    LabelList[0] = numberOfLevels;
    ExcessList[0] =     ExcessList[0] + livesOfMario;

    // Initial preflow
    for (int i = 0; i < numberOfLevels; i++)
        SendFromTo(0, i);

    // Main loop for algorithm
    // For each node
    for (int nodeToCheck = 1; nodeToCheck < numberOfLevels - 1;) {

        // Save label to temp and check node
        int temp = LabelList[nodeToCheck];
        CheckNode(nodeToCheck);

        // Start from first node
        if (LabelList[nodeToCheck] > temp) {
            nodeToCheck = 1;
        }// Else next node
        else
            nodeToCheck = nodeToCheck + 1;
    }

    // Calculate the ending lives of Mario
    for (int i = 0; i < numberOfLevels; i++)
        endingLives = endingLives + FlowMatrix[i][numberOfLevels - 1];
    endingLives = endingLives + ExtraLife[numberOfLevels - 1];
}

// Send available amount from "from" to "to"
void SendFromTo(int from, int to) {

    // Global variables
    extern int FlowMatrix[MAX][MAX];
    extern int CapacityMatrix[MAX][MAX];
    extern int ExcessList[MAX];

    // Available amount
    int Available = CapacityMatrix[from][to] - FlowMatrix[from][to];
    int Amount = 0;

    // If available is larger send as excess amount
    if (ExcessList[from] <= Available ) {
        Amount = ExcessList[from];
        ExcessList[from] = ExcessList[from] - Amount;
        ExcessList[to] = ExcessList[to] + Amount;
    }
    // If available is smaller send as available amount
    else {
        Amount = Available;
        ExcessList[from] = ExcessList[from] - Amount;
        ExcessList[to] = ExcessList[to] + Amount;
    }

    // Send calculated amount to flow matrix
    FlowMatrix[from][to] = FlowMatrix[from][to] + Amount;
    FlowMatrix[to][from] = FlowMatrix[to][from] - Amount;
}

// Check node for excess augmenting or changing its label
void CheckNode(int from) {

    // Global variables
    extern int FlowMatrix[MAX][MAX];
    extern int CapacityMatrix[MAX][MAX];
    extern long int numberOfLevels;
    extern int SeenList[MAX];

    // While there is excess and limit not exceeded
    while ( ExcessList[from] > 0 && Limit[from] < numberOfLevels*numberOfLevels) {

        //If not all neighbors have been tried since last relabel
        if (SeenList[from] < numberOfLevels) {

            int to = SeenList[from];
            Limit[from]++;

            //Try to push flow to an untried neighbor
            int Available = CapacityMatrix[from][to] - FlowMatrix[from][to];
            if ((Available > 0) && (LabelList[from] > LabelList[to])) {
                    SendFromTo(from, to);
            }
            // Else increase its checked occurrence
            else {
                SeenList[from] += 1;
            }
        }// Else change the label
        else {
            Limit[from]++;
            ChangeLabel(from);
            SeenList[from] = 0;
        }
    }
}

// Function for changing the label of node according to its neighbors
void ChangeLabel(int from) {

    // Global variables
    extern int FlowMatrix[MAX][MAX];
    extern int CapacityMatrix[MAX][MAX];
    extern long int numberOfLevels;
    extern int LabelList[MAX];

    // Temporary label
    int temp = BIGM;

    // Neighbors
    int to;

    // For each neighbor
    for (to = 0; to < numberOfLevels; to++) {
        // If available amount is larger than zero
        int Available = CapacityMatrix[from][to] - FlowMatrix[from][to];
        if (Available > 0) {
            // Change the label to the smallest of its neighbors
            if (temp >= LabelList[to])
                temp = LabelList[to];
            // Increase its own label
            LabelList[from] = temp + 1;
        }
    }
}

// Main function
int main() {

    // Global variables
    extern int CapacityMatrix[MAX][MAX];
    extern long int numberOfLevels;
    extern long int endingLives;
    extern int ExcessList[MAX];
    extern int ExtraLife[MAX];

    // Open input file
    ifstream myReadFile;
    myReadFile.open("warprun.inp");

    // Read the first line
    myReadFile >> livesOfMario;
    myReadFile >> numberOfLevels;
    myReadFile >> numberOfWarpGates;
    // cout << "test: " << livesOfMario << " " << numberOfLevels << " " << numberOfWarpGates << endl;

    // Read extra lives
    for (int i = 0; i < numberOfLevels; i++) {
        myReadFile >> LevelNames[i];
        myReadFile >> ExcessList[i];
        ExtraLife[i] = ExcessList[i];
    }

    // Read arc capacities
    for (int i = 0; i < numberOfWarpGates; i++) {
        int from;
        int to;
        int capacity;
        int j = 0;

        string fromName;
        string toName;

        myReadFile >> fromName >> toName >> capacity;

        // Find the first name
        for (; j < numberOfLevels; j++) {
            if (fromName.compare(LevelNames[j]) == 0) {
                from = j;
                break;
            }
        }

        j = 0;
        // Find the second name
        for (; j < numberOfLevels; j++) {
            if (toName.compare(LevelNames[j]) == 0) {
                to = j;
                break;
            }
        }
        // Save capacities
        CapacityMatrix[from][to] = capacity;
        CapacityMatrix[to][from] = capacity;
    }
    // Close file connection
    myReadFile.close();

    // Call the main algorithm
    PushRelabelAlgorithm();

    // Open the output file
    ofstream outputFile("warprun.out");

    // Combining output
    if (endingLives == 0)
        outputFile << "GAME OVER" << endl;
    else {

        outputFile << endingLives << endl;
        for (int i = 0; i < numberOfLevels; i++)
            for (int j = 0; j < numberOfLevels; j++)
                if (FlowMatrix[i][j] > 0)
                    outputFile << LevelNames[i] << " " << LevelNames[j] << " " << FlowMatrix[i][j] << endl;
    }

    // Close the output file
    outputFile.close();

    return 0;
}
// End of code
