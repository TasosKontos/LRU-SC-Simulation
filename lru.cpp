#include "lru.h"
#include <tuple>
#include <iostream>

using namespace std;

lru_ram_cell::lru_ram_cell(int page, std::string p, bool mod) {
    page_num=page;
    process=p;
    used=0;
    modified=mod;
}

lru_mem::lru_mem(int size) {
    mem_size= size;
    ram = new lru_ram_cell *[size];
    current_frames=0;

    for(int i=0; i<mem_size; i++){
        ram[i]=nullptr;
    }
}

lru_mem::~lru_mem(){
    for(int i=0; i<mem_size; i++){
        if(ram[i]!=nullptr)
            delete ram[i];
    }
    delete ram;
}

bool lru_mem::memory_full() {
    return (current_frames==mem_size);
}

int lru_mem::search(int page, string proc){
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

bool lru_mem::remove_victim() {
    lru_ram_cell victim=*(ram[0]);
    int index=0;

    for(int i=1; i<mem_size; i++){
        if(ram[i]->used>victim.used){
            index=i;
            victim=(*ram[i]);
        }
    }

    delete ram[index];
    ram[index]=nullptr;

    if(victim.process=="gcc")
        gcc_pt.erase(victim.page_num);
    else
        bzip_pt.erase(victim.page_num);

    return victim.modified;
}

void lru_mem::insert(lru_ram_cell rc) {
    int i;

    for(i=0; i<mem_size; i++){
        if(ram[i]==nullptr){
            ram[i]=new lru_ram_cell(rc.page_num, rc.process, rc.modified);
            break;
        }
    }

    if(rc.process=="gcc")
        gcc_pt.insert({rc.page_num, i});
    else
        bzip_pt.insert({rc.page_num, i});
}

void lru_mem::clock(){
    for(int i=0; i<mem_size; i++){
        if(ram[i]!=nullptr){
            ram[i]->used++;
        }
    }
}

void lru(vector<tuple<int, int, char, string>> trace, int frames){
    lru_mem memory(frames);

    int reads=0;
    int writes=0;
    int page_faults=0;

    for(auto & ref : trace){
        int page = get<0>(ref);
        int offset = get<1>(ref);
        char action = get<2>(ref);
        string proc = get<3>(ref);

        memory.clock();

        int temp=memory.search(page, proc);
        if(temp==-1){
            page_faults++;
            reads++;
            if(memory.memory_full()) {
                if(memory.remove_victim())
                    writes++;
                memory.current_frames--;
            }

            if(action=='W'){
                memory.insert(lru_ram_cell(page, proc, true));
            }
            else
                memory.insert(lru_ram_cell(page, proc, false));

            memory.current_frames++;
        }
        else{
            memory.ram[temp]->used=0;
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