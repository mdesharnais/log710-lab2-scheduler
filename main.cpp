#include "types.hpp"
#include "load_file.hpp"


bool check_if_enough_resources (process p) {
	//
}

void run_main_loop(std::chrono::steady_clock::time_point last_run, state) {
	//Empty list of new processes into usr_queue or real_time_queue
	for (auto p : new_proc_queue) {
		if p.priority == real_time{
			queue_real_time.push_back(p);
		}else{
			queue.queue_usr_waiting.push_back(p);
		}
	}

	//Stop currently running process and move it to a higher priority queue, if possible
	if (current_process != nullptr) {
		stop(current_process);
		current_process.priority = std::max(current_process.priority - 1, 1);
		queues_usr[current_process.priority].push_back(current_process);
	}

	//Run any real time process until completion
	while (not real_time_queue.empty()) {
		current_process = real_time_queue.front();
		real_time_queue.pop_front();
		start(current_process.id);
		waitpid(current_process.id, nullptr, 0);
	}

	//Add all processes that can be added to an usr queue
	for (auto p : queue_usr_waiting) {
		if (check_if_enough_resources(p)) {
			queues_usr[p.priority].push_back(p);
		}
	}

	//Run highest priority process
	bool cont = true;
	for (int i = 0; i < queues_usr.size() and cont; i++) {
		auto q = queues_usr[i];
		if (not q.empty()) {
			current_process = q.front();
			q.pop_front();
			start(current_process);
			cont = false;
		}
	}

	std::this_thread::sleep_until(last_run + TICK);
	run_main_loop(system::chrono::steady_clock::now());
}

int main() {
	std::ifstream file{"process.txt"};
	auto proc_list = load_file(file);

	auto state = scheduler_state{nullptr, process_queue{}, process_queue{}, process_queue{}, process_queue{}, process_queue{}, process_queue{}};

	run_main_loop(0, state);
}

