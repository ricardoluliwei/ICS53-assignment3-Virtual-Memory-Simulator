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

int Disk[8][8];

int Physical_MEM[4][8];

int PHY_MEM_INFO[4]; //store the logical address of related physical address

int Page_table[8][3]; //0 is valid bit 1 is dirty bit 2 is PN

int LTable[4]; // keep track of the activity score of each memory for LRU

int FTable[4]; //keep track of physical memory's swap time stamp for FIFO

int mode;  //mode 1: LRU  mode 0: FIFO; Default is 0(FIFO)

void write_page_table(int n, int b0, int b1, int b2){
    Page_table[n][0] = b0;
    Page_table[n][1] = b1;
    Page_table[n][2] = b2;
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

    LTable[ppn] = 0;
    PHY_MEM_INFO[ppn] = vpn;
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
    write_page_table(old_vnumber, 0, 0, old_vnumber);
    //
    for(i =0; i< 8; i++){// read from disk
        Physical_MEM[min_page][i] = Disk[disk_number][i];
    }
    //update PDE of new page
    write_page_table(disk_number, 1, 0, min_page);
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



void write_mem(int vaddr, int num){
    int i,j;
    int vpn = vaddr/8;
    int offset = vaddr%8;
    int ppn;

    if(Page_table[vpn][0]){
        // if it is in main memory
        ppn = Page_table[vpn][2];
    } else{
        ppn = mode ? LRU_swap(vpn) : FIFO_swap(vpn);
    }
    // write
    Physical_MEM[ppn][offset] = num;
    // change information
    LTable[ppn]++;
    
    return;
};

void showmain(int ppn){
    int i,j;
    for(i = 0; i<8; i++){
        printf("%d: %d\n", ppn*8+i, Physical_MEM[ppn][i]);
    }
    return;
};

void showptable(){
    int i,j;
    for(i = 0; i<8; i++){
        printf("%d:%d:%d:%d\n", i, Page_table[i][0] , Page_table[i][1] , Page_table[i][2]);
    }
};

void showdisk(int dpn){
    int i;
    for(i=0; i<8; i++){
        printf("%d: %d\n", dpn*8+i, Disk[dpn][i]);
    }
    return;
}

void init(){
    int i, j;
    mode = 0;
    counter = 0;
    for(i=0; i< 4;i++){
        FTable[i] = 0;
        LTable[i] = -1;
    }
    for(i=0; i< 8;i++){
        for(j = 0; j < 8; j++){
            Disk[i][j] = -1;
        }
    }
    for(i=0; i< 4;i++){
        for(j = 0; j < 8; j++){
            Physical_MEM[i][j] = -1;
        }
    }
    for(i=0; i< 8;i++){
         write_page_table(i, 0, 0, i);
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    init();
    if(strcmp(argv[1], "LRU")==0){mode = 1;}
    char input[800], *buffer;
    int bufsize = MAX_LINE;
    char* spliter = " \n";
    while (1) // while loop to get user input
       {
           printf("> ");
           memset(input, 0, 80);
           fgets(input, (sizeof input / sizeof input[0]), stdin);
           if(input[strlen(input)-1] == '\n') input[strlen(input)-1]=0; 
           if(strcmp(input, "quit") == 0){break;}
           buffer = strtok(input, spliter);

           if(strcmp(buffer, "read")==0){
               int vaddr;
               buffer = strtok(NULL, spliter);
               vaddr = atoi(buffer);
               read_mem(vaddr);
               continue;
           }
           if(strcmp(buffer, "write")==0){
               int vaddr;
               int num;
               buffer = strtok(NULL, spliter);
               vaddr = atoi(buffer);
               buffer = strtok(NULL, spliter);
               num = atoi(buffer);
               write_mem(vaddr, num);
               continue;
           }
           if(strcmp(buffer, "showmain")==0){
               int ppn;
               buffer = strtok(NULL, spliter);
               ppn = atoi(buffer);
               showmain(ppn);
               continue;
           }
           if(strcmp(buffer, "showdisk")==0){
                int dpn;
                buffer = strtok(NULL, spliter);
                dpn = atoi(buffer);
                showdisk(dpn);
                continue;
           }
           if(strcmp(buffer, "showptable")==0){
               showptable();
               continue;
           }
       }
    return 0;
}
