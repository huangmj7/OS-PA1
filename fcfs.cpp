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

void arrive_and_io(int Time,vector<char> &ready_queue,map<char,Process> &total){
	

	for(map<char,Process>::iterator i=total.begin(); i!=total.end();++i){
		
		if(i->second.state == 3){
			i->second.io_remain--;
			if(i->second.IsIOFinished()){
				i->second.update(3);
				ready_queue.push_back(i->second.name);
				cout<<"time "<<Time <<"ms: " << "Process "<<i->first <<" completed I/O; added to ready queue ";
				print_queue(ready_queue);
			}


		}

		if(i->second.IsArrive(Time)){
			ready_queue.push_back(i->second.name);
			cout <<"time "<< Time <<"ms: "<<"Process " << i->second.name <<" arrived; added to ready queue" << endl;
		}

		if(i->second.state == 1){i->second.wait_time ++;}
	}

}

char part_fcfs(int Time,bool &context_switched,bool &leave,int tcs,int &tx,char &last,char &current,vector<char> &ready_queue, map<char,Process> &total){
	
        
	if(current == '-' && ready_queue.size() == 0){return '0';} //finished simluation
	
	if(tx != 0){

		//cout << Time << endl;
		total[last].turnaround_time++;
		tx --;
		return last;

	}

	if(current == '-'){	
                
		//cout << "enter" << endl;
		int size = ready_queue.size();
		current = ready_queue[0];
		total[current].state = 2;
		ready_queue.erase(ready_queue.begin());
		if(leave){
			context_switched = true;
			last = current;
			tx = tcs;
			tx --;
			return current; //switching...
		}
		else{context_switched = true;}
	}
		
        //finished
	if(tx == 0 && context_switched){
		context_switched = false;
		leave = false;
		cout << "time "<< Time <<"ms: Process "<<current<<" started using the CPU for "<< total[current].cpu_remain<<"ms "<< "burst ";
		print_queue(ready_queue);
	}
			
	//cpu run 
	if(total[current].state == 2){
		
		total[current].cpu_remain --;
		total[current].turnaround_time++;
		char temp = current;

		if(total[current].IsCPUFinished()){

			int t = total[current].cpu_ptr;
			context_switched = true;
			total[current].update(2);
			int o = total[current].io_remain;
			int y = total[current].cpu_burst.size();
			if(y-t != 0){

				cout << "time "<<Time << "ms: " <<"Process "<< total[current].name<< " completed a CPU burst; "<< (y-t) <<" bursts to go ";
				print_queue(ready_queue);
				cout << "time "<<Time<<"ms: Process " <<total[current].name<<" switching out of CPU; will block on I/O until time "<< (Time + o)  <<"ms ";
			       	print_queue(ready_queue);
			}

			if(total[temp].IsProcessFinished()){
				
				total[temp].state = 0; //finished
				cout <<"Time "<<Time << "ms: " <<"Process "<<temp<< " terminated ";
				print_queue(ready_queue);
			}
			else{
				
				//cout << "leave" << endl;
				leave = true;
				tx = tcs;
				tx --;
				context_switched = true;
				last = temp;
			}

			current = '-';	
		}

	}
	 
	return '-';
        
}

void output(string algorithm,map<char,Process> total,int tcs,int tncs,int np){

	float ave_turnaround = 0.0;
	float ave_wait = 0.0;
	float ave_cpu_burst = 0.0;
	int total_preemetive = 0;
	int total_context_switch = 0;

	for(map<char,Process>::iterator itr=total.begin(); itr!= total.end(); ++itr){

		//cout << itr->first << " is " << itr->second.state << endl;
		ave_turnaround += itr->second.turnaround_time;
		ave_wait += itr -> second.wait_time;
		total_preemetive += itr->second.preemptive; 
		for(int j=0;j<itr->second.cpu_burst.size();j++){ave_cpu_burst += itr->second.cpu_burst[j];}
		ave_cpu_burst /= itr->second.cpu_burst.size();

	}

	ave_turnaround /= np;
	ave_wait /= np;
	ave_cpu_burst /= np;
	total_context_switch = tncs/tcs;

	cout <<"Algorithm "<< algorithm << endl;
	cout << "-- average CPU burst time: "<<ave_cpu_burst<<" ms" << endl;
	cout << "-- average wait time: "<<ave_wait<<"  ms" << endl;
	cout << "-- average turnaround time: "<<ave_turnaround<<" ms" << endl;
	cout << "-- total number of context switches: "<<total_context_switch << endl; 
	cout << "-- total number of preemptions: "<<total_preemetive << endl;

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
	cout << context_switch_time << endl;
	long long int Time = -1;
	int t_cs = context_switch_time/2;
	int tx = 0;
	int ntc = 0;
	char last = '-';
	bool context_switch = false;
	char current = '-';
	char result = '-';
	bool leave = false;
	//print_queue(ready_queue);
	vector<char> add_;
	
	while(Time < 400000){
                Time++;
		
		arrive_and_io(Time,ready_queue,total);
		result = part_fcfs(Time,context_switch,leave,t_cs,tx,last,current,ready_queue,total);
		if(leave){ntc ++;}
		//cout << ntc << endl;
		//if(tx != 0){cout<< Time << "  " << tx << endl;}
		//cout << "xed" << endl;
		//cout << Time << current << endl;
		//fcfs(Time,context_switch,alpha,current,ready_queue,io_queue,total);
		//cout<<Time  << " `B eak" << endl;
	}
        output("FCFS",total,context_switch_time,ntc,number_of_process);
	//output(total);




}
