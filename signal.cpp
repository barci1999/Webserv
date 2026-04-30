#include "signal.hpp"
#include <string>
#include <iostream>
volatile sig_atomic_t Signal::runnin = 1;
void Signal::reginster_singal()
{
	std::signal(SIGINT,Signal::handler);
}
void Signal::handler(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << "ctrl + c detected"<<std::endl;
		Signal::runnin = 0;
	}
	
}