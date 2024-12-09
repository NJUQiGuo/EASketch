#include <cassert>
#include <cstring>
#include <climits>
#include "cusketch.h"

cusketch::cusketch(size_t memory, int hash_num) 
	: size(memory / 4), hash_num(hash_num), hf(new hash_func[hash_num]) {
	_acs_cnt = 0;
	cells = (unsigned*) malloc(memory);
	memset(cells, 0, memory);
}

cusketch::~cusketch() {
	delete[] cells;
	delete[] hf;
}

void cusketch::add(elem_t e, int offset, int delta) {
	for (int _t = 0; _t < delta; ++_t) {
		int w = query(e, offset);
		for (int i = 0; i < hash_num; ++i) {
			unsigned &r = cells[(hf[i](e) + offset) % size];
			if (r == w) ++r;
		}
	}
}

int cusketch::query(elem_t e, int offset) const {
	_acs_cnt += hash_num;
	unsigned ret = INT_MAX;
	for (int i = 0; i < hash_num; ++i) 
		ret = std::min(ret, cells[(hf[i](e) + offset) % size]);
	return ret;
}

void cusketch::copy(cusketch *oth) {
	assert(oth->size == size);
	for (int i = 0; i < size; ++i)
		cells[i] = oth->cells[i];
	for (int i = 0; i < hash_num; ++i)
		hf[i] = oth->hf[i];
}

void cusketch::clear(bool reset_hf) {
	if (reset_hf) {
		for (int i = 0; i < hash_num; ++i)
			hf[i].reset(); // Use new hash functions
	}
	memset(cells, 0, sizeof(unsigned) * size);
}