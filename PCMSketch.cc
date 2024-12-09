#include "PCMSketch.h"

//Just for the consistency of the interface;
//For PCMSketch.h, the word_size must be 64;
PCMSketch::  PCMSketch(size_t _word_num, int _d, int word_size) {
    // Logging parameters for debugging

    if (_word_num <= 0 || _d <= 0 || word_size <= 4) {
        throw std::invalid_argument("Invalid parameters.");
    }

    d = _d;
    word_num = _word_num;
    word_index_size = 18;

    counter_index_size = (int)(log(word_size) / log(2)) - 2;
    counter_num = (_word_num << counter_index_size);

    for (int i = 0; i < 15; i++) {
        size_t size = word_num >> i;
        if (size <= 0) {
            throw std::invalid_argument("word_num must be sufficiently large for allocation.");
        }
        counter[i] = new uint64[size];
        memset(counter[i], 0, sizeof(uint64) * size);
    }

    for (int i = 0; i < d; i++) {
        bobhash[i] = new BOBHash(i + 1000);
    }

}


void PCMSketch::Insert(const char *str)
{
    // printf("the string is:%s\n",str);

	int min_value = 1 << 30;
	int value[MAX_HASH_NUM];
	int index[MAX_HASH_NUM];
	int counter_offset[MAX_HASH_NUM];
	
	uint64 hash_value = (bobhash[0]->run(str, strlen(str)));
	int my_word_index = (hash_value & ((1 << word_index_size) - 1)) % word_num;
	hash_value >>= word_index_size;

	int flag = 0xFFFF;

	for(int i = 0; i < d; i++)
	{
		counter_offset[i] = (hash_value & 0xFFF) % (1 << counter_index_size);
		index[i] = ((my_word_index << counter_index_size) + counter_offset[i]) % counter_num;
		hash_value >>= counter_index_size;
	
		value[i] = (counter[0][my_word_index] >> (counter_offset[i] << 2)) & 0xF;

		if(((flag >> counter_offset[i]) & 1) == 0)
			continue;

		flag &= (~(1 << counter_offset[i]));

		if (value[i] == 15)
		{
			counter[0][my_word_index] &= (~((uint64)0xF << (counter_offset[i] << 2)));
			carry(index[i]);
		}
		else
		{
			counter[0][my_word_index] += ((uint64)0x1 << (counter_offset[i] << 2));
		}
	}

	return;
}

int PCMSketch::Query(const char *str)
{
	int min_value = 1 << 30;

	int value[MAX_HASH_NUM];
	int index[MAX_HASH_NUM];
	int counter_offset[MAX_HASH_NUM];
	
	uint64 hash_value = (bobhash[0]->run(str, strlen(str)));
	int my_word_index = (hash_value & ((1 << word_index_size) - 1)) % word_num;
	hash_value >>= word_index_size;

	for(int i = 0; i < d; i++)
	{
		counter_offset[i] = (hash_value & 0xFFF) % (1 << counter_index_size);
		index[i] = ((my_word_index << counter_index_size) + counter_offset[i]) % counter_num;
		hash_value >>= counter_index_size;

		value[i] = (counter[0][my_word_index] >> (counter_offset[i] << 2)) & 0xF;
		value[i] += get_value(index[i]);
		min_value = value[i] < min_value ? value[i] : min_value;
	}


	return min_value;

}
void PCMSketch::Delete(const char *str)
{

	int min_value = 1 << 30;

	int value[MAX_HASH_NUM];
	int index[MAX_HASH_NUM];
	int counter_offset[MAX_HASH_NUM];
	
	uint64 hash_value = (bobhash[0]->run(str, strlen(str)));
	int my_word_index = (hash_value & ((1 << word_index_size) - 1)) % word_num;
	hash_value >>= word_index_size;

	int flag = 0xFFFF;


	for(int i = 0; i < d; i++)
	{
		counter_offset[i] = (hash_value & 0xFFF) % (1 << counter_index_size);
		index[i] = ((my_word_index << counter_index_size) + counter_offset[i]) % counter_num;
		hash_value >>= counter_index_size;
	
		value[i] = (counter[0][my_word_index] >> (counter_offset[i] << 2)) & 0xF;
		// min_value = value[i] < min_value ? value[i] : min_value;
		if(((flag >> counter_offset[i]) & 1) == 0)
			continue;

		flag &= (~(1 << counter_offset[i]));

		if (value[i] == 0)
		{
			counter[0][my_word_index] |= ((uint64)0xF << (counter_offset[i] << 2));
			down_carry(index[i]);
		}
		else
		{
			counter[0][my_word_index] -= ((uint64)0x1 << (counter_offset[i] << 2));
		}
	}
	return;

}

void PCMSketch::down_carry(int index)
{
	int left_or_right, up_left_or_right;	
	
	int value, up_value;
	int word_index = index >> 4, up_word_index;
	int offset = index & 0xF;
	int up_offset = offset;

	for(int i = 1; i < 15; i++)
	{

		left_or_right = word_index & 1;
		word_index >>= 1;

		up_word_index = (word_index >> 1);
		up_left_or_right = up_word_index & 1;

		value = (counter[i][word_index] >> (offset << 2)) & 0xF;

		if((value & 3) >= 2)
		{
			counter[i][word_index] -= ((uint64)0x1 << (offset << 2));
			return;
		}
		else if((value & 3) == 1)
		{
			up_value = (counter[i + 1][up_word_index] >> (up_offset << 2)) & 0xF;

			//change this layer's flag bit;
			if(((up_value >> (2 + up_left_or_right)) & 1) == 0)
			{
				counter[i][word_index] &= (~((uint64)0x1 << (2 + left_or_right + (offset << 2))));
			}
			
			counter[i][word_index] -= ((uint64)0x1 << (offset << 2));
			return;
		}
		else
		{
			counter[i][word_index] |= ((uint64)0x3 << (offset << 2));
		}
	}
}

size_t PCMSketch::memory()
{
    size_t total_memory = 0;
    
    for (int i = 0; i < 15; i++) {
        if (counter[i] != nullptr) {
            // Calculate the memory for each level of the counter array
            size_t size = word_num >> i;  // This is the number of elements in counter[i]
            total_memory += size * sizeof(uint64); // Calculate memory for counter[i] in bytes
        }
    }

    return total_memory; // Total memory in bytes
}

PCMSketch::~PCMSketch()
{
    // Deallocate each dynamically allocated counter array
    for (int i = 0; i < 15; i++)
    {
        if (counter[i] != nullptr) {
            delete[] counter[i];
            counter[i] = nullptr;
        }
    }

    // Deallocate each dynamically allocated BOBHash object
    for (int i = 0; i < d; i++)
    {
        if (bobhash[i] != nullptr) {
            delete bobhash[i];
            bobhash[i] = nullptr;
        }
    }
}



void PCMSketch::carry(int index)
{
	int left_or_right;	
	
	int value;
	int word_index = index >> 4;
	int offset = index & 0xF;

	for(int i = 1; i < 15; i++)
	{

		left_or_right = word_index & 1;
		word_index >>= 1;

		counter[i][word_index] |= ((uint64)0x1 << (2 + left_or_right + (offset << 2)));
		value = (counter[i][word_index] >> (offset << 2)) & 0xF;

		if((value & 3) != 3)
		{
			counter[i][word_index] += ((uint64)0x1 << (offset << 2));
			return;
		}
		counter[i][word_index] &= (~((uint64)0x3 << (offset << 2)));
	}
}

int PCMSketch::get_value(int index)
{
	int left_or_right;	
	int anti_left_or_right;
	
	int value;
	int word_index = index >> 4;
	int offset = index & 0xF;


	int high_value = 0;

	for(int i = 1; i < 15; i++)
	{
		
		left_or_right = word_index & 1;
		anti_left_or_right = (left_or_right ^ 1);

		word_index >>= 1;

		value = (counter[i][word_index] >> (offset << 2)) & 0xF;

		if(((value >> (2 + left_or_right)) & 1) == 0)
			return high_value;

		high_value += ((value & 3) - ((value >> (2 + anti_left_or_right)) & 1)) * (1 << (2 + 2 * i));
	}
    return high_value;
}
