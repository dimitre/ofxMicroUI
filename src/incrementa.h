#pragma once

/*

Incrementa

*/

#include <chrono>
using namespace std::chrono;
using namespace std::chrono_literals;

// Genérico pra Uso no featureCam.
struct incrementador {
	std::unordered_map <std::string, float> incrementadorTemporal;
	using space = std::chrono::duration<long double, std::nano>;
	space interval;
	float mult;
	std::unordered_map <std::string, std::chrono::time_point<steady_clock>> lastTick;

	float inc(const std::string & uniqueId, float amount) {
		if (incrementadorTemporal.find(uniqueId) == incrementadorTemporal.end() ) {
			reset(uniqueId);
		}

		if (lastTick.find(uniqueId) != lastTick.end()) {
			interval = steady_clock::now() - lastTick[uniqueId];
			mult = duration_cast<milliseconds>(interval).count()/33.0f;
			incrementadorTemporal[uniqueId] += mult * amount;
		}
		lastTick[uniqueId] = std::chrono::steady_clock::now();
		return incrementadorTemporal[uniqueId];
	}

	void reset(const std::string & uniqueId) {
		incrementadorTemporal[uniqueId] = 0;
	}
};
