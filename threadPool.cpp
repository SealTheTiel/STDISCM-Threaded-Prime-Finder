#include "threadPool.h"

#include <vector>
#include <utility>
#include <map>

using namespace std;
threadPool::threadPool(uint64_t nThreads, map<thread::id, uint64_t> &threadIds) {
	for (uint64_t i = 0; i < nThreads; i++) {
		threads.emplace_back(
			[this, i, &threadIds] {
				function<void()> task;
				threadIds.emplace(this_thread::get_id(), i);
				while (true) {
					{
						unique_lock<mutex> lock(this->poolMutex);
						this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
						if (this->stop && this->tasks.empty()) {
							return;
						}
						task = move(this->tasks.front());
						this->tasks.pop();
					}
					task();
				}
			}
		);
	}
}

void threadPool::enqueue(function<void()> &&task) {
	{
		unique_lock<mutex> lock(poolMutex);
		tasks.emplace(task);
	}
	condition.notify_one();
}


threadPool::~threadPool() {
	{
		unique_lock<mutex> lock(poolMutex);
		stop = true;
	}
	condition.notify_all();
	for (auto& thread : threads) {
		thread.join();
	}
}
