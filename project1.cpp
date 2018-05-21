/*
Weston Knuppel
Project 1 - Distance Vector Routing Simulator
CS-4310
11/7/2017
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

using namespace std;

string inputFileName;
string outputFileName;
int lastNode = -1;
int totalPackets = 0;
bool roundsFlag = false;

int arrSize = 20;
const int neighborSize = 30;

//Represents and holds DV packets
struct DV{
    int destination;
    int cost;
    int nextHop;
    DV()
    {
        destination = 0;
        cost = 0;
        nextHop = 0;
    }
};
//Represents each Node in topology file
struct Node{
    int name;
    int neighbors[neighborSize];
    DV *dvArray;
    Node()
    {
        name = 0;
        dvArray = new DV[arrSize];
    }
};
//Represents data in topology file
struct Topology{
    int initial;
    int destination;
    int cost;
    Topology()
    {
        initial = 0;
        destination = 0;
        cost = 0;
    }
};

//Resizes the topology array that holds ALL
//the information.
void resize(Topology *&oldArray, int &oldArrSize)
{
    int newArrSize = (oldArrSize/2) + oldArrSize;//Adds half the original size
    Topology *newArray = new Topology[newArrSize];
    for(int i = 0; i < oldArrSize; i++)
        newArray[i] = oldArray[i];

    delete [] oldArray;
    oldArray = newArray;
    oldArrSize = newArrSize;
}

//Checks if file exists and is not empty. If true, reads the file
//then stores the results in and array of Topology structs
void readFile(Topology *&topArray, int &arrSize)
{
    bool flag = true;
    ifstream inFile;
    inFile.open(inputFileName.c_str());
    while(flag)
    {
        //Arbitrary variable to test if a file has
        //any content
        int test;
        //Alter this to check for correct files?
        if(!inFile)
        {
            inFile.close();
            cout << "Error, file does NOT EXIST. Please enter a new file: ";
            cin >> inputFileName;
            cout << endl;
            inFile.open(inputFileName.c_str());
        }
        else if(!(inFile >> test))
        {
            inFile.close();
            cout << "Error, file EMPTY. Please enter a new file: ";
            cin >> inputFileName;
            cout << endl;
            inFile.open(inputFileName.c_str());
        }
        else
        {
            flag = false;
            //**Very Important**
            //Eliminates the "else if" from reading the first
            //Restarts the reading process back to the beginning.
            inFile.close();
            inFile.open(inputFileName.c_str());
        }
    }
    // Brings in all values from file and stores them in
    // their appropriate attributes within the Topology struct.
    int i = 0;
    while(inFile >> topArray[i].initial)
    {
        inFile >> topArray[i].destination;
        inFile >> topArray[i].cost;
        if(i == (arrSize - 1))
            resize(topArray, arrSize);
        i++;
    }
    inFile.close();
}

//Stores each node from top Array into nodeArray.
//Runs through and inserts every "name" and ignores repeats.
void initEachNode(Topology *topArray, Node *nodeArray, int &arrSize)
{
    //Initialize to non zero number to avoid elements
    //named "0" from being ignored.
    for(int i = 0; i < arrSize; i++)
    {
        nodeArray[i].name = -1;
        for(int j = 0; j < neighborSize; j++)
            nodeArray[i].neighbors[j] = -1;;
    }

    int i = 0;
    int j = 0;
    while(topArray[i].cost != 0)
    {
        bool flag1 = true;
        bool flag2 = true;

        int k = 0;
        while(topArray[k].cost != 0)
        {
            if(topArray[i].initial == nodeArray[k].name)
                flag1 = false;
            if(topArray[i].destination == nodeArray[k].name)
                flag2 = false;
            k++;
        }
        if(flag1)
        {
            nodeArray[j].name = topArray[i].initial;
            j++;
        }
        if(flag2)
        {
            nodeArray[j].name = topArray[i].destination;
            j++;
        }
        i++;
    }
}

//Initializes each node to represent each node "knowing" its neighbors
void fillNodeInfo(Topology *topArray, Node *nodeArray)
{
    int i = 0;
    while(nodeArray[i].name != -1)
    {
        int j = 0;
        int k = 0;
        while(topArray[k].cost != 0)
        {
            if(nodeArray[i].name == topArray[k].initial)
            {
                nodeArray[i].dvArray[j].destination = topArray[k].destination;
                nodeArray[i].dvArray[j].cost = topArray[k].cost;
                nodeArray[i].dvArray[j].nextHop = topArray[k].destination;
                nodeArray[i].neighbors[j] = topArray[k].destination;
                j++;
            }
            else if(nodeArray[i].name == topArray[k].destination)
            {
                nodeArray[i].dvArray[j].destination = topArray[k].initial;
                nodeArray[i].dvArray[j].cost = topArray[k].cost;
                nodeArray[i].dvArray[j].nextHop = topArray[k].initial;
                nodeArray[i].neighbors[j] = topArray[k].initial;
                j++;
            }
            k++;
        }
        i++;
    }
}

//Evaluates the DV Packet that is brought in and determines if an
//update is necessary. If the destination is not found it adds its
//info to the target routing table.
void processDvPacket(Node *nodeArray, Node *tempArr, DV *DvPacket, int neighbor, int nextHOP)
{
    int neighborCost;
    //this flag attempts to increase efficiency
    //by stoping the loop when the neighbors cost is found
    bool neighCostFlag = false;
    //this flag attempts to increase efficiency
    //by stoping the loop when the neighbors is found
    bool ifFoundFlag = false;
    //this flag is set to check of a destination already
    //exists. If it does, we compares cost of routes. If
    //it does not then we add the new route.
    bool packetMatchFlag;
    int i = 0;
    int j;
    int k;

    while(DvPacket[i].destination != -1 && !neighCostFlag)
    {
        if(DvPacket[i].destination == neighbor)
        {
            neighborCost = DvPacket[i].cost;
            neighCostFlag = true;
        }
        i++;
    }

    i = 0;
    while(tempArr[i].name != -1 && !ifFoundFlag)
    {
        if(tempArr[i].name == neighbor)
        {
            ifFoundFlag = true;
            j = 0;
            while(DvPacket[j].cost != 0)
            {
                if(tempArr[i].name != DvPacket[j].destination)
                {
                    packetMatchFlag = false;
                    k = 0;
                    while(tempArr[i].dvArray[k].cost != 0)
                    {
                        if(DvPacket[j].destination == tempArr[i].dvArray[k].destination)
                        {
                            packetMatchFlag = true;
                            if((neighborCost + DvPacket[j].cost) < tempArr[i].dvArray[k].cost)
                            {
                                nodeArray[i].dvArray[k].nextHop = nextHOP;
                                nodeArray[i].dvArray[k].cost = (neighborCost + DvPacket[j].cost);
                                //tests if anything changed in the round.
                                roundsFlag = true;
                                //holds the last node to change
                                lastNode = tempArr[i].name;
                            }
                        }
                        k++;
                    }
                    if(!packetMatchFlag)
                    {
                        nodeArray[i].dvArray[k].destination = DvPacket[j].destination;
                        nodeArray[i].dvArray[k].cost = (DvPacket[j].cost + neighborCost);
                        nodeArray[i].dvArray[k].nextHop = nextHOP;
                        //tests if anything changed in the round.
                        roundsFlag = true;
                        //holds the last node to change
                        lastNode = tempArr[i].name;
                    }
                }
                j++;
            }
        }
        i++;
    }
}

//Prepares a DV Packet to be sent to each nodes neighbors.
void updateNeighbors(Node *nodeArray, int arrSize)
{
    Node *tempArr = new Node[arrSize];
    DV *DvPacket;
    int neighbor;
    int nextHOP;
    //initialized to false to dictate nothing has changed so far
    roundsFlag = false;
    //Initialize tempArr.name and tempArr.neighbors to -1
    for(int i = 0; i < arrSize; i++)
    {
        tempArr[i].name = -1;
        for(int j = 0; j < neighborSize; j++)
            tempArr[i].neighbors[j] = -1;;
    }
    //Put everything from nodeArray int tempArray
    int  i = 0;
    while(nodeArray[i].name != -1)
    {
        tempArr[i] = nodeArray[i];
        i++;
    }

    i = 0;//cycles nodes in temp
    int j;//cycles neighbors in that nodes
    int k;//cycles nodes in temp again to match a node to the neighbor
    while(tempArr[i].name != -1)
    {
        nextHOP = tempArr[i].name;
        DvPacket = new DV[arrSize];
             j = 0;
             while(tempArr[i].dvArray[j].cost != 0)
             {
                 DvPacket[j] = tempArr[i].dvArray[j];
                 j++;
             }
             k = 0;
             while(tempArr[i].neighbors[k] != -1)
             {
                 neighbor = tempArr[i].neighbors[k];
                 processDvPacket(nodeArray, tempArr, DvPacket, neighbor ,nextHOP);
                 //records total packets sent.
                 totalPackets++;
                 k++;
             }

        delete [] DvPacket;
        i++;
    }
    delete [] tempArr;
}

int main(int argc, char *argv[])
{
    inputFileName = argv[1];
    int rounds = 0;
    rounds = std::atoi(argv[2]);

    int roundsCounter = 0;
    Topology *topArray = new Topology[arrSize];
    //Stores info after file check
    readFile(topArray, arrSize);
    //Needs to be initialized here to avoid
    //exceeding boundaries after arrSize
    //is resized.
    Node *nodeArray = new Node[arrSize];
    initEachNode(topArray, nodeArray, arrSize);
    fillNodeInfo(topArray, nodeArray);

    //runs the update the number of rounds inputed
    for(int i = 0; i < rounds; i++)
    {
       updateNeighbors(nodeArray, arrSize);
       if(roundsFlag)
        roundsCounter++;
    }

    //*** BEGIN TESTING AFTER FULL CONVERGENCE ********************************
    //
    ofstream outFile;
    outputFileName = "Results" + inputFileName;
    outFile.open(outputFileName.c_str());
    int i = 0;
    outFile << inputFileName << endl;
    while(nodeArray[i].name != -1)
    {
        int j = 0;
        outFile << "NODE # " << nodeArray[i].name << endl;
        outFile << setw(17) << left <<"Destination"
             << setw(10) << left << "Cost"
             << "NextHop" << endl;//setw(5) << left <<"NextHop" << endl;
        //cout << "    nodeArray[i].name == " << nodeArray[i].name << endl;
        while(nodeArray[i].dvArray[j].cost != 0)
        {
            //cout << "Destination: " << nodeArray[i].dvArray[j].destination
                 //<< "Cost: " << nodeArray[i].dvArray[j].cost << endl;
            outFile << setw(17) << left << nodeArray[i].dvArray[j].destination
                 << setw(10) << left << nodeArray[i].dvArray[j].cost
                 << nodeArray[i].dvArray[j].nextHop << endl;
            j++;
        }
        outFile << endl;
        i++;
    }
    outFile << "Total Rounds: " << roundsCounter<< endl;
    outFile << "ID of Last Node: " << lastNode << endl;
    outFile << "Total DV Messages: " << totalPackets << endl << endl;

    int destination = 0;
    i = 0;
    int j;
    bool flag = false;
    if(inputFileName == "topology1.txt")
    {
        outFile << "Path from Node 0 to Node 3" << endl;
        outFile << "Source     : Node " << destination << endl;
        while(destination != 3)
        {
            if(nodeArray[i].name == destination)
            {
                j = 0;
                while(nodeArray[i].dvArray[j].cost != 0 && (!flag))
                {
                    if(nodeArray[i].dvArray[j].destination == 3)
                    {
                        destination = nodeArray[i].dvArray[j].nextHop;
                        flag = true;
                        outFile << "Next Hop   : Node " <<destination << endl;
                    }
                    j++;
                }
                flag = false;
                i = -1;
            }
            i++;
        }
        outFile << "Destination: Node " << destination << endl;
    }
    else if(inputFileName == "topology2.txt")
    {
        outFile << "Path from Node 0 to Node 7" << endl;
        outFile << "Source     : Node " << destination << endl;
        while(destination != 7)
        {
            if(nodeArray[i].name == destination)
            {
                j = 0;
                while(nodeArray[i].dvArray[j].cost != 0 && (!flag))
                {
                    if(nodeArray[i].dvArray[j].destination == 7)
                    {
                        destination = nodeArray[i].dvArray[j].nextHop;
                        flag = true;
                        outFile << "Next Hop   : Node " <<destination << endl;
                    }
                    j++;
                }
                flag = false;
                i = -1;
            }
            i++;
        }
        outFile << "Destination: Node " << destination << endl;
    }
    else if(inputFileName == "topology3.txt")
    {
        outFile << "Path from Node 0 to Node 23" << endl;
        outFile << "Source     : Node " << destination << endl;
        while(destination != 23)
        {
            if(nodeArray[i].name == destination)
            {
                j = 0;
                while(nodeArray[i].dvArray[j].cost != 0 && (!flag))
                {
                    if(nodeArray[i].dvArray[j].destination == 23)
                    {
                        destination = nodeArray[i].dvArray[j].nextHop;
                        flag = true;
                        outFile << "Next Hop   : Node " <<destination << endl;
                    }
                    j++;
                }
                flag = false;
                i = -1;
            }
            i++;
        }
        outFile << "Destination: Node " << destination << endl;
    }
    else
    {
        cout << "Please enter the one of the three topology "
             << "options..." << endl;
        cout << "For example: topology1.txt" << endl;
    }
    //*** END TESTING AFTER FULL CONVERGENCE **********************************
    outFile.close();
    delete [] topArray;
    delete [] nodeArray;
    return 0;
}
