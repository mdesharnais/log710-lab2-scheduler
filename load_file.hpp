#include "types.hpp"

std::vector<std::string> extract_infos(std::string line, char delimiter) {

	std::vector<std::string> proc_infos;
	auto first = begin(line);
	auto last = std::find(first, std::end(line), delimiter);

	while (last != std::end(line)) {
		proc_infos.emplace_back(first, last);
		first = std::next(last);
		last = std::find(first, std::end(line), delimiter);
	}

	proc_infos.emplace_back(first, last);

	return proc_infos;
}

priority parse_priority(std::string s) {
	if (s == "0")
		return priority::real_time;
	else if (s == "1")
		return priority::user_1;
	else if (s == "2")
		return priority::user_2;
	else if (s == "3")
		return priority::user_3;
	else
		throw "input error on parse_priority(string): unrecognized priority";
}

process_queue load_file(std::ifstream &file) {
	//le format des données de chaque ligne est:
	//<temps d’arrivée>, <priorité>, <temps d’exécution>, <nombre d’imprimantes>,
	//<nombre de scanneurs>, <nombre de modems>, <nombre de CD>
	std::string cur_line;
	process_queue proc_list;
	char delimiter = ',';
	while (getline(file, cur_line)) {
		//cout << "///" << endl;
		std::string p_info;
			cur_line.erase(std::remove(cur_line.begin(), cur_line.end(), ' '), cur_line.end());
		auto proc_infos = extract_infos(cur_line, delimiter);
		process proc;
		proc.arrival = stoi(proc_infos[0]);
		proc.m_priority = parse_priority(proc_infos[1]);
		proc.exec_time = stoi(proc_infos[2]);
		proc.resources[resource::printer] = std::stoi(proc_infos[3]);
		proc.resources[resource::scanner] = std::stoi(proc_infos[4]);
		proc.resources[resource::modem] = std::stoi(proc_infos[5]);
		proc.resources[resource::cd] = std::stoi(proc_infos[6]);
		proc.m_id = 0;
		proc_list.push_back(proc);
	}


	return proc_list;
}
