#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "types.h"

#define NUM_THREADS 1
#define DEPTH 5

int main() {  
    srand(time(NULL)); 
    
    omp_set_num_threads(NUM_THREADS); 
    double t0 = omp_get_wtime(); 

    board_t brd[1];
    init_brd(brd); 
    int res = 0; 

    for (int d = 1; d <= DEPTH; d++) {
        res = para_negamax(brd, d, -1); 
        printf("Quarto: %d thds \t depth: %d \t result: %d \t nodes: %llu \t\t cuts: %llu \t\t time: %f s\n", NUM_THREADS, d, res, nb_nodes, nb_cuts, omp_get_wtime() - t0); 
    }    

    return 0; 
}