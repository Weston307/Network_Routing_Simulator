
Steps in order to run project1.cpp
Get into the correct directory
	- cd Project1
Compile code
-	g++ -o project1 project1.cpp
Run the program
	- ./project1 topology1.txt 1
	- ./project1 topology2.txt 3
	- ./project1 topology3.txt 4


In order to view the path that the simulator takes when sending a packet from 0 – 3, 0 – 7, or 0 – 23, the name of the input file must be identical to the above statements.
When executing this program, each topology file will result in a file called Results and then the topology name. For example “Resultstopology1.txt”
This file will have the routing table of each node of the network, the number of rounds, the ID of the last node to converge, the total DV messages, and the path the used to send a packet from source to destination.
I chose to do it this way so you could see exactly what the result was after each execution. This is separate from the “FinalResults” document which has all of the above info for all the topology files.
