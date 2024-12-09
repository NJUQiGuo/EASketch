#include "hopping.h"
#include "hpwin_sketch.h"
#include "env.h"
#include <climits>
#include <cstdio>
#include <math.h>

hopping_cm::hopping_cm(size_t memory, int size, int k, int period) : size(size), k(k), period(period) {
	cm = new cmsketch<unsigned>*[size];
	//printf("hop: %d %d\n", size, k);
	for (int i = 0; i < size; ++i)
		cm[i] = new cmsketch<unsigned>(memory / size, 1);
	last_win = 0;
	counter = 0;
}

hopping_cm::~hopping_cm() {
	for (int i = 0; i < size; ++i) {
		//fprintf(stderr, "free %d\n", i);
		delete cm[i];
	}
	delete[] cm;
}

void hopping_cm::new_win() {
	
	// if (counter % period == 0) {
	// 	last_win++;
	// 	cm[(last_win+k-1) % size]->clear();
	// }

	last_win++;
	cm[(last_win+k-1) % size]->clear();


	counter++;
}

void hopping_cm::ins_latest(elem_t e, int delta) {
	for (int i = 0; i < k; ++i) cm[(last_win + i) % size]->add(e, last_win, delta);
}

int hopping_cm::query(int win, elem_t e) const {
	
	// return cm[win%size]->query(e,win);
	// if (win > last_win+k-1)
	// {
	// 	return 0;
	// }

	if (win==-1)
	{
		win = last_win;
	}
	
	
	
	unsigned res = INT_MAX;
	for (int i = win; i < win + k; ++i)
		if (i > last_win + k - 1 - size) res = std::min(res, cm[i % size]->query(e, win));

	if (res==INT_MAX)
	{
		res = 0;
	}
	
	// printf("win:%d,res:%d.\n",win,res);
	return res;
}

// int hopping_cm::query(elem_t e) const {
// 	unsigned res = INT_MAX;
// 	for (int i = last_win; i < last_win + k; ++i)
// 		if (i > last_win + k - 1 - size) res = std::min(res, cm[i % size]->query(e, last_win));
// 	return res;
// }

int hopping_cm::query(int l, int r, elem_t e) const {
	int sum = 0;
	for (int i = l; i <= r; ++i){
		// printf("sk[i][%d]:%d.\n",i,query(i,e));
		sum += query(i, e);
	}
	return sum;
}


pcm1_base::pcm1_base(size_t memory, int size, int k, int period): size(size), k(k), period(period)
{
	sketch_size = memory/size;
	cm.push_back(new cmsketch<unsigned>(sketch_size,1));
	if (cm.empty())
	{
		fprintf(stderr, "Initial memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}
	if (!cm[0])
	{	
		fprintf(stderr, "Memory allocation for cmsketch<unsigned>[0] failed\n");
		exit(EXIT_FAILURE);
	}
	
	counter = 0;
	tmp = new int[k];
	if (!tmp) {
        fprintf(stderr, "Memory allocation for tmp array failed\n");
        exit(EXIT_FAILURE);  // Terminate the program if allocation fails
    }
}

pcm1_base::~pcm1_base()
{
	for (int i = 0; i < size; i++)
	{
		free(cm[i]);
	}
	cm.clear();
}
void pcm1_base::new_win()
{
	counter++;
	if (counter > 1)
	{
		cm.push_back(new cmsketch<unsigned>(sketch_size,1));
	}
}

void pcm1_base::ins_latest(elem_t e, int delta)
{
	cm[counter-1]->add(e,0,delta);
}

int pcm1_base::query(int l, int r, elem_t e) const
{
	int sum = 0;
	for (int i = l; i <= r; ++i) sum += query(i, e);
	return sum;
}

int pcm1_base::query(int ts, elem_t e) const
{
	return cm[ts]->query(e);
}
int pcm1_base::latest_query(elem_t e) const
{
	if (counter ==0)
	{	
		fprintf(stderr,"error, counter = 0.");
	}
	
	return cm[counter-1]->query(e);
}


/// @brief 
/// @return 
size_t pcm1_base::memory() const{
	// sum memory of every sketch
	size_t memory = 0;
	for (int i = 0; i < counter; i++)
	{
		memory += cm[i]->memory();
	}

	return memory;
}

void pcm1_base::print()
{
	for (int i = 0; i < counter; i++)
	{
		cm[i]->print();
	}
	
}

pyramid_pcm::pyramid_pcm(double memory, int size, int k, int period): size(size), k(k), period(period)
{
	sketch_size = memory/size;
	// printf("total memory:%d MB, size:%d, sketch_memory:%ld.\n",((size_t)memory)>>20,size,sketch_size);
	word_size = 64;
	// w_p = (size_t) memory * 8.0 / (word_size * 2);
	w_p = (size_t) (sketch_size *8 / (word_size * 2));
	// printf("sketch_size:%ld.\n",sketch_size);
	// cm.push_back(new cmsketch<unsigned>(sketch_size,1));
	pcm.push_back(new PCMSketch(w_p, LOW_HASH_NUM, word_size));
	if (pcm.empty())
	{
		fprintf(stderr, "Initial memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}
	if (!pcm[0])
	{	
		fprintf(stderr, "Memory allocation for cmsketch<unsigned>[0] failed\n");
		exit(EXIT_FAILURE);
	}
	
	counter = 0;
	tmp = new int[k];
	if (!tmp) {
        fprintf(stderr, "Memory allocation for tmp array failed\n");
        exit(EXIT_FAILURE);  // Terminate the program if allocation fails
    }
}

pyramid_pcm::~pyramid_pcm()
{
	for (int i = 0; i < size; i++)
	{
		delete pcm[i];
		// free(pcm[i]);
	}
	// delete pcm;
	pcm.clear();
}
void pyramid_pcm::new_win()
{
	counter++;
	if (counter > 1)
	{
		pcm.push_back(new PCMSketch(w_p, LOW_HASH_NUM, word_size));
	}
}

void pyramid_pcm::ins_latest(elem_t e, int delta)
{
	const char* ss = to_string(e).c_str();
	// printf("input string: %s,element:%ld, ",ss,e);
	// int value1 = pcm[counter-1]->Query(ss);
	
	// 
	// for (int j = 0; j< counter; j++)
	// {
	// 	printf("sk[i][%d]: before insert %ld,value=%d\n,",j,e,pcm[j]->Query(ss));
	// }
	pcm[counter-1]->Insert(ss);
	// int value2 = pcm[counter-1]->Query(ss);
	// if(value2-value1!=1) printf("insert error,element:%ld,value1:%d,value2:%d.\n",e,value1,value2);
	// for (int j = 0; j< counter; j++)
	// {
	// 	printf("sk[i][%d]: after insert %ld,value=%d\n,",j,e,pcm[j]->Query(ss));
	// }
	// int value2 = pcm[counter-1]->Query(ss);
	// if(e == 1939612054){
	// 	printf("PCM before inserting %s, value = %d, after insertion, value = %d.\n",ss,value1,value2);
	// }

}

int pyramid_pcm::query(int l, int r, elem_t e) const
{
	int sum = 0;
	for (int i = l; i <= r; ++i){
		// printf("PCM query sk[0][%d], value=%d.\n",i+1,query(i, e));
		sum += query(i, e);
	}
	// printf("PCM query: %ld, value=%d.\n",e,sum);
	return sum;
}

size_t pyramid_pcm::query(int ts, elem_t e) const
{
	if(ts==-1){
		return latest_query(e);
	}
	const char* ss = to_string(e).c_str();
	size_t result = pcm[ts]->Query(ss);
	// printf("PCM query: %ld, value=%ld.\n",e,result);
	return result;
	// return pcm[ts]->Query(ss);
}
int pyramid_pcm::latest_query(elem_t e) const
{
	if (counter ==0)
	{	
		fprintf(stderr,"error, counter = 0.");
	}
	const char* ss = to_string(e).c_str();
	return pcm[counter-1]->Query(ss);
}


/// @brief 
/// @return 
size_t pyramid_pcm::memory() const{
	// sum memory of every sketch
	// printf("size:%d,counter:%d.\n",size,counter);
	size_t memory = 0;
	for (int i = 0; i < counter; i++)
	{
		memory += pcm[i]->memory();
	}
	// printf("pyramid memory:%ld.\n",memory);
	return memory;
}

void pyramid_pcm::print()
{
	// for (int i = 0; i < counter; i++)
	// {
	// 	cm[i]->print();
	// }
	
}

easketch::easketch(size_t memory, int size, int k, int period): size(size), k(k), period(period)
{
	sketch_size = memory/size;
	w_p = sketch_size * 8 / (64 * 2);
	dynamic.push_back(new DynamicCMSketch(3,w_p));
	
	
	if (dynamic.empty())
	{
		fprintf(stderr, "Initial memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}
	// cm[0] = new cmsketch<unsigned>(memory/size, 1);
	if (!dynamic[0])
	{	
		fprintf(stderr, "Memory allocation for dynamyic[0] failed\n");
		exit(EXIT_FAILURE);
	}
	
	counter = 0;
	tmp = new int[k];
	if (!tmp) {
        fprintf(stderr, "Memory allocation for tmp array failed\n");
        exit(EXIT_FAILURE);  // Terminate the program if allocation fails
    }
}

easketch::~easketch()
{
	for (int i = 0; i < dynamic.size(); i++)
	{
		// printf("delete dynamic %d,size:%d.\n",i,dynamic.size());
		// if(dynamic[i]==nullptr){
		// 	printf("dynamic[%d] is null.\n",i);
		// }
		delete dynamic[i];
		// free(pcm[i]);
	}
	// delete pcm;
	dynamic.clear();

}

void easketch::new_win()
{
	counter++;
	if (counter > 1)
	{
		dynamic.push_back(new DynamicCMSketch(3, w_p));
	}
}

void easketch::ins_latest(elem_t e, int delta)
{

	const char* ss = to_string(e).c_str();

	// int value1 = dynamic[counter-1]->query(ss);

	// printf("input string: %s,element:%ld, ",ss,e);
	// int light1,light2;
	// std::vector<uint64> result1,result2;
	// int light_v1,light_v2;
	// int value1 = dynamic[counter-1]->query(ss,light_v1,result1);
	
	
	// // for (int j = 0; j< counter; j++)
	// // {
	// // 	printf("Dynamic sk[i][%d]: before insert %ld,value=%d\n,",j,e,dynamic[j]->query(ss));
	// // }
	// dynamic[counter-1]->insert(ss,light1,light2);
	dynamic[counter-1]->insert(ss);
	dynamic[counter-1]->query(ss);
	// int value2 = dynamic[counter-1]->query(ss,light_v2,result2);
	// if(value2-value1==0){
	// 	for (int i = 0; i < result1.size(); i++)
	// 	{
	// 		printf("result %d, before:%ld.\n",i,result1[i]);
	// 	}
	// 	for (int i = 0; i < result1.size(); i++)
	// 	{
	// 		printf("result %d, after:%ld.\n",i,result2[i]);
	// 	}
		
	// 	printf("insert element:%ld,value1:%d,light_v1:%d,value2:%d,light_v2:%d,light1:%d,light2:%d.\n",e,value1,light_v1,value2,light_v2,light1,light2);
	// }
	// for (int j = 0; j< counter; j++)
	// {
	// 	printf("Dynamic sk[i][%d]: after insert %ld,value=%d\n,",j,e,dynamic[j]->query(ss));
	// }
}

int easketch::query(int l, int r, elem_t e) const
{
	int sum = 0;
	for (int i = l; i <= r; ++i){
		// printf("Dynamic query sk[0][%d], value=%d.\n",i+1,query(i, e));
		sum += query(i, e);
	}
	// printf("Dynamic query: %ld, value=%d.\n",e,sum);
	return sum;
}

size_t easketch::query(int ts, elem_t e) const
{
    if(ts==-1){
		return latest_query(e);
	}
	const char* ss = to_string(e).c_str();
	size_t result = dynamic[ts]->query(ss);
	printf("Dynamic query: %ld, sk[i][%d],value=%ld.\n",e,ts,result);
	return result;
}

size_t easketch::memory() const
{
    size_t memory = 0;
	size_t med;
	// printf("calcaulate the memory,counter:%d..\n",counter);
	for (int i = 0; i < counter; i++)
	{
		med = dynamic[i]->memory();
		memory += med;
		// printf("dynamic[%d] memory:%ldMB\n",i,med>>20);
	}

	return memory;
}

void easketch::print()
{
}

int easketch::latest_query(elem_t e) const
{
    if (counter ==0)
	{	
		fprintf(stderr,"error, counter = 0.");
	}
	const char* ss = to_string(e).c_str();
	return dynamic[counter-1]->query(ss);
}


easketch_with_compress::easketch_with_compress(size_t memory, int size, int k, int period): size(size), k(k), period(period)
{
	sketch_size = memory/size;
	w_p = sketch_size * 8 / (64 * 2);
	dynamic.push_back(new DynamicSketchWithCompress(3,w_p));
	
	
	if (dynamic.empty())
	{
		fprintf(stderr, "Initial memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}
	// cm[0] = new cmsketch<unsigned>(memory/size, 1);
	if (!dynamic[0])
	{	
		fprintf(stderr, "Memory allocation for dynamyic[0] failed\n");
		exit(EXIT_FAILURE);
	}
	
	counter = 0;
	tmp = new int[k];
	if (!tmp) {
        fprintf(stderr, "Memory allocation for tmp array failed\n");
        exit(EXIT_FAILURE);  // Terminate the program if allocation fails
    }
}

easketch_with_compress::~easketch_with_compress()
{
	for (int i = 0; i < dynamic.size(); i++)
	{
		// printf("delete dynamic %d,size:%d.\n",i,dynamic.size());
		// if(dynamic[i]==nullptr){
		// 	printf("dynamic[%d] is null.\n",i);
		// }
		delete dynamic[i];
		// free(pcm[i]);
	}
	// delete pcm;
	dynamic.clear();

}

void easketch_with_compress::new_win()
{
	counter++;
	if (counter > 1)
	{
		dynamic.push_back(new DynamicSketchWithCompress(3, w_p));
		

		// compress
		size_t compress_max_index = log2(counter) + 1, compress_index;
		for (int i = 1; i < compress_max_index; i++)
		{
			compress_index = counter - (1<<i);
			if (compress_index<0) break;
			dynamic[compress_index]->compress();
		}

		// // compress
		// if (counter % period == 0)
		// {
		// 	// compress
		// 	size_t compress_max_index = log2(counter) + 1, compress_index;
		// 	for (int i = 1; i < compress_max_index; i++)
		// 	{
		// 		compress_index = counter - (1<<i);
		// 		if (compress_index<0) break;
		// 		dynamic[compress_index]->compress();
		// 	}
		// }
	}
}

void easketch_with_compress::ins_latest(elem_t e, int delta)
{

	const char* ss = to_string(e).c_str();

	// int value1 = dynamic[counter-1]->query(ss);

	// printf("input string: %s,element:%ld, ",ss,e);
	// int light1,light2;
	// std::vector<uint64> result1,result2;
	// int light_v1,light_v2;
	// int value1 = dynamic[counter-1]->query(ss,light_v1,result1);
	
	
	// // for (int j = 0; j< counter; j++)
	// // {
	// // 	printf("Dynamic sk[i][%d]: before insert %ld,value=%d\n,",j,e,dynamic[j]->query(ss));
	// // }
	// dynamic[counter-1]->insert(ss,light1,light2);
	dynamic[counter-1]->insert(ss);
	dynamic[counter-1]->query(ss);
	// int value2 = dynamic[counter-1]->query(ss,light_v2,result2);
	// if(value2-value1==0){
	// 	for (int i = 0; i < result1.size(); i++)
	// 	{
	// 		printf("result %d, before:%ld.\n",i,result1[i]);
	// 	}
	// 	for (int i = 0; i < result1.size(); i++)
	// 	{
	// 		printf("result %d, after:%ld.\n",i,result2[i]);
	// 	}
		
	// 	printf("insert element:%ld,value1:%d,light_v1:%d,value2:%d,light_v2:%d,light1:%d,light2:%d.\n",e,value1,light_v1,value2,light_v2,light1,light2);
	// }
	// for (int j = 0; j< counter; j++)
	// {
	// 	printf("Dynamic sk[i][%d]: after insert %ld,value=%d\n,",j,e,dynamic[j]->query(ss));
	// }
}

int easketch_with_compress::query(int l, int r, elem_t e) const
{
	int sum = 0;
	for (int i = l; i <= r; ++i){
		// printf("Dynamic query sk[0][%d], value=%d.\n",i+1,query(i, e));
		sum += query(i, e);
	}
	// printf("Dynamic query: %ld, value=%d.\n",e,sum);
	return sum;
}

size_t easketch_with_compress::query(int ts, elem_t e) const
{
    if(ts==-1){
		return latest_query(e);
	}
	const char* ss = to_string(e).c_str();
	size_t result = dynamic[ts]->query(ss);
	printf("Dynamic query: %ld, sk[i][%d],value=%ld.\n",e,ts,result);
	return result;
}

size_t easketch_with_compress::memory() const
{
    size_t memory = 0;
	size_t med;
	// printf("calcaulate the memory,counter:%d..\n",counter);
	for (int i = 0; i < counter; i++)
	{
		med = dynamic[i]->memory();
		memory += med;
		// printf("dynamic[%d] memory:%ldMB\n",i,med>>20);
	}

	return memory;
}

void easketch_with_compress::print()
{
}

int easketch_with_compress::latest_query(elem_t e) const
{
    if (counter ==0)
	{	
		fprintf(stderr,"error, counter = 0.");
	}
	const char* ss = to_string(e).c_str();
	return dynamic[counter-1]->query(ss);
}
