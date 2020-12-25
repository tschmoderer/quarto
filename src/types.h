#ifndef QUARTO_H
#define QUARTO_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h> 
#include <inttypes.h>
#include <assert.h>
#include <string.h>

#include <stdlib.h>
#include <time.h>

#ifndef NPARALLEL
    #include <omp.h>
#endif

/** TODO: 
 * Make options: 
 *  - NDEBUG(N) = SEQUENTIAL + DEBUG + ASSERT + PERFTEST(N)
 *  - NPTEST(N) = SEQUENTIAL + -O2 + -pg + PERFTEST(N) + PROFILER
 *  - RELEASE(N) = -O3 + NBTHDS(N)
**/

#define NB_PCE 16
#define NB_SQR 16
#define NB_WIN_POS 10
#define MAX_PLY 16

#define INFINITY 4096
#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

typedef uint8_t square_t; 
typedef uint8_t piece_t; 
typedef uint64_t brdkey_t; 

extern const piece_t pce_list[NB_PCE];
extern const square_t winning_positions[NB_WIN_POS][4];

extern unsigned long long nb_nodes;
extern unsigned long long nb_cuts;

typedef struct {
    square_t sqr;  // where to put the piece my opponent choose
	uint8_t  pce_nb;  // piece selected for the next player
} move_t; 

typedef struct {
    move_t moves[256]; // list of moves number of max moves = 16*16 = 256
    unsigned int cnt;  // number of moves
} movelist_t;

typedef struct {
	piece_t previous_selected_pce; // the previous pieces that was set to play
    move_t  the_mv;                // the move played
} history_t; 

typedef struct {
	bool    sqrs[NB_SQR];      // true if the square is occupied,
    piece_t pces[NB_SQR];      // the piece of a given square,
    bool    pces_left[NB_PCE]; // true if the pieces is not on the board

    piece_t   nextPce; 
    brdkey_t  key; // reservation 
    uint8_t   ply; 
    history_t game[MAX_PLY]; 
} board_t;

/** function prototypes **/

/* game.c */
void randomGame();
#ifndef NPARALLEL
void para_randomGame(); 
int para_negamax(board_t * , int , int );
#endif 
void explore(board_t * , int );
void perft(int );
int negamax(board_t * , int , int , int , int );

/* board.c */
void init_brd(board_t * );
bool win(board_t * );

/* eval.c */
int  eval(board_t *); 

#ifndef NDEBUG
bool check_board(board_t * );
#endif

/* movegen.c */
void movegen(board_t * , movelist_t * );

/* move.c */
void do_move(board_t * , move_t );
void undo_move(board_t * );

/* piece.c */
void print_pce(piece_t );
uint8_t common2(piece_t , piece_t );
bool common4(piece_t , piece_t , piece_t , piece_t );

#ifndef NDEBUG
void prob2();
void prob4();
#endif


#endif