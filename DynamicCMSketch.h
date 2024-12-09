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

class DynamicCMSketch {
private:
    
    uint64 *light;
    std::vector<uint64> heavy;
    std::vector<uint64> word_num;
    int counter_index_size;
    size_t counter_num, smallest_counter_num;
    BOBHash *bobhash[MAX_HASH_NUM];
    int d;

public:
    DynamicCMSketch(int _d, int _word_num);
    ~DynamicCMSketch();
    void insert(const char* str);
    void insert(const char* str,int &before,int& after);
    uint64 query(const char* str,int& light_v,std::vector<uint64>&results);
    uint64 memory();
    uint64 query(const char* str);
};
