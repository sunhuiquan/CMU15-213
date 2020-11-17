#include "cachelab.h"
#include <unistd.h>
#include <stdint.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct
{
    bool valid;
    uint64_t tag; /*find the right line(block)*/
    uint64_t time_counter;
    // a line only have one block,hence the line size
    // is same to the block size
} line;
typedef line *entry_of_lines;
typedef entry_of_lines *entry_of_sets;

typedef struct
{
    int hit;
    int miss;
    int eviction;
} result;

char *help_message = "Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n"
                     " -h: Optional help flag that prints usage info\n"
                     " -v: Optional verbose flag that displays trace info\n"
                     " -s <s>: Number of set index bits (S = 2sis the number of sets)\n"
                     " -E <E>: Associativity (number of lines per set)\n"
                     " -b <b>: Number of block bits (B = 2bis the block size)\n"
                     " -t <tracefile>: Name of the valgrind trace to replay\n";
char *command_options = "hvs:E:b:t:";
char file_name[128];
uint64_t s = 0;
uint64_t E = 0;
uint64_t b = 0;
uint64_t S = 0;
FILE *fp = NULL;
entry_of_sets cache = NULL;
bool verbose = false;
result res = {0, 0, 0};

void init_cache();
void read_test();
void hit_miss_evict(entry_of_lines search_line, uint64_t flag);
void free_cache();

int main(int argc, char *argv[])
{
    char ch;
    while ((ch = getopt(argc, argv, command_options)) != -1)
    {
        switch (ch)
        {
        case 'h':
            printf("%s\n", help_message);
            exit(EXIT_SUCCESS);
        case 'v':
            verbose = true;
            break;
        case 's':
	    if(atol(optarg) <= 0){
		printf("%s\n",help_message);
		exit(EXIT_FAILURE);
	    }
	    s = atol(optarg);
            S = 1 << s;
	    break;
        case 'E':
	    if(atol(optarg) <= 0){
		printf("%s\n",help_message);
		exit(EXIT_FAILURE);
	    }
            E = atol(optarg);
            break;
        case 'b':
	     if(atol(optarg) <= 0){
		printf("%s\n",help_message);
		exit(EXIT_FAILURE);
	    }
            b = atol(optarg);
            break;
        case 't':
            if ((fp = fopen(optarg, "r")) == NULL)
            {
                printf("Can't open the file!\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            printf("%s\n", help_message);
            exit(EXIT_FAILURE);
        }
    }

    init_cache();
    read_test();
    free_cache();

    printSummary(res.hit, res.miss, res.eviction);

    return 0;
}

void init_cache()
{
    if (s == 0 || b == 0 || E == 0)
    {
        printf("%s\n", help_message);
        exit(EXIT_FAILURE);
    }

    cache = (entry_of_sets)calloc(S, sizeof(entry_of_lines)); // cache can init member value

    for (int i = 0; i < S; i++)
    {
        cache[i] = (entry_of_lines)calloc(E, sizeof(line));
    }
}

void read_test()
{
    char ch;
    uint64_t address;

    // input format _X_*,* first is a ' ' don't forget
    while ((fscanf(fp, " %c %lx%*[^\n]",&ch,&address)) == 2) // %lx get the hex input value
    {
        if (ch == 'I') // just ignore
        {
            continue;
        }
        else
        {
            uint64_t mask = (1 << s) - 1;
            uint64_t set_index = (address >> b) & mask;
            uint64_t flag = (address >> b) >> s;

            if (ch == 'L' || ch == 'S')
            {
                if (verbose)
                    printf("%c %lx ", ch, address);
                hit_miss_evict(cache[set_index], flag);
            }
            else if (ch == 'M')
            {
                if (verbose)
                    printf("%c %lx ", ch, address);
                hit_miss_evict(cache[set_index], flag);
                hit_miss_evict(cache[set_index], flag);
            }
            else
            {
                continue;
            }
        }
    }
}

void hit_miss_evict(entry_of_lines search_line, uint64_t flag)
{
    int is_hit = 0;
    for (int i = 0; i < E; i++)
        if (search_line[i].tag == flag && search_line[i].valid == true)
        {
            is_hit = 1;
            ++res.hit;
            search_line[i].time_counter++;
            if (verbose)
                printf("hit\n");
            break;
        }

    uint64_t old = UINT64_MAX, old_i, young = 0;
    if (!is_hit)
    {
        if (verbose)
            printf("miss");
	++res.miss;

        for (int i = 0; i < E; i++)
            if (search_line[i].time_counter < old)
            {
                old = search_line[i].time_counter;
                old_i = i;
            }

        for (int i = 0; i < E; i++)
            if (search_line[i].time_counter > young)
                young = search_line[i].time_counter;

        search_line[old_i].tag = flag;
        search_line[old_i].time_counter = young + 1;
        if (search_line[old_i].valid == 0)
        {
            if (verbose)
                printf("\n");

            search_line[old_i].valid = 1;
        }
        else
        {
            if (verbose)
                printf(" and eviction\n");
		++res.eviction;
        }
    }
}

void free_cache()
{
    for (int i = 0; i < S; ++i)
        free(cache[i]);
    free(cache);
}
