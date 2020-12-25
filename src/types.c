#include "types.h"

const piece_t pce_list[NB_PCE] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

const square_t winning_positions[NB_WIN_POS][4] = {
    {3, 2, 1, 0}, {7, 6, 5, 4}, {11, 10, 9, 8}, {15, 14, 13, 12}, {15, 11, 7, 3}, {14, 10, 6, 2}, 
    {13, 9, 5, 1}, {12, 8, 4, 0}, {15, 10, 5, 0}, {12, 9, 6, 3}
};

unsigned long long nb_nodes;
unsigned long long nb_cuts; 