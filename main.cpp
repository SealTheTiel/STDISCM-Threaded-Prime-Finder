#include <iostream>
#include <sstream>
#include <cstdint>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <atomic>
#include "prime.h"
#include "log.h"

using namespace std;

typedef struct config {
	uint64_t number = NULL;
	uint64_t threads = NULL;
	int threadType = NULL;
	int printType = NULL;
	int delay = 0;
	mutex mutex;
};

void getPrimesFromRange(config &cfg, vector<logEntry> &logs, uint64_t start, uint64_t end, int threadId) {
	if (start > end) { return; }
	if (end <= 1) { return; }
	string primeString = "Primes: ";
	vector<uint64_t> primes;
	for (uint64_t i = start; i <= end; i++) {
		if (isPrime(i)) {
			primeString += to_string(i) + ", ";
		}
		this_thread::sleep_for(chrono::milliseconds(cfg.delay));
	}

	primeString.erase(primeString.length() - 2, primeString.length());
	if (primeString == "Primes") { primeString += ": None"; }
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
	for (uint64_t i = 0; i < cfg.threads - 1; i++) {
		threads.emplace_back(thread(getPrimesFromRange, ref(cfg), ref(logs), divisions[i], divisions[i + 1] - 1, i + 1));
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

void getSinglePrime(config &cfg, vector<logEntry> &logs, atomic<uint64_t> &currentNumber, int threadId) {
	uint64_t number = currentNumber.fetch_add((uint64_t) 1);
	while (number <= cfg.number) {
		this_thread::sleep_for(chrono::milliseconds(cfg.delay));
		if (isPrime(number)) {
			if (cfg.printType == 1) {
				printf("%s\n", logEntry(threadId, to_string(number) + " is prime.").getString().c_str());
			}
			else {
				lock_guard<mutex> lock(cfg.mutex);
				logs.emplace_back(logEntry(threadId, to_string(number) + " is prime."));
			}
		}
		number = currentNumber.fetch_add((uint64_t) 1);
	}
}

void getPrimesThreadPerNumber(config &cfg) {
	vector<thread> threads;
	vector<logEntry> logs;
	atomic<uint64_t> currentNumber = 2;

	cout << logEntry(0, "Starting threads...").getString() << endl;
	for (uint64_t i = 0; i < cfg.threads; i++) {
		threads.emplace_back(getSinglePrime, ref(cfg), ref(logs), ref(currentNumber), i + 1);
	}
	for (auto& thread : threads) {
		thread.join();
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
	cout << cfg.number << endl;

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
	cout << cfg.threads << endl;

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
	cout << cfg.threadType << endl;
	
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
	cout << cfg.printType << endl;

	cfg.delay = 0;
	if (cfg.threadType <= 1) {
		getPrimesByAllocation(ref(cfg));
	}
	else if (cfg.threadType >= 2) {
		getPrimesThreadPerNumber(ref(cfg));
	}

	return 0;
}