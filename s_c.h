#ifndef OS2_S_C_H
#define OS2_S_C_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <deque>


class s_c_ram_cell{
public:
    int page_num;
    bool modified;
    int second_chance;
    std::string process;

    s_c_ram_cell(int, std::string, bool);
};

class s_c_mem{
public:
    int mem_size;
    int current_frames;
    s_c_ram_cell ** ram;
    std::deque<std::pair<int, std::string>> ref_queue;
    std::unordered_map<int, int> gcc_pt;
    std::unordered_map<int, int> bzip_pt;

    s_c_mem(int);
    ~s_c_mem();
    bool memory_full();
    int search(int, std::string);
    bool replace_victim(s_c_ram_cell);
    void insert(s_c_ram_cell);
};

void s_c(std::vector<std::tuple<int, int, char, std::string>>, int frames);
#endif