#include<iostream>
#include<cstdlib>
#include<string>
#include<vector>
#include<cmath>
#include "Process.h"
using namespace std;

Process::Process(){

	name = ' ';
	state = 0;

	number_of_burst = 0;   //range from [1,100]
        tau  = 0;

	wait_time = 0;
        turnaround_time = 0;
        cpu_remain = cpu_burst[0];
        io_remain = io_time[0];

        cpu_ptr = 0;
        io_ptr = 0;
        preemptive = 0;
	finished = false;

	
}

Process::Process(char Name,float lamda,float limit){

	name = Name;
        state = 1;      //1:ready; 2:running: 3:block
        tau = 1/lamda;

	time_generation(limit,lamda);
	number_of_burst = cpu_burst.size();

	wait_time = 0;
        turnaround_time = 0;
        cpu_remain = cpu_burst[0];
        io_remain = io_time[0];

	cpu_ptr = 0;
	io_ptr = 0;
        preemptive = 0;
	finished = true;

}

Process::Process(const Process &p){

	name = p.name;
        state = p.state;      //1:read; 2:running: 3:block
        
	io_time = p.io_time;
        cpu_burst = p.cpu_burst;
	interarrival = p.interarrival;
	number_of_burst = p.number_of_burst;   //range from [1,100]
        tau = p.tau;
	arrive_time = p.arrive_time;

        wait_time = p.wait_time;
        turnaround_time = p.turnaround_time;
        
	cpu_remain = p.cpu_remain;
        io_remain = p.io_remain;

	cpu_ptr = p.cpu_ptr;
	io_ptr = p.io_ptr;

        preemptive = p.preemptive;
	finished = p.finished;

}


bool Process::IsCPUFinished() const {return ((state == 1 || state == 2) && cpu_remain == 0);} //true if state is 1 or 2 and cpu remain == 0, false if the state is 3 or cpu_remain != 0
bool Process::IsIOFinished() const {return (io_remain == 0 && state == 3);} // true if state is 3 and io_remain == 0; false else 

int Process::update(int s){

	if(s == 3){

		if(io_remain != 0 || io_ptr == io_time.size()){return -1;} //only update when io_remain == 0

		state = 1; //ready
		io_ptr ++;
		io_remain = io_time[io_ptr];
		return 1;
	} //update io

	if(s == 1 || s == 2){

		if(cpu_remain != 0 || cpu_ptr == cpu_burst.size()){return -1;} //only update when cpu_remain == 0

		state = 3;
		cpu_ptr ++;
		cpu_remain = cpu_burst[cpu_ptr];
		return 3;
	
	}
}
                
bool Process::IsProcessFinished() const{
	//cout << *io_ptr << endl;
	//cout << *cpu_ptr << endl;
	
	return (io_ptr == io_time.size() && cpu_ptr == cpu_burst.size());
}

void Process::time_generation(float limit,float lamda){

        while(1){
		double ra = drand48();
		arrive_time = -log(ra) / lamda; //we used int instead float
		if(arrive_time < limit){break;}
	}

	int num_of_burst =  drand48()*100 + 1; //1-100
	//cout << "#b: " <<num_of_burst << endl;
	//num_of_burst = 3; //test

	interarrival.push_back(arrive_time);
	
	bool c1 = true;
	bool c2 = true;
	while(c1 || c2){

                if(c1){ 
			while(1){
				double rc = drand48();
				int t_cpu = -log(rc) / lamda;
				if(t_cpu < limit){
					cpu_burst.push_back(t_cpu+1);
					break;
				}
			}

		}

		if(c2){
			while(1){
				double ri = drand48();
				int t_io = -log(ri) / lamda;
				if(t_io < limit){
					io_time.push_back(t_io+1);
					break;
				}
			}
		}

		if(cpu_burst.size() == num_of_burst){c1 = false;}
		if(io_time.size() == (num_of_burst - 1)){c2 = false;}

		//cout << "C: " << cpu_burst.size() << "I: " << io_time.size() << endl;
	}

	for(int i=0; i<cpu_burst.size(); i++){

		int time = cpu_burst[i] + interarrival[i];
		interarrival.push_back(time);
	}


}



     
bool Process::IsArrive(int time) const{

	
	/*
	for(int i=0; i< interarrival.size(); i++){
		if(interarrival[i] == time){return true;}
	}
	return false;
	*/

	return (time==arrive_time);

}

void Process::output(){

	cout << "Process " << name << "[NEW] (arrival time "<< arrive_time <<" ms) "<< cpu_burst.size()  <<" CPU bursts" << endl;
	for(int i=0; i<cpu_burst.size()-1; i++){
                    	
		cout << "--> CPU burst "<< cpu_burst[i] << " ms";
		cout << "--> I/O burst " << io_time[i] << " ms" << endl;
	}
	cout << "--> CPU burst " << cpu_burst[cpu_burst.size()-1] << " ms " <<endl;

}
