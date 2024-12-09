#include "config.h"
#include "pbf.h"
#include "hopping.h"
#include "hpwin_sketch.h"
#include "env.h"
#include <unordered_set>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <algorithm>
#include <chrono>
#include <map>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <set>
#include <vector>
#include <map>
#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <math.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <iomanip>


// #include "BOBHash.h"

config last_cfg;

std::unordered_map<elem_t, int> uu;
double first_time = 0.0;


template<typename T>
void build_sketch(hpwin_sketch<T> *sketch) {
	// printf("start building...\n");
	int j = 0;
	double fir = packets[0].ts;
	long long tot = 0;
	for (int i = 0; i < _MOD; ++i)
		latest[i] = std::make_pair(0, -1);
	for (int i = 1; i <= cfg.test_win_num; ++i) {
		// fprintf(stderr, "building %d\n", i);c// if (i % 1000 == 0) fprintf(stderr, "building %d\n", i);
		for (sketch->new_win(); j < packet_num && j < win_beg[i+1]; ++j) {
			elem_t e = packets[j].e;
			sketch->ins_latest(e,1);
		}
	}
}



// 包处理回调函数
// 读取数据包中的函数并且在sketch中更新
void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet1) {
	packet pkt_read;
	// 假设以太网头部长度为14字节
	struct ip *ip_header = (struct ip *)(packet1 + 14); 
	// Convert the source IP address to a string
	char ip_src_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip_header->ip_src), ip_src_str, INET_ADDRSTRLEN);

	// Print the source IP address as a string
	// printf("Source IP: %s\n", ip_src_str);

	// Convert the source IP address to an integer
	uint32_t ip_src_int;
	inet_pton(AF_INET, ip_src_str, &ip_src_int);
	pkt_read.e = ip_src_int;
	pkt_read.ts = pkthdr->ts.tv_sec+pkthdr->ts.tv_usec/1e6;
	packets.push_back(pkt_read);
	if (packet_num>0)
	{
		// if (pkt_read.ts - first_time > cfg.win_length * cfg.win_num)
		// {
		// 	pcap_breakloop(reinterpret_cast<pcap_t*>(user_data));
        // 	return;
		// }
		;
		
	}else if(packet_num==0){
		first_time = pkt_read.ts;
	}
	packet_num++;
	// printf("element:%ld, timestamp:%f\n",pkt_read.e,pkt_read.ts);
}

// Function to split the IP string into parts
std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to convert a single part of the IP to an integer
unsigned int part_to_int(const std::string &part) {
    unsigned int value = 0;
    for (char c : part) {
        value = value * 10 + c-'0';
    }
    return value;
}

size_t ip_to_int(const std::string &ip_str) {
    std::string processed_ip = ip_str;
	// char med='a';
	// int val = 0;
	// size_t val = 0;
    // for (auto &c : processed_ip) {
    //     if (isalpha(c)) {
	// 		val = val*10 + tolower(c)-'a';
    //     }
    // }
	// val %= 256;

	std::vector<std::string> parts = split(ip_str, '.');
    if (parts.size() != 4) {
        std::cerr << "Invalid IP address format: " << ip_str << std::endl;
        return 0;
    }

    unsigned int ip_int = 0;
    for (int i = 0; i < 3; ++i) {
        ip_int = (ip_int << 8) | part_to_int(parts[i]);
    }

	size_t val = 0;
	for(char c : parts[3]){
		if (isalpha(c)) {
			val = val*10 + tolower(c)-'a';
        }
	}
	val %=256;
	ip_int = (ip_int << 8) | val;
    return static_cast<size_t>(ip_int);

	// // printf("ip:%s.\n",ip_str.c_str());
    // struct in_addr ip_addr;
    // inet_pton(AF_INET, processed_ip.c_str(), &ip_addr);
    // return ip_addr.s_addr;
}

void read_csv_and_store_packets(const std::string &file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }
	bool first = true;
    std::string line;
    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
		
		
        std::stringstream ss(line);
        std::string ip, date, time,dummy;
        
        std::getline(ss, ip, ',');
        std::getline(ss, date, ',');
        std::getline(ss, time, ',');
		// Skip remaining fields
        for (int i = 0; i < 13; ++i) {
            std::getline(ss, dummy, ',');
        }
        // Combine date and time strings
        std::string datetime_str = date + " " + time;
		// fprintf(stderr,"time:%s\n",datetime_str.c_str());
        
        // Convert date and time to timestamp
        // std::tm tm = {};
        // strptime(datetime_str.c_str(), "%m/%d/%Y %H:%M:%S", &tm);
        // double timestamp = std::mktime(&tm);
		std::tm tm = {};
        std::istringstream datetime_ss(datetime_str.c_str());
        datetime_ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        // datetime_ss >> std::get_time(&tm, "%m/%d/%Y %H:%M:%S");
		if (datetime_ss.fail()) {
            std::cerr << "Failed to parse date and time: " << datetime_str << std::endl;
            continue; // Skip this line and continue with the next one
        }
        double timestamp = std::mktime(&tm);
		// fprintf(stderr,"timestamp:%f.\n",timestamp);
		if (first)
		{
			first_time = timestamp;
			first = false;
		}
        // Convert IP address to integer
        size_t ip_int = ip_to_int(ip);
		// printf("ip:%ld.\n",ip_int);
        // Store the data in the packet structure
        packet pkt;
        pkt.e = ip_int;
        pkt.ts = timestamp;
        packets.push_back(pkt);
    }

    file.close();
}


// Function to extract timestamp from file name
std::tm extractTimestamp(const std::string &fileName) {
    std::string timestampStr = fileName.substr(fileName.find_last_of('_') + 1, 14);
    std::tm tm = {};
    std::istringstream ss(timestampStr);
    ss >> std::get_time(&tm, "%Y%m%d%H%M%S");
    return tm;
}



void read_pcaps(){
    // 读取pcap文件
    std::ifstream catalog_file(FIVE_MIN_CATALOG_FILE_PATH);
    if (!catalog_file.is_open())
    {
        perror("Unable to open catalog file. \n");
    }
    std::string file_name, file_path,file_time;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    // pcap_set_snaplen(handle, 1024);
    while (std::getline(catalog_file, file_name))
    {
        if (file_name.empty())
        {
            continue;
        }
        file_path = std::string(DATASET_DIRECTORY) + file_name;
		// 读取pcap文件
		handle = pcap_open_offline(file_path.c_str(), errbuf);
		if (handle == NULL)
		{
			fprintf(stderr, "Couldn't open cap file: %s\n", errbuf);
			pcap_close(handle);
			continue;
		}
		// 读取数据包并且处理
		if (pcap_loop(handle, 0, packet_handler, reinterpret_cast<u_char*>(handle))<0)   
		{
			// fprintf(stderr, "Error reading packets: %s\n", pcap_geterr(handle));
			pcap_close(handle);
			continue;
		}
		// 关闭PCAP文件
		pcap_close(handle);
		
    }
}


void read_pcap(){
	char errbuf[PCAP_ERRBUF_SIZE];
	std::string file_pattern = "/home/orla/seafile/My Libraries/PrivateLIbraries/Sketch/code/RangeQuery/dataset/split_10000_each/small_dataset_00001_20240205130000.pcap";
	// 读取pcap文件
	pcap *handle = pcap_open_offline(file_pattern.c_str(), errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "Couldn't open cap file: %s\n", errbuf);
		pcap_close(handle);
	}
	// 读取数据包并且处理
	if (pcap_loop(handle, 0, packet_handler, NULL)<0)   
	{
		fprintf(stderr, "Error reading packets: %s\n", pcap_geterr(handle));
		pcap_close(handle);
	}
	// 关闭PCAP文件
	pcap_close(handle);
}



void prepare_data2() {
	if (!packets.empty()){
		packets.clear();
	}

	read_pcaps();
	packet_num = packets.size();
	printf("packet num:%d\n.",packet_num);
	fprintf(stderr,"first time:%f, last time:%f, duration:%f,cfg.win_length:%f,.\n",first_time,packets[packet_num-1].ts,packets[packet_num-1].ts-first_time,cfg.win_length);
	
	win_beg = new int[cfg.test_win_num + 2];
	int j = 0;
	// to get the beginning packet index of every window
	for (int i = 1; i <= cfg.test_win_num; ++i) 
	{
		fprintf(stderr,"window %d: begins at index:%d,element:%ld, timestamp:%f\n",i,j,packets[j].e,packets[j].ts);
		for (win_beg[i] = j; j < packet_num && packets[j].ts - first_time < i * cfg.win_length; ++j){;
		}
	}
	win_beg[cfg.test_win_num + 1] = packet_num;
	//printf("%ld\n", (size_t) packet_num * (sizeof(elem_t) + sizeof(double)));
}

void prepare_data_EDGAR() {
	if (!packets.empty()){
		packets.clear();
	}

	read_csv_and_store_packets(EDGAR_FILE_20160413_PATH);
	// read_pcaps();
	packet_num = packets.size();
	printf("packet num:%d\n.",packet_num);
	fprintf(stderr,"first time:%f, last time:%f, duration:%f,cfg.win_length:%f,.\n",first_time,packets[packet_num-1].ts,packets[packet_num-1].ts-first_time,cfg.win_length);
	
	win_beg = new int[cfg.test_win_num + 2];
	int j = 0;
	// to get the beginning packet index of every window
	for (int i = 1; i <= cfg.test_win_num; ++i) 
	{
		fprintf(stderr,"window %d: begins at index:%d,element:%ld, timestamp:%f\n",i,j,packets[j].e,packets[j].ts);
		for (win_beg[i] = j; j < packet_num && packets[j].ts - first_time < i * cfg.win_length; ++j){;
		}
	}
	win_beg[cfg.test_win_num + 1] = packet_num;
	//printf("%ld\n", (size_t) packet_num * (sizeof(elem_t) + sizeof(double)));
}




void test_are_pyramid_versus_query_length_five_min(){
	const int k = 16; // 128
	const int k1 = 17;
	const int k2 = 8;
	
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(6000), 3e5);
	// void configure(std::string dataset, int win_num, double win_length, int qy_win, size_t memory, int test_elem_num) 

	
	initialize_result_file();
	configure("webpage", 1<<k, 300. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data2();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=2000; i<=40000;i+=100)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/testdataset_inrange.txt");
		std::string line;
		size_t sketch_value;
		auto pyramid_pcm1 = new multi_layer2<pyramid_pcm>(MB(i),k2);
		ds = (hpwin_sketch<int>*) pyramid_pcm1;
		build_sketch(ds);		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;


		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,pyramid_pcm1,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,pyramid_pcm1,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		size_t memory = pyramid_pcm1->memory();
		printf("pyramid_pcm1 memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i),are);
		fprintf(result_file,"pyramid_pcm1 memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i),are);


		delete pyramid_pcm1;
		sleep(5);
	}
}


void test_are_pyramid_versus_query_length_edgar(){
	const int k = 10; // 128
	const int k1 = 9;
	const int k2 = 10;
	initialize_result_file();
	// configure("webpage", 1<<k1,1<<k, 86399. / (1<<k), 1<<k2, MB(1), 1e5);
	configure("webpage", 1<<k,1<<k, 86399. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data_EDGAR();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=4000; i<=40000;i+=100)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/test_dataset_edgar.txt");
		std::string line;
		size_t sketch_value;
		auto pyramid_pcm1 = new multi_layer2<pyramid_pcm>(MB(i),k2);
		ds = (hpwin_sketch<int>*) pyramid_pcm1;
		build_sketch(ds);		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;


		// while (std::getline(inputFile, line))
		// {
		// 	std::istringstream iss(line);
		// 	iss>>start_time>>end_time>>random_element>>real_value;
		// 	sketch_value = ds->query(start_time, end_time,random_element);
		// 	printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
		// 	if (sketch_value>real_value)
		// 	{
		// 		sum += (sketch_value - real_value) / real_value;
		// 		sumaae += sketch_value-real_value;
		// 	}else{
		// 		sum += (real_value - sketch_value) / real_value;
		// 		sumaae += real_value - sketch_value;
		// 	}
		// 	tot++;
		// }
		// are = sum / tot;
		// // fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		// size_t memory = pyramid_pcm1->memory();
		// printf("pyramid_pcm1 memory=%d MB, Ratio:%f, are=%f.\n",memory>>20,((double)(memory>>20)/i),are);
		// fprintf(result_file,"pyramid_pcm1 memory=%d MB, Ratio:%f, are=%f.\n",memory>>20,((double)(memory>>20)/i),are);


		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,pyramid_pcm1,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,pyramid_pcm1,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		size_t memory = pyramid_pcm1->memory();
		printf("pyramid_pcm1 memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i),are);
		fprintf(result_file,"pyramid_pcm1 memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i),are);


		delete pyramid_pcm1;
		sleep(5);
	}
}


void test_are_dynamic_versus_query_length_edgar(){
	const int k = 10; // 128
	const int k1 = 9;
	const int k2 = 10;
	initialize_result_file();
	// configure("webpage", 1<<k1,1<<k, 86399. / (1<<k), 1<<k2, MB(1), 1e5);
	configure("webpage", 1<<k,1<<k, 86399. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data_EDGAR();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=20; i<=8000;i+=20)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/test_dataset_edgar.txt");
		std::string line;
		size_t sketch_value;
		auto dynamic1 = new multi_layer2<easketch>(MB(i), k2);
		ds = (hpwin_sketch<int>*) dynamic1;
		build_sketch(ds);		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;
		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,dynamic1,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,dynamic1,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		size_t memory = dynamic1->memory();
		printf("dynamic1 memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i),are);
		fprintf(result_file,"dynamic1 memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i),are);
		delete dynamic1;
		sleep(5);
	}
}

void test_are_pcm_sw_versus_query_length_edgar(){
	const int k = 10; // 128
	const int k1 = 9;
	const int k2 = 10;
	initialize_result_file();
	// configure("webpage", 1<<k1,1<<k, 86399. / (1<<k), 1<<k2, MB(1), 1e5);
	configure("webpage", 1<<k,1<<k, 86399. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data_EDGAR();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=20; i<=200;i+=20)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/test_dataset_edgar.txt");
		std::string line;
		size_t sketch_value;
		auto pcm_sw = new multi_layer1<hopping_cm>(MB(i), k2);
		ds = (hpwin_sketch<int>*) pcm_sw;
		build_sketch(ds);		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;
		
		
		// while (std::getline(inputFile, line))
		// {
		// 	std::istringstream iss(line);
		// 	iss>>start_time>>end_time>>random_element>>real_value;
		// 	sketch_value = ds->query(start_time, end_time,random_element);
		// 	printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
		// 	// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
		// 	// if (real_value==0)
		// 	// {
		// 	// 	sum += fabs(sketch_value - real_value) *1e6;
		// 	// }else{
		// 	// 	sum += fabs(sketch_value - real_value) / real_value;
		// 	// }
		// 	if (sketch_value>real_value)
		// 	{
		// 		sum += (sketch_value - real_value) / real_value;
		// 		sumaae += sketch_value-real_value;
		// 	}else{
		// 		sum += (real_value - sketch_value) / real_value;
		// 		sumaae += real_value - sketch_value;
		// 	}
			
		// 	// sum += fabs(double(sketch_value - real_value)) / real_value;
		// 	// sumaae += fabs(double(sketch_value - real_value));
		// 	tot++;
		// 	// fprintf(result_file,"sketch_value:%d,real_value:%d,dis:%lf,sumaae:%lf,are now:%lf.\n",
		// 	// 					sketch_value,real_value,fabs(sketch_value-real_value),sumaae,sum/tot);
		// }
		// are = sum / tot;
		
		// // fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		// // size_t memory = dynamic1->memory();
		// printf("pcm_sw memory=%d MB, Ratio:%f, are=%f.\n",i,1.0,are);
		// fprintf(result_file,"pcm_sw memory=%d MB, Ratio:%f, are=%f.\n",i,1.0,are);


		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,pcm_sw,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,pcm_sw,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		// size_t memory = pcm_sw->memory();
		printf("pcm_sw memory=%d MB, Ratio:%f.\n",i,1.0,are);
		fprintf(result_file,"pcm_sw memory=%d MB, Ratio:%f.\n",i,1.0,are);

		delete pcm_sw;
		sleep(5);
	}
}


void test_are_pcm_versus_query_length_edgar(){
	const int k = 10; // 128
	const int k1 = 9;
	const int k2 = 10;
	initialize_result_file();
	// configure("webpage", 1<<k1,1<<k, 86399. / (1<<k), 1<<k2, MB(1), 1e5);
	configure("webpage", 1<<k,1<<k, 86399. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data_EDGAR();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=20; i<=200;i+=20)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/test_dataset_edgar.txt");
		std::string line;
		size_t sketch_value;
		auto pcm1 = new multi_layer1<pcm1_base>(MB(i), k2);
		ds = (hpwin_sketch<int>*) pcm1;
		build_sketch(ds);		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;


		// while (std::getline(inputFile, line))
		// {
		// 	std::istringstream iss(line);
		// 	iss>>start_time>>end_time>>random_element>>real_value;
		// 	sketch_value = ds->query(start_time, end_time,random_element);
		// 	// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
		// 	// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
		// 	sum += fabs(sketch_value - real_value) / real_value;
		// 	sumaae += fabs(sketch_value - real_value);
		// 	tot++;
		// }
		
		// are = sum / tot;
		
		// // fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		// // size_t memory = dynamic1->memory();
		// printf("pcm1 memory=%d MB, Ratio:%f, are=%f.\n",i,1.0,are);
		// fprintf(result_file,"pcm1 memory=%d MB, Ratio:%f, are=%f.\n",i,1.0,are);


		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,pcm1,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,pcm1,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		// size_t memory = pcm_sw->memory();
		printf("pcm1 memory=%d MB, Ratio:%f.\n",i,1.0,are);
		fprintf(result_file,"pcm1 memory=%d MB, Ratio:%f.\n",i,1.0,are);

		delete pcm1;
		sleep(5);
	}
}


void test_are_dynamic_versus_query_length_five_min(){
	const int k = 16; // 128
	const int k1 = 17;
	const int k2 = 8;
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(6000), 3e5);
	// void configure(std::string dataset, int win_num, double win_length, int qy_win, size_t memory, int test_elem_num) 

	
	initialize_result_file();
	configure("webpage", 1<<k, 300. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data2();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=3500; i<=4000;i+=100)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/testdataset_inrange.txt");
		std::string line;
		size_t sketch_value;
		auto dynamic1 = new multi_layer2<easketch>(MB(i), k2);
		ds = (hpwin_sketch<int>*) dynamic1;
		build_sketch(ds);		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;
		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,dynamic1,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,dynamic1,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		size_t memory = dynamic1->memory();
		printf("dynamic1 memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i),are);
		fprintf(result_file,"dynamic1 memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i),are);
		delete dynamic1;
		sleep(5);
	}
}


void test_are_dynamic_with_compress_versus_query_length_five_min(){
	const int k = 16; // 128
	const int k1 = 17;
	const int k2 = 8;
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(6000), 3e5);
	// void configure(std::string dataset, int win_num, double win_length, int qy_win, size_t memory, int test_elem_num) 

	
	initialize_result_file();
	configure("webpage", 1<<k, 300. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data2();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=2000; i<=40000;i+=100)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/testdataset_inrange.txt");
		std::string line;
		size_t sketch_value;
		auto dynamic1 = new multi_layer2<easketch_with_compress>(MB(i), k2);
		ds = (hpwin_sketch<int>*) dynamic1;
		build_sketch(ds);
		size_t memory = dynamic1->memory();		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;
		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch without compress,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch without compress,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		
		// printf("dynamic_with_compress memory=%d MB, Ratio:%f, are=%f.\n",memory>>20,((double)(memory>>20)/i),are);
		// fprintf(result_file,"dynamic_with_compress memory=%d MB, Ratio:%f, are=%f.\n",memory>>20,((double)(memory>>20)/i),are);
		printf("dynamic_with_compress, maximum=3, memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i));
		fprintf(result_file,"dynamic_with_compress,maximum=3, memory=%d MB, Ratio:%f.\n",memory>>20,((double)(memory>>20)/i));
		delete dynamic1;
		sleep(5);
	}
}


void test_are_pcm_sw_versus_query_length_five_min(){
	const int k = 16; // 128
	const int k1 = 17;
	const int k2 = 8;
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(6000), 3e5);
	// void configure(std::string dataset, int win_num, double win_length, int qy_win, size_t memory, int test_elem_num) 

	
	initialize_result_file();
	configure("webpage", 1<<k, 300. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data2();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=4100; i<=40000;i+=100)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/testdataset_inrange.txt");
		std::string line;
		size_t sketch_value;
		auto pcm_sw = new multi_layer1<hopping_cm>(MB(i), k2);
		ds = (hpwin_sketch<int>*) pcm_sw;
		build_sketch(ds);		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;

		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,pcm_sw,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,pcm_sw,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		// size_t memory = pcm_sw->memory();
		printf("pcm_sw memory=%d MB, Ratio:%f.\n",i,1.0,are);
		fprintf(result_file,"pcm_sw memory=%d MB, Ratio:%f.\n",i,1.0,are);

		delete pcm_sw;
		sleep(5);
	}
}

void test_are_pcm_versus_query_length_five_min(){
	const int k = 16; // 128
	const int k1 = 17;
	const int k2 = 8;
	//configure("CAIDA", 1<<k, 1.0 / (1<<6), 1<<k2, MB(6000), 3e5);
	// void configure(std::string dataset, int win_num, double win_length, int qy_win, size_t memory, int test_elem_num) 

	
	initialize_result_file();
	configure("webpage", 1<<k, 300. / (1<<k), 1<<k2, MB(1), 1e5);
	prepare_data2();
	int tot = 0;
	int start_time, end_time, real_value;
	elem_t random_element;
	double sum, sumaae,are;
	hpwin_sketch<int>* ds;
	printf("Memory(MB),Memory Ratio, ARE\n");
	for (int i=8300; i<=40000;i+=100)
	{
		// std::ifstream inputFile("dataset/testdataset.txt");
		std::ifstream inputFile("dataset/testdataset_inrange.txt");
		std::string line;
		size_t sketch_value;
		auto pcm1 = new multi_layer1<pcm1_base>(MB(i), k2);
		ds = (hpwin_sketch<int>*) pcm1;
		build_sketch(ds);		
		if (!inputFile.is_open()) {
			std::cerr << "Error opening file" << std::endl;
			return;
		}
		tot = 0;
		sum=0.0, sumaae=0.0, are=0.0;


		// while (std::getline(inputFile, line))
		// {
		// 	std::istringstream iss(line);
		// 	iss>>start_time>>end_time>>random_element>>real_value;
		// 	sketch_value = ds->query(start_time, end_time,random_element);
		// 	// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
		// 	// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
		// 	sum += fabs(sketch_value - real_value) / real_value;
		// 	sumaae += fabs(sketch_value - real_value);
		// 	tot++;
		// }
		
		// are = sum / tot;
		
		// // fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		// // size_t memory = dynamic1->memory();
		// printf("pcm1 memory=%d MB, Ratio:%f, are=%f.\n",i,1.0,are);
		// fprintf(result_file,"pcm1 memory=%d MB, Ratio:%f, are=%f.\n",i,1.0,are);


		int count = 0;
		while (std::getline(inputFile, line))
		{
			count++;
			std::istringstream iss(line);
			iss>>start_time>>end_time>>random_element>>real_value;
			sketch_value = ds->query(start_time, end_time,random_element);
			// printf("%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			// fprintf(result_file,"%d %d %ld %d %d\n",start_time, end_time, random_element, real_value,sketch_value);
			if (sketch_value>real_value)
			{
				sum += (sketch_value - real_value) / real_value;
				sumaae += sketch_value-real_value;
			}else{
				sum += (real_value - sketch_value) / real_value;
				sumaae += real_value - sketch_value;
			}
			tot++;
			if (count%1000==0)
			{
				are = sum / tot;
				fprintf(stderr,"memory:%dMB,query_length:%d,pcm1,are:%f.\n",i,1<<(count/1000-1),are);
				fprintf(result_file,"memory:%dMB,query_length:%d,pcm1,are:%f.\n",i,1<<(count/1000-1),are);
				// fprintf(stderr,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				// fprintf(result_file,"memory:%dMB,query_length:%d,tiered hf sketch,are:%f.\n",mem,1<<(count/1000-1),are);
				sum=0.0, sumaae=0.0, are=0.0;
				tot = 0;
			}
		}
		
		// fprintf(result_file,"pcm_sw sketch memory=%d MB, Ratio:%f, are=%f.\n",i,1.,are);
		// size_t memory = pcm_sw->memory();
		printf("pcm1 memory=%d MB, Ratio:%f.\n",i,1.0,are);
		fprintf(result_file,"pcm1 memory=%d MB, Ratio:%f.\n",i,1.0,are);

		delete pcm1;
		sleep(5);
	}
}








int main() {
	srand(1214);


	// test_pyramid_pcm_query();
	test_are_pyramid_versus_query_length_five_min();
	// test_are_dynamic_versus_query_length_five_min();
	// test_are_dynamic_with_compress_versus_query_length_five_min();
	// test_are_pcm_sw_versus_query_length_five_min();
	// test_are_pcm_versus_query_length_five_min();
	
	// test_are_pyramid_versus_query_length_edgar();
	// test_are_dynamic_versus_query_length_edgar();
	// test_are_pcm_versus_query_length_edgar();
	// test_are_pcm_sw_versus_query_length_edgar();


	// get_pyramid_test_dataset();

	// test_pcm_sw_sketch();
	// test_dynamic_sketch();
	// test_dynamic_sketch_with_compress();
	// test_pyramid_pcm_and_dynamic_query();
	return 0;
}
