#pragma once
#include "hpwin_sketch.h"
#include <cassert>


template<class sketch>
class pcnt1 : hpwin_sketch<int> {
public:
	pcnt1(size_t memory, int depth) : depth(depth) {
		sk = new sketch*[depth];
		for (int i = 0; i < depth; ++i)
			sk[i] = new sketch(memory / depth, (cfg.win_num + (1<<i) - 1) >> i, 3, 1 << i);
		rt_depth = 0;
		last_win = 0;
	}
	~pcnt1() {
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
		// for (int i = 0; i < depth; i++)
		// {
		// 	sk[i]->new_win();
		// }
		
		sk[0]->new_win();
		for (int i = 1; i < depth&&(last_win%(1<<i)==1); ++i)
			sk[i]->new_win();
	}
	void ins_latest(elem_t e, int delta = 1) {
		// int value1 = query(1,last_win,e);
		for (int i = 0; i < depth; ++i){
			// int value1 = sk[i]->query(-1,e);
			sk[i]->ins_latest(e, delta);
			// int value2 = sk[i]->query(-1,e);
			// printf("in layer %d, insert %ld, value1 = %d, value2=%d.\n",i,e,value1,value2);
		}
		// int value2 = query(1,last_win,e);
		// if (value1==value2)
		// {
		// 	printf("before insert %ld:value=%d,after insert:value=%d.\n",e,value1,value2);
		// }
		
		
	}
	int query(int l, int r, elem_t e) const {
		
		// int result1 = sk[0]->query(l-1,r-1,e);
		// int result2 = seg_query(1, 1 << rt_depth, l, r, 0, rt_depth, e);



		// for(int i = 0;i<depth;i++){
		// 	for(int j=0;j<(last_win>>i);j++){
		// 		printf("query sk[%d][%d], layer %d, range[%d,%d],query %ld, result:%d.\n",i,j,i,(j<<i)+1,((j+1)<<i),e,sk[i]->query(j,e));
		// 	}
		// }

		// int result3 = 0;
		// for (int k = 1; k < depth; k++)
		// {
		// 	int med_l = (l>>k)<<k+1;
		// 	int med_r = (r>>k)<<k;
		// 	if (((l>>k)<<k) == l)
		// 	{
		// 		med_l = l>>k;
		// 	}else{
		// 		med_l = (l>>k) + 1;
		// 	}
		// 	if (((r>>k)<<k)==r)
		// 	{
		// 		med_r = r>>k;
		// 	}else{
		// 		med_r = (r>>k)+1;
		// 	}
		// 	int level_val = sk[k]->query(med_l,med_r,e);
		// 	printf("cur_depth:%d,range[%d,%d],val:%d.\n",k,((med_l-1)<<k)+1,med_r<<k,level_val);
		// }
		
		// fprintf(stderr,"seg_query(1, %ld, %d, %d,0, %d, %ld)\n",1<<rt_depth,l,r,rt_depth,e);
		// int result4 = sk[0]->query(1,last_win,e);
		// printf("query range[%d,%d],result1=%d,result2=%d,result4=%d.\n",l,r,result1,result2,result4);
		// return result1;

		// return result1;
		// return sk[0]->query(l-1,r-1,e);
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
class hfsketch1 : hpwin_sketch<int> {
public:
	hfsketch1(size_t memory, int depth) : depth(depth) {
		sk = new sketch*[depth];
		for (int i = 0; i < depth; ++i)
			sk[i] = new sketch(memory / depth, (cfg.win_num + (1<<i) - 1) >> i, 3, 1 << i);
		rt_depth = 0;
		last_win = 0;
	}
	~hfsketch1() {
		for (int i = 0; i < depth; ++i) {
			// fprintf(stderr, "hfsketch1 free %d\n", i);
			delete sk[i];
		}
		delete[] sk;
	}
	void new_win() {
		
		last_win++;
		// fprintf(stderr,"hfsketch1::new_win, lastwin=%d.\n",last_win);
		// printf("last win:%d, creating new sketch....\n",last_win);
		if ((1 << rt_depth) < last_win) ++rt_depth;
		sk[0]->new_win();
		// fprintf(stderr,"hfsketch1 level 0 win creates.\n");
		// printf("level 0 creates.\n");
		for (int i = 1; i < depth; i++)
		{
			if (last_win%(1<<i)==1)
			{
				sk[i]->new_win();
				// fprintf(stderr,"hfsketch1 level %d win creates.\n",i);
				// printf("level:%d create.",i);
			}
			// printf("\n");
			
		}
		
		// for (int i = 0; i < depth; ++i)
		// 	sk[i]->new_win();
	}
	void ins_latest(elem_t e, int delta = 1) {
		//if (delta>1) fprintf(stderr, "???\n");
		// printf("inserting...\n");
		
		
		for (int i = 0; i < depth; ++i){
			// fprintf(stderr,"level = %d",i);
			// int before_ins = sk[i]->latest_query(e);
			sk[i]->ins_latest(e, delta);
			// int after_ins = sk[i]->latest_query(e);
			// fprintf(stderr,"hfsketch1::ins_latest, level = %d, insert value = %d, before insert value = %d, after insert value = %d.\n",i,delta,before_ins, after_ins);
		}
	}
	int query(int l, int r, elem_t e) const {
		// return sk[0]->query(l-1,r-1,e);
		// int sum = 0,med;
		// for (int i = l-1; i < r; i++)
		// {
		// 	med = sk[0]->query(i, e);
		// 	sum += med;
		// 	// printf("window %d value=%d,",i+1,med);
		// }
		// // printf("\n");
		// return sum;

		return seg_query(1, 1 << rt_depth, l, r, 0, rt_depth, e);
	}

	size_t memory() const{
		// fprintf(result_file,"Memory: \n");
		size_t memory = 0;
		size_t level_memory = 0;
		for (int i = 0; i < depth; i++)
		{
			level_memory = sk[i]->memory();
			// fprintf(result_file,"\tlevel %d: %ld\n",i,level_memory);
			memory += level_memory;
		}
		// fprintf(result_file,"Total Memory: %ld (B)= %ld (MB)\n",memory, memory>>20);
		return memory;
	}

	void print(){
		for (int i = 0; i < depth; i++)
		{
			// fprintf(result_file,"-----------------------------------level %d:-----------------------------------------\n",i);
			sk[i]->print();
		}
		
	}
	
private:
	int seg_query(int l, int r, int ql, int qr, int idx, int cur_depth, elem_t e) const {
		int mid = (l + r) / 2;
		if (ql <= l && r <= qr && cur_depth < depth) {
			assert(0 <= cur_depth && cur_depth < depth);
			//if (l != 1) printf("%d %d %d\n", cur_depth,l,idx);
			return sk[cur_depth]->query(idx, e);
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
class pyramidcm1 : hpwin_sketch<int> {
public:
	pyramidcm1(size_t memory, int depth) : depth(depth) {
		sk = new sketch*[depth];
		for (int i = 0; i < depth; ++i){
			// printf("in layer %d.\n",i);
			sk[i] = new sketch(memory / depth, (cfg.win_num + (1<<i) - 1) >> i, 3, 1 << i);
		}
		rt_depth = 0;
		last_win = 0;
	}
	~pyramidcm1() {
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
