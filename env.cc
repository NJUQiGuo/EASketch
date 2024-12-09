#include "env.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <time.h>

std::vector<packet> packets;
// packet* packets;
int packet_num=0;
int *win_beg;
std::pair<elem_t, int> latest[_MOD];
FILE *result_file;
std::chrono::duration<double> insert_consume_time(0);
void initialize_result_file(){
    time_t current_time = time(NULL);
    char time_str[64];
    struct tm *tm_info = localtime(&current_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    char result_file_path[128];
    sprintf(result_file_path,"result/%s.txt", time_str);
    result_file = fopen(result_file_path, "a");
}
