#include "types.h"

/*
    - Initialisation of a board 
    - We can always assume that the first piece that player should play is 0x0.  
*/
void init_brd(board_t * brd) {
    for (int pce = 0; pce < NB_PCE; pce++) {
        brd->pces_left[pce] = true; 
    }
    for (int sq = 0; sq < NB_SQR; sq++) {
        brd->sqrs[sq] = false; 
        brd->pces[sq] = 0x0; // not useful but for completness. 
    }

    brd->nextPce      = pce_list[0]; 
    brd->pces_left[0] = false; 

    for (int p = 0; p < MAX_PLY; p++) {
        brd->game[p].previous_selected_pce = 0xF0; 
        brd->game[p].the_mv.pce_nb         = 0; 
        brd->game[p].the_mv.sqr            = 0; 
    }
    brd->ply = 0;
    brd->key = 0;
}


/**
 * @brief Compute if there is a winning position on the board
 * 
 * @param brd a pointer to a board
 * @return true if there is at least one winning combination on the board
 * @return false if there is no winning position on the board
 */
bool win(board_t * brd) {
    for (int k = 0; k < NB_WIN_POS; k++) {
        // extract the four aligned squares
        square_t sq1 = winning_positions[k][0]; square_t sq2 = winning_positions[k][1]; 
        square_t sq3 = winning_positions[k][2]; square_t sq4 = winning_positions[k][3]; 
        // if 4 pieces are set, 
        if (brd->sqrs[sq1] && brd->sqrs[sq2] && brd->sqrs[sq3] && brd->sqrs[sq4]) {
            piece_t pce1 = brd->pces[sq1]; piece_t pce2 = brd->pces[sq2]; 
            piece_t pce3 = brd->pces[sq3]; piece_t pce4 = brd->pces[sq4]; 
            if (common4(pce1, pce2, pce3, pce4)) {
                return true; 
            }
        }
    }
    return false; 
}

#ifndef NDEBUG
bool check_board(board_t * brd) {
    return true; 
}
#endif

