#include<iostream>
#include<string>
#include<vector>
#include<cmath>
#include "Process.h"
using namespace std;

Process::Process(char Name, float io,float cpu){

	name = Name;
        state = 1;      //1:read; 2:running: 3:block
        io_time = io;
        cpu_burst = cpu;

	wait_time = 0;
        turnaround_time = 0;
        cpu_remain = 0;
        io_remain = 0;
        preemptive = 0;

}

vector<float> Process::get_times(int &prem) const{

	prem = preemptive;
	vector<float> ans(4,0.0);
	ans[0] = wait_time;
	ans[1] = turnaround_time;
	ans[2] = cpu_remain;
	ans[3] = io_remain;
	return ans;



}

Process::Process(const Process &p){

	name = p.name;
        state = p.state;      //1:read; 2:running: 3:block
        io_time = p.io_time;
        cpu_burst = p.cpu_burst;

	int prem = -1;
	vector<float> Targv = p.get_times(prem);

        wait_time = Targv[0];
        turnaround_time = Targv[1];
        cpu_remain = Targv[2];
        io_remain = Targv[3];
        preemptive = prem;

}


void Process::add_turnaround(){turnaround_time++;}

void Process::add_wait(){wait_time++;}

void Process::add_preemptive(){preemptive++;}

void Process::run(int s){

	state = s;
	if(s == 2){cpu_remain --;}
	else if(s == 3){io_remain --;}
	
}


float Process::get_remain(int type) const{

	float io = io_remain;
	float cpu = cpu_remain;

	if(type == 1){return cpu;}
	if(type == 2){return io;}
}  //1:cpu 2:io

bool Process::IsFinished() const{
	if(io_remain == 0 && cpu_remain == 0){return true;}
	else{return false;}
}






