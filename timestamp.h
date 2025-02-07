#pragma once
#include <string>

using namespace std;
class timestamp
{
public:
	int day;
	int month;
	int year;
	int hour;
	int minute;
	int second;
	int nanosecond;
	
	timestamp(int day, int month, int year, int hour, int minute, int second, int nanosecond);

	timestamp();

	string getString();
};

