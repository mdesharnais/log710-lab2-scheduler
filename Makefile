all:
	g++ -o scheduler -g -std=c++11 -Wfatal-errors -Wall -Wextra -Werror main.cpp
	./scheduler
