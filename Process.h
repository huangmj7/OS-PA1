#include<iostream>
#include<string>
#include<vector>
using namespace std;

class Process{

	public:
		char name;
		int state;      //1:read; 2:running: 3:block
		float io_time;
		float cpu_burst;

                /*Constructor*/
		Process(char Name,float io,float cpu);
		Process(const Process &p); //copy

		/*Mutator*/
		void add_turnaround(); 
		void add_wait();
		void add_preemptive();
		void run(int s);

		/*Observer*/
		float get_remain(int type) const;  //1:cpu 2:io
		vector<float> get_times(int &prem) const; //only for copy and final output
		bool IsFinished() const; //true if finished, false felse

	
	private:
		float wait_time;
		float turnaround_time;
		float cpu_remain;
		float io_remain;	
		int preemptive;	

};

