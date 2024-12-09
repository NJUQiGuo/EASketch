#pragma once
#include "utils.h"
#include "hash.h"
#include "types.h"

class cusketch {
public:	
	cusketch(size_t memory, int hash_num);
	~cusketch();

	cusketch(const cusketch &) = delete;
	void operator=(const cusketch &) = delete;

	void add(elem_t e, int offset = 0, int delta = 1);
	int query(elem_t e, int offset = 0) const;
	
	void copy(cusketch *bf);
	void clear(bool reset_hf = true);

	size_t memory() const { return size * 4; }
    uint64_t acs_cnt() { return _acs_cnt; }
private:
	mutable uint64_t _acs_cnt;
	size_t size;
	int hash_num;
	unsigned *cells;
	hash_func* hf;
};