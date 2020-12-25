#include "types.h"


void randomGame() { 
    srand(time(NULL));
    clock_t t0 = clock(); 
    uint32_t total = 0; uint32_t total_pA = 0; uint32_t total_pB = 0; 
    uint32_t ply_sum = 0; 
    int trial = 100000; 
    for (int k = 0; k < trial; k++) {
        board_t brd[1]; movelist_t lst[1];
        init_brd(brd);
        while ((!win(brd)) && (brd->ply != MAX_PLY)) {
            movegen(brd, lst); 
            do_move(brd, lst->moves[rand() % lst->cnt]); 
        }
        ply_sum += brd->ply; 
        if (win(brd)) {
            total++; 
            if (brd->ply % 2) {
                total_pA++; 
            } else {
                total_pB++;
            }
        }
    }
    printf("%d games made in %.3f ms\n", trial, (clock()-t0) * 1000.0 / CLOCKS_PER_SEC); 
    printf("average ply : %.3f\n ", ply_sum / (1. * trial));
    printf("TotA: %d\t TotB: %d\t nb win / total : %d / %d \n", total_pA, total_pB, total, trial);
}

#ifndef NPARALLEL
void para_randomGame() { 
    srand(time(NULL));
    double t0 = omp_get_wtime(); 
    uint32_t total = 0; uint32_t total_pA = 0; uint32_t total_pB = 0; 
    uint32_t ply_sum = 0; 
    int trial = 1000000; 
    #pragma omp parallel for schedule(dynamic) reduction(+: ply_sum, total_pA, total_pB, total)
    for (int k = 0; k < trial; k++) {
        board_t brd[1]; movelist_t lst[1];
        init_brd(brd);
        while ((!win(brd)) && (brd->ply != MAX_PLY)) {
            movegen(brd, lst); 
            do_move(brd, lst->moves[rand() % lst->cnt]); 
        }
        ply_sum += brd->ply; 
        if (win(brd)) {
            total++; 
            if (brd->ply % 2) {
                total_pB++; 
            } else {
                total_pA++;
            }
        }
    }
    printf("%d games made in %.3f ms\n", trial, (omp_get_wtime() - t0) * 1000.0); 
    printf("average ply : %.3f\n ", ply_sum / (1. * trial));
    printf("TotA: %d\t TotB: %d\t nb win / total : %d / %d \n", total_pA, total_pB, total, trial); 
}
#endif


uint32_t nb_nds = 0; 
void explore(board_t * brd, int depth) {
    if (depth <= 0) {
        return; 
    }
    movelist_t lst; 
    movegen(brd, &lst); 
    for (int k = 0; k < lst.cnt; k++) {
        nb_nds++; 
        do_move(brd, lst.moves[k]); 
        explore(brd, depth-1); 
        undo_move(brd); 
    }
    return; 
}

void perft(int depth) {
    board_t brd;
    clock_t t0; 
    for (int k = 1; k <= depth; k++) {
        t0 = clock();
        nb_nds = 0; 
        init_brd(&brd);
        explore(&brd, k); 
        printf("PERFT(%d) = %d nodes in %.3f ms\n", k, nb_nds, (clock()-t0) * 1000.0 / CLOCKS_PER_SEC); 
    }
}

#ifndef NPARALLEL
void para_explore(board_t * brd, int depth) {
    if (depth <= 0) {
        return; 
    }
    movelist_t lst; 
    movegen(brd, &lst); 

    #pragma omp parallel for
    for (int k = 0; k < lst.cnt; k++) {
        nb_nds++; 
        do_move(brd, lst.moves[k]); 
        explore(brd, depth-1); 
        undo_move(brd); 
    }
    return; 
}
#endif

int negamax(board_t * brd, int depth, int alpha, int beta, int color) {
    if (win(brd)) {
        return color * INFINITY; 
    }
    if (depth <= 0) {
        // eval function
        return 0; 
    }
    if (brd->ply == 16) {
        return 0; 
    }
    int res = - INFINITY; 
    movelist_t lst; 
    movegen(brd, &lst); 
    for (int k = 0; k < lst.cnt; k++) {
        nb_nodes++;
        do_move(brd, lst.moves[k]); 
        res = MAX(res, -negamax(brd, depth-1, -beta, -alpha, -color)); 
        undo_move(brd); 
        alpha = MAX(alpha, res); 
        if (alpha >= beta) {
            nb_cuts++;
            break; 
        }
    }

    return res;
}

#ifndef NPARALLEL
// lauch depth one in parallel
int para_negamax(board_t * brd, int depth, int color) {
    if (win(brd)) {
        return color * INFINITY; 
    }
    if ((depth <= 0) || (brd->ply == 16)) {
        return 0; 
    }
    int res = - INFINITY; 
    movelist_t lst; 
    movegen(brd, &lst); 
    #pragma omp parallel for schedule(dynamic) reduction(max:res) shared(brd, nb_nodes, nb_cuts) //private(depth, alpha, beta, color)
    for (int k = 0; k < lst.cnt; k++) {
        // printf("iteration: %d by thd: %d / %d\n", k+1, omp_get_thread_num()+1, omp_get_num_threads());
        nb_nodes++;

        board_t * brd_t = NULL; 
        brd_t = (board_t *)malloc(sizeof(board_t)); 
        memcpy(brd_t, brd, sizeof(board_t)); 

        do_move(brd_t, lst.moves[k]);
        res = MAX(res, -negamax(brd_t, depth-1, -INFINITY, INFINITY, -color)); 
        undo_move(brd_t); 

        free(brd_t);
    }
    return res;
}
#endif