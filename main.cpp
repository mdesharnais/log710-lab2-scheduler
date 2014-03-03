#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include <iostream>
#include <string.h>
#include <stdio.h>

using id = int;
using namespace std;

enum class priority {
	real_time,
	user_1,
	user_2,
	user_3
};

struct process {
	id m_id;
	int arrival;
	priority m_priority;
	int exec_time;
	int nb_printer;
	int nb_scanner;
	int nb_modem;
	int nb_cd;
};

using process_queue = std::vector<process>;

//queues schedule(queues) { }


vector<process> load_file(std::string file_name) {
	//le format des données de chaque ligne est:
	/*
	<temps d’arrivée>, <priorité>, <temps d’exécution>, <nombre d’imprimantes>, 
	<nombre de scanneurs>, <nombre de modems>, <nombre de CD> 
	*/
	string cur_line;
	vector<process> proc_list;
	ifstream file(file_name, ifstream::in);
	while(getline(file, cur_line)) {
		//cout << "///" << endl;
		cout << cur_line << endl;
		string p_info;
		int info_no=1;
		while(getline(cur_line, p_info, ', ')) {
			process p;
			switch (info_no) {
				case 1:
					p.arrival = stoi(p_info);
					break;
				case 2:
					p.m_priority = stoi(p_info);
					break;
				case 3:
					p.exec_time = stoi(p_info);
					break;
				case 4:
					p.nb_printer = stoi(p_info);
					break;
				case 5:
					p.nb_scanner = stoi(p_info);
					break;
				case 6:
					p.nb_modem = stoi(p_info);
					break;
				case 7:
					p.nb_cd = stoi(p_info);
					break;
				default:
					cout << "to much arguments in the process list file" << endl;
			}
			proc_list.push_back(p);
			info_no++;
		}
	}
	
	return proc_list;
}

int main() {
	vector<process> proc_list = load_file("process.txt");
}

void main_prog() {
/*
	auto processes = load_file("process.txt");

	for (;;) {
		if (is_time_for_new_process(processes)) {

		}
		if (not new_processes.empty()) {
			add()
		}

		// job principale

	}
*/
}
