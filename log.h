#pragma once
#include <string>
#include "timestamp.h"

using namespace std;
class logEntry {
public:
	string message;
	uint64_t threadId;
	timestamp time;

	logEntry();

	logEntry(uint64_t threadId, string message);
	
	string getString();
};