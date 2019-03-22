#include<iostream>
#include<string>
#include<vector>
using namespace std;

class Process {

public:

	int arrive_time; //process arrive time
	int wait_time;
	int turnaround_time;
	char name;
	int cpu_remain;
	int io_remain;

	Process(int arr, int wait, int turn, int cpu, int io, char n) {
		arrive_time = arr;
		wait_time = wait;
		turnaround_time = turn;
		name = n;
		cpu_remain = cpu;
		io_remain = io;
	}
};
