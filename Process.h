#include<iostream>
#include<string>
#include<vector>
using namespace std;

/*I did some adjustment, please see submitty's project assignment page 5*/
/*For running the process, you can still do as our discussed but need call IsBurstFinished() at every run end so new cpu burst can be update*/
/*Please tracking the state to make sure assign the porcess correctly*/
class Process{

	public:
		char name;
		int state;      //1:read; 2:running: 3:block
		int number_of_burst;   //range from [1,100]
		int tau;
		vector<int> io_time;   //number of io time is one less than cpu as process always end with cpu burst
		vector<int> cpu_burst; //number of cpu burst time arrange from [1,100], order from first arrive to the last arrive
		vector<int> interarrival; //the arrival time of each burst, some order as the cpu burst

		
		int arrive_time; //process arrive time
		int wait_time;
                int turnaround_time;
                int preemptive;

		int cpu_remain;
		int io_remain;
		
		int cpu_ptr; //indicate which cpu burst the process currently in 
		int io_ptr; //indicate which io burst the process currently in

	        bool finished;	

                /*Constructor*/
		Process(); //default constructor for map function 
		Process(char Name,float lamda,float limit); //seed for generating all these time and place
		Process(const Process &p); //copy

		/*Mutator*/
		int update(int state); //update the remain time of burst, 1 or 2 will update cpu_remain and set state to 3
		                       //3 will update io_remain and set state 1
				       //return -1 if fail, else return the new state of process

		/*Observer*/
		bool IsProcessFinished() const; //true if processfinished, false else
		bool IsCPUFinished() const;//true if state is 1 or 2 and cpu remain == 0, false if the state is 3 or cpu_remain != 0
		bool IsIOFinished() const;// true if state is 3 and io_remain == 0; false else 
		bool IsArrive(int time) const;//true if a process/burst arrive, false else
		void output(); //print all results for debugging

	private:
		void time_generation(float limit, float lamda);

};

