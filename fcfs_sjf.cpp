#include<cmath>
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<cstdlib>
#include<cmath>
#include<algorithm>
#include<iomanip>
#include<fstream>
#include "Process.h"

using namespace std;

//Pre-declare

vector<Process> generate(int seed, int np, float limit, float lamda);
map<char,Process> Upload(vector<Process> p);
void print_queue(vector<char> p);
void output(string algorithm,map<char,Process> total,int tcs,int tncs,int np);
void arrive_and_io_sjf(int Time,vector<char> &ready_queue,map<char,Process> &total);
vector<char> compare_tau(vector<char>ready_queue,map<char,Process> total);
char part_sjf(int Time,bool &context_switched,bool &leave,int tcs,int &tx,char &last,char &current,float alpha,vector<char> &ready_queue, map<char,Process> &total);
char part_fcfs(int Time,bool &context_switched,bool &leave,int tcs,int &tx,char &last,char &current,vector<char> &ready_queue, map<char,Process> &total);
void arrive_and_io_fcfs(int Time,vector<char> &ready_queue,map<char,Process> &total,int mode);

/*---------generater------processes----------------------------------------------------*/

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

map<char,Process> Upload(vector<Process> p){

        map<char,Process> total;
        for(int i=0; i<p.size(); i++){
                total.insert(pair<char,Process>(p[i].name,p[i]));
        }
        return total;
}

/*--------------------------------------------------------------------------------------*/


/*----------------------print-out-------------------------------------------------------*/
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


void output(string algorithm,map<char,Process> total,int tcs,int tncs,int np){

        float ave_turnaround = 0.0;
        float ave_wait = 0.0;
        float ave_cpu_burst = 0.0;
        int total_preemetive = 0;
        int total_context_switch = 0;

        for(map<char,Process>::iterator itr=total.begin(); itr!= total.end(); ++itr){

                //cout << itr->first << " is " << itr->second.state << endl;
                ave_turnaround += itr->second.turnaround_time;
		//cout << itr->second.turnaround_time << endl;
                ave_wait += itr -> second.wait_time;
                total_preemetive += itr->second.preemptive;
                for(int j=0;j<itr->second.cpu_burst.size();j++){ave_cpu_burst += itr->second.cpu_burst[j];}
                ave_cpu_burst /= itr->second.cpu_burst.size();

        }

        ave_turnaround /= np;
        ave_wait /= np;
        ave_cpu_burst /= np;
        total_context_switch = tncs;
        
	
	ofstream myfile;
	myfile.open("simout.txt",std::ios_base::app);
 
	myfile << std::fixed <<std::setprecision(3);
        myfile <<"Algorithm "<< algorithm << endl;
        myfile << "-- average CPU burst time: "<<ave_cpu_burst<<" ms" << endl;
        myfile << "-- average wait time: "<<ave_wait<<"  ms" << endl;
        myfile << "-- average turnaround time: "<<ave_turnaround<<" ms" << endl;
        myfile << "-- total number of context switches: "<<total_context_switch << endl;
        myfile << "-- total number of preemptions: "<<total_preemetive << endl;

	myfile.close();

}

/*-----------------------------------------------------------------------------------------*/


/*----------------------------------------SJF----------------------------------------------*/

void arrive_and_io_sjf(int Time,vector<char> &ready_queue,map<char,Process> &total){


        for(map<char,Process>::iterator i=total.begin(); i!=total.end();++i){

                if(i->second.state == 3){
                        i->second.io_remain--;
                        if(i->second.IsIOFinished()){
                                i->second.update(3);
                                ready_queue.push_back(i->second.name);
                                ready_queue = compare_tau(ready_queue,total);
                                cout<<"time "<<Time <<"ms: " << "Process "<<i->first<< "(tau "<< i->second.tau <<"ms)" <<" completed I/O; added to ready queue ";
                                print_queue(ready_queue);
                        }


                }

                if(i->second.IsArrive(Time)){
                        ready_queue.push_back(i->second.name);
                        ready_queue = compare_tau(ready_queue,total);
                        cout <<"time "<< Time <<"ms: "<<"Process " << i->second.name <<"(tau "<< i->second.tau <<"ms)"<<" arrived; added to ready queue" << endl;
                        //print_queue(ready_queue);
                }

                //if(i->second.arrive_time > Time && i->second.state == 1){i->second.wait_time ++;}
        }

}



vector<char> compare_tau(vector<char>ready_queue,map<char,Process> total){

        vector<char> temp =ready_queue;
        if(ready_queue.size()<2){return temp;}
        int j=0;
        for(int i=1; i<temp.size();i++){

                char key = temp[i];
                j = i-1;

                while(j >= 0 && total[temp[j]].tau > total[key].tau){

                        //cout << key << endl;
                        temp[j+1] = temp[j];
                        j--;
                }
                temp[j+1] = key;
        }
        return temp;
}

int compute_tau(float alpha, int tau, int t){return (alpha*t +  (1-alpha)*tau);}


char part_sjf(int Time,bool &context_switched,bool &leave,int tcs,int &tx,char &last,char &current,float alpha,int &ntc,vector<char> &ready_queue, map<char,Process> &total){
	
        
	if(current == '-' && ready_queue.size() == 0 && !leave){return '-';} //finished simluation
	
	if(tx != 0){

		//cout << Time << endl;
		total[last].turnaround_time++;
		tx --;
		return last;

	}

	if(current == '-' && ready_queue.size() != 0){	
                
		//cout << "enter" << endl;
		int size = ready_queue.size();
		current = ready_queue[0];
		total[current].state = 2;
		ready_queue.erase(ready_queue.begin());
		
		context_switched = true;
		total[current].turnaround_time ++;
		last = current;
		tx = tcs;
		tx --;
		return current; //switching...
		
	}

	if(current == '-'){return current;}
		
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
			
			ntc ++;
			int t = total[current].cpu_ptr;
			int tt = total[current].cpu_burst[t];
			context_switched = true;
			total[current].update(2);
			int tta = total[current].tau;
			total[current].tau = compute_tau(alpha,tta,tt);
			int o = total[current].io_remain;
			int y = total[current].cpu_burst.size();
			if(y-t != 1){

				cout << "time "<<Time << "ms: " <<"Process "<< total[current].name<< " completed a CPU burst; "<< (y-t-1) <<" bursts to go ";
				print_queue(ready_queue);
				cout << "time "<<Time << "ms: Recalculated tau = "<< total[current].tau<<"ms for process "<< current <<" ";
				print_queue(ready_queue);
				cout << "time "<<Time<<"ms: Process " <<total[current].name<<" switching out of CPU; will block on I/O until time "<< (Time + o)  <<"ms ";
			       	print_queue(ready_queue);
			}

			if(total[temp].IsProcessFinished()){
				
				total[temp].state = 0; //finished
				cout <<"time "<<Time << "ms: " <<"Process "<<temp<< " terminated ";
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


/*---------------------------------------------------------------------------------------------------------*/

/*--------------------------------FCFS---------------------------------------------------------------------*/

void arrive_and_io_fcfs(int Time,vector<char> &ready_queue,map<char,Process> &total){


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

                //if(i->second.state == 1){i->second.wait_time ++;}
        }

}


char part_fcfs(int Time,bool &context_switched,bool &leave,int tcs,int &tx,char &last,char &current,int &ntc,vector<char> &ready_queue, map<char,Process> &total){


	if(current == '-' && ready_queue.size() == 0 && !leave){return '-';} //finished simluation

	if(tx != 0){

		//cout << tx << endl;
		total[last].turnaround_time++;
		tx --;
		return last;

	}

	if(current == '-' && ready_queue.size() != 0){

		//cout << "enter" << endl;
		int size = ready_queue.size();
		current = ready_queue[0];
		total[current].state = 2;
		ready_queue.erase(ready_queue.begin());
		
		context_switched = true;
		total[current].turnaround_time ++;
		last = current;
		tx = tcs;
		tx --;
		return current; //switching...
		
		
	}

	if(current == '-'){return '-';} //single on io

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
			//context_switched = true;
			total[current].update(2);
			int o = total[current].io_remain;
			int y = total[current].cpu_burst.size();
			ntc ++;
			if(y-t != 1){

				cout << "time "<<Time << "ms: " <<"Process "<< total[current].name<< " completed a CPU burst; "<< (y-t-1) <<" bursts to go ";
				print_queue(ready_queue);
				cout << "time "<<Time<<"ms: Process " <<total[current].name<<" switching out of CPU; will block on I/O until time "<< (Time + o)  <<"ms ";
			       	print_queue(ready_queue);
			}

			if(total[temp].IsProcessFinished()){

				total[temp].state = 0; //finished
				cout <<"time "<<Time << "ms: " <<"Process "<<temp<< " terminated ";
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
/*---------------------------------------------------------------------------------------------------------*/


/*-----------------Wrap--------and--------------tool-------------------------------------------------------*/
bool all_finished(map<char,Process> total){

	for(map<char,Process>::iterator itr = total.begin(); itr!=total.end(); itr++){if(itr->second.state != 0){return false;}}
	return true;
}

void output_process(map<char,Process> total){

	for(map<char,Process>::iterator itr = total.begin(); itr!=total.end(); itr++){
		cout << "Process "<<itr->second.name<<" [NEW] (arrival time"<< itr->second.arrive_time <<"ms) "<< itr->second.number_of_burst <<" CPU bursts" << endl;
	}
}

void wait(vector<char> ready_queue,map<char,Process> &total){

	for(int i=0;i<ready_queue.size();i++){total[ready_queue[i]].wait_time++;}
}

void fcfs(map<char,Process> &total,vector<char> &ready_queue,int context_switch_time,int number_of_process){

	long long int Time = -1;
	int t_cs = context_switch_time/2;
        int tx = 0;
        int ntc = 0;
        char last = '-';
        bool context_switch = false;
        char current = '-';
        char result = '-';
        bool leave = false;

	while(!all_finished(total)){

		Time ++;
		arrive_and_io_fcfs(Time,ready_queue,total);
                result = part_fcfs(Time,context_switch,leave,t_cs,tx,last,current,ntc,ready_queue,total);
		wait(ready_queue,total);

	}
	cout << "time "<< Time<< "ms: Simulator ended for FCFS ";
	print_queue(ready_queue);
	output("FCFS",total,context_switch_time,ntc,number_of_process);

}

void sjf(map<char,Process> &total, vector<char> &ready_queue, int context_switch_time, int number_of_process, float alpha){

	long long int Time = -1;
        int t_cs = context_switch_time/2;
        int tx = 0;
        int ntc = 0;
        char last = '-';
        bool context_switch = false;
        char current = '-';
        char result = '-';
        bool leave = false;
        bool finish = false;
        //print_queue(ready_queue);
        vector<char> add_;

        while(!all_finished(total)){
                
		Time++;
                
		if(Time == 0){
			cout << "time "<<Time<<"ms: Simulator started for SJF ";
			print_queue(ready_queue);
		}
                arrive_and_io_sjf(Time,ready_queue,total);
                result = part_sjf(Time,context_switch,leave,t_cs,tx,last,current,alpha,ntc,ready_queue,total);
		wait(ready_queue,total);
		//cout << ntc << endl;
                
        }
	cout << "time "<< Time<< "ms: Simulator ended for SJF ";
	print_queue(ready_queue);
        output("SJF",total,context_switch_time,ntc,number_of_process);



}
/*---------------------------------------------------------------------------------------------------------*/



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


	vector<Process> P;
        P = generate(seed,number_of_process,limit, lamda); //<----------------------Copy this for different algorthim
        //for(int itr=0;itr < P.size(); ++itr){cout << P[itr].arrive_time << endl;}
        
	map<char,Process> total_fcfs,total_sjf; //<---------------------What I used
        total_sjf = Upload(P);
	total_fcfs = Upload(P);
        //cout << P.size() << endl;
        //for(map<char,Process>::iterator itr = total.begin(); itr!=total.end();itr++){cout << itr->first << " == " << itr->second.arrive_time << endl;}

        //cpu and io
        vector<char> ready_queue;

	output_process(total_sjf);	
	sjf(total_sjf,ready_queue,context_switch_time,number_of_process,alpha);
	//for(int i=0; i<P[0].io_time.size();i++){cout << P[0].io_time[i] << endl;}

	output_process(total_fcfs);
	fcfs(total_fcfs,ready_queue,context_switch_time,number_of_process);



	return 0;
}
