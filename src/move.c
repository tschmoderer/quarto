#include "types.h"

void do_move(board_t * brd, move_t mv) {
    // svg history
    brd->game[brd->ply].previous_selected_pce = brd->nextPce; 
    brd->game[brd->ply].the_mv.pce_nb = mv.pce_nb;
    brd->game[brd->ply].the_mv.sqr    = mv.sqr;

    // do move 
    if (brd->ply == 15) {
        brd->sqrs[mv.sqr] = true; 
        brd->pces[mv.sqr] = brd->nextPce; 
        brd->ply++; 
        return; 
    }
    brd->sqrs[mv.sqr] = true; 
    brd->pces[mv.sqr] = brd->nextPce; 
    brd->pces_left[mv.pce_nb] = false; 
    brd->nextPce = pce_list[mv.pce_nb];
    brd->ply++; 
}

void undo_move(board_t * brd) {
    brd->ply--; 
    brd->nextPce = brd->game[brd->ply].previous_selected_pce; 
    brd->sqrs[ brd->game[brd->ply].the_mv.sqr ] = false;
    brd->pces_left[ brd->game[brd->ply].the_mv.pce_nb ] = true; 
}