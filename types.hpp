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

enum class priority {
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
	}else if (p == priority::real_time) {
		return priority::real_time;
	}else{
		return p;
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

std::ostream& operator<<(std::ostream& out, process /* p */) {
	out << "*";
	return out;
}

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

using process_queue = std::deque<process>;

struct scheduler_state {
	std::unique_ptr<process> current_process;
	process_queue proc_list;
	process_queue queue_new_process;
	process_queue queue_usr_waiting;
	process_queue queue_real_time;
	std::map<priority, process_queue> queues_usr;
};

process launch(process p) {
	auto child_pid = fork();

	switch (child_pid) {
		case -1: // Error
			std::cerr << "Error on fork(): " << strerror(errno) << "\n";
			throw "OMG!";
		case 0: //child process
		{
			auto ret = execl("./log710h14process", "log710h14process", std::to_string(p.exec_time).c_str(), nullptr);
			if (ret == -1) {
				throw "OMG!!!!";
			}
		}
		default: //Parent process
		{
			p.m_id = child_pid;
			return p;
		}
	}
}

bool is_process_finished(process p) {
	int status;
	int ret = waitpid(p.m_id, &status, WNOHANG);
	if (ret == -1 || (WEXITSTATUS(status) == 0 && ret != 0)) {
		return true;
	}else{
		return false;
	}
}

process pause(process p) {
	kill(p.m_id, SIGTSTP);
	return p;
}

process start(process p) {
	if (p.m_id == 0) {
		return launch(p);
	}else{
		kill(p.m_id, SIGCONT);
		return p;
	}
}

process end(process p) {
	kill(p.m_id, SIGINT);
	return p;
}
#endif
