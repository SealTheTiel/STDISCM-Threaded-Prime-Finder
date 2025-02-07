#pragma once
#include <string>
#include "timestamp.h"

class logEntry {
public:
	string message;
	uint64_t threadId;
	timestamp time;

	logEntry() {
		this->threadId = 0;
		this->message = "";
		this->time = timestamp();
	}

	logEntry(uint64_t threadId, string message) {
		this->threadId = threadId;
		this->message = message;
		this->time = timestamp();
	}
	
	string getString() {
		return this->time.getString() + "  [Thread: " + to_string(this->threadId) + "]: \t" + this->message;
	}
};