#include "DisplayAtInterval.h"
#include <iostream>
#include <Windows.h>

DisplayAtInterval::DisplayAtInterval()
{
	this->s_RepeatText = "Default Repeat Text";
	this->f_Interval = 1;
	this->b_Display = false;
}

DisplayAtInterval::DisplayAtInterval(std::string s_RepeatText, float f_Interval)
{
	this->s_RepeatText = s_RepeatText;
	this->f_Interval = f_Interval;
	this->b_Display = false;
}

void DisplayAtInterval::Display()
{
	while (b_Display)
	{
		std::cout << "Thread " << std::this_thread::get_id() << ": " << s_RepeatText << std::endl;
		Sleep(f_Interval * 1000);
	}
}

