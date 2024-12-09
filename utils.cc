#include "utils.h"
#include <cstdio>
#include <cassert>
#include <cstring>

bitset::bitset(int bits) : bits(bits), cells((bits + 63) / 64) {
	bs = new uint64_t[cells];
	memset(bs, 0, sizeof(uint64_t) * cells);
}

bitset::bitset(const bitset &oth) : bits(oth.bits), cells(oth.cells) {
	bs = new uint64_t[cells];
	for (int i = 0; i < cells; ++i)
		bs[i] = oth.bs[i];
}

bitset::~bitset() { delete[] bs; }

bool bitset::operator[](int i) const {
	assert(0 <= i && i < bits);
	return (bs[i >> 6] >> (i & 63) & 1);
}

void bitset::set(int i) {
	assert(0 <= i && i < bits);
	bs[i >> 6] |= (1ull << (i & 63));
}

void bitset::clear() { memset(bs, 0, sizeof(uint64_t) * cells); }

int bitset::count() const {
	int cnt = 0;
	for (int i = 0; i < cells; ++i)
		cnt += __builtin_popcountll(bs[i]);
	return cnt;
}

int bitset::memory() const { return cells * 8; }

hashmap::hashmap() {
	mod = 1e6 + 7;
	cnt_del = 0;
	head = new int[mod];
	for (int i = 0; i < mod; ++i) head[i] = -1;
}

void hashmap::add(elem_t e, int dta)  {
	for (int *pt = &head[e % mod], i = *pt; i != -1; pt = &nodes[i].nxt, i = *pt) 
		if (nodes[i].idx == e) {
			if ((nodes[i].val += dta) == 0) {
				*pt = nodes[i].nxt;
				cnt_del++;
				if (cnt_del * 2 > nodes.size()) clear_dead();
			
			}
			return;
		}
	nodes.push_back(hashnode(e, dta, head[e % mod]));
	head[e % mod] = nodes.size() - 1;
	// Debug print statement
    // printf("Added element %ld with data %d\n", e, dta);

}

void hashmap::clear() {
    // Delete the head array
    delete[] head;
    
    // Reallocate and initialize the head array
    head = new int[mod];
    for (int i = 0; i < mod; ++i) {
        head[i] = -1;
    }
    
    // Clear the nodes vector
    nodes.clear();
    
    // Reset the cnt_del counter
    cnt_del = 0;
}


std::pair<elem_t, int> hashmap::sample() const {
	//fprintf(stderr, "sample beg\n");
	while (true) {
		assert(nodes.size() > 0);
		//fprintf(stderr, "%d %d\n", nodes.size)
		int idx = rand() % nodes.size();
		if (nodes[idx].val != 0)
			return std::make_pair(nodes[idx].idx, nodes[idx].val);
	}
}

bool hashmap::empty() const { return nodes.size() == cnt_del; }

void hashmap::clear_dead() {
	//fprintf(stderr, "clear begin\n");
	std::vector<hashnode> _nodes;
	std::vector<std::pair<int, int> > _heads;
	_nodes.swap(nodes);
	for (const hashnode &node : _nodes) {
		if (head[node.idx % mod] == -1) continue;
		_heads.push_back(std::make_pair(node.idx % mod, nodes.size()));
		for (int i = head[node.idx % mod]; i != -1; i = _nodes[i].nxt) {
			nodes.push_back(_nodes[i]);
			if (nodes.back().nxt != -1) nodes.back().nxt = nodes.size();
		}
		head[node.idx % mod] = -1;
	}

	for (auto pr : _heads) head[pr.first] = pr.second;
	cnt_del = 0;
	//fprintf(stderr, "clear end\n");
}