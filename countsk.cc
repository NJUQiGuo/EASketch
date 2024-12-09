#include <cassert>
#include <cstring>
#include "countsk.h"

int sgn(int u) { return u == 0 ? +1 : -1; }

countsk::countsk(size_t memory, int hash_num) 
	: size(memory / 4), hash_num(hash_num), hf(new hash_func[hash_num]) {
	cells = (unsigned*) malloc(memory);
	_acs_cnt = 0;
	memset(cells, 0, sizeof(cells));
}

countsk::~countsk() {
	delete[] cells;
	delete[] hf;
}

void countsk::add(elem_t e, int offset, int delta) {
	for (int i = 0; i < hash_num; ++i) {
		unsigned hval = hf[i](e);
		cells[(hval / 2 + offset) % size] += sgn(hval % 2) * delta;
	}
}

int countsk::query(elem_t e, int offset) const {
	_acs_cnt += hash_num;
	long sum = 0;
	std::vector<int> res;
	for (int i = 0; i < hash_num; ++i) {
		unsigned hval = hf[i](e);
		res.push_back(sgn(hval % 2) * (int) cells[(hval / 2 + offset) % size]);
	}
	std::sort(res.begin(), res.end());
	if (res.size() % 2) return res[res.size()/2];
	else return (res[res.size()/2] + res[res.size()/2-1])/2;
}

void countsk::copy(countsk *oth) {
	assert(oth->size == size);
	for (int i = 0; i < size; ++i)
		cells[i] = oth->cells[i];
	for (int i = 0; i < hash_num; ++i)
		hf[i] = oth->hf[i];
}

void countsk::clear(bool reset_hf) {
	if (reset_hf) {
		for (int i = 0; i < hash_num; ++i)
			hf[i].reset(); // Use new hash functions
	}
	memset(cells, 0, sizeof(unsigned) * size);
}