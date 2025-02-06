#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include "prime.h"
#include "log.h"

using namespace std;

typedef unsigned long long ull;

typedef struct config {
	ull number;
	int threads;
	int threadType;
	int printType;
	mutex mutex;
};

void getPrimesFromRange(config &cfg, vector<logEntry> &logs, ull start, ull end, int threadId) {
	if (start > end) { return; }
	if (end <= 1) { return; }
	string primeString = "Primes: ";
	vector<ull> primes;
	for (ull i = start; i <= end; i++) {
		if (isPrime(i)) {
			primeString += to_string(i) + ", ";
			this_thread::sleep_for(chrono::milliseconds(50));
		}
	}
	primeString.erase(primeString.length() - 2, primeString.length());
	if (cfg.printType == 1) {
		printf(logEntry(threadId, primeString).getString().c_str());
	}
	else {
		cfg.mutex.lock();
		logs.emplace_back(logEntry(threadId, primeString));
		cfg.mutex.unlock();
	}
}

void getPrimesByAllocation(config &cfg) {
	vector<ull> divisions = { (ull)0 };
	vector<thread> threads;
	vector<logEntry> logs;
	for (int i = 1; i <= cfg.threads; i++) {
		divisions.push_back(cfg.number / cfg.threads * i);
	}
	if (cfg.number % cfg.threads != 0) {
		divisions[divisions.size() - 1] += cfg.number % cfg.threads;
	}
	cout << logEntry(0, "Starting threads...").getString();
	for (int i = 0; i < cfg.threads; i++) {
		threads.emplace_back(thread(getPrimesFromRange, ref(cfg), ref(logs), divisions[i], divisions[i + 1], i + 1));
	}

	for (auto& thread : threads) {
		thread.join();
	}

	if (cfg.printType == 2)
	{	
		for (auto& log : logs) {
			cout << log.getString();
		}
	}

	cout << logEntry(0, "Threads finished.").getString() << endl;
}

void getSinglePrime(config &cfg, vector<logEntry> &logs, ull number, int threadId) {
	if (isPrime(number)) {
		printf(logEntry(threadId, to_string(number) + " is prime.").getString().c_str());
	}
}

void getPrimesPerThread(config &cfg) {
	vector<thread> threads(cfg.threads);
	vector<logEntry> logs;
	atomic<ull> currentNumber = 1;
	
	while (currentNumber <= cfg.number) {

	}
}

int main() {
	config cfg;
	cout << "[Max number]\n    Highest number to check for primes.\n\nEnter: ";
	cin >> cfg.number;
	cout << "\n[Threads]\n    Number of threads to use.\n\nEnter: ";
	cin >> cfg.threads;
	cout << "\n[Threading type]\n    [1] Divide across the max number\n    [2] Thread each number linearly\n\nEnter: ";
	cin >> cfg.threadType;
	cout << "\n[Print type]\n    [1] Print as soon as a thread finishes\n    [2] Print when all threads are finished\n\nEnter: ";
	cin >> cfg.printType;

	if (cfg.threadType <= 1) {
		getPrimesByAllocation(ref(cfg));
	}
	else if (cfg.threadType >= 2) {
		getPrimesPerThread(ref(cfg));
	}

	return 0;
}