#include <vector>

using id = int;

enum class priority
{
	real_time,
	user_1,
	user_2,
	user_3
};

struct process
{
	id m_id;
	priority m_priority;
};

using process_queue = std::vector<process>;
using queues = std::vector<process_queue>;

struct queues {
	current_process
	queue_new_process
	queue_usr_waiting
	queue_real_time
	queue_usr_1
	queue_usr_2
	queue_usr_3
};

//queues schedule(queues) { }

queues add_process(queues q, process p)
{
}

int main()
{
	auto processes = load_file();

	for (;;) {
	if (is_time_for_new_process(processes)) { }
		if (not new_processes.empty()) { add() }

	// job principale

	}
}
