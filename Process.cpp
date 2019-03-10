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
	wait_time = 0;
        turnaround_time = 0;
        cpu_remain = cpu_burst[0];
        io_remain = io_time[0];

        cpu_ptr = cpu_burst.begin();
        io_ptr = cpu_burst.begin();
        preemptive = 0;

	
}

Process::Process(char Name,float lamda,float limit){

	name = Name;
        state = 1;      //1:ready; 2:running: 3:block
	time_generation(limit,lamda);

	wait_time = 0;
        turnaround_time = 0;
        cpu_remain = cpu_burst[0];
        io_remain = io_time[0];

	cpu_ptr = cpu_burst.begin();
	io_ptr = cpu_burst.begin();
        preemptive = 0;

}

Process::Process(const Process &p){

	name = p.name;
        state = p.state;      //1:read; 2:running: 3:block
        
	io_time = p.io_time;
        cpu_burst = p.cpu_burst;
	interarrival = p.interarrival;

        wait_time = p.wait_time;
        turnaround_time = p.turnaround_time;
        
	cpu_remain = p.cpu_remain;
        io_remain = p.io_remain;

	cpu_ptr = p.cpu_ptr;
	io_ptr = p.io_ptr;

        preemptive = p.preemptive;

}

bool Process::IsBurstFinished(){
	
	if(io_ptr == io_time.end() || cpu_ptr == cpu_burst.end()){return false;}
	bool rv = false;
	/*io*/
	if(io_remain == 0){
		
		io_ptr ++;
		io_remain = *io_ptr;
		state = 1;
		rv = true;
			
	
	}
        
	/*cpu*/
	if(cpu_remain == 0){

		cpu_ptr ++;
		cpu_remain = *cpu_ptr;
		if(cpu_ptr != cpu_burst.end()){state = 3;} // enter io unless it is the last burst
		rv = true;
	
	}

	return rv;
}
                
bool Process::IsProcessFinished() const{return (io_ptr == io_time.end() && cpu_ptr == cpu_burst.end());}

void Process::time_generation(float limit,float lamda){

	arrive_time = drand48()*1000; //we used int instead float
	cout << "Arrival: " << arrive_time  << endl;

	int num_of_burst =  drand48()*100 + 1; //1-100
	cout << "#b: " <<num_of_burst << endl;
	int actual_cpu_time =  drand48()*1000;
	cout <<"#cpu time: " <<actual_cpu_time << endl;
	int actual_io_time =  drand48()*1000;
	num_of_burst = 2;//test
	//cout << "L: "<< lamda << endl;

	for(int h=0; h<num_of_burst; h++){

		if(h < (num_of_burst -1) ){io_time.push_back(actual_io_time);}
		cpu_burst.push_back(actual_cpu_time);
	}

	interarrival.push_back(arrive_time);
	for(int i=1; i<num_of_burst; i++){
		double r = drand48();
		int time = -log(r) / lamda;
		
		//cout << "t= "<<time << endl;
		
		if(time > limit) {
                        cout << "S "<< time << endl;
                        i--;
                        continue;
                }

		int x = i-1;
		time += interarrival[x]; //in ascending order
		cout <<"T: " <<time << endl;
		interarrival.push_back(time);
	}

}



     
bool Process::IsArrive(int time){

	for(int i=0; i< interarrival.size(); i++){
		if(interarrival[i] == time){return true;}
	}
	return false;

}

