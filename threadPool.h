#pragma once
#include <future>
#include <vector>
#include <queue>
#include <utility>
#include <map>
using namespace std;

//template from geeks4geeks
class threadPool
{
public:
	threadPool(uint64_t nThreads, map<thread::id, uint64_t> &threadIds);
	~threadPool();

	void enqueue(function<void()>&& task);
private:
	vector<thread> threads;
	queue<function<void()>> tasks;
	mutex poolMutex;
	condition_variable condition;
	bool stop;
};


