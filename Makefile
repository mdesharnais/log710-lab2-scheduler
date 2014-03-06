all:
	g++ -o scheduler -std=c++11 -Wall -Wextra -Werror main.cpp
	./scheduler
