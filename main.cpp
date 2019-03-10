#include<iostream>
#include<string>
#include<vector>
#include<map>
#include "Process.h"
using namespace std;



int main(int argc, char** argv){

	if(argc < 4){
		cout << "ERROR: NOT ENOUGH INPUT" << endl;
		return -1;
	}

	cout << "Hello" << endl;
	int seed = atoi(argv[1]);
	float limit = atof(argv[3]);
	float lamda = atof(argv[2]);
	srand48(seed);
	Process p('A',lamda,limit);
	cout << p.name << endl;
	Process np('B',lamda,limit);
	cout << np.name << endl;

	vector<Process*> p_queue;
	vector<Process*> ready_queue;
	vector<Process*> wait_queue;

	p_queue.push_back(&p);
	p_queue.push_back(&np);



	int i= 0;
	while(i<100000){
		i++;
		
		//arrive

		//cout << "Arrive" << endl;
		for(int j = 0; j <p_queue.size(); j++){
			if(p_queue[j]->IsArrive(i)){ready_queue.push_back(p_queue[j]);}

		}
                
		//io
		
		//cout << "io" << endl;
		for(int k=0; k < wait_queue.size(); k++){
			wait_queue[k]->io_remain --;
			if(wait_queue[k]->IsBurstFinished()){
				wait_queue.erase(wait_queue.begin()+k);
			}
		}
		
		//run
		
		//cout << "run: " <<endl;
		if(ready_queue.size() > 0){
			Process* current = ready_queue[0];
			current->cpu_remain --;
			current->turnaround_time ++;
			if(current->IsBurstFinished()){
				cout << current->name << "  " << i <<endl;
				wait_queue.push_back(current);
				if(current->IsProcessFinished()){cout << current->name << "Finished"<< endl;}
				ready_queue.erase(ready_queue.begin());

			}
		}	

	
	}


	return 0;
}

