#pragma once

bool isPrime(uint64_t number) {
	if (number <= 1) return false;
	if (number <= 3) return true;

	/*
	 * Integers can be represented as 6k + i, where i = 0 -> 5
	 * If i = 0, 2, 3, or 4, then 6k + i is divisible by 2 and/or 3
	 * So we only need to check for i = 1 and i = 5
	 */

	if (number % 2 == 0 || number % 3 == 0) return false;
	for (uint64_t i = 5; i * i <= number; i += (uint64_t) 6) {
		if (number % i == 0 || number % (i + 2) == 0) return false;
	}
	return true;
}