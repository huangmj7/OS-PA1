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
		p.output();
	}

	return 0;
}
