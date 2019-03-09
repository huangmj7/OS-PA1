#include<iostream>
#include<string>
#include<vector>
#include<cmath>
#include "Process.h"
using namespace std;

Process::Process(char Name,int io,int cpu,int arrive){

	name = Name;
        state = 1;      //1:read; 2:running: 3:block
        io_time = io;
        cpu_burst = cpu;
	arrive_time = arrive;

	wait_time = 0;
        turnaround_time = 0;
        cpu_remain = cpu_burst;
        io_remain = io_time;
        preemptive = 0;

}

Process::Process(const Process &p){

	name = p.name;
        state = p.state;      //1:read; 2:running: 3:block
        io_time = p.io_time;
        cpu_burst = p.cpu_burst;

        wait_time = p.wait_time;
        turnaround_time = p.turnaround_time;
        cpu_remain = p.cpu_remain;
        io_remain = p.io_remain;
        preemptive = p.preemptive;

}

bool Process::IsFinished() const{
	if(io_remain == 0 && cpu_remain == 0){return true;}
	else{return false;}
}






