#ifndef TYPES_HPP
#define TYPES_HPP

#include <sys/types.h>
#include <signal.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using id = int;

auto TICK = std::chrono::seconds(1);

enum class priority {
	real_time,
	user_1,
	user_2,
	user_3
};

enum class ressource {
	cd,
	printer,
	scanner,
	modem
};

std::map<ressource, int> available_ressources = {
	{ressource::cd, 2},
	{ressource::printer, 2},
	{ressource::scanner, 1},
	{ressource::modem, 1}
};

struct process {
	id m_id;
	int arrival;
	priority m_priority;
	int exec_time;
	std::map<ressource, int> ressources;
};


using process_queue = std::deque<process>;

struct scheduler_state {
	std::unique_ptr<process> current_process;
	process_queue queue_new_process;
	process_queue queue_usr_waiting;
	process_queue queue_real_time;
	std::vector<process_queue> queues_usr;
};

void launch(process /* p */) {

}

void pause(process p) {
	kill(p.m_id, SIGTSTP);
}

void start(process p) {
	kill(p.m_id, SIGCONT);
}

void end(process p) {
	kill(p.m_id, SIGINT);
}

#endif
