#pragma once
#include <vector>
#include <cstdlib>
#include "types.h"

#define MAX_PRIME32 1229
#define MAX_BIG_PRIME32 50

class BOBHash32 {
public:
	BOBHash32();
	~BOBHash32();
	BOBHash32(uint32_t prime32Num);
	void initialize(uint32_t prime32Num);
	uint32_t run(const char * str, uint32_t len);	// produce a hash number
	static uint32_t get_random_prime_index();
	static std::vector<uint32_t> get_random_prime_index_list(int n);
	uint32_t get_prime32() const;
private:
	uint32_t prime32Num;
};

/**
 * Hash function for type T
 * Use BOBHash to compute hash values
 */
template<class T>
class base_hash_func {
public:
	base_hash_func() {
		uint32_t seed = rand() % MAX_PRIME32;
		BOB.initialize(seed);
	}

	void reset() {
		uint32_t seed = rand() % MAX_PRIME32;
		BOB.initialize(seed);
	}

	uint32_t operator()(T e) {
		uint32_t result = BOB.run((char *)&e, sizeof(T));
		return result;
	}

private:
	BOBHash32 BOB;
};

typedef base_hash_func<elem_t> hash_func;