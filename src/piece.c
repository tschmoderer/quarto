#include "types.h"

/**
 * @brief Print the caracteristics of a piece
 * 
 * @param p0 a piece 
 */
void print_pce(piece_t p0) {
    assert((p0 >= 0) && (p0 <= 0xF)); 
    uint8_t b3 = p0 & 0x8; uint8_t b2 = p0 & 0x4; 
    uint8_t b1 = p0 & 0x2; uint8_t b0 = p0 & 0x1; 

    if (b3 > 0) {printf("(Red, ");} else {printf("(White, ");}
    if (b2 > 0) {printf("Tall, ");} else {printf("Short, ");}
    if (b1 > 0) {printf("Square, ");} else {printf("Circular, ");}
    if (b0 > 0) {printf("Hollow)\n");} else {printf("Solid)\n");}
}


/**
 * @brief Compute the common point between two pieces withe bitwise XNOR
 * 
 * @param p1 the first piece, must be between 0x0 and 0xF
 * @param p2 the second piece, must be between 0x0 and 0xF
 * @return uint8_t a 4 bits number where a '1' denotes a common point between p1 and p2
 */
uint8_t common2(piece_t p1, piece_t p2) {
    assert((p1 >= 0) && (p1 <= 0xF));
    assert((p2 >= 0) && (p2 <= 0xF));
    return ~(p1 ^ p2);
}

/**
 * @brief Compute if four pieces have at least a common point. 
 * 
 * @param p1 the first piece, must be between 0x0 and 0xF
 * @param p2 the second piece, must be between 0x0 and 0xF
 * @param p3 the third piece, must be between 0x0 and 0xF
 * @param p4 the fourth piece, must be between 0x0 and 0xF
 * @return true if the four pieces have at least one common point
 * @return false if the four pieces do not share a common point
 */
bool common4(piece_t p1, piece_t p2, piece_t p3, piece_t p4) {
    assert((p1 >= 0) && (p1 <= 0xF)); assert((p2 >= 0) && (p2 <= 0xF));
    assert((p3 >= 0) && (p3 <= 0xF)); assert((p4 >= 0) && (p4 <= 0xF));
    assert(((common2(p1,p2) & common2(p2,p3) & common2(p3,p4)) & 0xF ) == ((((~(p1 ^ p2)) & (~(p2 ^ p3)) )& (~(p3 ^ p4))) & 0xF));
    return (((((~(p1 ^ p2)) & (~(p2 ^ p3)) )& (~(p3 ^ p4))) & 0xF) > 0);
}

#ifndef NDEBUG
int nb_trial = 1000000000; 
void prob2() {     
    int nb_success = 0; 
	for (int k = 0; k < nb_trial; k++) {
        uint8_t pce_nb1 = rand() % NB_PCE; uint8_t pce_nb2 = rand() % NB_PCE;
        piece_t p1 = pce_list[pce_nb1]; piece_t p2 = pce_list[pce_nb2]; 
        if ((common2(p1, p2) & 0xF) > 0) {
            nb_success++;
        } 
    }
    printf("Prob2 (success / total): %d / %d.\n", nb_success, nb_trial); 
}
void prob4() {
    int nb_trial = 1000000000; 
    int nb_success = 0; 
	for (int k = 0; k < nb_trial; k++) {
        uint8_t pce_nb1 = rand() % NB_PCE; uint8_t pce_nb2 = rand() % NB_PCE;
        uint8_t pce_nb3 = rand() % NB_PCE; uint8_t pce_nb4 = rand() % NB_PCE;
        piece_t p1 = pce_list[pce_nb1]; piece_t p2 = pce_list[pce_nb2]; 
        piece_t p3 = pce_list[pce_nb3]; piece_t p4 = pce_list[pce_nb4]; 
        if (common4(p1,p2,p3,p4)) {
            nb_success++;
        }
    }
    printf("Prob4 (success / total): %d / %d.\n", nb_success, nb_trial); 
}
#endif
