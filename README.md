# HoppingSketch
HoppingSketch is the first sketch to implement temporal membership queries for sliding windows, and is a general and efficient data stream processing framework that can implement different tasks through different atomic sketches (Bloom filter, CM sketch, CU sketch, Count sketch, etc.). This repostory contains all the codes related to the manuscript submitted to IEEE TKDE.
# Files
bloom_filter - Bloom Filter  
cmsketch - CM sketch  
cusketch - CU sketch  
countsk - Count sketch  
file_reader - Dataset Reader  

hpwin_sketch: Virtual base class for HoppingSketch  

Hop Basic Sketch: Baseline for HoppingSketch  
* hop_basic - Bloom Filter  
* hpbsc_cm - CM sketch  
* hpbsc_csk - Count sketch  
* hpbsc_cu - CU sketch  

hopping - HoppingSketch (Our Algorithm)  
pbf - Persistent Bloom Filter & Persistnet CM&Count&CU sketch (PCM, PC, PCU in paper)  
test - Test functions  
utils - Bitset & Hashmap  
