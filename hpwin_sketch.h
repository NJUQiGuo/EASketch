#pragma once
#include "types.h"

template<typename T>
class hpwin_sketch {
public:
	virtual void new_win() = 0;
	virtual void ins_latest(elem_t e, int delta) = 0;
	virtual T query(int l, int r, elem_t e) const = 0;
};