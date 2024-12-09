#pragma once
#include "types.h"
#include <vector>
#include <algorithm>

class bitset {
public:
	bitset(int bits);
	bitset(const bitset &oth);
	~bitset();

	bool operator[](int i) const;
	void set(int i);
	void clear();
	int count() const;
	int memory() const;

private:
	const int bits;
	const int cells;
	uint64_t *bs;
};

struct hashnode {
	elem_t idx;
	int val, nxt;

	hashnode(elem_t idx, int val, int nxt)
		: idx(idx), val(val), nxt(nxt) {}
};

class hashmap {
public:
	hashmap();
	void add(elem_t e, int dta);
	std::pair<elem_t, int> sample() const;
	bool empty() const;
	void clear();
private:

	void clear_dead();

	int mod, cnt_del;
	int *head;
	std::vector<hashnode> nodes;
};