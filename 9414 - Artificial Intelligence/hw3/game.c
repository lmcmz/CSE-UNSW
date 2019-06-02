/*********************************************************
 *  game.c
 *  Nine-Board Tic-Tac-Toe Game Engine
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <stdio.h>
#include "common.h"
#include "game.h"

char sb[5] =  "XO.xo";
int  sg[2] = {1,-1};

/*********************************************************
   Reset all squares to empty
*/
void reset_board( int board[10][10] )
{
  int b,c;
  for( b = 0; b <= 9; b++ ) {
    for( c = 1; c <= 9; c++ ) {
      board[b][c] = EMPTY;
    }
  }
}

/*********************************************************
   Print one row of the board
*/
void print_board_row(
                     FILE *fp,
                     int bd[10][10],
                     int a, int b, int c,
                     int i, int j, int k
                    )
{
  fprintf(fp," %c %c %c |",sb[bd[a][i]],sb[bd[a][j]],sb[bd[a][k]]);
  fprintf(fp," %c %c %c |",sb[bd[b][i]],sb[bd[b][j]],sb[bd[b][k]]);
  fprintf(fp," %c %c %c\n",sb[bd[c][i]],sb[bd[c][j]],sb[bd[c][k]]);
  fflush(fp);
}

/*********************************************************
   Print the entire board
*/
void print_board(
                 FILE *fp,
                 int board[10][10],
                 int board_num,
                 int prev_move
                )
{
  board[board_num][prev_move] += 3; // upper case
  print_board_row( fp,board,1,2,3,1,2,3 );
  print_board_row( fp,board,1,2,3,4,5,6 );
  print_board_row( fp,board,1,2,3,7,8,9 );
  fprintf(fp," ------+-------+------\n");
  print_board_row( fp,board,4,5,6,1,2,3 );
  print_board_row( fp,board,4,5,6,4,5,6 );
  print_board_row( fp,board,4,5,6,7,8,9 );
  fprintf(fp," ------+-------+------\n");
  print_board_row( fp,board,7,8,9,1,2,3 );
  print_board_row( fp,board,7,8,9,4,5,6 );
  print_board_row( fp,board,7,8,9,7,8,9 );
  fprintf( fp, "\n" );
  board[board_num][prev_move] -= 3; // lower case
}

/*********************************************************
   Return TRUE if the game has been won on this sub-board
*/
int gamewon( int p, int bb[10] )
{
  return(  ( bb[1] == p && bb[2] == p && bb[3] == p )
         ||( bb[4] == p && bb[5] == p && bb[6] == p )
         ||( bb[7] == p && bb[8] == p && bb[9] == p )
         ||( bb[1] == p && bb[4] == p && bb[7] == p )
         ||( bb[2] == p && bb[5] == p && bb[8] == p )
         ||( bb[3] == p && bb[6] == p && bb[9] == p )
         ||( bb[1] == p && bb[5] == p && bb[9] == p )
         ||( bb[3] == p && bb[5] == p && bb[7] == p ));
}

/*********************************************************
   Return TRUE if this sub-board is full
*/
int full_board( int bb[] )
{
  int c=1;
  while( c <= 9 && bb[c] != EMPTY ) {
    c++;
  }
  return( c == 10 );
}

/*********************************************************
   Make specified move on board and return game status
*/
int make_move(
              int player,
              int m,
              int move[],
              int board[10][10]
             )
{
  if( board[move[m-1]][move[m]] != EMPTY ) {
    return( ILLEGAL_MOVE );
  }
  board[move[m-1]][move[m]] = player;

  if( gamewon( player,board[move[m-1]] )) {
    return( WIN );
  }
  if( full_board( board[move[m]] )) {
    return( DRAW );
  }

  return STILL_PLAYING;
}
