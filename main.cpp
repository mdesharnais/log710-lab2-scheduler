#include "types.hpp"
#include "load_file.hpp"

#include <iterator>

bool debug = false;

bool check_if_enough_resources (process p) {
	auto res = p.resources;

	if (p.resources[resource::printer] <= available_resources[resource::printer]
		&& p.resources[resource::modem] <= available_resources[resource::modem]
		&& p.resources[resource::cd] <= available_resources[resource::cd]
		&& p.resources[resource::scanner] <= available_resources[resource::scanner]
	) {
		return true;
	}else{
		return false;
	}
}

process substract_resources (process p) {
	for (auto& pair : p.resources) {
		available_resources[pair.first] -= pair.second;
	}
	return p;
}

process release_resources (process p) {
	for (auto& pair : p.resources) {
		available_resources[pair.first] += pair.second;
	}
	return p;
}

void run_main_loop(std::chrono::steady_clock::time_point last_run, int nb_tick, scheduler_state& state) {
	if (debug) {
		std::cout << "run_main_loop() nb_tick = " << nb_tick << std::endl;

		std::cout << "\t, state.current_process = " << (state.current_process ? "*" : "null") << std::endl;

		std::cout << "\t, state.proc_list = [";
		std::copy(std::begin(state.proc_list), std::end(state.proc_list), std::ostream_iterator<process>(std::cout));
		std::cout << "]" << std::endl;

		std::cout << "\t, state.queue_new_process = [";
		std::copy(std::begin(state.queue_new_process), std::end(state.queue_new_process), std::ostream_iterator<process>(std::cout));
		std::cout << "]" << std::endl;

		std::cout << "\t, state.queue_usr_waiting = [";
		std::copy(std::begin(state.queue_usr_waiting), std::end(state.queue_usr_waiting), std::ostream_iterator<process>(std::cout));
		std::cout << "]" << std::endl;

		std::cout << "\t, state.queue_real_time = [";
		std::copy(std::begin(state.queue_real_time), std::end(state.queue_real_time), std::ostream_iterator<process>(std::cout));
		std::cout << "]" << std::endl;

		for (auto& pair : state.queues_usr) {
			std::cout << "\t, state.queue_usr = [";
			std::copy(std::begin(pair.second), std::end(pair.second), std::ostream_iterator<process>(std::cout));
			std::cout << "]" << std::endl;
		}
	}

	//Check if we can start a new process
	auto it = std::begin(state.proc_list);
	while (it != std::end(state.proc_list)) {
		if (it->arrival == nb_tick) {
			state.queue_new_process.push_back(*it);
			it = state.proc_list.erase(it);
		} else {
			++it;
		}
	}

	//Empty list of new processes into usr_queue or queue_real_time
	for (auto p : state.queue_new_process) {
		if (p.m_priority == priority::real_time) {
			if (debug) {std::cout << "REAL-TIME-QUEUE" << std::endl;}
			state.queue_real_time.push_back(p);
		}else{
			if (debug) {std::cout << "USR-WAITING" << std::endl;}
			state.queue_usr_waiting.push_back(p);
		}
	}
	state.queue_new_process.clear();

	//Stop currently running process and move it to a higher priority queue, if possible
	if (state.current_process) {
		*state.current_process = pause(*state.current_process);
		state.current_process->m_priority = decrease_priority(state.current_process->m_priority);
		state.queues_usr[state.current_process->m_priority].push_back(*state.current_process);
	}

	//Run any real time process until completion
	while (not state.queue_real_time.empty()) {
		if (debug) {std::cout << "state.current_process = " << state.queue_real_time.front().m_id << std::endl;}
		state.current_process = make_unique<process>(state.queue_real_time.front());
		state.queue_real_time.pop_front();
		if (debug) {std::cout << "Size of queue_real_time: " << state.queue_real_time.size() << std::endl;}
		*state.current_process = start(*state.current_process);
		waitpid(state.current_process->m_id, nullptr, 0);
		state.current_process = nullptr;
	}

	//Add all processes that can be added to an usr queue
	{
		auto it = std::begin(state.queue_usr_waiting);
		while (it != std::end(state.queue_usr_waiting)) {
			if (check_if_enough_resources(*it)) {
				substract_resources(*it);
				state.queues_usr[it->m_priority].push_back(*it);
				it = state.queue_usr_waiting.erase(it);
			} else {
				++it;
			}
		}
	}

	//Run highest priority process
	for (auto& pair : state.queues_usr) {
		if (not pair.second.empty()) {
			state.current_process = make_unique<process>(pair.second.front());
			pair.second.pop_front();
			*state.current_process = start(*state.current_process);
			break;
		}
	}

	//Sleeeeeeeep
	auto wait = last_run + TICK;
	auto now = std::chrono::steady_clock::now();

	if (state.current_process) {
		while (not is_process_finished(*state.current_process) && now < wait) {
			std::this_thread::sleep_for(microTICK);
			now = std::chrono::steady_clock::now();
		}
		if (is_process_finished(*state.current_process)) {
			release_resources(*state.current_process);
			state.current_process = nullptr;
		}
	}
	std::this_thread::sleep_until(wait);

	bool empty = true;
	if (not state.proc_list.empty()) {
		if (debug) {std::cout<<"__1__"<<std::endl;}
		empty = false;
	}
	if (not state.queue_real_time.empty()) {
		if (debug) {std::cout<<"__2__"<<std::endl;}
		empty = false;
	}
	for (auto& pair : state.queues_usr) {
		pair.second.erase(std::remove_if(pair.second.begin(), pair.second.end(), is_process_finished), pair.second.end());
		if (not pair.second.empty()) {
			if (debug) {std::cout<<"__3__"<<std::endl;}
			empty = false;
		}
	}
	if (not empty) {
		run_main_loop(std::chrono::steady_clock::now(), ++nb_tick, state);
	}
}

int main() {
	std::ifstream file{"process.txt"};
	auto proc_list = load_file(file);

	auto state = scheduler_state{nullptr, proc_list, process_queue{}, process_queue{}, process_queue{}, {
		{priority::user_1, process_queue{}},
		{priority::user_2, process_queue{}},
		{priority::user_3, process_queue{}}
	}};
	run_main_loop(std::chrono::steady_clock::now(), 0, state);
}

