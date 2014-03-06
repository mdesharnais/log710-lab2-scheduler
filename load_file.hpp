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

std::vector<process> load_file(std::ifstream &file) {
	//le format des données de chaque ligne est:
	//<temps d’arrivée>, <priorité>, <temps d’exécution>, <nombre d’imprimantes>, 
	//<nombre de scanneurs>, <nombre de modems>, <nombre de CD> 
	std::string cur_line;
	std::vector<process> proc_list;
	char delimiter = ',';
	while (getline(file, cur_line)) {
		//cout << "///" << endl;
		std::cout << cur_line << std::endl;
		std::string p_info;
    		cur_line.erase(std::remove(cur_line.begin(), cur_line.end(), ' '), cur_line.end());

		auto proc_infos = extract_infos(cur_line, delimiter); 
	}


	return proc_list;
}