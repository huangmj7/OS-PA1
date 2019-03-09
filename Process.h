#include<iostream>
#include<string>
#include<vector>
using namespace std;

class Process{

	public:
		char name;
		int state;      //1:read; 2:running: 3:block
		int io_time;
		int cpu_burst;
		int arrive_time;
		int wait_time;
                int turnaround_time;
                int cpu_remain;
                int io_remain;
                int preemptive;

                /*Constructor*/
		Process(char Name,int io,int cpu,int arrive);
		Process(const Process &p); //copy

		/*Observer*/
		bool IsFinished() const; //true if finished, false else

		
};

