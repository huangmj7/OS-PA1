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
vector<Process> sort_update_ready(vector<Process> p, vector<Process> ready);
vector<Process> update_ready(vector<Process> p, vector<Process> ready);
int srt(int &Time, int tcs, vector<Process> &p, vector<Process> &ready_queue, int &cpu_occupy, Process &running_p, double alpha, int &tcs_check);
int rr(int &Time, int tcs, vector<Process> &p, vector<Process> &ready_queue, int &cpu_occupy, Process &running_p, double alpha, int time_slice, int& track_time_slice, int order, int &tcs_check);

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
vector<Process> sort_update_ready(vector<Process> p, vector<Process> ready){
	for (unsigned int i = 0; i < ready.size(); i++){
		for (unsigned int j = 0; j < p.size(); j++){
			if (p[j].name == ready[i].name){
				//ready[i].burst_num = p[j].burst_num;
				//ready[i].tau = p[j].tau;
				//ready[i].cpu_remain = p[j].cpu_remain;
				//ready[i].io_remain = p[j].io_remain;
				//ready[i].state = p[j].state;
				ready[i] = p[j];
				//ready[i].burst_num = p[j].burst_num;
			}
		}
	}
 
	//sort(ready.begin(),ready.end(),Sort); 
	for(unsigned  int i = 1;i < ready.size();++i){
        for(int j = i;j > 0;--j){
            if(ready[j].tau < ready[j - 1].tau){
                Process temp = ready[j];
                ready[j] = ready[j-1];
                ready[j-1] = temp;
            }
			else if (ready[j].tau == ready[j - 1].tau){
				if (ready[j].name < ready[j - 1].name){
					Process temp = ready[j];
                	ready[j] = ready[j-1];
                	ready[j-1] = temp;
				}
			}
        }
    }
	return ready;
}
vector<Process> update_ready(vector<Process> p, vector<Process> ready){
	for (unsigned int i = 0; i < ready.size(); i++){
		for (unsigned int j = 0; j < p.size(); j++){
			if (p[j].name == ready[i].name){
				//ready[i].burst_num = p[j].burst_num;
				//ready[i].tau = p[j].tau;
				//ready[i].cpu_remain = p[j].cpu_remain;
				//ready[i].io_remain = p[j].io_remain;
				//ready[i].state = p[j].state;
				ready[i] = p[j];
				//ready[i].burst_num = p[j].burst_num;
			}
		}
	}
	return ready;
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
int srt(int &Time, int tcs, vector<Process> &p, vector<Process> &ready_queue, int &cpu_occupy, Process &running_p, double alpha, int &tcs_check) {
	ready_queue = sort_update_ready(p,ready_queue);
	/*Inistialization*/
	if (Time == 0) {
		for (unsigned int y = 0; y < p.size(); y++){
			cout << "Process " << p[y].name << " [NEW] (arrival time " << p[y].arrive_time << "ms) " << p[y].number_of_burst;
			if (p[y].number_of_burst != 1){
				cout << " CPU bursts" << endl;
			} 
			else {
				cout << " CPU burst" << endl;
			}
		}
		cout << "time 0ms: Simulator started for SRT [Q <empty>]" << endl;
		for (unsigned int i = 0; i < p.size(); i++) {
			p[i].state = 0;
			p[i].burst_num = 0;
			p[i].cpu_remain = 0;
			p[i].io_remain = 0;
			p[i].wait_time = 0;
			p[i].preemptive = 0;
			p[i].context_switch = 0;
			for (unsigned int j = 0; j < p[i].io_time.size(); j++){
				p[i].io_time[j] += 2;
			}
		}
	}
	/*Process arrival*/
	for (unsigned int i = 0; i < p.size(); i++) {
		if (p[i].arrive_time == Time) {
			p[i].state = 1;
			p[i].cpu_remain = 0;
			p[i].io_remain = 0;
			ready_queue.push_back(p[i]);
			ready_queue = sort_update_ready(p,ready_queue);
			cout << "time " << Time << "ms: Process " << p[i].name;
			cout << " (tau " << p[i].tau << "ms) arrived: added to ready queue [Q";
			/*Print ready queue*/
			for (unsigned int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << " <empty>";
			}
			cout << "]" << endl;
			if (tcs_check != tcs/2-1){
				tcs_check = -1;
			}
		}
	}
	/*If io completes during context switch*/
    if (running_p.state == 4){
        tcs_check++;        
        if (tcs_check != tcs / 2 + 1){
            for (unsigned int i = 0; i < p.size(); i++){
                if (p[i].state == 1 || p[i].state == 4){
                    p[i].wait_time++;
                }
               	else if (p[i].state == 3) {
					p[i].io_remain--;
					//cout << p[i].name << ":" <<p[i].io_remain << endl;
					if (p[i].io_remain == 0) {
						vector<Process> temp;
						for (unsigned int j = 0; j < p.size(); j++) {
							if (p[j].state == 1 || p[j].state == 2 || p[j].state == 3) {
								temp.push_back(p[j]);
							}
						}
						temp = sort_update_ready(p,temp);

						if (temp[0].name == p[i].name && running_p.state != -1 && running_p.name != p[i].name) {
							cout << "time " << Time << "ms: Process " << temp[0].name << " (tau " << temp[0].tau << "ms) completed I/O and will preempt ";
							cout << running_p.name;
							cout << " [Q";
							ready_queue.push_back(p[i]);
							ready_queue = sort_update_ready(p,ready_queue);
							for (unsigned int j = 0; j < ready_queue.size(); j++) {
								cout << " " << ready_queue[j].name;
							}
							if (ready_queue.size() == 0) {
								cout << " <empty>";
							}
							cout << "]" << endl;
							for (unsigned int j = 0; j < p.size(); j++) {
								if (running_p.name == p[j].name) {
									p[j].preemptive++;
									p[j].state = 1;
									p[j].context_switch++;
									running_p.state = 4;
								}
							}
							p[i].state = 1;
							cpu_occupy = 0;
							ready_queue = sort_update_ready(p,ready_queue);
						}
						/*
						Special situation: when two i/o complete within tcs/2, the output should be different
						else if (temp[0].name != running_p.name && )
						*/
						else /*if (temp[0].name == running_p.name || running_p.state == 3 || running_p.state == -1) */{
							cout << "time " << Time << "ms: Process " << p[i].name << " (tau " << p[i].tau << "ms) completed I/O; added to ready queue";
							cout << " [Q";
							ready_queue.push_back(p[i]);
							ready_queue = sort_update_ready(p,ready_queue);
							for (unsigned int j = 0; j < ready_queue.size(); j++) {
								cout << " " << ready_queue[j].name;
							}
							if (ready_queue.size() == 0) {
								cout << " <empty>";
							}
							cout << "]" << endl;
							p[i].state = 1;
							//cpu_occupy = 1;
							ready_queue = sort_update_ready(p,ready_queue);
						}
					}
				}  
            }
			return 1;
		}
		else {
			tcs_check = 0;
			running_p.state = 1;
			for (unsigned int i = 0; i < p.size(); i++){
				if (p[i].state == running_p.name){
					p[i].state = 1;		
				}
			}
			ready_queue.push_back(running_p);
		}
	}
	/*Add to cpu*/
	if (cpu_occupy == 0 && ready_queue.size() != 0 && running_p.state != 4) {
		ready_queue = sort_update_ready(p,ready_queue);
		tcs_check++;
		if (tcs_check == tcs / 2) {
			tcs_check = 0;
			for (unsigned int l = 0; l < p.size(); l++){
				if (p[l].name==ready_queue[0].name){
					ready_queue[0].burst_num = p[l].burst_num;
				}
			}
			if (ready_queue[0].cpu_burst[ready_queue[0].burst_num] != ready_queue[0].cpu_remain && ready_queue[0].cpu_remain > 0){
				cout << "time " << Time << "ms: Process " << ready_queue[0].name << " started using the CPU with ";
				cout << ready_queue[0].cpu_remain << "ms remaining [Q";		
				
			}
			else {
					
				cout << "time " << Time << "ms: Process " << ready_queue[0].name << " started using the CPU for ";
				cout << ready_queue[0].cpu_burst[ready_queue[0].burst_num] << "ms burst [Q";
			}
			
			for (unsigned int i = 0; i < p.size(); i++){
				if (p[i].name == ready_queue[0].name){
					p[i].state = 2;
					running_p = p[i];
					if (running_p.cpu_remain == 0){
						running_p.cpu_remain = ready_queue[0].cpu_burst[ready_queue[0].burst_num]+1;
						//Time++;
						p[i].cpu_remain = running_p.cpu_remain;
					}
				}
			}
			ready_queue.erase(ready_queue.begin(), ready_queue.begin()+1);
			ready_queue = sort_update_ready(p,ready_queue);
			for (unsigned int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << " <empty>";
			}	
			cout << "]" << endl;
			cpu_occupy = 1;
		}
	}
	/*Cpu burst, i/o burst, and wait*/
	for (unsigned int i = 0; i < p.size(); i++) {
		if (p[i].state == 1 || p[i].state == 4) {
			p[i].wait_time++;
		}
		else if (p[i].state == 2) {
			p[i].cpu_remain--;
			if (running_p.name == p[i].name){
				running_p.cpu_remain--;
			}
			if (p[i].cpu_remain == 0) {
				if (p[i].number_of_burst == 1) {
					cout << "time " << Time << "ms: Process " << p[i].name << " terminated [Q";
					ready_queue = sort_update_ready(p,ready_queue);
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;
					cpu_occupy = 0;
					p[i].state = -1;
					p[i].context_switch++;
					running_p.state = -1;
					p[i].number_of_burst = 0;					
					int count_end = 0;
					for (unsigned int j = 0; j < p.size(); j++) {
						if (p[j].state == -1) {
							count_end++;
						}
					}
					if (count_end == (int)p.size()) {
						Time+= tcs/2;
						cout << "time " << Time << "ms: Simulator ended for SRT [Q <empty>]" << endl;
						return 0;
					}
					tcs_check = - tcs / 2;
				}
				else if (p[i].number_of_burst > 1) {
					cout << "time " << Time << "ms: Process " << p[i].name << " completed a CPU burst: ";
					p[i].number_of_burst--;
					p[i].context_switch++;
					cout << p[i].number_of_burst << " bursts to go [Q";
					ready_queue = sort_update_ready(p,ready_queue);
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;
					p[i].tau = (1 - alpha)*p[i].tau + alpha * p[i].cpu_burst[p[i].burst_num];	
					cout << "time " << Time << "ms: Recalculated tau = " << p[i].tau << "ms for process " << p[i].name << " [Q";
					ready_queue = sort_update_ready(p,ready_queue);
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;
					cout << "time " << Time << "ms: Process " << p[i].name << " switching out of CPU; will block on I/O until time ";
					//Time++;
					cout << Time + p[i].io_time[p[i].burst_num] << "ms [Q";
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;

						
					
					p[i].io_remain = p[i].io_time[p[i].burst_num];
					p[i].burst_num++;
					p[i].state = 3;
					running_p.state = 3;
					cpu_occupy = 0;
					ready_queue = sort_update_ready(p,ready_queue);
					tcs_check = -tcs/2;
				}
			}
		}
		else if (p[i].state == 3) {
			p[i].io_remain--;
			//cout << p[i].name << ":" <<p[i].io_remain << endl;
			if (p[i].io_remain == 0) {
				vector<Process> temp;
				for (unsigned int j = 0; j < p.size(); j++) {
					if (p[j].state == 1 || p[j].state == 2 || p[j].state == 3) {
						temp.push_back(p[j]);
					}
				}
				temp = sort_update_ready(p,temp);
				for (int c = 0; c < p.size();c++){
					if (running_p.name == p[c].name){
						running_p = p[c];
					}
				}
				if (temp[0].name == p[i].name && running_p.state != -1 && running_p.name != p[i].name && running_p.state != 3) {
					cout << "time " << Time << "ms: Process " << temp[0].name << " (tau " << temp[0].tau << "ms) completed I/O and will preempt ";
					cout << running_p.name;
					cout << " [Q";
					ready_queue.push_back(p[i]);
					ready_queue = sort_update_ready(p,ready_queue);
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;
					for (unsigned int j = 0; j < p.size(); j++) {
						if (running_p.name == p[j].name) {
							p[j].preemptive++;
							p[j].state = 1;
							p[j].context_switch++;
                            running_p.state = 4;
						}
					}
                    p[i].state = 1;
                    cpu_occupy = 0;
					ready_queue = sort_update_ready(p,ready_queue);
					tcs_check = 0;
				}
                else /*if (temp[0].name == running_p.name || running_p.state == 3 || running_p.state == -1) */{
					cout << "time " << Time << "ms: Process " << p[i].name << " (tau " << p[i].tau << "ms) completed I/O; added to ready queue";
					cout << " [Q";
					ready_queue.push_back(p[i]);
					ready_queue = sort_update_ready(p,ready_queue);
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;
					p[i].state = 1;
                    //cpu_occupy = 1;
					ready_queue = sort_update_ready(p,ready_queue);
					if (running_p.state == 3){
						tcs_check = 0;
					}
				}
			}
		}
		p[i].turnaround_time = Time - p[i].arrive_time;
	}
	return 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/*track_time_slice is 0 initially*/
int rr(int &Time, int tcs, vector<Process> &p, vector<Process> &ready_queue, int &cpu_occupy, Process &running_p, double alpha, int time_slice, int& track_time_slice, int order, int &tcs_check) {
	ready_queue = update_ready(p,ready_queue);
	/*Inistialization*/
	if (Time == 0) {
		for (unsigned int y = 0; y < p.size(); y++){
			cout << "Process " << p[y].name << " [NEW] (arrival time " << p[y].arrive_time << "ms) " << p[y].number_of_burst;
			if (p[y].number_of_burst != 1){
				cout << " CPU bursts" << endl;
			} 
			else {
				cout << " CPU burst" << endl;
			}
		}
		cout << "time 0ms: Simulator started for RR [Q <empty>]" << endl;
		for (unsigned int i = 0; i < p.size(); i++) {
			p[i].state = 0;
			p[i].burst_num = 0;
			p[i].cpu_remain = 0;
			p[i].io_remain = 0;
			p[i].wait_time = 0;
			p[i].preemptive = 0;
			p[i].context_switch = 0;
			for (unsigned int j = 0; j < p[i].io_time.size(); j++){
				p[i].io_time[j] += 2;
			}
		}
	}
	/*Process arrival*/
	for (unsigned int i = 0; i < p.size(); i++) {
		if (p[i].arrive_time == Time) {
			p[i].state = 1;
			p[i].cpu_remain = 0;
			p[i].io_remain = 0;
			if (order == 1){
				ready_queue.push_back(p[i]);
				ready_queue = update_ready(p,ready_queue);
			}
			else if (order == 0){
				ready_queue.insert(ready_queue.begin(),p[i]);
				ready_queue = update_ready(p,ready_queue);
			}
			cout << "time " << Time << "ms: Process " << p[i].name;
			cout << " (tau " << p[i].tau << "ms) arrived: added to ready queue [Q";
			/*Print ready queue*/
			for (unsigned int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << " <empty>";
			}
			cout << "]" << endl;
			tcs_check = -1;
		}
	}

	if (track_time_slice == time_slice && running_p.state == 2 && running_p.cpu_remain != 1) {
		ready_queue = update_ready(p,ready_queue);
		if (ready_queue.size() != 0) {
			cout << "time " << Time << "ms: Time slice expired; process " << running_p.name << " preempted with ";
			cout << running_p.cpu_remain << "ms to go [Q ";
			for (unsigned int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;
			if (order == 1){
				ready_queue.push_back(running_p);
				ready_queue = update_ready(p,ready_queue);
			}
			
			else if (order == 0){
				ready_queue.insert(ready_queue.begin(),running_p);
				ready_queue = update_ready(p,ready_queue);
			}
			track_time_slice = 0;
			cpu_occupy = 0;
			running_p.state = 1;
			for (unsigned int i = 0; i < p.size(); i++) {
				if (p[i].name == running_p.name) {
					p[i].preemptive++;
					p[i].state = 1;
				}
			}
			tcs_check = -tcs/2 - 1;
		}
		else {
			cout << "time " << Time << "ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]" << endl;
			track_time_slice = 0;
			cpu_occupy = 1;
			running_p.state = 2;
			for (unsigned int i = 0; i < p.size(); i++) {
				if (p[i].name == running_p.name) {
					p[i].state = 2;
				}
			}
		}
	}
	/*Add process from ready queue to cpu burst*/
	if (cpu_occupy == 0 && ready_queue.size() != 0) {
		ready_queue = update_ready(p,ready_queue);
		tcs_check++;
		if (tcs_check == tcs / 2) {
			tcs_check = 0;
			for (unsigned int l = 0; l < p.size(); l++){
				if (p[l].name==ready_queue[0].name){
					ready_queue[0].burst_num = p[l].burst_num;
				}
			}
			if (ready_queue[0].cpu_burst[ready_queue[0].burst_num] != ready_queue[0].cpu_remain && ready_queue[0].cpu_remain > 0){
				cout << "time " << Time << "ms: Process " << ready_queue[0].name << " started using the CPU with ";
				cout << ready_queue[0].cpu_remain << "ms remaining [Q";		
				
			}
			else {
					
				cout << "time " << Time << "ms: Process " << ready_queue[0].name << " started using the CPU for ";
				cout << ready_queue[0].cpu_burst[ready_queue[0].burst_num] << "ms burst [Q";
			}
			
			for (unsigned int i = 0; i < p.size(); i++){
				if (p[i].name == ready_queue[0].name){
					p[i].state = 2;
					running_p = p[i];
					if (running_p.cpu_remain == 0){
						running_p.cpu_remain = ready_queue[0].cpu_burst[ready_queue[0].burst_num]+1;
						p[i].cpu_remain = running_p.cpu_remain;
					}
				}
			}
			ready_queue.erase(ready_queue.begin(), ready_queue.begin()+1);
			ready_queue = update_ready(p,ready_queue);
			for (unsigned int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << " <empty>";
			}	
			cout << "]" << endl;
			cpu_occupy = 1;
		}
	}
	/*Cpu, io, wait*/
	for (unsigned int i = 0; i < p.size(); i++) {
		if (p[i].state == 1) {
			p[i].wait_time++;
		}
		else if (p[i].state == 2) {
			p[i].cpu_remain--;
			if (running_p.name == p[i].name){
				running_p.cpu_remain--;
			}
			track_time_slice++;
			if (p[i].cpu_remain == 0) {
				if (p[i].number_of_burst == 1) {
					cout << "time " << Time << "ms: Process " << p[i].name << " terminated [Q";
					ready_queue = update_ready(p,ready_queue);
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;
					cpu_occupy = 0;
					p[i].state = -1;
					p[i].context_switch++;
					running_p.state = -1;
					p[i].number_of_burst = 0;					
					int count_end = 0;
					for (unsigned int j = 0; j < p.size(); j++) {
						if (p[j].state == -1) {
							count_end++;
						}
					}
					if (count_end == (int)p.size()) {
						Time+= tcs/2;
						cout << "time " << Time << "ms: Simulator ended for RR [Q <empty>]" << endl;
						return 0;
					}
					track_time_slice = 0;
					tcs_check = - tcs / 2;
				}
				else if (p[i].number_of_burst > 1) {
					cout << "time " << Time << "ms: Process " << p[i].name << " completed a CPU burst: ";
					p[i].number_of_burst--;
					p[i].context_switch++;
					if (p[i].number_of_burst == 1){
						cout << p[i].number_of_burst << " burst to go [Q";
					}
					else{
						cout << p[i].number_of_burst << " bursts to go [Q";
					}
					
					ready_queue = update_ready(p,ready_queue);
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;
					p[i].tau = (1 - alpha)*p[i].tau + alpha * p[i].cpu_burst[p[i].burst_num];	
					/*cout << "time " << Time << "ms: Recalculated tau = " << p[i].tau << "ms for process " << p[i].name << " [Q";
					ready_queue = update_ready(p,ready_queue);
					for (int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;*/
					cout << "time " << Time << "ms: Process " << p[i].name << " switching out of CPU; will block on I/O until time ";
					cout << Time + p[i].io_time[p[i].burst_num] << "ms [Q";
					for (unsigned int j = 0; j < ready_queue.size(); j++) {
						cout << " " << ready_queue[j].name;
					}
					if (ready_queue.size() == 0) {
						cout << " <empty>";
					}
					cout << "]" << endl;
					
					p[i].io_remain = p[i].io_time[p[i].burst_num];
					p[i].burst_num++;
					p[i].state = 3;
					running_p.state = 3;
					//p[i].
					cpu_occupy = 0;
					ready_queue = update_ready(p,ready_queue);
					track_time_slice = 0;
					tcs_check = -tcs/2;
				}
			}
		}
		else if (p[i].state == 3) {
			p[i].io_remain--;
			if (p[i].io_remain == 0) {
				cout << "time " << Time << "ms: Process " << p[i].name /*<< " (tau " << p[i].tau << "ms) */<<" completed I/O; added to ready queue";
				cout << " [Q";
				if (order == 1){
					ready_queue.push_back(p[i]);
					ready_queue = update_ready(p,ready_queue);
				}
				else if (order == 0){
					ready_queue.insert(ready_queue.begin(),p[i]);
					ready_queue = update_ready(p,ready_queue);
				}
				ready_queue = update_ready(p,ready_queue);
				for (unsigned int j = 0; j < ready_queue.size(); j++) {
					cout << " " << ready_queue[j].name;
				}
				if (ready_queue.size() == 0) {
					cout << " <empty>";
				}
				cout << "]" << endl;
				p[i].state = 1;
                //cpu_occupy = 1;
				ready_queue = update_ready(p,ready_queue);
				if (running_p.state == 3){
					tcs_check = 0;
				}
			}
		}
		p[i].turnaround_time = Time - p[i].arrive_time;
	}	
	return 1;
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

	
	/*int srt(int &Time, int tcs, vector<Process> &p, vector<Process> ready_queue, int &cpu_occupy, Process *running_p, double alpha, int &tcs_check)*/
	
	int Time = 0;
	int cpu_occupy = 0;
	int check = -1;
	vector<Process> ready;
	Process running(' ',lamda,limit);
	running.cpu_remain = 0;
	cout << endl;
	while (Time < 12000000){
		int temp = srt(Time,(int)context_switch_time,P,ready,cpu_occupy,running,(double)alpha, check);
		if (temp == 0){
			break;
		}
		Time++;
	}
	cout << endl;
	
	output_process(total_fcfs);
	fcfs(total_fcfs,ready_queue,context_switch_time,number_of_process);
	P = generate(seed,number_of_process,limit, lamda);
	int order = 1;
	check = -1;
	int track_time_slice = 0;
	Time = 0;
	cpu_occupy = 0;
	vector<Process> ready1;
	Process running1(' ',lamda,limit);
	running1.cpu_remain = 0;
	cout << endl;
	while (Time < 12000000){
		//int &Time, int tcs, vector<Process> &p, vector<Process> &ready_queue, int &cpu_occupy, Process &running_p, double alpha, int time_slice, int& track_time_slice, int order
		int temp = rr(Time,(int)context_switch_time,P,ready1,cpu_occupy,running1,(double)alpha,time_slice,track_time_slice,order,check);
		if (temp == 0){
			break;
		}
		Time++;
	}
	
	return 0;
}

