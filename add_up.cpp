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

void  compare_tau(vector<Process>&ready_queue){

        if(ready_queue.size()<2){return;}
        int j=0;
        for(int i=1; i<ready_queue.size();i++){

                char key = ready_queue[i];
                j = i-1;

                while(j >= 0 && ready_queue[j].tau > ready_queue[key].tau){

                        //cout << key << endl;
                        ready_queue[j+1] = ready_queue[j];
                        j--;
                }
                ready_queue[j+1] = key;
        }
}



void output(string algorithm,vector<Process> total,int tcs,int tncs,int np){

        float ave_turnaround = 0.0;
        float ave_wait = 0.0;
        float ave_cpu_burst = 0.0;
        int total_preemetive = 0;
        int total_context_switch = 0;

        for(vector<Process>::iterator itr=total.begin(); itr!= total.end(); ++itr){

                //cout << itr->first << " is " << itr->second.state << endl;
                ave_turnaround += itr->turnaround_time;
                //cout << itr->second.turnaround_time << endl;
                ave_wait += itr -> wait_time;
                total_preemetive += itr->preemptive;
                for(int j=0;j<itr->cpu_burst.size();j++){ave_cpu_burst += itr->cpu_burst[j];}
                ave_cpu_burst /= itr->cpu_burst.size();

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

