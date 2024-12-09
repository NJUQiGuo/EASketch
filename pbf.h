#pragma once
#include "hpwin_sketch.h"
#include <cassert>


template<class sketch>
class multi_layer1 : hpwin_sketch<int> {
public:
	multi_layer1(size_t memory, int depth) : depth(depth) {
		sk = new sketch*[depth];
		for (int i = 0; i < depth; ++i)
			sk[i] = new sketch(memory / depth, (cfg.win_num + (1<<i) - 1) >> i, 3, 1 << i);
		rt_depth = 0;
		last_win = 0;
	}
	~multi_layer1() {
		for (int i = 0; i < depth; ++i) {
			delete sk[i];
		}
		delete[] sk;
	}
	void new_win() {
		last_win++;	
		if ((1 << rt_depth) < last_win) ++rt_depth;
		sk[0]->new_win();
		for (int i = 1; i < depth&&(last_win%(1<<i)==1); ++i)
			sk[i]->new_win();
	}
	void ins_latest(elem_t e, int delta = 1) {
		for (int i = 0; i < depth; ++i){
			sk[i]->ins_latest(e, delta);
		}	
	}
	int query(int l, int r, elem_t e) const {
		return seg_query(1, 1 << rt_depth, l, r, 0, rt_depth, e);
	}
	
private:
	int seg_query(int l, int r, int ql, int qr, int idx, int cur_depth, elem_t e) const {
		// fprintf(stderr,"seg_query(%d, %d, %d, %d,%d, %d, %ld)\n",l,r,ql,qr,idx,cur_depth,e);
		int mid = (l + r) / 2;
		if (ql <= l && r <= qr && cur_depth < depth) {
			assert(0 <= cur_depth && cur_depth < depth);
			//if (l != 1) printf("%d %d %d\n", cur_depth,l,idx);
			// int result = sk[cur_depth]->query(idx + 1, e);
			int result = sk[cur_depth]->query(idx, e);// idx aims to pyramid

			// printf("query depth:%d,range[%d,%d],value=%d.\n",cur_depth,(idx<<cur_depth)+1,(idx+1)<<cur_depth,result);
			return result;
			// return sk[cur_depth]->query(idx + 1, e);
		}

		if (qr <= mid)
			return seg_query(l, mid, ql, qr, idx*2, cur_depth - 1, e);
		else if (ql > mid)
			return seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth - 1, e);
		else
			return seg_query(l, mid, ql, qr, idx*2, cur_depth - 1, e) +
					seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth - 1, e);
	}
	
	sketch **sk;
	int depth;
	int rt_depth;
	int last_win;
};

template<class sketch>
class multi_layer2 : hpwin_sketch<int> {
public:
	multi_layer2(size_t memory, int depth) : depth(depth) {
		sk = new sketch*[depth];
		for (int i = 0; i < depth; ++i){
			// printf("in layer %d.\n",i);
			sk[i] = new sketch(memory / depth, (cfg.win_num + (1<<i) - 1) >> i, 3, 1 << i);
		}
		rt_depth = 0;
		last_win = 0;
	}
	~multi_layer2() {
		for (int i = 0; i < depth; ++i) {
			// fprintf(stderr, "pcnt free %d\n", i);
			delete sk[i];
		}
		delete[] sk;
	}
	void new_win() {
		last_win++;	
		// printf("pbh new win, last_win:%d.\n",last_win);
		if ((1 << rt_depth) < last_win) ++rt_depth;
		sk[0]->new_win();
		for (int i = 1; i < depth&&(last_win%(1<<i)==1); ++i)
			sk[i]->new_win();
	}
	void ins_latest(elem_t e, int delta = 1) {
		for (int i = 0; i < depth; ++i){
			// printf("in layer %d,\n",i);
			// int value1= sk[i]->query(-1,e);
			sk[i]->ins_latest(e, delta);
			// int value2 = sk[i]->query(-1,e);
			// fprintf(result_file,"in layer %d,insert element %ld,value1:%d,value2:%d.\n",i,e,value1,value2);
			// if(value1==value2) printf("in layer %d,insert error:value1:%d,value2:%d.\n",i,value1,value2);
		}
		// query(1,last_win,e);

	}
	int query(int l, int r, elem_t e) const {
		// int result1 = sk[0]->query(l-1,r-1,e);
		// int result2 = seg_query(1, 1 << rt_depth, l, r, 0, rt_depth, e);
		// printf("Now, for element %ld,[%d,%d], the frequency1 is: %d, frequency2 is %d.\n",e,l,r,result1,result2);
		// return result1;

		return seg_query(1, 1 << rt_depth, l, r, 0, rt_depth, e);
	}
	size_t memory() const{
		size_t result = 0;
		for (int i = 0; i < depth; ++i){
			// printf("in layer i:%d,memory:%d MB.\n",i,(sk[i]->memory())>>20);
			result+= sk[i]->memory();
		}
		return result;
	}
	
private:
	int seg_query(int l, int r, int ql, int qr, int idx, int cur_depth, elem_t e) const {
		// fprintf(stderr,"seg_query(%d, %d, %d, %d,%d, %d, %ld)\n",l,r,ql,qr,idx,cur_depth,e);
		int mid = (l + r) / 2;
		if (ql <= l && r <= qr && cur_depth < depth) {
			assert(0 <= cur_depth && cur_depth < depth);
			//if (l != 1) printf("%d %d %d\n", cur_depth,l,idx);
			// int result = sk[cur_depth]->query(idx + 1, e);
			int result = sk[cur_depth]->query(idx, e);// idx aims to pyramid

			// printf("query depth:%d,idx:%d,range[%d,%d],value=%d.\n",cur_depth,idx,(idx<<cur_depth)+1,(idx+1)<<cur_depth,result);
			return result;
			// return sk[cur_depth]->query(idx + 1, e);
		}

		if (qr <= mid)
			return seg_query(l, mid, ql, qr, idx*2, cur_depth - 1, e);
		else if (ql > mid)
			return seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth - 1, e);
		else
			return seg_query(l, mid, ql, qr, idx*2, cur_depth - 1, e) +
					seg_query(mid+1, r, ql, qr, idx*2+1, cur_depth - 1, e);
	}
	
	sketch **sk;
	int depth;
	int rt_depth;
	int last_win;
};
