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
#include <unistd.h>
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

enum class resource {
	cd,
	printer,
	scanner,
	modem
};

std::map<resource, int> available_resources = {
	{resource::cd, 2},
	{resource::printer, 2},
	{resource::scanner, 1},
	{resource::modem, 1}
};

struct process {
	id m_id;
	int arrival;
	priority m_priority;
	int exec_time;
	std::map<resource, int> resources;
};


using process_queue = std::deque<process>;

struct scheduler_state {
	std::unique_ptr<process> current_process;
	process_queue queue_new_process;
	process_queue queue_usr_waiting;
	process_queue queue_real_time;
	std::vector<process_queue> queues_usr;
};

process launch(process p) {
	p.m_id = execl("./log710h14process", "log710h14process", nullptr);
	return p;
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
