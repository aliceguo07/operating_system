#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdio.h>  
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream> 
#include <cstdlib>

void print_array(int place[]);
void opt(std::vector<int> pages);
void lru(std::vector<int> pages);
void lfu(std::vector<int> pages);

int main(int argc, char* argv[]){
	std::ifstream input_file(argv[2]);
    
    if (!input_file.good()){
        fprintf(stderr, "ERROR: Cannot open input file %s\n", argv[1]);
    }
    
    std::vector<int> pages;
    int page;
    
    while(input_file>>page){
    	//printf("%d ", page);
    	pages.push_back(page);
    }
    
    // opt(pages);
    // printf("\n");
    // lru(pages);
    // printf("\n");
    lfu(pages);
    return 0;
}

void print_array(int place[]){
    printf("[mem: ");
    for(int i = 0; i <3; i++){
        if(place[i] == -1){
            printf(".");
        }
        else{
            printf("%d", place[i]);
        }
        if(i == 2){
            printf("] PAGE FAULT");
        }
        else{
            printf(" ");
        }
    }
}

void opt(std::vector<int> pages){
    std::map<int, int> lru_map;
    int place [] = {-1,-1,-1};
    int page_fault = 0;
    //print_array(place);
    printf("Simulating OPT with fixed frame size of 3\n");
    for (unsigned int i = 0; i <pages.size(); i++){
        bool victim = false;
        bool has = false;
        for(int a = 0; a < 3; a++){
            if(pages[i] == place[a]){
                has = true;
                break;
            }
        }
        if(has == false){
            bool placed = false;
            int victim_page = 0;
            page_fault++;
            for(int a = 0; a < 3; a++){
                if(place[a] == -1){
                    place[a] = pages[i]; 
                    placed = true;
                    break;
                }
            }
            if(placed == false){
                int next_show = -1;
                int index = 4;
                for(int a = 0; a < 3; a++){
                    int show_pos = (int)pages.size();
                    if(std::find(pages.begin()+i, pages.end(), place[a])!=pages.end()){
                        for(unsigned int w = i; w <pages.size(); w++){
                            if(pages[w] == place[a]){
                                show_pos = w;
                                break;
                            }
                        }
                    }
                    if(next_show<show_pos){
                        next_show = show_pos;
                        index = a;
                        victim_page = place[a];
                        victim = true;
                    }
                    else if(next_show==show_pos){
                        if(place[a]<victim_page){
                            index = a;
                            victim_page = place[a];
                            victim = true;
                        }
                    }
                }
                place[index] = pages[i];
            }
            printf("referencing page %d ", pages[i]);
            print_array(place);
            if(victim == false){
                printf(" (no victim page)\n");
            }
            else{
                printf(" (victim page %d)\n", victim_page);
            }
        }
    }
    printf("End of OPT simulation (%d page faults)\n",page_fault );
}

// 	The Optimal (OPT) algorithm is a forward-looking algorithm that selects the “victim” page by
// 	identifying the frame that will be accessed the longest time in the future (or not at all). If multiple
// 	pages are identified, this “tie” is broken by selecting the lowest-numbered page.
	

void lru(std::vector<int> pages){
    std::map<int, int> lru_map;
    int place [] = {-1,-1,-1};
    int page_fault = 0;
    //print_array(place);
    printf("Simulating LRU with fixed frame size of 3\n");
    for (unsigned int i = 0; i <pages.size(); i++){
        lru_map[pages[i]] = i;
        //printf("%d\n", pages[i] );
        bool victim = false;
        bool has = false;
        for(int a = 0; a < 3; a++){
            if(pages[i] == place[a]){
                has = true;
                break;
            }
        }
        if(has == false){
            bool placed = false;
            int victim_page = 0;
            page_fault++;
            for(int a = 0; a < 3; a++){
                if(place[a] == -1){
                    place[a] = pages[i]; 
                    placed = true;
                    break;
                }
            }
            if(placed == false){
                int last_show = (int)pages.size();
                int index = 4;
                for(int a = 0; a < 3; a++){
                    if(lru_map[place[a]]<last_show){
                        last_show = lru_map[place[a]];
                        index = a;
                        victim_page = place[a];
                        victim = true;
                    }
                    else if(lru_map[place[a]]==last_show){
                        if(place[a]<victim_page){
                            index = a;
                            victim_page = place[a];
                            victim = true;
                        }
                    }
                }
                place[index] = pages[i];
            }
            printf("referencing page %d ", pages[i]);
            print_array(place);
            if(victim == false){
                printf(" (no victim page)\n");
            }
            else{
                printf(" (victim page %d)\n", victim_page);
            }
        }
    }
    printf("End of LRU simulation (%d page faults)\n",page_fault );
}
	
// 	The Least-Recently Used (LRU) algorithm is a backward-looking algorithm that selects the “victim”
// 	page by identifying the frame that has the oldest access time.

void lfu(std::vector<int> pages){
    std::map<int, int> lfu_map;
    int place [] = {-1,-1,-1};
    int page_fault = 0;
    printf("Simulating LFU with fixed frame size of 3\n");
    for (unsigned int i = 0; i <pages.size(); i++){
        lfu_map[pages[i]] = 0;
    }
    for (unsigned int i = 0; i <pages.size(); i++){
        lfu_map[pages[i]] += 1; //+1
        bool victim = false;
        bool has = false;

        for(int a = 0; a < 3; a++){
            if(pages[i] == place[a]){
                has = true; 
                break;
            }
        }
        if(has == false){
            bool placed = false;
            int victim_page = 0;
            page_fault++;
            for(int a = 0; a < 3; a++){
                if(place[a] == -1){
                    place[a] = pages[i]; 
                    placed = true;
                    break;
                }
            }
            if(placed == false){
                
                int index = 4;
                int freq = pages.size();

                for (int a = 0; a < 3; a++){
                    //printf("%d %d\n", place[a], lfu_map[place[a]]);
                    if(lfu_map[place[a]] < freq){
                        freq = lfu_map[place[a]];
                        index = a;
                        victim_page = place[a];
                        victim = true;
                    }
                    else if(lfu_map[place[a]] == freq){
                        //printf("here\n");
                        if(place[a]<victim_page){
                            index = a;
                            victim_page = place[a];
                            victim = true;
                        }
                    }
                }
                lfu_map[place[index]] = 0;
                place[index] = pages[i]; //update
            }

            printf("referencing page %d ", pages[i]);
            print_array(place);
            if(victim == false){
                printf(" (no victim page)\n");
            }
            else{
                printf(" (victim page %d)\n", victim_page);
            }
        }
    }
    printf("End of LFU simulation (%d page faults)\n",page_fault );
}

// 	The Least-Frequently Used (LFU) algorithm is a backward-looking algorithm that selects the “victim”
// 	page by identifying the frame with the lowest number of accesses. When a page fault occurs
// 	for a given page, its reference count is set (or reset) to 1; each subsequent access increments this reference
// 	count. If multiple pages are identified, this “tie” is broken by selecting the lowest-numbered
// 	page.

