#include "countsk.h"
#include "hpwin_sketch.h"

class hpskc : hpwin_sketch<int> {
public:
	hpskc(int memory, int hash_num, int win_num) : win_num(win_num) {
		csk = new countsk*[win_num];
		
		for (int i = 0; i < win_num; ++i)
			csk[i] = new countsk(memory / win_num, hash_num);
	}

	void new_win() {
		csk[(lat+1) % win_num]->copy(csk[lat]);
		lat = (lat+1) % win_num;
	}
	void ins_latest(elem_t e, int delta) { csk[lat % win_num]->add(e, 0, delta); }

	int query(int l, int r, elem_t e) const {
		return (int) (csk[r % win_num]->query(e) - csk[(l-1) % win_num]->query(e));
	}

private:
	int lat, win_num;
	countsk **csk;
};