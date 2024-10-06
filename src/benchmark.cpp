#include "benchmark.h"

using namespace std::chrono;

void Benchmark::start()
{
	start_point = high_resolution_clock::now();
}

void Benchmark::stop()
{
	end_point = high_resolution_clock::now();

	auto start = time_point_cast<microseconds>( start_point ).time_since_epoch().count();
	auto end = time_point_cast<microseconds>( end_point ).time_since_epoch().count();

	time = static_cast<int>( end - start );
}

void Benchmark::reset()
{
	time = 0;
}

int Benchmark::get_micro_seconds() const
{
	return time;
}

int Benchmark::get_milliseconds() const
{
	return time / 1000;
}

float Benchmark::get_seconds() const
{
	return time / 1000000.0f;
}