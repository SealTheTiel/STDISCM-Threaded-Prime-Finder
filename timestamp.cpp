#include "timestamp.h"

#include <string>
#include <time.h>
#include <chrono>
using namespace std;
using namespace chrono;

timestamp::timestamp(int day, int month, int year, int hour, int minute, int second, int nanosecond) {
	this->day = day;
	this->month = month;
	this->year = year;
	this->hour = hour;
	this->minute = minute;
	this->second = second;
	this->nanosecond = nanosecond;
};

timestamp::timestamp() {
	time_t time_tNow = time(0);
	struct tm tmNow;
	localtime_s(&tmNow, &time_tNow);
	this->day = tmNow.tm_mday;
	this->month = tmNow.tm_mon + 1;
	this->year = tmNow.tm_year + 1900;
	this->hour = tmNow.tm_hour;
	this->minute = tmNow.tm_min;
	this->second = tmNow.tm_sec;
	this->nanosecond = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count() % 1000000000;
};

string timestamp::getString() {
	char temp[32];
	sprintf_s(temp, sizeof(temp), "[%02d/%02d/%04d %02d:%02d:%02d.%09d]", this->day, this->month, this->year, this->hour, this->minute, this->second, this->nanosecond);
	return string(temp);
}