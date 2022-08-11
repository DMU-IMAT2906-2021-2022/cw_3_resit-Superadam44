#pragma once
#include <thread>
#include <string>

class DisplayAtInterval
{
public:
	DisplayAtInterval();

	DisplayAtInterval(std::string s_RepeatText, float f_Interval);

	void Display();

	void SetText(std::string text)
	{
		s_RepeatText = text;
	}

	void SetInterval(float interval)
	{
		f_Interval = interval;
	}

	void Start()
	{
		b_Display = true;
	}
	void Stop()
	{
		b_Display = false;
	}
	
private:

	std::string s_RepeatText;
	float f_Interval;
	bool b_Display;

};

