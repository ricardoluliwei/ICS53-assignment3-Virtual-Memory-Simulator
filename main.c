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

int counter = 0;

int Virtual_MEM[8][8];

int Disk[8][8];

int Physical_MEM[4][8];

int PHY_MEM_INFO[4]; //store the logical address of related physical address

int Page_table[8][3]; //0 is valid bit 1 is dirty bit 2 is PN

int LTable[4]; // keep track of the activity score of each memory for LRU

int FTable[4]; //keep track of physical memory's swap time stamp for FIFO

int mode;  //mode 1: LRU  mode 0: FIFO; Default is 0(FIFO)

void write_page_table(int n, int f, int s, int t){
    Page_table[n][0] = f;
    Page_table[n][1] = s;
    Page_table[n][2] = t;
}

// return a ppn
int LRU_swap(int vpn){
    int victim;
    int lowest;
    int ppn;
    int i;

    for(i = 0; i < 4; i++){
        if (LTable[i] == -1){
            // find empty main memory page, copy disk page into it.
            memcpy(Physical_MEM[i], Disk[vpn], 8 * sizeof(int));
            // set up pagetable entry
            write_page_table(vpn, 1, 0, i);
            return i;
        }
    }

    // if no main memory page is empty, we need to find a victim

    lowest = INT32_MAX;
    for(i = 0; i < 8; i++){
        if(Page_table[i][0]){
            // the page is in main memory, try to find victim
            if(LTable[Page_table[i][2]] < lowest){
                victim = i;
            }
        }
    }
    ppn = Page_table[victim][2];
    memcpy(Disk[victim], Physical_MEM[ppn], 8 * sizeof(int));
    memcpy(Physical_MEM[ppn], Disk[vpn], 8 * sizeof(int));

    write_page_table(victim, 0, 0, victim);
    write_page_table(vpn, 1, 0, ppn);
    return ppn;
};

int FIFO_swap(int disk_number){
    int i, min_page = 0, minbuf = -1, old_vnumber = -1; //min page: physical address that needs to be swaped
    for(i =0; i< 4; i++){ // find phy addr with least usage
        if(FTable[i] <=minbuf){
            min_page = i;
            minbuf = FTable[i];
        }
    }
    //find old vnumber
    for(i =0; i< 8; i++){
        if(Page_table[i][2] == min_page && Page_table[i][0] == 1){
            old_vnumber = i;
            break;
        }
    }
    if(old_vnumber == -1) {perror("cannot find old vnumber");}
    for(i =0; i< 8; i++){//save phy mem to disk
        Disk[old_vnumber][i] = Physical_MEM[min_page][i];
    }
    //update PDE of original page
    Page_table[old_vnumber][0] = 0;
    Page_table[old_vnumber][1] = 0;
    Page_table[old_vnumber][2] = old_vnumber;
    //
    for(i =0; i< 8; i++){// read from disk
        Physical_MEM[min_page][i] = Disk[disk_number][i];
    }
    //update PDE of new page
    Page_table[disk_number][0] = 1;
    Page_table[disk_number][1] = 0;
    Page_table[disk_number][2] = min_page;
    //update Ftable
    FTable[min_page] = counter;
    return min_page;
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

void init(){
    int i, j;
    mode = 0;
    counter = 0;
    for(i=0; i< 4;i++){
        FTable[i] = 0;
        LTable[i] = 0;
    }
    for(i=0; i< 8;i++){
        for(j = 0; j < 8; j++){
            Disk[i][j] = -1;
            Virtual_MEM[i][j] = -1;
        }
    }
    for(i=0; i< 4;i++){
        for(j = 0; j < 8; j++){
            Physical_MEM[i][j] = -1;
        }
    }
    for(i=0; i< 8;i++){
        Page_table[i][0] = 0;
        Page_table[i][0] = 0;
        Page_table[i][0] = i;
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    init();
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
           counter++;
       }
    return 0;
}
