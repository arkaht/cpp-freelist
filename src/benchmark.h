#pragma once

#include <chrono>

class Benchmark
{
public:
	void start();
	void stop();
	void reset();

	int get_micro_seconds() const;
	int get_milliseconds() const;
	float get_seconds() const;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_point {}, end_point {};
	int time = 0;
};
