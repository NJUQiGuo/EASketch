#pragma once
#include "cmsketch.h"
#include "cusketch.h"
#include "countsk.h"
#include "hpwin_sketch.h"
#include "PCMSketch.h"
#include "DynamicCMSketch.h"
#include "DynamicSketchWithCompress.h"




class hopping_cm : hpwin_sketch<int> {
public:
	hopping_cm(size_t memory, int size, int k, int per = 1);
	~hopping_cm();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	int query(int l, int r, elem_t e) const;
	int query(int win, elem_t e) const;
private:
	cmsketch<unsigned> **cm;
	int size, k, last_win, period, counter;
};


class pcm1_base : hpwin_sketch<int> {
public:
	pcm1_base(size_t memory, int size, int k, int period = 1);
	~pcm1_base();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	int query(int l, int r, elem_t e) const;
	int query(int ts, elem_t e) const;
	size_t memory() const;
	void print();
	int latest_query(elem_t e) const;

private:
	// cmsketch **cm; 
	std::vector<cmsketch<unsigned>*> cm;
	int size, k,  period, counter, sketch_size, *tmp;
	// int size, k, last_timestamp, period, counter, *tmp;
};




class easketch : hpwin_sketch<int> {
public:
	easketch(size_t memory, int size, int k, int period = 1);
	~easketch();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	int query(int l, int r, elem_t e) const;
	size_t query(int ts, elem_t e) const;
	size_t memory() const;
	void print();
	int latest_query(elem_t e) const;

private:
	// cmsketch **cm; 
	std::vector<DynamicCMSketch*> dynamic;
	int size, k,  period, counter, sketch_size, *tmp;
	size_t w_p;
	// int size, k, last_timestamp, period, counter, *tmp;
};

class easketch_with_compress : hpwin_sketch<int> {
public:
	easketch_with_compress(size_t memory, int size, int k, int period = 1);
	~easketch_with_compress();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	int query(int l, int r, elem_t e) const;
	size_t query(int ts, elem_t e) const;
	size_t memory() const;
	void print();
	int latest_query(elem_t e) const;

private:
	// cmsketch **cm; 
	std::vector<DynamicSketchWithCompress*> dynamic;
	int size, k,  period, counter, sketch_size, *tmp;
	size_t w_p;
	// int size, k, last_timestamp, period, counter, *tmp;
};

class pyramid_pcm : hpwin_sketch<int> {
public:
	pyramid_pcm(double memory, int size, int k, int period = 1);
	~pyramid_pcm();
	void new_win();
	void ins_latest(elem_t e, int delta = 1);
	int query(int l, int r, elem_t e) const;
	size_t query(int ts, elem_t e) const;
	size_t memory() const;
	void print();
	int latest_query(elem_t e) const;

private:
	// // cmsketch **cm; 
	// std::vector<cmsketch<uint8_t>*> light_cm;
	// std::vector<cmsketch<uint16_t>*> heavy_cm;
	std::vector<PCMSketch*> pcm;
	int size, k,  period, counter, sketch_size, *tmp, word_size;
	size_t w_p;
	// int size, k, last_timestamp, period, counter, *tmp;
};
