#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "Process.h"

using namespace std;

/*Initially, ready queue is empty, cpu_occupy is 0, running_p is whatever, burst_num is 0*/
void srt(int &Time, int tcs, vector<Process> &p, vector<Process> &ready_queue, int &cpu_occupy, Process &running_p, double alpha) {
	/*Check Arrival*/
	for (int i = 0; i < p.size(); i++) {
		if (p[i].arrive_time == Time) {
			ready_queue.push_back(p[i]);
			cout << "time " << Time << "ms: Process " << p[i].name;
			cout << " (tau " << p[i].tau << "ms) arrived: added to ready queue [Q";
			/*Print ready queue*/
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;
			p[i].state = 1;
		}
	}

	/*Add shortest process from ready queue to cpu burst*/
	if (cpu_occupy == 0 && ready_queue.size() != 0) {
		int shortest = 0;
		for (int i = 0; i < ready_queue.size()-1; i++) {
			if (ready_queue[i].tau > ready_queue[i + 1].tau) {
				shortest = i + 1;
			}
		}
		
		Time += tcs / 2;
		cout << "time " << Time << "ms: Process " << ready_queue[shortest].name << " started using the CPU for ";
		cout << ready_queue[shortest].cpu_burst[ready_queue[shortest].burst_num] << "ms burst [Q ";
		ready_queue[shortest].state = 2;
		running_p = ready_queue[shortest];
		running_p.cpu_remain += ready_queue[shortest].cpu_burst[ready_queue[shortest].burst_num];
		ready_queue.erase(ready_queue.begin() + shortest, ready_queue.begin() + shortest + 1);
		for (int j = 0; j < ready_queue.size(); j++) {
			cout << " " << ready_queue[j].name;
		}
		if (ready_queue.size() == 0) {
			cout << "<empty>";
		}
		cout << "]" << endl;
		cpu_occupy = 1;
		
	}

	/*Process finishes and goes into i/o state*/
	if (running_p.cpu_remain == 0) {
		if (running_p.number_of_burst == 1) {
			cout << "time " << Time << "ms: Process " << running_p.name << " terminated [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;
			cpu_occupy = 0;
			Time += tcs / 2;
		}
		else {
			cout << "time " << Time << "ms: Process " << running_p.name << " completed a CPU burst: ";
			running_p.number_of_burst--;
			cout << running_p.number_of_burst << "bursts to go [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;

			running_p.tau = (1 - alpha)*running_p.tau + alpha * running_p.cpu_burst[running_p.burst_num];
			cout << "time " << Time << "ms: Recalculated tau = " << running_p.tau << "ms for process " << running_p.name << " [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;

			cout << "time " << Time << "ms: Process " << running_p.name << " switching out of CPU; will block on I/O until time ";
			cout << Time + running_p.io_time[running_p.burst_num] << "ms [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;
			running_p.io_remain += running_p.io_time[running_p.burst_num];
			running_p.burst_num++;
			Time += tcs / 2;
			running_p.state = 3;
			cpu_occupy = 0;
		}
	}

	/*Once i/o finish, check preemption*/
	for (int i = 0; i < p.size(); i++) {
		if (p[i].state == 3) {
			if (p[i].io_remain == 0) {
				ready_queue.push_back(p[i]);
				p[i].state = 1;
			}
		}
	}
	vector<Process> temp;
	for (int i = 0; i < p.size(); i++) {
		if (p[i].state == 1 || p[i].state == 2) {
			temp.push_back(p[i]);
		}
	}
	int shortest = 0;
	for (int i = 0; i < temp.size() - 1; i++) {
		if (temp[i].tau > temp[i + 1].tau) {
			shortest = i + 1;
		}
	}
	if (temp[shortest].state == 1) {
		temp[shortest].preemptive++;
		cout << "time " << Time << "ms: Process " << temp[shortest].name << " (tau  " << temp[shortest].tau << "ms) completed I/O and will preempt ";
		cout << running_p.name << " [Q ";
		for (int j = 0; j < ready_queue.size(); j++) {
			cout << " " << ready_queue[j].name;
		}
		if (ready_queue.size() == 0) {
			cout << "<empty>";
		}
		int index = 0;
		for (int i = 0; i < ready_queue.size(); i++) {
			if (temp[shortest].name == ready_queue[i].name) {
				index = i;
				break;
			}
		}
		ready_queue.erase(ready_queue.begin() + index, ready_queue.begin() + index + 1);
		Time += tcs / 2;
	}

	/*Running state*/
	for (int i = 0; i < p.size(); i++) {
		if (p[i].state == 1) {
			p[i].wait_time++;
		}
		else if (p[i].state == 2) {
			p[i].cpu_remain--;
		}
		else if (p[i].state == 3) {
			p[i].io_remain--;
		}
		p[i].turnaround_time = Time - p[i].arrive_time;
	}
}
/*track_time_slice is 0 initially*/
void rr(int &Time, int tcs, vector<Process> &p, vector<Process> &ready_queue, int &cpu_occupy, Process &running_p, double alpha, int time_slice, int& track_time_slice) {
	/*Check Arrival*/
	for (int i = 0; i < p.size(); i++) {
		if (p[i].arrive_time == Time) {
			ready_queue.push_back(p[i]);
			cout << "time " << Time << "ms: Process " << p[i].name;
			cout << " (tau " << p[i].tau << "ms) arrived: added to ready queue [Q";
			/*Print ready queue*/
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;
			p[i].state = 1;
		}
	}

	/*Add process from ready queue to cpu burst based on insertion sequence*/
	if (cpu_occupy == 0 && ready_queue.size() != 0) {
		Time += tcs / 2;
		cout << "time " << Time << "ms: Process " << ready_queue[0].name << " started using the CPU for ";
		cout << ready_queue[0].cpu_burst[ready_queue[0].burst_num] << "ms burst [Q ";
		ready_queue[0].state = 2;
		running_p = ready_queue[0];
		if (running_p.cpu_remain == 0) {
			running_p.cpu_remain += ready_queue[0].cpu_burst[ready_queue[0].burst_num];
		}
		ready_queue.erase(ready_queue.begin(), ready_queue.begin() + 1);
		for (int j = 0; j < ready_queue.size(); j++) {
			cout << " " << ready_queue[j].name;
		}
		if (ready_queue.size() == 0) {
			cout << "<empty>";
		}
		cout << "]" << endl;
		cpu_occupy = 1;

	}

	/*Process finishes and goes into i/o state*/
	if (running_p.cpu_remain == 0) {
		if (running_p.number_of_burst == 1) {
			cout << "time " << Time << "ms: Process " << running_p.name << " terminated [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;
			cpu_occupy = 0;
			Time += tcs / 2;
			track_time_slice = 0;
		}
		else {
			cout << "time " << Time << "ms: Process " << running_p.name << " completed a CPU burst: ";
			running_p.number_of_burst--;
			cout << running_p.number_of_burst << "bursts to go [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;

			running_p.tau = (1 - alpha)*running_p.tau + alpha * running_p.cpu_burst[running_p.burst_num];
			cout << "time " << Time << "ms: Recalculated tau = " << running_p.tau << "ms for process " << running_p.name << " [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;

			cout << "time " << Time << "ms: Process " << running_p.name << " switching out of CPU; will block on I/O until time ";
			cout << Time + running_p.io_time[running_p.burst_num] << "ms [Q";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;
			if (running_p.io_remain != 0) {
				running_p.io_remain += running_p.io_time[running_p.burst_num];
			}
			running_p.burst_num++;
			Time += tcs / 2;
			running_p.state = 3;
			cpu_occupy = 0;
			track_time_slice = 0;
		}
	}
	/*When i/o burst finishes, push it back to ready queue*/
	for (int i = 0; i < p.size(); i++) {
		if (p[i].state == 3) {
			if (p[i].io_remain == 0) {
				ready_queue.push_back(p[i]);
				p[i].state = 1;
			}
		}
	}

	/*Preemption*/
	if (track_time_slice == time_slice) {
		if (ready_queue.size() != 0) {
			cout << "time " << Time << " ms: Time slice expired; process " << running_p.name << " preempted with ";
			cout << running_p.cpu_remain << "ms to go [Q ";
			for (int j = 0; j < ready_queue.size(); j++) {
				cout << " " << ready_queue[j].name;
			}
			if (ready_queue.size() == 0) {
				cout << "<empty>";
			}
			cout << "]" << endl;
			ready_queue.push_back(running_p);
			Time += tcs / 2;
			track_time_slice = 0;
			cpu_occupy = 0;
			running_p.state = 1;
			running_p.preemptive++;
		}
		else {
			cout << "time " << Time << "ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]" << endl;
			track_time_slice = 0;
			cpu_occupy = 1;
			running_p.state = 2;
		}
	}

	/*Running state*/
	for (int i = 0; i < p.size(); i++) {
		if (p[i].state == 1) {
			p[i].wait_time++;
		}
		else if (p[i].state == 2) {
			p[i].cpu_remain--;
		}
		else if (p[i].state == 3) {
			p[i].io_remain--;
		}
		p[i].turnaround_time = Time - p[i].arrive_time;
		track_time_slice++;
	}

}