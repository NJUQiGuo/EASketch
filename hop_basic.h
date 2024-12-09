#include "bloom_filter.h"
#include "hpwin_sketch.h"

#include <cassert>

class hop_basic_bf : hpwin_sketch<bool> {
public:
	hop_basic_bf(int memory, int size, int hf_num, int per = 1) : size(size), period(per) {
		int mem = memory / size;
		bf = new bloom_filter*[size];
		lst = 0;
		counter = 0;
		for (int i = 0; i < size; ++i)
			bf[i] = new bloom_filter(mem, hf_num);
	}
	~hop_basic_bf() {
		for (int i = 0; i < size; ++i)
			delete bf[i];
		delete bf;
	}
	bool query(int win, elem_t e) const {
		assert(lst - size < win && win <= lst);
		return bf[win % size]->query(e);
	}
	bool query(int l, int r, elem_t e) const {
		for (int i = l; i <= r; ++i) if (query(i, e)) return true;
		return false;
	}
	void new_win() {
		if (counter % period == 0) {
			lst++; bf[lst % size]->clear();
		}
		counter++;
	}
	void ins_latest(elem_t e, int delta = 1) { bf[lst % size]->add(e); }
private:
	bloom_filter** bf;
	int size, lst, period;
	int counter;
};