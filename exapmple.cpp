#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<cstdlib>
#include<cmath>
#include "Process.h"
using namespace std;

int check_drand_list(int seed, int range, float lamda){
	
	srand48(seed);
        for(int i=0; i<range; i++){
                double r = drand48();
                int x = -log(r) / lamda+1;
                cout << i <<" --> " <<"[" << r << "] = " << x << endl;
        }

}


//return the procss lists
vector<Process> generate(int seed, int np, float limit, float lamda){

	srand48(seed);
        string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        vector<Process> p_queue;
	if(np < 27){
		for(int i=0; i<np; i++){Process p(alphabet[i],lamda,limit);}
	}
	//if input is invalid, a empty vector will be return 
	return p_queue;
}



int main(int argc, char** argv){

	 if(argc < 7){
                cout << "ERROR: NOT ENOUGH INPUT" << endl;
                return -1;
        }

        cout << "Hello" << endl;
        int seed = atoi(argv[1]);
	int np = atoi(argv[4]); //number of process
        float limit = atof(argv[3]);
        float lamda = atof(argv[2]);
        srand48(seed);
	string alphabet = "abcdefghijklmnopqrstuvwxyz";
	vector<Process> p_queue;
	for(int i=0; i<np; i++){

		Process p(alphabet[i],lamda,limit);
		p_queue.push_back(p);
		//p.output();
	}


	Process* cp = &p_queue[0];
	vector<Process*> ready;
	vector<Process*> waiting; //io
	vector<Process*>  remain; //actuall store the finished one 

	int t = -1;
	bool wait = true;
	cp -> output();
	while(t < 50000){
		
		t++;
		if(cp->IsArrive(t)){
			wait = false;
			cout << "here " << t << endl;
		}

		if(wait){continue;}
		if(cp->state == 3){
			cp->io_remain --;
			if(cp->IsIOFinished()){
				cp->update(3);
				cout << "IO "<< t << endl;
			}
		}
		else{
			cp->cpu_remain --;
			cp->turnaround_time ++;
			if(cp->IsCPUFinished()){
				cp->update(1);
				cout << "Burst: " << t << endl;
			}

		}

		if(cp->IsProcessFinished()){
			cout << "FInish "<< t << endl;
			break;
		}




	}

	return 0;
}
