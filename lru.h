#ifndef OS2_LRU_H
#define OS2_LRU_H

#include <unordered_map>
#include <utility>
#include <vector>

class lru_ram_cell{
public:
    int page_num;
    bool modified;
    int used;
    std::string process;

    lru_ram_cell(int, std::string, bool);
};

class lru_mem{
public:
    int mem_size;
    int current_frames;
    lru_ram_cell ** ram;
    std::unordered_map<int, int> gcc_pt;
    std::unordered_map<int, int> bzip_pt;

    lru_mem(int);
    ~lru_mem();
    bool memory_full();
    int search(int, std::string);
    bool remove_victim();
    void insert(lru_ram_cell);
    void clock();
};

void lru(std::vector<std::tuple<int, int, char, std::string>>, int frames);

#endif
