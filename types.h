#pragma once
#include <cstdint>

typedef uint64_t elem_t;

struct packet {
	elem_t e;
	double ts;
};

#define MB(k) ((size_t) (k) << 20)