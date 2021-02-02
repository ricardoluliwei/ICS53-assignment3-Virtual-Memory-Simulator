//
//  main.c
//  53-Assignment3
//
//  Created by Tony on 2021/1/31.
//

#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#define MAX_LINE 80

struct linked_list{
    int phy_number;
    struct linked_list* next;
};

int Virtual_MEM[8][8];

int Disk[8][8];

int Physical_MEM[4][8];

int PHY_MEM_INFO[4]; //store the logical address of related physical address

struct linked_list* start; //start node for FIFO

int LTable[4]; // keep track of the activity score of each memory for LRU

int mode;  //mode 1: LRU  mode 0: FIFO; Default is 0(FIFO)

int LRU_swap(int disk_number){
    int i, max_page = 0, maxbuf = -1;
    for(i =0; i< 4; i++){ // find phy addr with least usage
        if(LTable[i] >=maxbuf){
            max_page = i;
            maxbuf = LTable[i];
        }
    }
    for(i =0; i< 8; i++){//save phy mem to disk
        Disk[PHY_MEM_INFO[max_page]][i] = Physical_MEM[max_page][i];
    }
    for(i =0; i< 8; i++){// read from disk
        Physical_MEM[max_page][i] = Disk[disk_number][i];
    }
    PHY_MEM_INFO[max_page] = disk_number;
    return max_page;
};

int FIFO_swap(int disk_number){
    int i;
    int phy_needs_to_swaped = start->phy_number;
    for(i =0; i< 8; i++){ //save phy mem to disk
        Disk[PHY_MEM_INFO[phy_needs_to_swaped]][i] = Physical_MEM[phy_needs_to_swaped][i];
    }
    for(i =0; i< 8; i++){ // read from disk
        Physical_MEM[phy_needs_to_swaped][i] = Disk[disk_number][i];
    }
    struct linked_list *new_page = malloc(sizeof(struct linked_list));
    new_page->next = NULL;
    new_page->phy_number = disk_number;
    struct linked_list *buf = start->next;
    start->next = start->next->next;
    free(buf);
    struct linked_list *buf2 = start;
    while(buf2->next!=NULL){
        buf2 = buf2->next;
    }
    buf2->next = new_page;
    PHY_MEM_INFO[phy_needs_to_swaped] = disk_number; // update phy mem info
    return phy_needs_to_swaped;
};

void read_mem(int vaddr){
    int i,j;
    int pnumber = vaddr/8;
    int offset = vaddr%8;
    for(i =0 ;i < 4; i++){
        if(PHY_MEM_INFO[i] == pnumber){
            printf("%d\n", Physical_MEM[i][offset]);
            for(j = 0; j< 4; j++){ //Update LRU Information
                if(j != i){
                    LTable[j] ++; //other page: add age
                }else{
                    LTable[j] = 0; // reset that page to 0
                }
            }
            return;
        }
    }
    printf("A Page Fault Has Occurred\n");
    int n_pnumber = 0;
    if(mode){
        n_pnumber = LRU_swap(vaddr);
    }else{
        n_pnumber = FIFO_swap(vaddr);
    }
    for(i =0 ;i < 4; i++){
        if(PHY_MEM_INFO[i] == n_pnumber){
            printf("%d\n", Physical_MEM[i][offset]);
            for(j = 0; j< 4; j++){ //Update LRU Information
                if(j != i){
                    LTable[j] ++; //other page: add age
                }else{
                    LTable[j] = 0; // reset that page to 0
                }
            }
            return;
        }
    }
    //printf("%d\n", Physical_MEM[i][offset]);
    return;
};



void write_mem(int v_addr, int num){
    return;
};

void showmain(int ppn){
    return;
};

void showptable(){
    return;
};

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    mode = 0;
    if(strcmp(argv[1], "LRU")==0){mode = 1;}
    char input[800], *buffer, full[800];
    int bufsize = MAX_LINE;
    char **args = malloc(bufsize *sizeof(char *));
    int counter = 0;
    while (1) // while loop to get user input
       {
           printf("> ");
           memset(input, 0, 80);
           memset(args, 0, bufsize *sizeof(char *));
           fgets(input, (sizeof input / sizeof input[0]), stdin);
           if(input[strlen(input)-1] == '\n') input[strlen(input)-1]=0;
           
           if(strcmp(input, "quit") == 0){break;}
           strcpy(full, input);
           buffer = strtok(input, " ");

           counter = 0;
           while(buffer) {
               //printf( "%s\n", buffer );
               //strcpy(args[counter], buffer);
               args[counter] = buffer;
               buffer = strtok(NULL, " ");
               counter++;
           }
           if(strcmp(args[0], "read")==0){
               continue;
           }
           if(strcmp(args[0], "write")==0){
               continue;
           }
           if(strcmp(args[0], "showmain")==0){
               continue;
           }
           if(strcmp(args[0], "showdisk")==0){
               continue;
           }
           if(strcmp(args[0], "showptable")==0){
               continue;
           }
           
       }
    return 0;
}
