#include <algorithm>
#include <cstring>
#include <string.h>
#include "params.h"
#include "BOBHash.h"
#include <iostream>
#include <math.h>
#include <vector>
#include <atomic>
#include <stdexcept>

typedef unsigned long long int uint64;

class DynamicSketchWithCompress {
private:
    
    uint64 *light;
    std::vector<uint64> heavy;
    std::vector<uint64> word_num;
    int counter_index_size;
    size_t counter_num, smallest_counter_num,compress_threshold_light_word_num,compress_maximum_times,compress_count;
    BOBHash *bobhash[MAX_HASH_NUM];
    int d;
    uint64 merge_word(uint64 w1, uint64 w2);
    uint64 regular_power_2(uint n);

public:
    DynamicSketchWithCompress(int _d, int _word_num);
    ~DynamicSketchWithCompress();
    void insert(const char* str);
    void insert(const char* str,int &before,int& after);
    uint64 query(const char* str,int& light_v,std::vector<uint64>&results);
    uint64 memory();
    uint64 query(const char* str);
    void compress();
};
