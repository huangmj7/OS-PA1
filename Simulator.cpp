#include<iostream>
#include<string>
#include<vector>
#include<cmath>
#include "Process.h"
#include "Simulator.h"
using namespace std;


/*Constructor*/
Simulator::Simulator();

/*Muataor*/
int Simulator::generate_simualtion(); //success return 0, violentation on given requrie return -1,else return -2
int Simulator::run(int mode); //1:FCFS 2:SJF 3:SRF 4:RR return -1 if fail else return 0
int Simulator::arrive(Porcess p); //add process to simulator
int Simulator::finish(process p); //remove finish process from simulator
void Simulator::current_process(process p); //set current process when empty
void Simulator::set_cpu(); //set each burst time based on equation:
bool Simulator::IsSwitch(); //is in the switch time
void Simulator::PrintOut(); //print out result in queue



