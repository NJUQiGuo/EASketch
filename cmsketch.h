#pragma once
#include "utils.h"
#include "hash.h"
#include "types.h"
#include <cassert>
#include <cstring>
#include <climits>
#include <cstdio>
#include "env.h"

template<typename T>
class cmsketch {
public:	
	cmsketch(size_t memory, int hash_num);
	~cmsketch();

	cmsketch(const cmsketch &) = delete;
	void operator=(const cmsketch &) = delete;

	void add(elem_t e, int offset = 0, int delta = 1);
	T query(elem_t e, int offset = 0) const;
	
	void copy(cmsketch *bf);
	void clear(bool reset_hf = true);

	bool compress();
	void set_flag(elem_t e);
	void set_counter(elem_t e,size_t val);
	void print();

	size_t memory() const { return size * sizeof(T); }
    uint64_t acs_cnt() { return _acs_cnt; }
private:
	mutable uint64_t _acs_cnt;
	size_t size;
	int hash_num;
	T *cells;
	hash_func* hf;
};



template<typename T>
cmsketch<T>::cmsketch(size_t memory, int hash_num) 
	: size(memory / sizeof(T)), hash_num(hash_num), hf(new hash_func[hash_num]) {
	_acs_cnt = 0;
	cells = (T*) malloc(memory);
	memset(cells, 0, memory);
	// printf("raw size: %ld\n", size);
}

template<typename T>
cmsketch<T>::~cmsketch() {
	delete[] cells;
	delete[] hf;
}

template<typename T>
void cmsketch<T>::add(elem_t e, int offset, int delta) {
	//if (delta>1) fprintf(stderr, "???\n");
	for (int i = 0; i < hash_num; ++i) 
		cells[(hf[i](e) + offset) % size] += delta;
}

template<typename T>
T cmsketch<T>::query(elem_t e, int offset) const {
	if (cells == nullptr) {
        fprintf(stderr, "Error: cells array is not initialized.\n");
        return 0;
    }

    if (hash_num <= 0) {
        fprintf(stderr, "Error: hash_num is non-positive.\n");
        return 0;
    }
	// printf("query element: %ld, offset:%d..\n",e,offset);
	_acs_cnt += hash_num;
	T ret;
	if (sizeof(T)==4)
	{
		ret = UINT_MAX;
	}
	else{
		ret = UINT8_MAX;
	}
	
	for (int i = 0; i < hash_num; ++i) 
		ret = std::min(ret, cells[(hf[i](e) + offset) % size]);
	return ret;
}

template<typename T>
void cmsketch<T>::copy(cmsketch *oth) {
	assert(oth->size == size);
	for (int i = 0; i < size; ++i)
		cells[i] = oth->cells[i];
	for (int i = 0; i < hash_num; ++i)
		hf[i] = oth->hf[i];
}

template<typename T>
void cmsketch<T>::clear(bool reset_hf) {
	if (reset_hf) {
		for (int i = 0; i < hash_num; ++i)
			hf[i].reset(); // Use new hash functions
	}
	for (size_t i = 0; i < size; i++)
	{
		cells[i]=0;
	}
	
	// memset(cells, 0, sizeof(T) * size);
}

template<typename T>
bool cmsketch<T>::compress()
{
	if (size <= 1<<10) 
		return false;
	int width = size / hash_num;
	int new_width = width / 2;
	// Check if the new width is valid
    if (new_width <= 0) {
        fprintf(stderr, "Invalid new width: %d\n", new_width);
        return false;
    }
	for (int i = 0; i < hash_num; i++)
	{
		for (int j = 0; j < new_width; j++)
		{
			int old_index1 = i * width + j;
            int old_index2 = i * width + j + new_width;

            // Ensure the indices are within bounds
            if (old_index1 >= size || old_index2 >= size) {
                fprintf(stderr, "Index out of bounds: old_index1=%d, old_index2=%d, size=%ld\n", old_index1, old_index2, size);
                return false;
            }
			cells[i * new_width + j] = std::max(cells[old_index1], cells[old_index2]);
		}
		
	}
	size /= 2;
	T* new_cells = (T*)realloc(cells, size * sizeof(T));
	// Check if reallocation was successful
    if (new_cells == NULL) {
        fprintf(stderr, "Memory reallocation failed\n");
        return false;
    }
    cells = new_cells;
	return true;
}

template<typename T>
void cmsketch<T>::print()
{
	// fprintf(result_file,"\nCM Sketch:\n");
	// for (int i = 0; i < size; i++)
	// {
	// 	fprintf(result_file,"%d ",cells[i]);
	// }
	// fprintf(result_file,"\n");
	
}

template<typename T>
void cmsketch<T>::set_flag(elem_t e)
{
	for (int i = 0; i < hash_num; ++i) 
		cells[(hf[i](e)) % size] |= 1<<(sizeof(T)*8-1);
}

template <typename T>
void cmsketch<T>::set_counter(elem_t e,size_t val)
{
	uint8_t flag;
	size_t hash_index;
	uint8_t bits_len = sizeof(T)*8 ;
	for (int i = 0; i < hash_num; ++i){
		hash_index = (hf[i](e)) % size;
		// get the flag
		flag = cells[hash_index]>>(bits_len-1);
		cells[hash_index] = val ;
		if (flag>0)
		{
			cells[hash_index] += 1<<(bits_len-1);
		}
		
	}
}
