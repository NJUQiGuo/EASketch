
#include <algorithm>
#include <cstring>
#include <string.h>
#include "params.h"
#include "BOBHash.h"
#include <iostream>
#include <math.h>


using namespace std;

typedef unsigned long long int uint64;

class PCMSketch
{
private:
	int d;
	uint64 *counter[60];

	int word_index_size, counter_index_size;
	size_t word_num, counter_num;
	//word_num is the number of words in the first level.

	BOBHash * bobhash[MAX_HASH_NUM];


public:
	PCMSketch(size_t _word_num, int _d, int word_size);
	void Insert(const char * str);
	int Query(const char *str);
	void Delete(const char *str);


	//carry from the lower layer to the higher layer, maybe we will allocate the new memory;
	void carry(int index);
	int get_value(int index);
	void down_carry(int index);
	size_t memory();

	~PCMSketch();	
};

