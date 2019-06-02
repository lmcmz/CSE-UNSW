/*********************************************************
 *  common.h
 *  Nine-Board Tic-Tac-Toe
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#define FALSE          0
#define TRUE           1

#define EMPTY          2

// game status or result
#define ILLEGAL_MOVE   0
#define STILL_PLAYING  1
#define WIN            2
#define LOSS           3
#define DRAW           4
#define TRIPLE         5
#define TIMEOUT        6
#define FULL_BOARD     7

extern char sb[5];
extern int  sg[2];
