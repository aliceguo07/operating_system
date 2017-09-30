//===================================
//Yiqing Guo guoy6
//Yiyang Li liy31
//===================================

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

class process{
public:
	process(char* letter, int frame_lenth, int initialt, int runt);

    int getinitialtime(){return initial_time;}
    int getframelenth(){return frame;}
    int getendtime(){return end_time;}
    char getid(){return *id;}
    void printprocess(){printf("%s %d %d %d\n", id, frame, initial_time, run_time);}

	void change_initial_time(int time_add);
    void change_end_time(int time_add);

	char* id;
	int frame, initial_time, run_time, end_time;
};

process::process(char *idc, int framen, int initialt, int runt){
	id = idc;
	frame = framen;
	initial_time = initialt;
	run_time = runt;
    end_time = initial_time+run_time;
}


void process::change_initial_time(int time_add){
	initial_time = initial_time+time_add;
    end_time = initial_time+run_time;
}

void process::change_end_time(int time_add){
    end_time = end_time+time_add;
}

void print_memory(char memory[]);

void next_fit(std::vector<process> process_vector);
void best_fit(std::vector<process> process_vector);
void worst_fit(std::vector<process> process_vector);
void first_fit(std::vector<process> process_vector);

void print_array(int place[]);
void opt(std::vector<int> pages);
void lru(std::vector<int> pages);
void lfu(std::vector<int> pages);



int main(int argc, char* argv[]){
	if(argc != 3)
    {
        fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file>\n");
        return EXIT_FAILURE;
    }

    std::ifstream input_file(argv[1]);
    if (!input_file.good()){
        fprintf(stderr, "ERROR: Cannot open input file %s\n", argv[1]);
    }

    std::vector<process> process_vector;

    std::string in_line;
    while (getline(input_file, in_line)){
    	if(isdigit(in_line[0])){
            //const char * c = in_line.c_str();
    		//total_process = atoi(c);
    	}
    	else if(isalpha(in_line[0])){
    		char *letter = (char *)malloc(sizeof(char));
    		int frame_lenth;
            const char *c_line = in_line.c_str();
    		sscanf(c_line, "%s %d", letter, &frame_lenth);
    		char temp_int[32];
    		int pos_lenth = sprintf(temp_int, "%d", frame_lenth);
    		pos_lenth = pos_lenth +3;
    		int lenth = in_line.length() - pos_lenth;
    		char* rest_of_string = (char *)calloc(lenth, sizeof(char));
    		memcpy(rest_of_string, c_line+pos_lenth, lenth);
            char* tmp = NULL;
            tmp = strtok(rest_of_string," ");
            int initialt, runt;
            while(tmp!= NULL){
                sscanf(tmp, "%d/%d",  &initialt, &runt);
                process aprocess(letter, frame_lenth,initialt, runt);
                process_vector.push_back(aprocess);
                tmp = strtok(NULL," ");
            }
    	}
    }
    

    next_fit(process_vector);
    printf("\n");
    best_fit(process_vector);
    printf("\n");
    worst_fit(process_vector);
    printf("\n");
    first_fit(process_vector);
    printf("\n");

    std::ifstream input_file1(argv[2]);
    
    if (!input_file1.good()){
        fprintf(stderr, "ERROR: Cannot open input file %s\n", argv[1]);
    }
    
    std::vector<int> pages;
    int page;
    
    while(input_file1>>page){
        //printf("%d ", page);
        pages.push_back(page);
    }
    
    opt(pages);
    printf("\n");
    lru(pages);
    printf("\n");
    lfu(pages);


    return 0;
}

void print_memory(char memory[]){
    char head_tail[33];
    memset(head_tail,'=',sizeof(head_tail));
    head_tail[32] = '\0';
    printf("%s\n", head_tail);
    int i;
    for(i = 0; i<8; i++){
        char temp_print[33];
        strncpy(temp_print, memory+(i*32), 32);
        temp_print[32] = '\0';
        printf("%s\n", temp_print);
    }
    printf("%s\n", head_tail);
}

void next_fit(std::vector<process> process_vector){
    char memory[256];
    memset(memory,'.',sizeof(memory));
    int time_count = 0;
    int last_position = 0;
    char * last_char = (char *)malloc(sizeof(char));
    std::vector<process> running_process, waiting_process;
    waiting_process = process_vector;
    printf("time %dms: Simulator started (Contiguous -- Next-Fit)\n", time_count);
    while(1){
        if(waiting_process.empty() && running_process.empty()){
            break;
        }
        /*==========================================running process==========================================*/
        for (unsigned int i = 0; i <running_process.size(); i++){
            if(running_process[i].getendtime() == time_count){
                bool start = false;
                for(int q = 0; q<256; q++){
                    if(memory[q] == running_process[i].getid() && start == false){
                        start = true;
                        // if(*last_char == running_process[i].getid() ){
                        //     last_position = q;
                        // }
                        memory[q] = '.';
                    }
                    else if(memory[q] == running_process[i].getid() && start == true){
                        memory[q] = '.';
                    }
                }
                printf("time %dms: Process %c removed:\n", time_count, running_process[i].getid());
                print_memory(memory);
                running_process.erase(running_process.begin()+i);
                i--;
            }
        }
        /*============================================place process==================================*/
        for (unsigned int i = 0; i <waiting_process.size(); i++){
            if(waiting_process[i].getinitialtime() == time_count){
                //printf("here %d\n", last_position);
                printf("time %dms: Process %c arrived (requires %d frames)\n",  time_count,
                waiting_process[i].getid(), waiting_process[i].getframelenth() );
                int framecount = 0;
                int start_pos;
                int space = 0;
                bool placed = false;
                bool start = false;
                /*===================================next fit======================================*/
                for(int q = last_position; q<256; q++){
                    if(memory[q] == '.' && start == false){
                        start_pos = q;
                        start = true;
                        framecount++;
                        space++;
                    }
                    else if(memory[q] == '.' && start == true){
                        framecount++;
                        space++;
                    }
                    else{
                        start = false;
                        framecount = 0;
                    }
                    /*============================placed in=======================================*/
                    if(framecount == waiting_process[i].getframelenth() ){
                        last_position = q+1;
                        *last_char = waiting_process[i].getid();
                        placed = true;
                        for(int w = start_pos; w <= q; w++){
                            memory[w] = waiting_process[i].getid();
                        }
                        printf("time %dms: Placed process %c:\n",time_count, waiting_process[i].getid() );
                        print_memory(memory);
                        /*==========================change the vector========================*/
                        running_process.push_back(waiting_process[i]);
                        waiting_process.erase(waiting_process.begin()+i);
                        i--;
                    }
                    if (placed == true){
                       break;
                    }
                }
                /*=================================from top========================================*/
                if(placed == false){
                    //printf("here %d\n", last_position);
                    start = false;
                    framecount = 0;
                    for(int q = 0; q<last_position; q++){
                        if(memory[q] == '.' && start == false){
                            start_pos = q;
                            start = true;
                            framecount++;
                            space++;
                        }
                        else if(memory[q] == '.' && start == true){
                            framecount++;
                            space++;
                        }
                        else{
                            start = false;
                            framecount = 0;
                        }
                        /*============================placed in=======================================*/
                        if(framecount == waiting_process[i].getframelenth() ){
                            last_position = q+1;
                            placed = true;
                            for(int w = start_pos; w <= q; w++){
                                memory[w] = waiting_process[i].getid();
                            }
                            printf("time %dms: Placed process %c:\n",time_count, waiting_process[i].getid() );
                            print_memory(memory);
                            /*==========================change the vector========================*/
                            running_process.push_back(waiting_process[i]);
                            waiting_process.erase(waiting_process.begin()+i);
                            i--;
                        }
                        if (placed == true){
                            break;
                        }
                    }
                    /*========================defrg could happend=================================*/
                    if(placed == false && space >= waiting_process[i].getframelenth()){
                        bool start = false;
                        int t_memmove = 0;
                        int start_place;
                        int add = 0;
                        printf("time %dms: Cannot place process %c -- starting defragmentation\n", 
                        time_count, waiting_process[i].getid() );
                        std::vector<char> v;
                        for (int q = 0; q< 256; q++){
                            if(memory[q] == '.' && start == false){
                                start = true;
                                start_place = q;
                            }
                            else if(memory[q] != '.' && start == true){
                                memory[start_place+add] = memory[q];
                                if(std::find(v.begin(), v.end(), memory[q])!=v.end()){
                                }
                                else
                                {
                                    v.push_back(memory[q]);
                                }
                                memory[q] = '.';
                                add++;
                                time_count++;
                                t_memmove++;
                            }
                        }
                        for (int q = 255; q>= 0; q--){
                            if(memory[q] == '.'){
                                last_position = q;
                            }
                        }
                        for(unsigned int z = 0; z <waiting_process.size(); z++){
                            waiting_process[z].change_initial_time(t_memmove);
                        }
                        std::sort(v.begin(), v.end());
                        printf("time %dms: Defragmentation complete (moved %d frames:", time_count, t_memmove);
                        for(unsigned int z = 0; z<v.size(); z++){
                            if(z!=0){
                                printf(",");
                            }
                            printf(" %c",v[z]);
                        }
                        for(unsigned int z = 0; z <running_process.size(); z++){
                            running_process[z].change_end_time(t_memmove);
                        }
                        printf(")\n");
                        print_memory(memory);
                        start_pos = last_position;
                        for(int w = start_pos; w < start_pos+waiting_process[i].getframelenth(); w++){
                            memory[w] = waiting_process[i].getid();
                        }
                        printf("time %dms: Placed process %c:\n",time_count, waiting_process[i].getid() );
                        print_memory(memory);
                        /*==========================change the vector========================*/
                        running_process.push_back(waiting_process[i]);
                        waiting_process.erase(waiting_process.begin()+i);
                        i--;
                        placed = true;
                    }
                    /*=============================skip====================================*/
                    if(placed == false){
                        printf("time %dms: Cannot place process %c -- skipped!\n", time_count, waiting_process[i].getid());
                        print_memory(memory);
                        waiting_process.erase(waiting_process.begin()+i);
                        i--;
                    }
                }
            }
        }
        time_count++;
    }//while()
    //print_memory(memory);
    printf("time %dms: Simulator ended (Contiguous -- Next-Fit)\n", time_count-1 );
}

void best_fit(std::vector<process> process_vector){
    char memory[256];
    memset(memory,'.',sizeof(memory));
    int time_count = 0;
    int last_position = 0;
    char * last_char = (char *)malloc(sizeof(char));
    std::vector<process> running_process, waiting_process;
    waiting_process = process_vector;
    printf("time %dms: Simulator started (Contiguous -- Best-Fit)\n", time_count);
    while(1){
        if(waiting_process.empty() && running_process.empty()){
            break;
        }
        /*==========================================running process==========================================*/
        for (unsigned int i = 0; i <running_process.size(); i++){
            if(running_process[i].getendtime() == time_count){
                bool start = false;
                for(int q = 0; q<256; q++){
                    if(memory[q] == running_process[i].getid() && start == false){
                        start = true;
                        if(*last_char == running_process[i].getid() ){
                            last_position = q;
                        }
                        memory[q] = '.';
                    }
                    else if(memory[q] == running_process[i].getid() && start == true){
                        memory[q] = '.';
                    }
                }
                printf("time %dms: Process %c removed:\n", time_count, running_process[i].getid());
                print_memory(memory);
                running_process.erase(running_process.begin()+i);
                i--;
            }
        }
        /*============================================place process==================================*/
        for (unsigned int i = 0; i <waiting_process.size(); i++){
            if(waiting_process[i].getinitialtime() == time_count){
                printf("time %dms: Process %c arrived (requires %d frames)\n",  time_count,
                waiting_process[i].getid(), waiting_process[i].getframelenth() );
                int framecount = 0;
                int start_pos;
                int space = 0;
                bool placed = false;
                bool start = false;
                int current_space = 257;
                std::map<int, int> place_map;
                /*===================================best fit======================================*/
                for(int q = 0; q<256; q++){
                    if(memory[q] == '.' && start == false){
                        //printf("%d\n", q);
                        start_pos = q;
                        start = true;
                        framecount++;
                        space++;
                    }
                    else if(memory[q] == '.' && start == true){
                        framecount++;
                        space++;
                    }
                    else if(start == true){
                        place_map[start_pos] = framecount;
                        start = false;
                        framecount = 0;
                    }
                }
                if(start == true){
                    place_map[start_pos] = framecount;
                }
                start = false;
                framecount = 0;
                /*===================================find best fit===================================*/
                for (std::map<int,int>::iterator it=place_map.begin(); it!=place_map.end(); ++it){
                    if(it->second>= waiting_process[i].getframelenth() ){
                        if(current_space > it->second){
                            start_pos = it->first;
                            current_space = it->second;
                            placed = true;
                        }
                    }
                    //printf("%d %d\n", it->first, it->second  );
                }
                /*===================================place======================================*/
                if(placed == true){
                    int number_placed = 0;
                    for(int w = start_pos; w < 256; w++){
                        if(number_placed == waiting_process[i].getframelenth() ){
                            break;
                        }
                        memory[w] = waiting_process[i].getid();
                        number_placed++;
                    }
                    printf("time %dms: Placed process %c:\n",time_count, waiting_process[i].getid() );
                    print_memory(memory);
                    /*==========================change the vector========================*/
                    running_process.push_back(waiting_process[i]);
                    waiting_process.erase(waiting_process.begin()+i);
                    place_map.clear();
                    i--;
                }
                /*========================defrg could happend=================================*/
                if(placed == false && space >= waiting_process[i].getframelenth()){
                    bool start = false;
                    int t_memmove = 0;
                    int start_place;
                    int add = 0;
                    std::vector<char> v;
                    printf("time %dms: Cannot place process %c -- starting defragmentation\n", 
                        time_count, waiting_process[i].getid() );
                    for (int q = 0; q< 256; q++){
                        if(memory[q] == '.' && start == false){
                            start = true;
                            start_place = q;
                        }
                        else if(memory[q] != '.'  && start == true){
                            memory[start_place+add] = memory[q];
                            if(std::find(v.begin(), v.end(), memory[q])!=v.end()){}
                            else
                            {
                                v.push_back(memory[q]);
                            }
                            memory[q] = '.';
                            add++;
                            time_count++;
                            t_memmove++;
                        }
                    }
                    for (int q = 255; q>= 0; q--){
                        if(memory[q] == '.'){
                            last_position = q;
                        }
                    }
                    for(unsigned int z = 0; z <waiting_process.size(); z++){
                        waiting_process[z].change_initial_time(t_memmove);
                    }
                    printf("time %dms: Defragmentation complete (moved %d frames:", time_count, t_memmove);
                    for(unsigned int z = 0; z <running_process.size(); z++){
                        running_process[z].change_end_time(t_memmove);
                        //printf(" %c",running_process[z].getid());
                    }
                    for(unsigned int z = 0; z<v.size(); z++){
                        if(z!=0){
                            printf(",");
                        }
                        printf(" %c",v[z]);
                    }
                    printf(")\n");
                    print_memory(memory);
                    start_pos = last_position;
                    for(int w = start_pos; w < start_pos+waiting_process[i].getframelenth(); w++){
                        memory[w] = waiting_process[i].getid();
                    }
                    printf("time %dms: Placed process %c:\n",time_count, waiting_process[i].getid() );
                    print_memory(memory);
                    /*==========================change the vector========================*/
                    running_process.push_back(waiting_process[i]);
                    waiting_process.erase(waiting_process.begin()+i);
                    i--;
                    placed = true;
                }
                /*=============================skip====================================*/
                if(placed == false){
                    printf("time %dms: Cannot place process %c -- skipped!\n", time_count, waiting_process[i].getid());
                    print_memory(memory);
                    waiting_process.erase(waiting_process.begin()+i);
                    i--;
                }

            }
        }
        time_count++;
    }//while()
    //print_memory(memory);
    printf("time %dms: Simulator ended (Contiguous -- Best-Fit)\n", time_count-1 );
}

void worst_fit(std::vector<process> process_vector){
    char memory[256];
    memset(memory,'.',sizeof(memory));
    int time_count = 0;
    int last_position = 0;
    char * last_char = (char *)malloc(sizeof(char));
    std::vector<process> running_process, waiting_process;
    waiting_process = process_vector;
    printf("time %dms: Simulator started (Contiguous -- Worst-Fit)\n", time_count);
    while(1){
        if(waiting_process.empty() && running_process.empty()){
            break;
        }
        /*==========================================running process==========================================*/
        for (unsigned int i = 0; i <running_process.size(); i++){
            if(running_process[i].getendtime() == time_count){
                bool start = false;
                for(int q = 0; q<256; q++){
                    if(memory[q] == running_process[i].getid() && start == false){
                        start = true;
                        if(*last_char == running_process[i].getid() ){
                            last_position = q;
                        }
                        memory[q] = '.';
                    }
                    else if(memory[q] == running_process[i].getid() && start == true){
                        memory[q] = '.';
                    }
                }
                printf("time %dms: Process %c removed:\n", time_count, running_process[i].getid());
                print_memory(memory);
                running_process.erase(running_process.begin()+i);
                i--;
            }
        }
        /*============================================place process==================================*/
        for (unsigned int i = 0; i <waiting_process.size(); i++){
            if(waiting_process[i].getinitialtime() == time_count){
                printf("time %dms: Process %c arrived (requires %d frames)\n",  time_count,
                waiting_process[i].getid(), waiting_process[i].getframelenth() );
                int framecount = 0;
                int start_pos;
                int space = 0;
                bool placed = false;
                bool start = false;
                int current_space = waiting_process[i].getframelenth()-1;
                std::map<int, int> place_map;
                /*===================================worst fit======================================*/
                for(int q = 0; q<256; q++){
                    if(memory[q] == '.' && start == false){
                        //printf("%d %c\n", q, memory[q]);
                        start_pos = q;
                        start = true;
                        framecount++;
                        space++;
                    }
                    else if(memory[q] == '.' && start == true){
                        //printf("%d %c\n", q, memory[q]);
                        framecount++;
                        space++;
                    }
                    else if(start == true){
                        //printf("%d %c\n", q, memory[q]);
                        place_map[start_pos] = framecount;
                        start = false;
                        framecount = 0;
                    }
                }
                if(start == true){
                    place_map[start_pos] = framecount;
                }
                start = false;
                framecount = 0;
                /*===================================find worst fit===================================*/
                for (std::map<int,int>::iterator it=place_map.begin(); it!=place_map.end(); ++it){
                    if(it->second>= waiting_process[i].getframelenth() ){
                        if(current_space < it->second){
                            start_pos = it->first;
                            current_space = it->second;
                            placed = true;
                        }
                    }
                }
                /*===================================place======================================*/
                if(placed == true){
                    int number_placed = 0;
                    for(int w = start_pos; w < 256; w++){
                        if(number_placed == waiting_process[i].getframelenth() ){
                            break;
                        }
                        memory[w] = waiting_process[i].getid();
                        number_placed++;
                    }
                    printf("time %dms: Placed process %c:\n",time_count, waiting_process[i].getid() );
                    print_memory(memory);
                    /*==========================change the vector========================*/
                    running_process.push_back(waiting_process[i]);
                    waiting_process.erase(waiting_process.begin()+i);
                    place_map.clear();
                    i--;
                }
                /*========================defrg could happend=================================*/
                if(placed == false && space >= waiting_process[i].getframelenth()){
                    bool start = false;
                    int t_memmove = 0;
                    int start_place;
                    int add = 0;
                    std::vector<char> v;
                    printf("time %dms: Cannot place process %c -- starting defragmentation\n", 
                        time_count, waiting_process[i].getid() );
                    for (int q = 0; q< 256; q++){
                        if(memory[q] == '.' && start == false){
                            start = true;
                            start_place = q;
                        }
                        else if(memory[q] != '.' && start == true){
                            memory[start_place+add] = memory[q];
                            if(std::find(v.begin(), v.end(), memory[q])!=v.end()){}
                            else
                            {
                                v.push_back(memory[q]);
                            }
                            memory[q] = '.';
                            add++;
                            time_count++;
                            t_memmove++;
                        }
                    }
                    for (int q = 255; q>= 0; q--){
                        if(memory[q] == '.'){
                            last_position = q;
                        }
                    }
                    for(unsigned int z = 0; z <waiting_process.size(); z++){
                        waiting_process[z].change_initial_time(t_memmove);
                    }
                    printf("time %dms: Defragmentation complete (moved %d frames:", time_count, t_memmove);
                    for(unsigned int z = 0; z <running_process.size(); z++){
                        running_process[z].change_end_time(t_memmove);
                    }
                    for(unsigned int z = 0; z<v.size(); z++){
                        if(z!=0){
                            printf(",");
                        }
                        printf(" %c",v[z]);
                    }
                    printf(")\n");
                    print_memory(memory);
                    start_pos = last_position;
                    for(int w = start_pos; w < start_pos+waiting_process[i].getframelenth(); w++){
                        memory[w] = waiting_process[i].getid();
                    }
                    printf("time %dms: Placed process %c:\n",time_count, waiting_process[i].getid() );
                    print_memory(memory);
                    /*==========================change the vector========================*/
                    running_process.push_back(waiting_process[i]);
                    waiting_process.erase(waiting_process.begin()+i);
                    i--;
                    placed = true;
                }
                /*=============================skip====================================*/
                if(placed == false){
                    printf("time %dms: Cannot place process %c -- skipped!\n", time_count, waiting_process[i].getid());
                    print_memory(memory);
                    waiting_process.erase(waiting_process.begin()+i);
                    i--;
                }

            }
        }
        time_count++;
    }//while()
    printf("time %dms: Simulator ended (Contiguous -- Worst-Fit)\n", time_count-1 );
}

void first_fit(std::vector<process> process_vector){
    char memory[256];
    memset(memory,'.',sizeof(memory));
    int time_count = 0;
    char * last_char = (char *)malloc(sizeof(char));
    std::vector<process> running_process, waiting_process;
    waiting_process = process_vector;
    printf("time %dms: Simulator started (Non-contiguous)\n", time_count);
    while(1){
        if(waiting_process.empty() && running_process.empty()){
            break;
        }
        /*==========================================running process==========================================*/
        for (unsigned int i = 0; i <running_process.size(); i++){
            if(running_process[i].getendtime() == time_count){
                bool start = false;
                for(int q = 0; q<256; q++){
                    if(memory[q] == running_process[i].getid() && start == false){
                        start = true;
                        memory[q] = '.';
                    }
                    else if(memory[q] == running_process[i].getid() && start == true){
                        memory[q] = '.';
                    }
                }
                printf("time %dms: Process %c removed:\n", time_count, running_process[i].getid());
                print_memory(memory);
                running_process.erase(running_process.begin()+i);
                i--;
            }
        }
        /*============================================place process==================================*/
        for (unsigned int i = 0; i <waiting_process.size(); i++){
            if(waiting_process[i].getinitialtime() == time_count){
                printf("time %dms: Process %c arrived (requires %d frames)\n",  time_count,
                waiting_process[i].getid(), waiting_process[i].getframelenth() );
                int framecount = 0;
                int start_pos;
                //int space = 0;
                bool placed = false;
                bool start = false;
                /*===================================first fit======================================*/
                for(int q = 0; q<256; q++){
                    if(memory[q] == '.' && start == false){
                        start_pos = q;
                        start = true;
                        framecount++;
                        //space++;
                    }
                    else if(memory[q] == '.' && start == true){
                        framecount++;
                    }
                    /*============================placed in=======================================*/
                    if(framecount == waiting_process[i].getframelenth() ){
                      
                        *last_char = waiting_process[i].getid();
                        placed = true;
                        for(int w = start_pos; w <= q; w++){
                            if(memory[w] == '.'){
                                memory[w] = waiting_process[i].getid();
                            }
                        }
                        printf("time %dms: Placed process %c:\n",time_count, waiting_process[i].getid() );
                        print_memory(memory);
                        /*==========================change the vector========================*/
                        running_process.push_back(waiting_process[i]);
                        waiting_process.erase(waiting_process.begin()+i);
                        i--;
                    }
                    if (placed == true){
                       break;
                    }
                }
                /*=================================from top========================================*/
                if(placed == false){
                /*=============================skip====================================*/
                    printf("time %dms: Cannot place process %c -- skipped!\n", time_count, waiting_process[i].getid());
                    print_memory(memory);
                    waiting_process.erase(waiting_process.begin()+i);
                    i--;
                }
            }
        }
        time_count++;
    }//while()
    printf("time %dms: Simulator ended (Non-contiguous)\n", time_count-1 );
}

//print array
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

//opt
void opt(std::vector<int> pages){
    std::map<int, int> lru_map;
    int place [] = {-1,-1,-1};
    int page_fault = 0;
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
        //if no that page
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
            //if cannot be added
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

//lru
void lru(std::vector<int> pages){
    std::map<int, int> lru_map;
    int place [] = {-1,-1,-1};
    int page_fault = 0;
    printf("Simulating LRU with fixed frame size of 3\n");
    for (unsigned int i = 0; i <pages.size(); i++){
        lru_map[pages[i]] = i;
        bool victim = false;
        bool has = false;
        for(int a = 0; a < 3; a++){
            if(pages[i] == place[a]){
                has = true;
                break;
            }
        }
        //if no that page
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
            //if cannot be added
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

//lfu
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
        //if no that page
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
            //if cannot be added
            if(placed == false){
                
                int index = 4;
                int freq = pages.size();

                for (int a = 0; a < 3; a++){
                    if(lfu_map[place[a]] < freq){
                        freq = lfu_map[place[a]];
                        index = a;
                        victim_page = place[a];
                        victim = true;
                    }
                    else if(lfu_map[place[a]] == freq){
                       
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