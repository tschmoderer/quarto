#include "types.h"

// move generation function
void movegen(board_t * brd, movelist_t * lst) {
    lst->cnt = 0; 
    // 0. if ply == 15 then the only move is to place the piece in the only remaining suare
    if (brd->ply == 15) {
        for (int sq = 0; sq < NB_SQR; sq++) {
            if (brd->sqrs[sq] == false) {
                    lst->moves[lst->cnt].sqr = sq; 
                    lst->cnt++;
                    break; 
            }
        }
        assert(lst->cnt == 1); 
        return;
    }
    // Regular moves
    // 1. we try to place the piece on the board
    for (int sq = 0; sq < NB_SQR; sq++) {
        if (brd->sqrs[sq] == false) {
            // 2. we try to pick a piece
            for (int pce = 0; pce < NB_PCE; pce++) {
                if (brd->pces_left[pce] == true) {
                    lst->moves[lst->cnt].sqr = sq; 
                    lst->moves[lst->cnt].pce_nb = pce; 
                    lst->cnt++; 
                }
            }  
        }
    }
    assert(lst->cnt >= 2);   // min moves : 2 squares  x 1  piece left
    assert(lst->cnt <= 240); // max moves : 16 squares x 15 pieces left
}