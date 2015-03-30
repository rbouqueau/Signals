#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>


#define FORMAT(i, max) std::setw(1+(std::streamsize)log10(max)) << i

// Runs a function at instanciation:
// Use to initialize C libraries at program startup.
// Example:
// auto g_InitAv = runAtStartup(&av_register_all);
struct DummyStruct {
};

template<class R, class... Args>
DummyStruct runAtStartup(R f(Args...), Args... argVal) {
	f(argVal...);
	return DummyStruct();
}

inline
std::vector<char> stringDup(const char *src) {
	const size_t srcStrLen = strlen(src) + 1;
	std::vector<char> data(srcStrLen);
	strncpy(data.data(), src, srcStrLen);
	return data;
}

template<typename T>
T divUp(T num, T divisor) {
	return (num + divisor - 1) / divisor;
}

template<typename T>
std::vector<T> makeVector() {
	return std::vector<T>();
}

template<typename T>
std::vector<T> makeVector(T val) {
	std::vector<T> r;
	r.push_back(val);
	return r;
}

template<typename T, typename... Arguments>
std::vector<T> makeVector(T val, Arguments... args) {
	std::vector<T> r;
	r.push_back(val);
	auto const tail = makeVector(args...);
	r.insert(r.end(), tail.begin(), tail.end());
	return r;
}

template<typename Lambda, typename T, typename... Arguments>
std::vector<T> apply(Lambda func, std::vector<T>& input, Arguments... args) {
	std::vector<T> r;
	for(auto& element : input)
		r.push_back(func(element, args...));
	return r;
}

template<typename T>
std::unique_ptr<T> uptr(T* p) {
	return std::unique_ptr<T>(p);
}

template<typename T, typename U>
std::shared_ptr<T> safe_cast(std::shared_ptr<U> p)
{
	if (!p)
		return nullptr;
	auto r = std::dynamic_pointer_cast<T>(p);
	if(!r)
	{
		std::string s;
		s += "dynamic cast error: could not convert from ";
		s += typeid(U).name();
		s += " to ";
		s += typeid(T).name();
		throw std::runtime_error(s.c_str());
	}
	return r;
}

template<typename T, typename U>
T* safe_cast(U* p)
{
	if (!p)
		return nullptr;
	auto r = dynamic_cast<T*>(p);
	if(!r)
	{
		std::string s;
		s += "dynamic cast error: could not convert from ";
		s += typeid(U).name();
		s += " to ";
		s += typeid(T).name();
		throw std::runtime_error(s.c_str());
	}
	return r;
}

namespace Tools {

class Profiler {
public:
	Profiler(const std::string &name) : name(name) {
#ifdef _WIN32
		QueryPerformanceCounter(&startTime);
#else
		gettimeofday(&startTime, nullptr);
#endif
	}

	~Profiler() {
		std::cout << "[" << name.c_str() << "] " << FORMAT(elapsedInUs(), maxDurationInSec*unit) << " us" << std::endl;
	}

	uint64_t elapsedInUs() {
#ifdef _WIN32
		LARGE_INTEGER stopTime;
		QueryPerformanceCounter(&stopTime);
		LARGE_INTEGER countsPerSecond;
		QueryPerformanceFrequency(&countsPerSecond);
		return (uint64_t)((unit * (stopTime.QuadPart - startTime.QuadPart)) / countsPerSecond.QuadPart);
#else
		struct timeval stopTime;
		gettimeofday(&stopTime, nullptr);
		return ((uint64_t)stopTime.tv_sec * 1000000 + stopTime.tv_usec) - ((uint64_t)startTime.tv_sec * 1000000 + startTime.tv_usec);
#endif
	}

private:
	Profiler& operator= (const Profiler&) = delete;

	std::string name;
#ifdef _WIN32
	LARGE_INTEGER startTime;
#else
	struct timeval startTime;
#endif
	const int unit = 1000000;
	const int maxDurationInSec = 100;
};

}