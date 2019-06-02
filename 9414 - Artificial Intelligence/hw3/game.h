/*********************************************************
 *  game.h
 *  Nine-Board Tic-Tac-Toe Game Engine
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
void reset_board( int board[10][10] );
int   full_board( int bb[] );
void print_board( FILE *fp,int board[10][10],
		  int board_num,int prev_move );
int make_move(int player,int m,int move[],int board[10][10]);
