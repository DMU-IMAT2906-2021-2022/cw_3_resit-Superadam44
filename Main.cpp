#include <iostream>
#include <thread>
#include <string>
#include <Windows.h>
#include "DisplayAtInterval.h"
#include <mutex>
#include <future>
#include <fstream>
#include <vector>
#include <sstream>
#include <chrono>

bool b_DisplayTime;

float f_Seconds;

int results[10];

std::mutex mtx;

std::mutex fileReadMutex;

std::vector<std::string> stringsFromFile;
std::vector<std::future<void>> futures; //Add async futures to a vector so they aren't destroyed before finishing execution

std::ifstream MyFile("credits.csv");

std::chrono::time_point<std::chrono::steady_clock> start;
std::chrono::time_point<std::chrono::steady_clock> end;
std::chrono::duration<float> duration;


struct Timer
{
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;

	Timer()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;

		float ms = duration.count() * 1000.0f;
		std::cout << "Timer took " << ms << "ms" << std::endl;
	}
};

void AddNumbers(int num1, int num2)
{
	//Timer timer;
	std::vector<std::vector<int>> nums; //Make 2d vector and add numbers to it with nested for loop to create a longer task
	for (int i = 0; i < 10000; i++)
	{
		std::vector<int> row(num1, num2);
		nums.push_back(row);
		for (int j = 0; j < 10000; j++)
		{
			nums[i].push_back(num1 + num2 * j);

		}
	}
	std::cout << "Total was " << nums[nums.size() - 1][nums.size() - 1] << std::endl;

	end = std::chrono::high_resolution_clock::now();
	duration = end - start;
	std::cout << duration.count() << "s\n";
}

void ReadFileFunc()
{
	start = std::chrono::high_resolution_clock::now();
	std::ifstream MyFile("credits.csv");
	std::string line;
	std::string cell;
	std::lock_guard<std::mutex> lock(fileReadMutex); //Mutex stops other threads accessing the vector until current thread is done with it

	while (std::getline(MyFile, line))
	{
		std::stringstream lineStream(line); //Create linestream to read through the current line
		while (std::getline(lineStream, cell, ',')) //Gets each part of the string between the commas
		{
			stringsFromFile.push_back(cell);        //Adds separated strings to vector
		}
		if (!lineStream && cell.empty())            //Checks for empty 'cells' where there was no data
		{
			stringsFromFile.push_back("");
		}
	}
	MyFile.close();
}

void WriteFileFunc()
{
	std::ofstream MyFile("single-column-credits.txt"); //Create new file
	std::lock_guard<std::mutex> lock(fileReadMutex);

	for (int i = 0; i < stringsFromFile.size(); i++)
	{
		MyFile << stringsFromFile[i] << "\n";
	}

	MyFile.close();
	
}



void SetNumbers()
{
	const std::lock_guard<std::mutex> lock(mtx); //The mutex lock does not allow other threads to access any shared data for the duration of this scope

	for (int i = 0; i < 10; i++)
	{
		results[i] = i * 2; //Results cannot be accessed by the calculating thread. It must wait until setNumbers() has finished and the lock guard will be released.
		std::cout << "Set number " << i << std::endl;
		Sleep(1000);
	}
}

//Without the lock guard, both threads access the results simultaneously so incomplete and incorrect calculations will be made, as there is no order.
void CalculateResults()
{
	const std::lock_guard<std::mutex> lock(mtx);

	for (int i = 0; i < 10; i++)
	{
		results[i] *= results[i];
		std::cout << "Calculated number " << i << std::endl;
		Sleep(1000);
	}

	std::cout << "Results are: ";
	for (int i = 0; i < 10; i++)
	{
		std::cout << results[i] << ", ";
	}
	std::cout << std::endl;
}


void DisplayTime()
{
	while (true)
	{
		while (b_DisplayTime) //user input can determine whether to loop
		{
			auto local = std::chrono::system_clock::now();
			std::time_t time = std::chrono::system_clock::to_time_t(local);

			std::cout << "Current date and time: " << ctime(&time) << std::endl;
			Sleep(f_Seconds * 1000); //Sleep thread for f_Seconds -> milliseconds amount of time determined by user input

		}
	}
}

//Example 1 of multithreading- The time is being displayed from one thread while another thread gets user commands to control the output of the time.
void InputControl()
{
	std::cout << "The time is currently being displayed in the console every second." << std::endl;
	std::cout << "Enter 'Stop' to stop displaying the time and 'Start' to start again." << std::endl;
	std::cout << "Enter a number in seconds to set the interval for displaying the time." << std::endl;
	
	while (true)
	{
		float x;
		std::string input;
		std::cin >> input;
		for (int i = 0; i < input.length(); i++) { input[i] = tolower(input[i]); } //Converts each char in string to lowercase to make string comparison accurate.
		if (input == "stop")
		{
			b_DisplayTime = false;
		}
		else if (input == "start")
		{
			b_DisplayTime = true;
		}
		else
		{
			x = std::stof(input); //convert string input to float
			if (x > 0)
			{
				f_Seconds = x;
			}
			else
			{
				std::cout << "Invalid input, try again." << std::endl;
			}
		}
	}
}

int main(){
	std::cout << "CW_3 Threading" << std::endl;
	std::cout << "Thread " << std::this_thread::get_id() << ": Hello from main thread" << std::endl;
	b_DisplayTime = true;
	f_Seconds = 1;
	
	//std::thread first(DisplayTime);

	//std::thread second(InputControl);

	//Example 2 of multithreading- Class designed to have each object display something to the console on a different thread.
	//DisplayAtInterval announce = DisplayAtInterval("Announcement!", 1); //Instantiate an object to display with text and interval params 
	//announce.Start(); //Start Display() loop
	//std::thread annoucementThread(&DisplayAtInterval::Display, &announce); //Call Display() function from the object on a new thread

	//DisplayAtInterval message = DisplayAtInterval("Hello, this is a different thread", 2);
	//message.Start();
	//std::thread messageThread(&DisplayAtInterval::Display, &message);

	//annoucementThread.join(); //Main Thread waits for these threads to finish before proceeding.
	//messageThread.join();
	//first.join();
	//second.join();

	//Mutex lock guard threads
	/*std::thread setNumbersThread(SetNumbers);
	std::thread calculateThread(CalculateResults);

	setNumbersThread.join();
	calculateThread.join();*/

	/*ReadFileFunc();
	WriteFileFunc();
	AddNumbers(100, 100);*/
	//auto start = std::chrono::high_resolution_clock::now();
	/*std::thread readFileThread(ReadFileFunc);
	std::thread writeFileThread(WriteFileFunc);
	std::thread addNumbersThread(AddNumbers, 100, 100);
	readFileThread.join();
	writeFileThread.join();
	addNumbersThread.join();*/

	//Calls functions asynchronously + Add async futures to a vector so they aren't destroyed before finishing execution
	futures.push_back(std::async(std::launch::async, ReadFileFunc));
	futures.push_back(std::async(std::launch::async, WriteFileFunc));
	futures.push_back(std::async(std::launch::async, AddNumbers, 100, 100));

	//auto end = std::chrono::high_resolution_clock::now();

};