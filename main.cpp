#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include "lru.h"
#include "s_c.h"

using namespace std;

int main(int argc, char* argv[]) {
    if(argc!=5){
        cout<<"4 arguments must be given!"<<endl;
        return 1;
    }
    string replacement_algorithm = argv[1];
    if(replacement_algorithm!="LRU" and replacement_algorithm!="SC"){
        cout<<"Replacement algorithm must be LRU (least recently used) or SC (second chance)."<<endl;
        return 2;
    }

    int frames = stoi(argv[2]);
    if(frames<=0){
        cout<<"Number of frames must be a positive integer."<<endl;
        return 3;
    }

    int q = stoi(argv[3]);
    if(q<=0){
        cout<<"Reference count must be a positive integer."<<endl;
        return 4;
    }

    int max_references = stoi(argv[4]);
    if(max_references<2*q){
        cout<<"Max references must be a positive integer at least 2 times greater than reference count."<<endl;
        return 5;
    }

    ifstream gcc("gcc.trace");
    ifstream bzip("bzip.trace");

    int current_references=0;
    vector<tuple<int, int, char, string>> refs; //tuple holds page# - offset - r/w - bzip/gcc

    while(current_references<max_references){
        string trace, page , offset;
        char mode;
        int i;


        if(current_references+2*q>max_references){
            int diff=max_references-current_references;

            i=0;
            while(i<(diff/2+diff%2) and bzip>>trace>>mode){
                page= trace.substr(0,5);
                offset=trace.substr(4,3);
                refs.push_back(make_tuple(stoi(page, nullptr, 16), stoi(offset, nullptr, 16), mode, "bzip"));
                i++;
            }

            i=0;
            while(i<diff/2 and gcc>>trace>>mode){
                page= trace.substr(0,5);
                offset=trace.substr(4,3);
                refs.push_back(make_tuple(stoi(page, nullptr, 16), stoi(offset, nullptr, 16), mode, "gcc"));
                i++;
            }

            current_references=max_references;
        }
        else{
            i=0;
            while(i<q and (bzip>>trace>>mode)){
                page= trace.substr(0,5);
                offset=trace.substr(4,3);
                refs.push_back(make_tuple(stoi(page, nullptr, 16), stoi(offset, nullptr, 16), mode, "bzip"));
                i++;
            }

            i=0;
            while(i<q and (gcc>>trace>>mode)){
                page= trace.substr(0,5);
                offset=trace.substr(4,3);
                refs.push_back(make_tuple(stoi(page, nullptr, 16), stoi(offset, nullptr, 16), mode, "gcc"));
                i++;
            }

            current_references+=2*q;
        }

    }

    if(replacement_algorithm=="LRU"){
        lru(refs, frames);
    }
    else{
        s_c(refs, frames);
    }

}
