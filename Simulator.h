#include<iostream>
#include<string>
#include<vector>
#include "Process.h"
using namespace std;


class Simulator{

        public:
                /*Constructor*/
                Simulator();

                /*Muataor*/
                int generate_simualtion(); //success return 0, violentation on given requrie return -1,else return -2
                int run(int mode); //1:FCFS 2:SJF 3:SRF 4:RR return -1 if fail else return 0
                int arrive(Porcess p); //add process to simulator
                int finish(process p); //remove finish process from simulator
                void current_process(process p); //set current process when empty
                void set_cpu(); //set each burst time based on equation:
                bool IsSwitch(); //is in the switch time
                void PrintOut(); //print out result in queue

        private:

                vector<Process> queue;
                vector<Process> finished_queue;
                Process current_p;
                float time_counter;
                int num_process;//argv[4]
                float lmbda; //argv[2]
                float interval_time;//argv[1]
                float switch_time;//argv[5]
                float upper_bound_time;//argv[3]
                float alphe; //argv[6]
                float time_slice;//argv[7]
                string RR_option;//argv[8]

};

