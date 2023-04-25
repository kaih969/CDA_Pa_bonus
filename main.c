#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define SIZE 32

struct cache
{
    char BlockAddr[SIZE];
    int cache_accesses;
    int cache_hits;
    int misses;
};

int hextoDecimal(char * address);
struct cache * initialize(struct cache * addresses, int assoc);
struct cache ** initialize2set();
struct cache * directMapped(char * address, struct cache *addresses);
struct cache ** twoWayAssociative(char * address, struct cache **addresses, int rp);
struct cache * fullyAssociative(char * address,struct cache *addresses, int rp);
void printResult (struct cache *addresses, int assoc);
// print the results of the cache simulation
void printResultfor2Set(struct cache **addresses);
// print the results of the two-way set associative cache simulation


int main(int argc, char *argv[]) {

    FILE * ifp = fopen("traces.txt", "r");
    // pointer to a FILE structure to the variable ifp
    char addr[32];
    //an array of characters named addr with a length of 32
    struct cache *directA;
    directA = initialize(directA, 1);
    // initialize it as a direct-mapped cache with associativity 1
    struct cache *fullyA = initialize(fullyA, 0);
    //pointer to a cache struct named fullyA and initializes it as a fully associative cache
    struct cache **twoWay = initialize2set();
    //used to implement a two-way set-associative cache.
    int c = getc(ifp);
    int count = 0;
    //a file pointer to a file named "traces.txt" that was opened earlier in the code.
    while (c != EOF)
    {
        fscanf(ifp, "%s", addr);
        count++;
        directA = directMapped(addr, directA);
        fullyA = fullyAssociative(addr, fullyA, 1);
        twoWay = twoWayAssociative(addr, twoWay, 1);
        //printf("%s\t%d\n", addr, count);
        c = getc(ifp);
    }
    //a loop that reads through the file character-by-character until it reaches the end of the file (EOF). Inside the loop
    printResult(directA,1);
    printResult(fullyA,3);
    printResultfor2Set(twoWay);
    // print out the cache statistics
    free(directA);
    for(int i = 0; i < 2; i++)
    {
        free(twoWay[i]);
    }
    free(twoWay);
    //each element must be freed individually before freeing the array itself
    printf("done");
    fclose(ifp);
    //closes the input file
    return 0;
}

//converting the 16 base to 10 base
int hextoDecimal(char * address)
{
    int result = 0;
    int exp = 0;

    for(int i = strlen(address)-1; i > 1; i --)
    {
        if(address[i] >= 'a' && address[i] <= 'f')
            result += (address[i] - 'a' +10) * pow(16, exp);
        else
            result += (address[i] - '0') * pow(16, exp);
        exp++;
    }
    return result;
}
// takes in a hexadecimal address as a string and converts it to its equivalent decimal representation
struct cache * initialize(struct cache * addresses, int assoc)
{
    //condition associative is 1
    if (assoc == 1)
    {
        addresses = (struct cache*)malloc(sizeof(struct cache) * 32);
        for(int i  = 0 ; i < SIZE ; i++)
        {
            strcpy(addresses[i].BlockAddr, "EMPTY");
            addresses[i].cache_accesses = 0;
            addresses[i].cache_hits = 0;
        }
    }
    else
    {
        struct cache fullset[36];
        addresses = fullset;
        for(int i  = 0 ; i < SIZE ; i++)
        {
            strcpy(addresses[i].BlockAddr, "EMPTY");
            addresses[i].cache_accesses = 0;
            addresses[i].cache_hits = 0;
        }
    }
    return addresses;
}
//This function initializes the cache with the given associativity
struct cache ** initialize2set()
{
    struct cache ** twoSets = malloc(sizeof(struct cache*)*2);
    twoSets[0] = malloc(sizeof(struct cache) * 16);
    twoSets[1] = malloc(sizeof(struct cache) * 16);

    for(int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            strcpy(twoSets[i][j].BlockAddr, "EMPTY");
            twoSets[i][j].cache_accesses = 0;
            twoSets[i][j].cache_hits = 0;
        }
    }
    return twoSets;
}
//initializes a 2-way set-associative cache by allocating memory for an array of two pointers to struct cache
struct cache * directMapped(char * address, struct cache *addresses)
{
    int ret = hextoDecimal(address);
    int block_nr = ret % SIZE;

    //compare the two address
    if(strcmp(address,addresses[block_nr].BlockAddr) != 0)
    {
        addresses[block_nr].cache_accesses++;
        strcpy(addresses[block_nr].BlockAddr, address);
    }
    else
    {
        addresses[block_nr].cache_accesses++;
        addresses[block_nr].cache_hits++;
    }
    return addresses;
}


struct cache * fullyAssociative(char * address,struct cache *addresses, int rp)
{
    srand(time(0));
    //initialize the random number generator

    //looping for fully associative
    for(int i = 0; i < 32; i++)
        // iterates over all cache blocks (32)
    {
        //compare the each address
        if(strcmp(address,addresses[i].BlockAddr) == 0)
        {

            addresses[i].cache_accesses++;
            addresses[i].cache_hits++;
            break;
        }
            //If the given memory address address is found in the cache block i
        else if(strcmp(addresses[i].BlockAddr, "EMPTY") == 0)
        {
            addresses[i].cache_accesses++;
            strcpy(addresses[i].BlockAddr, address);
            break;
        }
        else
        {
            if (rp == 1)  // LRU replacement policy
            {
                int least = addresses[0].cache_accesses;
                int index = 0;
                for (int j = 1; j < 32; j++)
                {
                    if(addresses[j].cache_accesses < least)
                    {
                        least = addresses[j].cache_accesses;
                        index = j;
                    }
                }
                addresses[index].cache_accesses++;
                strcpy(addresses[index].BlockAddr, address);
                break;
            }
            else
            {
                int r = rand() % 32;
                addresses[r].cache_accesses++;
                strcpy(addresses[r].BlockAddr, address);
                break;
            }

        }
    }
    return addresses;
}
struct cache ** twoWayAssociative(char * address, struct cache **addresses, int rp)
{
    int ret = hextoDecimal(address);
    //hexadecimal address to decimal
    int block_nr = ret % 16;
    srand(time(0));
    int r = rand() %2;
    if(strcmp(address, addresses[0][block_nr].BlockAddr) != 0 && strcmp(address, addresses[1][block_nr].BlockAddr) !=0 )
        // given address is not present in either of the two cache sets
    {
        //condition if replacement is not 1
        if (rp != 1) // not LRU replacement policy
        {
            addresses[r][block_nr].cache_accesses++;
            strcpy(addresses[r][block_nr].BlockAddr, address);
        }
        else  // lRU replacement policy
        {
            if(addresses[0][block_nr].cache_accesses < addresses[1][block_nr].cache_accesses )
            {
                addresses[0][block_nr].cache_accesses++;
                strcpy(addresses[0][block_nr].BlockAddr, address);
            }
            else
            {
                addresses[1][block_nr].cache_accesses++;
                strcpy(addresses[1][block_nr].BlockAddr, address);
            }
        }
    }
        //updated address
    else
    {
        if(strcmp(address, addresses[0][block_nr].BlockAddr) == 0)
        {
            addresses[0][block_nr].cache_accesses++;
            addresses[0][block_nr].cache_hits++;
        }
        else
        {
            addresses[1][block_nr].cache_accesses++;
            addresses[1][block_nr].cache_hits++;
        }
    }

}
void printResult (struct cache *addresses, int assoc)
{
    int total_cache_accesses = 0;
    int total_hit = 0;
    if ( assoc == 1)
    {
        printf("Direct Way\n");
    }
    if ( assoc == 3)
    {
        printf("Fully Associative\n");
    }

    for(int i = 0; i < 32; i++ )
    {

        total_cache_accesses +=addresses[i].cache_accesses;
        total_hit +=addresses[i].cache_hits;
    }
    printf("Total Accesses: %d\n", total_cache_accesses);
    printf("Total cache_hits: %d\n", total_hit);
    printf("Hit rate: %f\n\n", (float)total_hit/total_cache_accesses);
}

void printResultfor2Set(struct cache **addresses)
{
    int total_cache_accesses = 0;
    int total_hit = 0;
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            total_cache_accesses += addresses[i][j].cache_accesses;
            total_hit += addresses[i][j].cache_hits;
        }
    }
    printf("Total Accesses: %d\n", total_cache_accesses);
    printf("Total cache_hits: %d\n", total_hit);
    printf("Hit rate: %f\n\n", (float)total_hit/total_cache_accesses);
}