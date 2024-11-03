/*  ./test-csim
 *                       Your simulator     Reference simulator
 *  Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
 *      3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
 *      3 (4,2,4)       4       5       2       4       5       2  traces/yi.trace
 *      3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
 *      3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
 *      0 (2,2,3)     202      36      28     201      37      29  traces/trans.trace
 *      3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
 *      3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
 *      6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
 *    24
 *
 *   TEST_CSIM_RESULTS=24
 */


#include "cachelab.h"
#include <assert.h>
#include<getopt.h>
#include <time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

//void printSummary(int hits, int misses, int evictions)
//{
//    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
//    FILE* output_fp = fopen(".csim_results", "w");
//    assert(output_fp);
//    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
//    fclose(output_fp);
//}

void printfusage(){
    printf("./csim: Missing required command line argument\n");
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n\n");
    printf("  Examples:\n");
    printf("  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
    return;
}
int hits;
int misses;
int evictions;
typedef struct{
    int valid;
    int clru;
    long tag;
} cacheline;
cacheline **initcache(int S,int e){
    cacheline **cache;
    cache=(cacheline **)malloc(S*(sizeof(cacheline *)));
    for(int i=0;i<S;i++){
        cache[i]=(cacheline *)malloc(e*(sizeof(cacheline)));
        for(int j=0;j<e;j++){
            cache[i][j].valid=0;
            cache[i][j].clru=0;
            cache[i][j].tag=0;
        }    
    }
    return cache;
}
void hitcount(int set,int e,long tagaddress,cacheline **cache){
    for(int i=0;i<e;i++){
        if((cache[set][i].valid==1)&&(cache[set][i].tag==tagaddress)){
            hits++;
            cache[set][i].clru=0;
            for(int j=0;j<e;j++){
                if((cache[set][j].valid==1)&&(j!=i)){
                    cache[set][j].clru++;
                }
            }
            return ;
        }        
    }
    misses++;
    int max;
    int f=0;
    for(int i=0;i<e;i++){
        if(cache[set][i].valid==1){
            f++;
        }
    }
    if(f<e){
        for(int j=0;j<e;j++){
                if(cache[set][j].valid==0){
                    cache[set][j].tag=tagaddress;
                    cache[set][j].valid=1;
                    cache[set][j].clru=0;
                    break;
                }
    }
    }        
    if(f==e){
        max=cache[set][0].clru;
                    for(int k=1;k<e;k++){
                        if(cache[set][k].clru>max){
                            max=cache[set][k].clru;
                            }
                        }
                    for(int g=0;g<e;g++){
                        if(cache[set][g].clru==max){
                            cache[set][g].tag=tagaddress;
                            cache[set][g].valid=1;
                            cache[set][g].clru=0;
                            evictions++;
                            break;
                        }
                    }                    
                    }
return;     
}
int main(int argc, char *argv[]){
    char opt;
    int s;
    int e;
    int b;
    char *T;
    while((opt=(getopt(argc,argv ,"hvs:E:b:t:")))!=-1){
        switch(opt){
            case'h':
            printfusage();
            break;
            case'v':
            printf("This is version1.0\n");
            break;
            case's':
            s=atoi(optarg);
            break;
            case'E':
            e=atoi(optarg);
            break;
            case'b':
            b=atoi(optarg);
            break;
            case't':
            T=optarg;
            break;
            default:
            printf("Please check, put -h for help\n");  
                

        }
    }
    printf("s=%d E=%d b=%d t=%s\n",s,e,b,T); 
    int S=(1<<s);
//    int B=(1<<b);
    cacheline **cache;
    cache=initcache(S,e);
//    printf("  cache[0][0]地址是%x\n  cache[0][1]地址%x\n  cache[1][0]地址%x\n",&cache[0][0],&cache[0][1],&cache[1][0]);
    char operation;
    long address;
    int size;
    
    FILE *fd;
    fd=fopen(T,"r");
    if(fd==NULL){
        printf("cannot open the file\n");
    }
    while((fscanf(fd,"%c %lx %d",&operation,&address,&size))>0){
        int set=((address>>b)%S);
        long tagaddress=(address>>(s+b));        
        switch(operation){
            case'L':
            case'S':
//            printf("%c %x %d\n",operation,address,size);
            hitcount(set,e,tagaddress,cache);
            break;            
            case'M':
//            printf("%c %x %d\n",operation,address,size);
            hitcount(set,e,tagaddress,cache);
            hitcount(set,e,tagaddress,cache);
            printf("hit%d miss%d eviction%d\n",hits,misses,evictions);
            break;
            
        }
        
    }
    printf("hit%d miss%d eviction%d\n",hits,misses,evictions);
    fclose(fd);
    free(cache);
    printSummary(hits, misses, evictions);
    return 0;
}
