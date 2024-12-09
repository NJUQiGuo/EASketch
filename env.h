#include "types.h"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include <chrono>


#define _MOD 100007
#define DATASET_SIZE 10e7
#define CATALOG_FILE_PATH "/home/orla/seafile/My Libraries/PrivateLIbraries/Sketch/code/RangeQuery/dataset/catalog.txt"
#define ONE_MIN_CATALOG_FILE_PATH "/home/orla/seafile/My Libraries/PrivateLIbraries/Sketch/code/RangeQuery/dataset/catalog_1min.txt"
#define TWO_MIN_CATALOG_FILE_PATH "/home/orla/seafile/My Libraries/PrivateLIbraries/Sketch/code/RangeQuery/dataset/catalog_2min.txt"
#define FIVE_MIN_CATALOG_FILE_PATH "/home/orla/seafile/My Libraries/PrivateLIbraries/Sketch/code/RangeQuery/dataset/catalog_5min.txt"
#define DATASET_DIRECTORY "/home/orla/seafile/My Libraries/PrivateLIbraries/Sketch/code/RangeQuery/dataset/split_10000_each/"
#define EDGAR_FILE_20140601_PATH "/home/orla/Learn/sketch/code/HoppingSketch-main/dataset/log20140601/log20140601.csv"
#define EDGAR_FILE_20160413_PATH "/home/orla/Learn/sketch/code/HoppingSketch-main/dataset/log20160413/log20160413.csv"

extern std::vector<packet> packets;
extern int packet_num, *win_beg;
extern std::pair<elem_t, int> latest[_MOD];
extern FILE *result_file;
extern std::chrono::duration<double> insert_consume_time;

void initialize_result_file();