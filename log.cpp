#include "log.h"
#include "timestamp.h"

#include <string>

logEntry::logEntry() {
	this->threadId = 0;
	this->message = "";
	this->time = timestamp();
};

logEntry::logEntry(uint64_t threadId, string message) {
	this->threadId = threadId;
	this->message = message;
	this->time = timestamp();
};

string logEntry::getString() {
	return this->time.getString() + "  [Thread: " + to_string(this->threadId) + "]: \t" + this->message;
}