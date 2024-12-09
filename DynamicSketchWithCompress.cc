#include "DynamicSketchWithCompress.h"
#include <bits/atomic_base.h>
#include <iostream>
#include <bitset>


DynamicSketchWithCompress::DynamicSketchWithCompress(int _d, int _word_num) : d(_d) {
    _word_num = regular_power_2(_word_num);
    word_num.push_back(_word_num);
    counter_index_size = _word_num;
    counter_num = 16 * _word_num;
    smallest_counter_num = 2 * 16;
    compress_threshold_light_word_num = 1<<40;
    compress_maximum_times = 3;
    compress_count = 0;
    // compress_threshold_light_word_num = 1<<40;
    light = new (std::nothrow) uint64[_word_num];
    if (!light) {
        throw std::runtime_error("Memory allocation failed for light part");
    }
    std::memset(light, 0, sizeof(uint64) * _word_num);
    for (int i = 0; i < d; i++) {
        bobhash[i] = new (std::nothrow) BOBHash(i + 1000);
        if (!bobhash[i]) {
            throw std::runtime_error("Memory allocation failed for BOBHash");
        }
    }
}

DynamicSketchWithCompress::~DynamicSketchWithCompress() {
    if (light!=nullptr) {
        delete[] light;
        light = nullptr;
    }
    // printf("light delete.\n");
    if(!heavy.empty()){
        // for (int i = 0; i < heavy.size(); i++)
        // {
        //     heavy[i].clear();
        // }
        
        heavy.clear();
        // heavy.clear();
        heavy.shrink_to_fit();
    }
    // printf("heavy delete.\n");
    for (int i = 0; i < d; ++i) {
        if (bobhash[i]) {
            delete bobhash[i];
            // bobhash[i] = nullptr;
        }
    }
    // printf("bob delete.\n");
}

void DynamicSketchWithCompress::insert(const char* str) {
    uint64 hash_val, counter_index, light_val, word_index, heavy_val,word_count;
    uint64 counter_num_of_part = counter_num;
    bool carry = false;

    for (int i = 0; i < d; i++) {
        word_count=0;
        counter_num_of_part = counter_num;
        hash_val = bobhash[i]->run(str, std::strlen(str));
        word_index = hash_val % word_num[0];
        counter_index = hash_val%16;

        light_val = (light[word_index] >> (counter_index * 4)) & 0xF;
        carry = (light_val & 0x7) == 0x7;

        if (light_val != 0xF) {
            light[word_index] += (1ULL << (counter_index * 4));
        } else {
            light[word_index] = (light[word_index] & ~(0xFULL << (counter_index * 4))) | (0x8ULL << (counter_index * 4));
        }

        // printf("before insert, light val:%d, after insert,val:%d\n",light_val,(light[word_index] >> (counter_index * 4)) & 0xF);

        if(!carry) continue;
        counter_num_of_part/=8;
        if (counter_num_of_part<smallest_counter_num)
        {
            counter_num_of_part = smallest_counter_num;
        }
        
        for (int j = 1; carry||j < word_num.size(); j++)
        {
            if (j==word_num.size())
            {
                word_num.push_back(counter_num_of_part / 16);
                heavy.resize(heavy.size() + counter_num_of_part / 16, 0);
                // printf("word_num:%d.\n",counter_num_of_part/16);
                // heavy.emplace_back(counter_num_of_part / 16, 0); // safer way to expand and avoid invalid references
                // printf("j:%d,heavt size:%d.\n",j,heavy.size());
            }
            
            // counter_index = hash_val % (word_num[j]*16);
            // word_index = counter_index / 16;
            // counter_index %= 16;

            // counter_index = hash_val % (word_num[j]*16);
            // word_index = counter_index / 16;
            word_index = word_index % word_num[j];
            // counter_index %= 16;

            // printf("j:%d,heavy_size:%d,word_index:%d,word_num:%d,before insert:"
            //         ,j,heavy.size(),word_index,counter_num_of_part/16);
            // std::cout << std::bitset<64>(heavy[j][word_index]);


            heavy_val = (heavy[word_count+word_index] >> (counter_index * 4)) & 0xF;

            if (heavy_val == 0xF) {
                heavy[word_count+word_index] &= ~(0xFULL << (counter_index * 4));

                // std::cout <<",after insert:" <<std::bitset<64>(heavy[j][word_index]) << std::endl;
            } else {
                heavy[word_count+word_index] += (1ULL << (counter_index * 4));
                // std::cout <<",after insert:" <<std::bitset<64>(heavy[j][word_index]) << std::endl;
                carry = false;
                break;
            }
            if (counter_num_of_part/2 > smallest_counter_num)
            {
                counter_num_of_part/=2;
            }else{
                counter_num_of_part=smallest_counter_num;
            }
            word_count += word_num[j];  
        }
    }
    // std::atomic_thread_fence(std::memory_order_seq_cst);
}

void DynamicSketchWithCompress::insert(const char* str,int &before,int& after) {
    uint64 hash_val, counter_index, light_val, word_index, heavy_val,word_count;
    uint64 counter_num_of_part = counter_num;
    bool carry = false;

    for (int i = 0; i < d; i++) {
        word_count=0;
        counter_num_of_part = counter_num;
        hash_val = bobhash[i]->run(str, std::strlen(str));
        word_index = hash_val % word_num[0];
        counter_index = hash_val%16;

        // printf("insert:the %d-th hash value:%ld,word_num[0]:%d,word_index:%d,counter_index:%d,light word:%ld,",
        //         i,hash_val,word_num[0],word_index,counter_index,light[word_index]);

        light_val = (light[word_index] >> (counter_index * 4)) & 0xF;
        carry = (light_val & 0x7) == 0x7;


        if (light_val != 0xF) {
            light[word_index] += (1ULL << (counter_index * 4));
        } else {
            light[word_index] = (light[word_index] & ~(0xFULL << (counter_index * 4))) | (0x8ULL << (counter_index * 4));
        }

        before = light_val;
        after = (light[word_index] >> (counter_index * 4)) & 0xF;
        // printf("before light val:%d, after light val:%d,carry:%d\n",before,after,carry);

        if(!carry) continue;
        counter_num_of_part/=8;
        if (counter_num_of_part<smallest_counter_num)
        {
            counter_num_of_part = smallest_counter_num;
        }
        
        for (int j = 1; true; j++)
        {
            if (j==word_num.size())
            {
                word_num.push_back(counter_num_of_part / 16);
                heavy.resize(heavy.size() + counter_num_of_part / 16, 0);
            }
            
            // counter_index = hash_val % (word_num[j]*16);
            // word_index = counter_index / 16;
            // counter_index %= 16;

            word_index %= word_num[j];


            heavy_val = (heavy[word_count+word_index] >> (counter_index * 4)) & 0xFULL;
            // printf("before:heavy[%d],word_count:%ld,word_num:%d,word_index:%d,counter_index:%d,value=%d,heavy_word:%ld\n",
            //         j-1,word_count,word_num[j],word_index,counter_index,(heavy[word_count+word_index] >> (counter_index * 4)) & 0xFULL,heavy[word_count+word_index]);
           
            if (heavy_val == 0xF) {
                heavy[word_count+word_index] &= ~(0xFULL << (counter_index * 4));

                // printf("after:heavy[%d],word_count:%ld,word_num:%d,word_index:%d,counter_index:%d,value=%d,heavy_word:%ld\n",
                //     j-1,word_count,word_num[j],word_index,counter_index,heavy_val,heavy[word_count+word_index]);
            } else {
                heavy[word_count+word_index] += (1ULL << (counter_index * 4));
                // printf("after:heavy[%d],word_count:%ld,word_num:%d,word_index:%d,counter_index:%d,value=%d,heavy_word:%ld\n",
                //     j-1,word_count,word_num[j],word_index,counter_index,(heavy[word_count+word_index] >> (counter_index * 4)) & 0xFULL,heavy[word_count+word_index]);
                carry = false;
                
                break;
            }

             if (counter_num_of_part/2 > smallest_counter_num)
            {
                counter_num_of_part/=2;
            }else{
                counter_num_of_part=smallest_counter_num;
            }
            word_count += word_num[j];  
        }
    }
    // std::atomic_thread_fence(std::memory_order_seq_cst);
}

uint64 DynamicSketchWithCompress::memory() {
    size_t total_memory = 0;
    for (size_t i = 0; i < word_num.size(); i++) {
        total_memory += word_num[i] * sizeof(uint64);
    }
    return total_memory;
}

uint64 DynamicSketchWithCompress::query(const char* str) {
    uint64 min_value = UINT64_MAX;
    uint64 hash_val, counter_index, light_val, word_index, heavy_val,word_count;
    // uint64 counter_num_of_part = counter_num;
    int j = 0;

    // for (int i = 1; i < word_num.size(); i++)
    // {
    //     if(word_num[i]!=heavy[i-1].size()) printf("length error.\n");
    // }
    

    for (int i = 0; i < d; i++) {
        j = 0;
        // counter_num_of_part = counter_num;
        word_count=0;
        hash_val = bobhash[i]->run(str, std::strlen(str));
        word_index = hash_val % word_num[0];
        counter_index = hash_val%16;

        light_val = (light[word_index] >> (counter_index * 4)) & 0xFULL;
        if (light_val < 0x8) {
            min_value = std::min(min_value, light_val);
        } else {
            uint64 result = light_val & 0x7ULL;
            int count = 3;
            for (size_t k = 1; k < word_num.size(); k++) {
                // counter_index = hash_val % (word_num[k] * 16);
                // word_index = counter_index / 16;
                // counter_index %= 16;
                word_index %= word_num[k];
                heavy_val = (heavy[word_count+word_index] >> (counter_index * 4)) & 0xFULL;
                result += (heavy_val << count);
                count += 4;
                word_count += word_num[k];
            }
            min_value = std::min(min_value, result);
        }
    }
    return min_value;
}

void DynamicSketchWithCompress::compress()
{
    int counter_value1,counter_value2;
    if (compress_count>=compress_maximum_times) return;
    compress_count++;
    // if (word_num[0]/2<compress_threshold_light_word_num) return;
    // Compress the light part
    for (int j = word_num[0]/2; j < word_num[0]; j++)
    {
        // merge the word
        light[j%(word_num[0]/2)] = merge_word(light[j],light[j%(word_num[0]/2)]);
        
    }
    word_num[0]/=2;
    light = (uint64*)realloc(light,word_num[0]*sizeof(uint64));
    // Compress the heavy part
    int word_count_before_compress = 0, word_count_after_compress=0;
    for (int i = 1; i < word_num.size(); i++)
    {
        if (word_num[i]/2<smallest_counter_num/16)
        {
            break;
        }
        for (int j = word_num[i]/2; j < word_num[i]; j++)
        {
            // merge the word
            heavy[word_count_after_compress+j%(word_num[i]/2)] = merge_word(heavy[word_count_before_compress+j%(word_num[i]/2)],
                                                                            heavy[word_count_before_compress+j]);
        }
        word_count_before_compress += word_num[i];
        word_num[i]/=2;
        word_count_after_compress += word_num[i];
        
    }
    heavy.resize(word_count_after_compress);
}

uint64 DynamicSketchWithCompress::merge_word(uint64 w1, uint64 w2) {
    uint64 result = 0;
    for (int i = 0; i < 16; ++i) {
        uint64 mask = 0xFULL << (i * 4); // Mask for the i-th 4-bit counter
        uint64 counter1 = (w1 & mask) >> (i * 4);
        uint64 counter2 = (w2 & mask) >> (i * 4);
        uint64 max_counter = std::max(counter1, counter2);
        result |= (max_counter << (i * 4));
    }
    // printf("w1:%ld,w2:%ld,merge:%ld.\n",w1,w2,result);
    return result;
}

uint64 DynamicSketchWithCompress::regular_power_2(uint n) {
    if (n == 0) {
        return 0;  // Return 0 if input is 0 as no power of 2 is smaller than or equal to 0
    }

    uint64 result = 1;

    while (result <= n) {
        result <<= 1;
    }

    return result >> 1;  // Divide by 2 to get the largest power of 2 less than or equal to n
}


uint64 DynamicSketchWithCompress::query(const char* str,int& light_v,std::vector<uint64>&results) {
    uint64 min_value;
    uint64 hash_val, counter_index, light_val, word_index, heavy_val,word_count;
    int j = 0;

    

    for (int i = 0; i < d; i++) {
        j = 0;
        // counter_num_of_part = counter_num;
        word_count=0;
        hash_val = bobhash[i]->run(str, std::strlen(str));
        

        word_index = hash_val % word_num[0];
        counter_index = hash_val%16;
        // counter_index = hash_val % (word_num[0] * 16);
        // word_index = counter_index / 16;
        // counter_index %= 16;

        printf("the %d-th hash value:%ld,word_num[0]:%d,word_index:%ld,",i,hash_val,word_num[0],word_index);

        light_val = (light[word_index] >> (counter_index * 4)) & 0xFULL;
        // printf("light_val:%d,",light_val);
        light_v = light_val;
        results.push_back(light_val);
        if (light_val < 0x8) {
            if (i==0)
            {
                min_value = light_val;
            }else{
                min_value = std::min(min_value, light_val);
            }
            results.push_back(light_val);
            printf("result %d:%ld,min_value:%ld.\n",i,light_val,min_value);
        } else {
            uint64 result = light_val & 0x7ULL;
            int count = 3;
            printf("heavy size:%d,",word_num.size()-1);
            results.push_back(word_num.size());
            for (size_t k = 1; k < word_num.size(); k++) {
                // counter_index = hash_val % (word_num[k] * 16);
                // word_index = counter_index / 16;
                // counter_index %= 16;
                word_index %= word_num[k];
                heavy_val = (heavy[word_count+word_index] >> (counter_index * 4)) & 0xFULL;
                result += (heavy_val << count);
                results.push_back(heavy_val);
                printf("heavy[%d],word_count:%ld,word_num:%d,word_index:%d,counter_index:%d,value=%d,heavy_word:%ld",
                        k-1,word_count,word_num[k],word_index,counter_index,heavy_val,heavy[word_count+word_index]);
                count += 4;
                word_count += word_num[k];
            }
            results.push_back(result);
            printf("result %d:%ld,min_value:%ld.\n",i,result,min_value);
            if (i==0)
            {
                min_value = result;
            }else{
                min_value = std::min(min_value, result);
            }
            // min_value = std::min(min_value, result);
        }
    }
    return min_value;
}