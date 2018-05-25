#include "timer_sample.h"
#include "..\acs\include\timer.h"
#include<thread>
#include<iostream>
using acs::clock::Timer;

int timer_sample() {
	auto sw = Timer();
	sw.registerTime(std::chrono::milliseconds(100));
	sw.start();
	std::cout << "sw.run = " << sw.isRunning() << std::endl;
	for (size_t i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << sw.getRemainingTime().count() << std::endl;
	}
	sw.stop();
	std::cout << "sw = run" << sw.isRunning() << std::endl;
	for (size_t i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << "" << sw.getRemainingTime().count() << std::endl;
	}
	sw.reset();
	sw.start();
	std::cout << "sw.run = " << sw.isRunning() << std::endl;
	for (size_t i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << sw.getRemainingTime().count() << std::endl;
	}
	sw.stop();
	std::cout << "sw = run" << sw.isRunning() << std::endl;
	for (size_t i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << "" << sw.getRemainingTime().count() << std::endl;
	}
	return 0;
}