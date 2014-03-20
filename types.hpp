#ifndef TYPES_HPP
#define TYPES_HPP

#include <sys/types.h>
#include <sys/wait.h>
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
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

using id = int;

auto TICK = std::chrono::seconds(1);
auto microTICK = std::chrono::milliseconds(1);

enum priority {
	user_1,
	user_2,
	user_3,
	real_time
};

priority decrease_priority (priority p) {
	if (p == priority::user_3) {
		return priority::user_2;
	}else if (p == priority::user_2) {
		return priority::user_1;
	}else if (p == priority::user_1) {
		return priority::user_1;
	}else{
		return priority::real_time;
	}
}

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

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

using process_queue = std::deque<process>;

struct scheduler_state {
	std::unique_ptr<process> current_process;
	process_queue queue_new_process;
	process_queue queue_usr_waiting;
	process_queue queue_real_time;
	std::vector<process_queue> queues_usr;
};

process launch(process p) {
	auto child_pid = fork();

	switch (child_pid) {
		case -1: // Error
			std::cerr << "Error on fork(): " << strerror(errno) << "\n";
		case 0: //child process
		{
			execl("./log710h14process", "log710h14process", nullptr);
		}
		default: //Parent process
		{
			p.m_id = child_pid;
			return p;
		}
	}
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
