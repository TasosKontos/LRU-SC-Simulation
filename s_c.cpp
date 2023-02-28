#include "s_c.h"
#include <tuple>
#include <iostream>

using namespace std;

s_c_ram_cell::s_c_ram_cell(int page, std::string p, bool mod) {
    page_num=page;
    modified=mod;
    process=p;
    second_chance=0;
}

s_c_mem::s_c_mem(int size) {
    mem_size= size;
    ram = new s_c_ram_cell *[size];
    current_frames=0;

    for(int i=0; i<mem_size; i++){
        ram[i]=nullptr;
    }
}

s_c_mem::~s_c_mem() {
    for(int i=0; i<mem_size; i++){
        if(ram[i]!=nullptr)
            delete ram[i];
    }
    delete ram;
}

bool s_c_mem::memory_full() {
    return (current_frames==mem_size);
}

int s_c_mem::search(int page, string proc){
    unordered_map<int, int> ::iterator it;

    if(proc=="bzip"){
        if(bzip_pt.empty())return -1;

        it=bzip_pt.find(page);
        if(it!=bzip_pt.end())
            return it->second;
        else
            return -1;
    }
    else{
        if(gcc_pt.empty()) return -1;

        it=gcc_pt.find(page);
        if(it!=gcc_pt.end())
            return it->second;
        else
            return -1;
    }
}

bool s_c_mem::replace_victim(s_c_ram_cell rc) {
    int v;
    bool flag=true;

    while(flag) {
        for (int i = ref_queue.size()-1; i >= 0; i--) {
            v = search(ref_queue[i].first, ref_queue[i].second);
            if (ram[v]->second_chance == 0) {
                ref_queue.erase(ref_queue.begin() + i);
                flag=false;
                break;
            } else {
                ram[v]->second_chance = 0;
            }
        }
    }

    s_c_ram_cell victim=(*ram[v]);

    bool result=victim.modified;

    delete ram[v];
    ram[v]=nullptr;

    if(victim.process=="gcc")
        gcc_pt.erase(victim.page_num);
    else
        bzip_pt.erase(victim.page_num);

    insert(rc);

    return result;


}

void s_c_mem::insert(s_c_ram_cell rc) {
    int i;

    for(i=0; i<mem_size; i++){
        if(ram[i]==nullptr){
            ram[i]=new s_c_ram_cell(rc.page_num, rc.process, rc.modified);
            break;
        }
    }

    if(rc.process=="gcc")
        gcc_pt.insert({rc.page_num, i});
    else
        bzip_pt.insert({rc.page_num, i});

    ref_queue.push_front(make_pair(rc.page_num, rc.process));
}

void s_c(vector<tuple<int, int, char, string>> trace, int frames){
    s_c_mem memory(frames);

    int reads=0;
    int writes=0;
    int page_faults=0;

    for(auto & ref : trace){
        int page = get<0>(ref);
        int offset = get<1>(ref);
        char action = get<2>(ref);
        string proc = get<3>(ref);

        int temp=memory.search(page, proc);
        if(temp==-1){
            page_faults++;
            reads++;

            if(memory.memory_full()){
                if(action=='W'){
                    if(memory.replace_victim(s_c_ram_cell(page, proc, true)))
                        writes++;
                }
                else
                    if(memory.replace_victim(s_c_ram_cell(page, proc, false)))
                        writes++;
            }
            else{
                if(action=='W'){
                    memory.insert(s_c_ram_cell(page, proc, true));
                }
                else
                    memory.insert(s_c_ram_cell(page, proc, false));

                memory.current_frames++;
            }
        }
        else{
            memory.ram[temp]->second_chance=1;
            if(action=='W')
                memory.ram[temp]->modified=true;
        }
    }

    cout<<"Disk reads: "<<reads<<endl;
    cout<<"Disk writes: "<<writes<< endl;
    cout<<"Page faults: "<<page_faults<<endl;
    cout<<"References checked: "<<trace.size()<<endl;
    cout<<"Memory frames: "<<frames<<endl;
}
