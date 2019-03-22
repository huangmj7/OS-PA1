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

//use command (valgrind) ./your-program-name 70 0.001 3000 1 8 0.5 80 end
//               ^                                ^    ^   ^
//               |                                |    |   |
//               memory debugger(delete)       lamda limit number of process


vector<Process> generate(int seed, int np, float limit, float lamda); //<-----Generator vector of Process
map<char,Process> Upload(vector<Process> p); //<------Generate dictionary
void print_queue(vector<char> p); //<--- print out what in ready_queue
void output(string algorithm,map<char,Process> total,int tcs,int tncs,int np);//<------final output

/*function--------------------------------------------------------------------------------------------------------*/

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


/*----------------------------------------------------------------------------------------------------------------*/





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


	//YOUR Algorithm GO HERE
	


        return 0;
}
