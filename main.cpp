#include "types.hpp"
#include "load_file.hpp"


bool check_if_enough_resources (process p) {
	auto res = p.resources;
	auto av_res = available_resources;

	if (res[resource::printer] <= av_res[resource::printer]
	&& res[resource::modem] <= av_res[resource::modem]
	&& res[resource::cd] <= av_res[resource::cd]
	&& res[resource::scanner] <= av_res[resource::scanner])
		return true;
	else
		return false;
}

void run_main_loop(std::chrono::steady_clock::time_point last_run, int nb_tick, scheduler_state& state) {
	std::cout << "run_main_loop() [nb_tick = " << nb_tick << ", state.proc_list = [";
	for (auto p : state.proc_list) {
		std::cout << "*";
	}
	std::cout << "]]" << std::endl;

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
			state.queue_real_time.push_back(p);
		}else{
			state.queue_usr_waiting.push_back(p);
		}
	}

	//Stop currently running process and move it to a higher priority queue, if possible
	if (state.current_process) {
		*state.current_process = pause(*state.current_process);
		state.current_process->m_priority = decrease_priority(state.current_process->m_priority);
		state.queues_usr[state.current_process->m_priority].push_back(*state.current_process);
	}

	//Run any real time process until completion
	while (not state.queue_real_time.empty()) {
		std::cout << "state.current_process = " << state.queue_real_time.front().m_id << std::endl;
		state.current_process = make_unique<process>(state.queue_real_time.front());
		state.queue_real_time.pop_front();
		std::cout << "Size of queue_real_time: " << state.queue_real_time.size() << std::endl;
		*state.current_process = start(*state.current_process);
		waitpid(state.current_process->m_id, nullptr, 0);
	}

	//Add all processes that can be added to an usr queue
	for (auto p : state.queue_usr_waiting) {
		if (check_if_enough_resources(p)) {
			state.queues_usr[p.m_priority].push_back(p);
		}
	}

	//Run highest priority process
	for (auto q : state.queues_usr) {
		if (not q.empty()) {
			state.current_process = make_unique<process>(q.front());
			q.pop_front();
			*state.current_process = start(*state.current_process);
			break;
		}
	}

	//Sleeeeeeeep
	int status;
	int ret = 0;
	auto wait = last_run + TICK;
	auto now = std::chrono::steady_clock::now();
	while (not (ret == -1 || (WEXITSTATUS(status) == 0 && ret != 0)) && now < wait) {
		if (state.current_process) {
			ret = waitpid(state.current_process->m_id, &status, WNOHANG);
		}else{
			ret = -1;
		}
		std::this_thread::sleep_for(microTICK);
		now = std::chrono::steady_clock::now();
	}

	bool empty = true;
	if (not state.proc_list.empty()) {
		empty = false;
	}
	if (not state.queue_real_time.empty()) {
		empty = false;
	}
	for (auto p : state.queues_usr) {
		if (not p.empty()) {
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



	auto state = scheduler_state{nullptr, proc_list, process_queue{}, process_queue{}, process_queue{}, {process_queue{}, process_queue{}, process_queue{}}};
	run_main_loop(std::chrono::steady_clock::now(), 0, state);
}

