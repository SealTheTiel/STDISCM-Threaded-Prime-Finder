#include <iostream>
#include <sstream>
#include <cstdint>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <atomic>
#include <map>
#include <cmath>

#include "prime.h"
#include "log.h"
#include "threadPool.h"

using namespace std;

typedef struct config {
	uint64_t number = NULL;
	uint64_t threads = NULL;
	int threadType = NULL;
	int printType = NULL;
	int delay = 0;
	mutex mutex;
};

void getPrimesFromRange(config &cfg, vector<logEntry> &logs, uint64_t start, uint64_t end, uint64_t threadId) {
	if (start > end) { return; }
	if (end <= 1) { return; }
	string primeString = "Primes: ";
	vector<uint64_t> primes;
	for (uint64_t i = start + 1; i <= end; i++) {
		if (isPrime(i)) {
			primeString += to_string(i) + ", ";
		}
		this_thread::sleep_for(chrono::milliseconds(cfg.delay));
	}

	primeString.erase(primeString.length() - 2, primeString.length());
	if (primeString == "Primes") { 
		primeString += ": None"; 
	}
	if (cfg.printType == 1) {
		printf("%s\n", logEntry(threadId, primeString).getString().c_str());
	}
	else {
		lock_guard<mutex> lock(cfg.mutex);
		logs.emplace_back(logEntry(threadId, primeString));
	}
}


void getPrimesByAllocation(config &cfg) {
	vector<uint64_t> divisions = { (uint64_t)0 };
	vector<thread> threads;
	vector<logEntry> logs;
	if (cfg.threads > cfg.number) { cfg.threads = cfg.number; }

	for (uint64_t i = 1; i <= cfg.threads; i++) {
		divisions.push_back(cfg.number / cfg.threads * i);
	}
	if (cfg.number % cfg.threads != 0) {
		divisions[divisions.size() - 1] += cfg.number % cfg.threads;
	}
	cout << logEntry(0, "Starting threads...").getString() << endl;
	for (uint64_t i = 0; i < cfg.threads; i++) {
		threads.emplace_back(thread(getPrimesFromRange, ref(cfg), ref(logs), divisions[i], divisions[i + 1], i));
	}

	for (auto& thread : threads) {
		thread.join();
	}

	if (cfg.printType == 2)
	{	
		for (auto& log : logs) {
			cout << log.getString() << endl;
		}
	}

	cout << logEntry(0, "Threads finished.").getString() << endl;
}

void checkPrimeMultithread(config &cfg, vector<logEntry> &logs, threadPool &pool, map<thread::id, uint64_t> &threadIdMap,  uint64_t number) {
	if (number <= 1) { return; }
	if (number <= 3) { 
		if (cfg.printType == 1) {
			printf("%s\n", logEntry(0, to_string(number) + " is prime.").getString().c_str());
		}
		else {
			lock_guard<mutex> lock(cfg.mutex);
			logs.emplace_back(logEntry(0, to_string(number) + " is prime."));
		}
		return;
	}
	atomic<bool> isPrime = true;
	atomic<thread::id> lastThreadChecked;

	/*
	 * Integers can be represented as 6k + i, where i = 0 -> 5
	 * If i = 0, 2, 3, or 4, then 6k + i is divisible by 2 and/or 3
	 * So we only need to check for i = 1 and i = 5
	 */
	if (number % 2 == 0 || number % 3 == 0) { isPrime = false; }
	//cout << number << endl;
	for (uint64_t i = 2; i * i <= number; i ++) {
		pool.enqueue([number, i, &isPrime, &lastThreadChecked] {
			//printf("%d enqueued!\n", i);
			lastThreadChecked = this_thread::get_id();
			if (number % i == 0) {
				isPrime = false;
			}
		});
	}
	if (isPrime) {
		uint64_t id = NULL;
		for (auto& pair : threadIdMap) {
			if (pair.first == lastThreadChecked) {
				id = pair.second;
				break;
			}
		}
		if (id == NULL) { return; }
		if (cfg.printType == 1) {
			printf("%s\n", logEntry(id, to_string(number) + " is prime.").getString().c_str());
		}
		else {
			lock_guard<mutex> lock(cfg.mutex);
			logs.emplace_back(logEntry(id, to_string(number) + " is prime."));
		}
	}
}

void getPrimesThreadPerNumber(config& cfg) {
	map<thread::id, uint64_t> threadIdMap;
	threadPool threads(cfg.threads, ref(threadIdMap));
	vector<logEntry> logs;
	atomic<uint64_t> currentNumber = 2;
	cout << logEntry(0, "Starting threads...").getString() << endl;
	for (uint64_t i = 0; i <= cfg.number; i++) {
		checkPrimeMultithread(ref(cfg), ref(logs), ref(threads), ref(threadIdMap), i);
	}
	if (cfg.printType == 2) {
		for (auto& log : logs) {
			cout << log.getString() << endl;
		}
	}
	cout << logEntry(0, "Threads finished.").getString() << endl;
	 
}


int main() {
	config cfg;
	string temp;
	while (cfg.number == NULL) {
		cout << "[Max number]\n    Highest number to check for primes.\n\nEnter: ";
		cin >> temp;
		istringstream iss(temp);
		iss >> cfg.number;
		if (iss.fail() || !iss.eof() || cfg.number < 3 || temp[0] == '-') {
			cout << "Please enter a number from 2 to 18,446,744,073,709,551,615\n\n";
			cfg.number = NULL;
		}
	}

	while (cfg.threads == NULL) {
		cout << "\n[Threads]\n    Number of threads to use.\n\nEnter: ";
		cin >> temp;
		istringstream iss(temp);
		iss >> cfg.threads;
		if (iss.fail() || !iss.eof() || cfg.number < 3 || temp[0] == '-') {
			cout << "Please enter a number from 2 to 18,446,744,073,709,551,615\n\n";
			cfg.threads = NULL;
		}
	}

	while (cfg.threadType == NULL) {
		cout << "\n[Threading type]\n    [1/A] Divide across the max number\n    [2/B] Thread each number linearly\n\nEnter: ";
		cin >> temp;
		char allowed[] = { 'A', 'B', 'a', 'b', '1', '2' };
		for (char c : allowed) {
			if (temp[0] == c) {
				if (c == 'A' || c == 'a' || c == '1') { cfg.threadType = 1; }
				else { cfg.threadType = 2; }
				break;
			}
		}
		if (cfg.threadType == NULL) {
			cout << "Please enter 1/A/a or 2/B/b\n\n";
		}
	}
	while (cfg.printType == NULL) {
		cout << "\n[Print type]\n    [1/A] Print as soon as a thread finishes\n    [2/B] Print when all threads are finished\n\nEnter: ";
		cin >> temp;
		char allowed[] = { 'A', 'B', 'a', 'b', '1', '2' };
		for (char c : allowed) {
			if (temp[0] == c) {
				if (c == 'A' || c == 'a' || c == '1') { cfg.printType = 1; }
				else { cfg.printType = 2; }
				break;
			}
		}
		if (cfg.printType == NULL) {
			cout << "Please enter 1/A/a or 2/B/b\n\n";
		}
	}

	cfg.delay = 0;
	if (cfg.threadType <= 1) {
		getPrimesByAllocation(ref(cfg));
	}
	else if (cfg.threadType >= 2) {
		getPrimesThreadPerNumber(ref(cfg));
	}

	return 0;
}