#include"stopwatch_sample.h"
#include<thread>
#include<iostream>
#include "..\acs\include\stopwatch.h"
using acs::clock::Stopwatch;

int stopwatch_sample() {
	auto sw = Stopwatch();
	sw.start();
	std::cout << "sw.run = " << sw.isRunning() << std::endl;
	for (size_t i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << sw.getTime().count() << std::endl;
	}
	sw.stop();
	std::cout << "sw = run" << sw.isRunning() << std::endl;
	for (size_t i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << "" << sw.getTime().count() << std::endl;
	}
	sw.reset();
	sw.start();
	std::cout << "sw.run = " << sw.isRunning() << std::endl;
	for (size_t i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << sw.getTime().count() << std::endl;
	}
	sw.stop();
	std::cout << "sw = run" << sw.isRunning() << std::endl;
	for (size_t i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << "" << sw.getTime().count() << std::endl;
	}
	return 0;
}