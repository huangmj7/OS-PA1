#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<cstdlib>
#include<cmath>
#include<algorithm>
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
                for(int i=0; i<np; i++){
			Process p(alphabet[i],lamda,limit);
			p_queue.push_back(p);
		}
        }
        //if input is invalid, a empty vector will be return 
        return p_queue;
}

void print_queue(vector<char> p){
	
	cout << "[Q ";
	if(p.size()==0){cout << " <empty> ";}
	else{
		for(vector<char>::iterator itr=p.begin(); itr!=p.end(); ++itr){
			cout<<*itr;
			if(itr!=p.end()-1){cout << " ";}
		}
	}
	cout << "]" << endl;
}


int tau(float alpha, int tau, int t){return (alpha*t +  (1-alpha)*tau);}

map<char,Process> Upload(vector<Process> p){
        
        map<char,Process> total;
        for(int i=0; i<p.size(); i++){
                total.insert(pair<char,Process>(p[i].name,p[i]));
	}
	return total;	
}



int fcfs(int Time,bool &context_switch,float alpha,char &current,vector<char> &ready_queue,vector<char> &io_queue,map<char,Process> &total){
	
	//arrive&io
	for(map<char,Process>::iterator i=total.begin(); i!=total.end();++i){
		
		if(i->second.state == 3){
			i->second.io_remain--;
			if(i->second.IsIOFinished()){
				i->second.update(3);
				//i->second.state = 2;
				ready_queue.push_back(i->second.name);
				cout<<"Time "<<Time <<"ms: " << "Process "<<i->first <<" completed I/O; added to ready queue ";
				print_queue(ready_queue);
			}

		}

		if(i->second.IsArrive(Time)){
			ready_queue.push_back(i->second.name);
			cout <<"Time "<< Time <<"ms: "<<"Process " << i->second.name <<" arrived; added to ready queue" << endl;
		}


	}


	if(ready_queue.size() == 0 && current == '-'){return 1;}

	//preemetive --> won't happed
	//for(map<char,Process>::iterator itr = total.begin(); itr!=total.end(); itr++){if(itr->second.state == 1){itr->second.wait_time++;}}
	
	//wait
        for(int i=0; i<ready_queue.size(); i++){
		char t = ready_queue[i];
		total[t].wait_time++;
	}

	//context switch
	//leave
	/*
	if(context_switch && current == '-'){

		switching --;
		total[last].turnaround_time --;
		if(switching == 0){
			switching = tcs/2;
		}//for next enter

		return 2;
	}
	*/
	if(current == '-'){
		//context_switch_enter = (true && (enter!=0));
		current = ready_queue[0];
		total[current].state = 2;
		ready_queue.erase(ready_queue.begin());
		if(!context_switch){
			cout << "time "<< Time <<"ms: Process "<<current<<" started using the CPU for "<< total[current].cpu_remain<<"ms "<< "burst ";
			print_queue(ready_queue);
		}
		/*
		else{
			switching --;
			total[current].trunaround_time++;
			if(switching == 0){
				switching = tcs/2;
				context_switch = false;
			}
			return 2;
			
		}
		*/
	}

	//cpu
	if(total[current].state == 2){

		total[current].cpu_remain --;
		total[current].turnaround_time++;
		char temp = current;

		if(total[current].IsCPUFinished()){

			int t = total[current].cpu_ptr;
			total[current].update(2);
			int y = total[current].cpu_burst.size();
			if(y-t != 1){

				cout << "time "<<Time << "ms: " <<"Process "<< total[current].name<< " completed a CPU burst; "<< (y-t) <<" bursts to go ";
			       	print_queue(ready_queue);
			}
			current = '-';	
		}

		if(total[temp].IsProcessFinished()){
			
			total[temp].state = 0; //finished
			cout <<"Time "<<Time << "ms: " <<"Process "<<temp<< " terminated ";
 			print_queue(ready_queue);
		}
	}
        //print_queue(ready_queue);
	return 0;
	
}







int main(int argc, char** argv){

         if(argc < 8){
                cout << "ERROR: NOT ENOUGH INPUT" << endl;
                return -1;
        }

        int seed,number_of_process;
        float limit,lamda,context_switch_time,alpha,time_slice;
        string RR_order;

        try{

                seed = atoi(argv[1]);
                number_of_process = atoi(argv[4]); //number of process
                limit = atof(argv[3]);
                lamda = atof(argv[2]);
                context_switch_time = atof(argv[5]);
                alpha = atof(argv[6]);
                time_slice = atof(argv[7]);
                RR_order = argv[8];
        }
	catch(int e){
		cout << "ERROR: FAIL TO GET INPUT FOR REASON OF "<< e << endl;
		return -1;
	}

	//process queue
	vector<Process> P;
	P = generate(seed,number_of_process,limit, lamda);
	//for(int itr=0;itr < P.size(); ++itr){cout << P[itr].arrive_time << endl;}
	map<char,Process> total;
	total = Upload(P);
	//cout << P.size() << endl;
	//for(map<char,Process>::iterator itr = total.begin(); itr!=total.end();itr++){cout << itr->first << " == " << itr->second.arrive_time << endl;}

	//cpu and io
	vector<char> ready_queue,io_queue;
	//for(map<char,Process>::iterator itr=total.begin();itr!=total.end(); ++itr){ready_queue.push_back(itr->first);}

	//time
	long long int Time = -1;
	int t_cs_e = context_switch_time/2;
	int t_cs_l = context_switch_time/2;
	bool context_switch = false;
	char current = '-';
	print_queue(ready_queue);
	
	
	while(Time < 400000){
                Time++;
		//cout << Time << current << endl;
		fcfs(Time,context_switch,alpha,current,ready_queue,io_queue,total);
		//cout<<Time  << " `B eak" << endl;
	}

	//Output(total);




}
