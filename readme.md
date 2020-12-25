# Solving the Quarto

The purpose of this project is to implement the quarto game in C language and try to see if it is solvable or not. First, let us describe this 2 player game (taken from [Wikipédia](https://en.wikipedia.org/wiki/Quarto_(board_game))): 

> Quarto is a board game for two players invented by Swiss mathematician Blaise Müller. It is published and copyrighted by Gigamic. The game is played on a 4×4 board. 
>
> |                  Picture of the game board                   | Description of Quarto                                        |
> | :----------------------------------------------------------: | :----------------------------------------------------------- |
> | <img src="https://upload.wikimedia.org/wikipedia/commons/b/b4/QuartoSpiel.JPG" alt="Game board" style="zoom:100%;" /> | There are 16 unique pieces to play with, each of which is either:<br />* tall or short;<br />* red or white;<br />* square or circular;<br />* hollow-top or solid-top.<br />Players take turns choosing a piece which the other player must then place on the board. A player wins by placing a piece on the board which forms a horizontal, vertical, or diagonal row of four pieces, all of which have a common attribute (all short, all circular, etc.). |

[TOC]

## Make and run (todo)

-- Description of the makefile --

## Description of the implementation 

We describe the theory and the practical implementation in this paragraph.  The implementation is done in C with the following macros and standard libraries: 

``` c
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h> 

#define NB_PCE 16
#define NB_SQR 16
#define NB_WIN_POS 10
```

## The pieces

First we describe how we consider a piece of **Quarto**. Basically, a piece is represented by a 4-tuple of boolean value. 

#### Representation

A piece is represented by a 4 bits number. So we have $P=b_32^3+b_22^2+b_12+b_0$ with $b_i\in\{0,1\}$. We use the following convention: 

| Properties        | Corresponding bit | Value for default |
| ----------------- | ----------------- | ----------------- |
| Red / White       | $b_3$             | 1 / 0             |
| Tall / Short      | $b_2$             | 1 / 0             |
| Square / Circular | $b_1$             | 1 / 0             |
| Hollow / Solid    | $b_0$             | 1 / 0             |

For example: 

* The piece which is (White, Tall, Circular, Hollow) is the number $(0101)_2=5$,
* The piece which is (Red, Tall, Circular, Solid) is the number $(1100)_2=12$,

In our program, we have to use 8 bits numbers as there does not exists smaller types. 

``` c
typedef uint8_t piece_t; 
const piece_t pce_list[NB_PCE] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF
};

void print_pce(piece_t p0) {
    assert((p0 >= 0) && (p0 <= 0xF)); 
    uint8_t b3 = p0 & 0x8; uint8_t b2 = p0 & 0x4; 
    uint8_t b1 = p0 & 0x2; uint8_t b0 = p0 & 0x1; 
    if (b3 > 0) {printf("(Red, ");} else {printf("(White, ");}
    if (b2 > 0) {printf("Tall, ");} else {printf("Short, ");}
    if (b1 > 0) {printf("Square, ");} else {printf("Circular, ");}
    if (b0 > 0) {printf("Hollow)\n");} else {printf("Solid)\n");}
}
```

#### Test for common point

Let $P_1$ and $P_2$ be two pieces. Then we can test if they have a common trait with the **XNOR ** = $\odot$ operation whose truth table is: 

| $A$  | $B$  | $A\odot B$ |
| :--: | :--: | :--------: |
|  0   |  0   |     1      |
|  0   |  1   |     0      |
|  1   |  0   |     0      |
|  1   |  1   |     1      |

For instance consider: $P_1=$ (White, Tall, Circular, Hollow) and $P_2=$ (Red, Tall, Circular, Solid) . One can see that they have two common points. Let us see how this reflect under the XNOR properties: 
$$
P_1\odot P_2 = (0101)_2\odot(1100)_2=(0110)_2.
$$
Hence two pieces have a common point if and only if $P_1\odot P_2 > 0$. 

Due to our implementation in C, the bitwise XNOR operator has to be built from other bitwise operators, one may compute that  XNOR($P_1$, $P_2$) =  NOT(XOR($P_1$, $P_2$) ). 

Moreover in our implementation the pieces are 8 bits number with the four first bits set to zero , then the NOT operation will flip those bits to and we will always get a positive result. There are two way to deal with this problem. The first one is to test if the result is greater than $(F0)_{16}=240$ , the other solution consists in anding the results with $(0F)_{16}$.  In our test the first solution seems to have less assembler instructions therefor it is the one that we present. 

```c
// return the common traits between two pieces - implementation of the XNOR operator on 4 bits numbers 
uint8_t common2(piece_t p1, piece_t p2) {
    assert((p1 >= 0) && (p1 <= 0xF));
    assert((p2 >= 0) && (p2 <= 0xF));
    return ((~(p1 ^ p2)) & 0xF);
}
```

To test if four pieces $(P_1,P_2,P_3,P_4)$ have a common trait we have to be a little bit careful. We test if $P_1$ and $P_2$ have a common trait and then we have to 
$$
(P_1\odot P_2)\land(P_2\odot P_3)\land(P_3\odot P_4) >0
$$
In C, we explicitly code this operation and we __and__ the result with $(F)_{16}$ in order to get only the last four bits.  

```c
// Return true if the four pieces have at least a common property
bool common4(piece_t p1, piece_t p2, piece_t p3, piece_t p4) {
    assert((p1 >= 0) && (p1 <= 0xF)); assert((p2 >= 0) && (p2 <= 0xF));
    assert((p3 >= 0) && (p3 <= 0xF)); assert((p4 >= 0) && (p4 <= 0xF));
    // return (common2(p1, p2) & common2(p2, p3) & common2(p3, p4)) > 0; 
    return ((~(p1 ^ p2) & ~(p2 ^ p3) & ~(p3 ^ p4)) & 0xF) > 0;
}
```

##### A bit of mathematics

As an exercise we compute the probability that two or four random chosen (the same piece can be picked multiple times) pieces have a common traits. Let us denote four pieces by the four bits that compose their number : $P_1=(a_3,a_2,a_1,a_0)$, $P_2=(b_3,b_2,b_1,b_0)$, $P_3=(c_3,c_2,c_1,c_0)$, and $P_4=(d_3,d_2,d_1,d_0)$. Each can digit can be seen as a random variable that takes its values in $\{0,1\}$.  Therefore, 
$$
P(\text{common}(P_1,P_2))=P\left(\bigcup_{i=0}^3(a_i=b_i)\right)=1-P\left(\bigcap_{i=0}^3(a_i\neq b_i)\right)=1-\prod_{i=0}^3P\left(a_i\neq b_i\right) = \frac{15}{16}.
$$


Moreover, 
$$
\begin{align}
P(\text{common}(P_1,P_2, P_3,P_4))&=P\left(\bigcup_{i=0}^3(a_i=b_i)\cap(a_i=c_i)\cap(a_i=d_i)\right)\\
&= 1-\prod_{i=0}^3P((a_i\neq b_i)\cup(a_i\neq c_i)\cup(a_i\neq d_i))\\
&= 1-\prod_{i=0}^3\frac{7}{8} = \frac{1695}{4096}\approx 41.38. 
\end{align}
$$
We check those with our code. 

```c
void prob2() {     
    int nb_trial = 1000000000; 
    int nb_success = 0; 
	for (int k = 0; k < nb_trial; k++) {
        uint8_t pce_nb1 = rand() % 16; uint8_t pce_nb2 = rand() % 16;
        piece_t p1 = pce_list[pce_nb1]; piece_t p2 = pce_list[pce_nb2]; 
        if (common2(p1, p2) > 0) {
            nb_success++;
        } 
    }
    printf("Prob2 (success / total): %d / %d.\n", nb_success, nb_trial); 
}
```

and, 

```c
void prob4() {
    int nb_trial = 1000000000; 
    int nb_success = 0; 
	for (int k = 0; k < nb_trial; k++) {
        uint8_t pce_nb1 = rand() % 16; uint8_t pce_nb2 = rand() % 16;
        uint8_t pce_nb3 = rand() % 16; uint8_t pce_nb4 = rand() % 16;
        piece_t p1 = pce_list[pce_nb1]; piece_t p2 = pce_list[pce_nb2]; 
        piece_t p3 = pce_list[pce_nb3]; piece_t p4 = pce_list[pce_nb4]; 
        if (common4(p1,p2,p3,p4)) {
            nb_success++;
        }
    }
    printf("Prob4 (success / total): %d / %d.\n", nb_success, nb_trial); 
}
```

and the output is: 

> Prob2 (success / total) : 937503664 / 1000000000 
>
> Prob4 (success / total) : 413813072 / 1000000000

### The board

The board is represented by 16 cases, numbered from 0 to 15, 

| $15$ | $14$ | $13$ | $12$ |
| :--: | :--: | :--: | :--: |
| $11$ | $10$ | $9$  | $8$  |
| $7$  | $6$  | $5$  | $4$  |
| $3$  | $2$  | $1$  | $0$  |

and by the pieces that are not yet set on the board.  

```c
typedef uint8_t square_t; 
```

For each square we need to know if there is a piece on it and if so, which piece it is.  Therefore our first representation of a Board in C is given by: 

```c
typedef struct {
	bool    sqrs[NB_SQR];      // true if the square is occupied,
    piece_t pces[NB_SQR];      // the piece of a given square,
    bool    pces_left[NB_PCE]; // true if the pieces is not on the board
} board_t;

// initialisation function 
void init_brd(board_t * brd) {
    for (int pce = 0; pce < NB_PCE; pce++) {
        brd->pces_left[pce] = true; 
    }
    for (int sq = 0; sq < NB_SQR; sq++) {
        brd->sqrs[sq] = false; 
        brd->pces[sq] = 0x0; // not useful but for completness. 
    }
}
```

Let's do a bit combinatorics, there are 16 square on the board and 16 pieces. Thus we have $16!\approx 2\cdot 10^{13}$ possible boards. However two board can represent the same position under the following transformations: 

* Rotation by $0°$,  $90°$, $180°$ or $270°$, 
* Symmetry about the vertical and the horizontal axis, 
* Flip around both diagonals, 
* Swapping at least one property: i.e. all Red becomes White, or, all Tall becomes Short .... 

The first 3 transformations span the dihedral group $D_4$ of all symmetries of a square. The fourth type of symmetry imply that each board configuration can be transformed in 16 distinct configurations. 

Since the order of $D_4$ is $8$ we have then each board is equivalent to $16*8=128$ configurations. Therefore, there is at most $\frac{16!}{128}\approx1,6\cdot10^{11}$ unique final configurations of board. Of course among all this configuration there are a lot which are won some moves before getting to the full board. 

#### Wining position on the board

Based on our definition of the board, we can store all winning combination on the board in a double array. 

```c
const square_t winning_positions[NB_WIN_POS][4] = {
    {3, 2, 1, 0}, {7, 6, 5, 4}, {11, 10, 9, 8}, {15, 14, 13, 12}, {15, 11, 7, 3}, {14, 10, 6, 2}, {13, 9, 5, 1}, {12, 8, 4, 0}, {15, 10, 5, 0}, {12, 9, 6, 3}
};
```

This definition allow us to extend the number of winning positions. We can now easily test if there is at least one winning position on this board.

```c
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
```

#### Generate all boards (work in progress)

Starting from a board, generate all boards. 

```c
uint64_t nb_brd = 0; 
uint64_t nb_brd_win = 0; 
void gen_boards(board_t * brd) {
    for (int pce = 0; pce < NB_SQR; pce++) {
        if (brd->set[pce] == false) {
            for (int sq = 0; sq < NB_SQR; sq++){
                nb_brd++; 
                brd->set[pce] = true; 
                brd->pieces[sq] = pce_list[pce]; 
                gen_boards(brd); 
            }
        }
    }
    if (win(brd)) {
        nb_brd_win++; 
    }
    return; 
}
```

#### Unique identifier of the board(work in progress)

The representant of a board in its transformation class is chosen like this:

1. The piece (White, Short, Circular, Solid) = $0$ is in the square $0$, this fixes all transformation except flip around diagonal, 
2. The piece in the square $1$ is greater than the piece in the square $4$, 

Then a unique id can be stored as a 64 bits number (in fact 60 is sufficient since the four last bits will always be 0) given by
$$
K(\bar{B})=\sum_{s=0}^{15}P[k]4^k
$$
where $\bar{B}$ is a normalized board and $P$ is its piece list.

So we complete the board type with a 64 bit field to store its unique identifier, this might be useful when exploring the game tree to avoid exploring several times the same position. 

```c
typedef uint64_t key_t; 
typedef struct {
	bool    set[16]; 
    piece_t pieces[16]; 
    key_t   key; 
} board_t;

void compute_id(board_t * brd) {
    piece_t new_pces[NB_SQR]; 
    // 1 . Normalise the board
    piece_t pce0 = brd->pieces[0]; 
    // a. set the piece 0 in the square 0
    for (int sq = 0; sq < NB_SQR; sq++) {
        new_pces[sq] = 0x0F & (brd->pieces[sq] ^ pce0); 
    }
    // b. if necessary swap the pieces around the diagonal
    if (new_pces[4] > new_pces[1]) {
        piece_t tmp; 
        tmp = new_pces[4]; new_pces[4] = new_pces[1]; new_pces[1] = tmp; // swap sq 4 and 1
        tmp = new_pces[8]; new_pces[8] = new_pces[2]; new_pces[2] = tmp; // swap sq 8 and 2
        tmp = new_pces[6]; new_pces[9] = new_pces[6]; new_pces[6] = tmp; // swap sq 9 and 6
        tmp = new_pces[12]; new_pces[12] = new_pces[3]; new_pces[3] = tmp; // swap sq 12 and 3
        tmp = new_pces[13]; new_pces[13] = new_pces[7]; new_pces[7] = tmp; // swap sq 13 and 7
        tmp = new_pces[14]; new_pces[14] = new_pces[11]; new_pces[11] = tmp; // swap sq 14 and 11
    }
    // 2. Compute the key
    key_t key = 0; 
    for (int sq = 0; sq < NB_SQR; sq++) {
        key += (new_pces[sq] << 4*sq);
    }
    brd->key = key; 
}
```

During a game, when all pieces are not set,  we can compute a unique identifier in almost the same fashion. To make this work properly, observe that two boards can be equivalent if and only if the number of pieces on the two board is the same. Since the number of piece on the board is equal to the current ply we can use this in the key to identify board. Then the idea is to compute the key just like before and we add the ply to the four first bits.  
$$
K(\bar{B})=(\bar{B}.N\mod 16) + \sum_{s=1}^{15}S[k]P[k]4^k
$$

where $S[k]=1$ if a piece is on this square. 

```c
typedef uint64_t key_t; 
typedef struct {
	bool    set[16]; 
    piece_t pieces[16]; 
    uint8_t ply; 
    key_t   key; 
} board_t;

key_t compute_id(board_t * brd) {
    // 1 . Normalise the board
    
    // 2. Compute the key
    key_t key = 0; 
    
    return key;
}
```

### Moves

This is the most difficult part of this project. In **Quarto**, a game consists in the following table, 

| Ply  | Player   | Move                                                   | Nb of piece remaining before the action of player |
| ---- | -------- | ------------------------------------------------------ | :-----------------------------------------------: |
| init | Player A | Choose a piece (we can always assume the piece is 0x0) |                        16                         |
| 0    | Player B | - Place the piece<br />- Choose a Piece                |                        15                         |
| 1    | Player A | - Place the piece<br />- Choose a Piece                |                        14                         |
| ...  |          |                                                        |                                                   |
| 14   | Player B | - Place the piece<br />- Choose a Piece                |                         1                         |
| 15   | Player A | Place the piece in the only remaining square           |                         0                         |

What can we learn fro this array. First we see that before ply 4 there is no way to have a winning position. So our research must go at least to depth 4 in order to get a chance to have a winning position. 

Secondly we see that a game consists in at most 17 successive actions. 

```c
typedef struct {
    square_t sqr; // where to put the piece my opponent choose
	uint8_t  pce; // piece selected for the next player
} move_t; 
```

We see that we have to update our definition of a board by keeping in memory which pieces should we place, and the ply the game is. 

``` c
typedef struct {
    /* Basic Board representation */
	bool    sqrs[NB_SQR];      // true if the square is occupied,
    piece_t pces[NB_SQR];      // the piece of a given square,
    bool    pces_left[NB_PCE]; // true if the pieces is not on the board
    
    /* Advanced board settings for automated game */
    uint8_t ply;     // the ply can range from 0 (empty board) to 17 (full board)
    piece_t nextPce; // number of the piece that the next player should place on the board
    brdkey_t key;    // for later 
    
    /* Recording of the previous state */
    history_t game[17]; 
} board_t;

// initialisation function 
void init_brd(board_t * brd) {
    for (int sq = 0; sq < NB_SQR; sq++) {
        brd->sqrs[sq] = false; 
        brd->pces[sq] = 0x0; // not useful but for completness. 
    }
    for (int pce = 0; pce < NB_PCE; pce++) {
        brd->pces_left[pce] = true; 
    }
    brd->ply = 0; 
    brd->selected_pce_nb = 0;     
}
```



#### History

Before doing a move we need to keep track of the history of a game, this will be helpful. 

```c
typedef struct {
	uint8_t  previous_selected_pce_nb; 
    square_t square_played; 
} history_t; 
```



#### Do a move

``` c
void do_move(board_t * brd, move_t mv) {
    if (brd->ply == 0) {
        brd->nextPce = mv.pce; 
        brd->ply++; 
        return;
    }
    if (brd->ply == 16) {
        brd->sqrs[mv.sqr] = true; 
        brd->pces[mv.sqr] = brd->nextPce; 
        brd->ply++; 
        return; 
    }
    brd->sqrs[mv.sqr] = true; 
    brd->pces[mv.sqr] = brd->nextPce; 
    brd->nextPce = mv.pce;
    brd->ply++; 
}
```



#### Undo a move

#### Generate all moves

```c
typedef struct {
    move_t moves[256]; // number of max moves = 16*15 = 240
    unsigned int cnt; 
} movelist_t;

void movegen(board_t * brd, movelist_t * lst) {
    lst->cnt = 0; 
    // generate first move
    if (brd->ply == 0) {
        for (int pce = 0; pce < NB_PCE; pce++) {
            lst->moves[lst->cnt].pce = pce_list[pce];  
            lst->cnt++;
        }
        return; 
    }
    // generate last move
    if (brd->ply == 16) {
        for (int sq = 0; sq < NB_SQR; sq++) {
            if (brd->sqrs[sq] == false) {
                lst->moves[lst->cnt].sqr = sq;  
                lst->cnt++;
            }
        }
        assert(lst->cnt == 1);
        return; 
    }
    
    // generate regular moves
    for (int pce = 0; pce < NB_PCE; pce++) {
        if (brd->pces_left[pce] == true) {
            for (int sq = 0; sq < NB_SQR; sq++) {
                if (brd->sqrs[sq] == false) {
                    lst->moves[lst->cnt].pce = pce; 
                    lst->moves[lst->cnt].sqr = sq; 
                    lst->cnt++;
                }
            }
        }
    }
}

```

## Game

### random game 

```c
uint32_t total = 0; uint32_t total_pA = 0; uint32_t total_pB = 0; 
uint32_t ply_sum = 0; 

void randomGame(board_t * brd, movelist_t * lst) { 
    while ((!win(brd)) && (brd->ply != 17)) {
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

int main() {
    srand(time(NULL));
    board_t brd; movelist_t lst; 

    int trial = 10000000; 
    for (int k=0; k<trial; k++) {
        init_brd(&brd);
        randomGame(&brd, &lst); 
    }
    printf("average ply : %.3f ", ply_sum / (1. * trial));
    printf("\nTotA: %d\t TotB: %d\t nb win / total : %d / %d \n ", total_pA, total_pB, total, trial);
    return 0; 
}
```

The output is, 

```bash
average ply : 11.922 
TotA: 4993046    TotB: 4965087   nb win / total : 9958133 / 10000000 
```

Based on this experiment, there is a 99% chance that a game ended by win with playing random. The average length of a game is about 12 plies. The first player has slightly more chance to win 50,14% while player B has 49,86%. 

If we add 9 winning position given by all $2$ by $2$ square on the board:

```bash
average ply : 10.912 
TotA: 5001150    TotB: 4993861   nb win / total : 9995011 / 10000000 
```

It is a bit surprising to not have better results? 

### Explore the game

```c
uint32_t nb_nds = 0; uint32_t fin_nds = 0; 
int explore(board_t * brd, int depth) {
    if (depth <= 0) {
        fin_nds++; 
        return 0; 
    }
    movelist_t lst; 
    movegen(brd, &lst); 
    for (int k = 0; k < lst.cnt; k++) {
        nb_nds++; 
        do_move(brd, lst.moves[k]); 
        explore(brd, depth-1); 
        undo_move(brd); 
    }
    return 0; 
}
int main() {
    srand(time(NULL));
    board_t brd;
	init_brd(&brd);
    explore(&brd, 1); 
    printf("Number of node explored: %d\t final nodes: %d\n", nb_nds, fin_nds); 
    return 0; 
}
```

We get the following results: 

| Depth |      Theoretical nodes       | Terminal nodes | Total nodes |
| :---: | :--------------------------: | :------------: | :---------: |
|  $1$  |             $16$             |      $16$      |    $16$     |
|  $2$  |          $16+16^3$           |     $4096$     |   $4112$    |
|   3   | $16+16^3+16^3\cdot16\cdot15$ |    $983040$    |  $987152$   |
|   4   |    $16+16^3+16^4\cdot15+$    |                |             |
|       |                              |                |             |

## Parallel computing 

I used openmp. On the random game function first, on 1000000 games

en mode débug: 

| Depth | Sequential | Parallel (2) | Parallel (4) |
| :---: | :--------: | :----------: | :----------: |
|   1   |  0,199 ms  |    0,118     |              |
|   2   |   14 ms    |    0,010     |              |
|   3   |   62 ms    |              |              |
|   4   |   565 ms   |              |              |
|   5   |  5,729 ms  |              |              |
|   6   | 93,006 ms  |              |              |

en mode optim: 

| Depth | Sequential | Parallel (2) | Parallel (4) |      |
| :---: | :--------: | :----------: | :----------: | ---- |
|   1   |    0 ms    |     0 ms     |     0 ms     |      |
|   2   |    2 ms    |     2 ms     |     4 ms     |      |
|   3   |   23 ms    |    17 ms     |    20 ms     |      |
|   4   |   182 ms   |    172 ms    |    20 7ms    |      |
|   5   |  2,297 ms  |   1,349 ms   |    973 ms    |      |
|   6   | 28,108 ms  |  25,133 ms   |  31,465 ms   |      |
|   7   |            |              |              |      |
|   8   |            |              |              |      |
|   9   |            |              |              |      |