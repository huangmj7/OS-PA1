#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "Process.h"

using namespace std;
void srt(int &, int, vector<Process> &, vector<char> &, int &, int &, int &, char &);


int main() {
	vector<Process> p;
	for (int i = 0; i < 10; i++) {
		Process temp = Process((i+1) * 10, 0, 0, (i + 1) * 5, (i + 1) * 4, 'A');
		p.push_back(temp);
		cout << "arrive: " << p[i].arrive_time << " io: " << p[i].io_remain << " cpu: " << p[i].cpu_remain << " name: " << p[i].name << endl;
	}
	for (int i = 0; i < 10; i++) {
		Process temp = Process((i + 1) * 10+5, 0, 0, (i + 1) * 10+5, (i + 1) * 4+5, 'B');
		p.push_back(temp);
		cout << "arrive: " << p[i+10].arrive_time << " io: " << p[i+10].io_remain << " cpu: " << p[i+10].cpu_remain << " name: " << p[i + 10].name << endl;
	}
	vector<char> q;
	int t = 0, pre = 0, tau = 0, io = -1;
	char in = ' ';
	while (t < 1200) {
		srt(t, 0, p, q, pre, tau, io, in);
		t++;
	}
	return 0;
}

void srt(int &Time, int tcs, vector<Process> &total, vector<char> &ready_queue, int &pre_index, int &tau, int &io_finish, char &in_io) {
	if (Time == io_finish) {
		ready_queue.push_back(in_io);
		in_io = ' ';
	}
	/*Add process to queue*/
	for (int i = 0; i < total.size(); i++) {
		if (total[i].arrive_time == Time) {
			int check_multiple = 0;
			for (int k = 0; k < ready_queue.size(); k++) {
				if (total[i].name == ready_queue[k]) {
					check_multiple = 1;
				}
			}
			if (check_multiple == 0) {
				ready_queue.push_back(total[i].name);
			}
			cout << "time " << Time << "ms: Process " << total[i].name;
			cout << " (tau " << tau << "ms) arrived: added to ready queue [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				char temp = ready_queue[j];
				cout << " " << temp;
			}
			cout << "]" << endl;
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
		}
	}

	/*Find the shortest remain time of arrived process and record its index*/
	int index = 0;
	//Process smallest_remain = null;
	for (int i = 0; i < total.size(); i++) {
		if (total[i].arrive_time <= Time && total[i].cpu_remain > 0 && total[i].name != in_io) {
			//Process smallest_remain = total[i];
			index = i;
			for (int j = i + 1; j < total.size(); j++) {
				if (total[j].arrive_time <= Time && total[j].cpu_remain > 0) {
					if (total[j].cpu_remain < total[i].cpu_remain && total[i].name != in_io) {
						//smallest_remain = total[j];
						index = j;
					}
				}
			}
		}
	}


	/*pop_back out the shortest remain time process*/
	int check_queue = 0;
	for (int j = 0; j < ready_queue.size(); j++) {
		if (ready_queue[j] == total[index].name) {
			ready_queue.erase(ready_queue.begin()+j);
		}
	}

	bool context_switched = false;
	/*Check context-switch*/
	if (index != pre_index || Time == total[index].arrive_time) {
		context_switched = true;
	}
	else {
		context_switched = false;
	}

	/*Check preemption*/
	bool preemption = false;
	if (total[pre_index].name != total[index].name) {
		preemption = true;
	}

	/*Start Processing...*/
	if (context_switched == false && total[index].arrive_time <= Time) {
		total[index].cpu_remain--;
		//total[index].io_remain--;
		total[index].turnaround_time = Time - total[index].arrive_time;
	}
	else if (context_switched == true && preemption == false) {
		Time += tcs / 2;
		cout << "time " << Time << "ms: Process " << total[index].name << " started using the CPU for ";
		cout << total[index].cpu_remain << "ms burst [Q ";
		for (int j = 0; j < ready_queue.size(); j++) {
			char temp = ready_queue[j];
			cout << " " << temp;
		}
		if (ready_queue.size() == 0) {
			cout << "<empty>";
		}
		cout << "]" << endl;
	}

	/*When CPU burst is finished...*/
	if (total[index].cpu_remain <= 0) {
		
		/*Start to print complete message*/
		cout << "time " << Time << "ms: Process " << total[index].name << " completed a CPU burst; ";
		cout << 15 << " bursts to go [Q";
		for (int j = 0; j < ready_queue.size(); j++) {
			char temp = ready_queue[j];
			cout << " " << temp;
		}
		if (ready_queue.size() == 0) {
			cout << "<empty>";
		}
		cout << "]" << endl;
		/*Recalculate tau*/
		tau = 100; //To be changed
		cout << "time " << Time << "ms: Recalculated tau = " << tau << "ms for process " << total[index].name << " [Q";
		for (int j = 0; j < ready_queue.size(); j++) {
			char temp = ready_queue[j];
			cout << " " << temp;
		}
		if (ready_queue.size() == 0) {
			cout << "<empty>";
		}
		cout << "]" << endl;

		/*Start to print i/o message*/
		cout << "time " << Time << "ms: Process " << total[index].name << " switching out of CPU; will block on I/O until time ";
		cout << Time + total[index].io_remain << "ms [Q";
		io_finish = Time + total[index].io_remain;
		in_io = total[index].name;
		for (int j = 0; j < ready_queue.size(); j++) {
			char temp = ready_queue[j];
			cout << " " << temp;
		}
		if (ready_queue.size() == 0) {
			cout << "<empty>";
		}
		cout << "]" << endl;

		/*Exit time*/
		Time += tcs / 2;
	}
	/*When preemption is happened...*/
	if (preemption == true) {
		cout << "time " << Time << "ms: Process " << total[pre_index].name << " (tau  " << tau << "ms) completed I/O and will preempt ";
		cout << total[index].name << " [Q ";
		for (int j = 0; j < ready_queue.size(); j++) {
			char temp = ready_queue[j];
			cout << " " << temp;
		}
		if (ready_queue.size() == 0) {
			cout << "<empty>";
		}
		cout << "]" << endl;
		Time += tcs;
	}

	/*Change previous index to current index for the next iteration*/
	pre_index = index;

}