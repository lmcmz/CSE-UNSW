/*********************************************************
 *  randt9.c
 *  Nine-Board Tic-Tac-Toe Random Player
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "agent.h"
#include "game.h"

#define MAX_MOVE 81

int board[10][10];
int move[MAX_MOVE+1];
int player;
int m;

/*********************************************************//*
   Print usage information and exit
*/
void usage( char argv0[] )
{
  printf("Usage: %s\n",argv0);
  printf("       [-p port]\n"); // tcp port
  printf("       [-h host]\n"); // tcp host
  exit(1);
}

/*********************************************************//*
   Parse command-line arguments
*/
void agent_parse_args( int argc, char *argv[] )
{
  int i=1;
  while( i < argc ) {
    if( strcmp( argv[i], "-p" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      port = atoi(argv[i+1]);
      i += 2;
    }
    else if( strcmp( argv[i], "-h" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      host = argv[i+1];
      i += 2;
    }
    else {
      usage( argv[0] );
    }
  }
}

/*********************************************************//*
   Called at the beginning of a series of games
*/
void agent_init()
{
  struct timeval tp;

  // generate a new random seed each time
  gettimeofday( &tp, NULL );
  srandom(( unsigned int )( tp.tv_usec ));
}

/*********************************************************//*
   Called at the beginning of each game
*/
void agent_start( int this_player )
{
  reset_board( board );
  m = 0;
  move[m] = 0;
  player = this_player;
}

/*********************************************************//*
   Choose second move and return it
*/
int agent_second_move( int board_num, int prev_move )
{
  int this_move;
  move[0] = board_num;
  move[1] = prev_move;
  board[board_num][prev_move] = !player;
  m = 2;
  do {
    this_move = 1 + random()% 9;
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move;
  board[prev_move][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Choose third move and return it
*/
int agent_third_move(
                     int board_num,
                     int first_move,
                     int prev_move
                    )
{
  int this_move;
  move[0] = board_num;
  move[1] = first_move;
  move[2] = prev_move;
  board[board_num][first_move] =  player;
  board[first_move][prev_move] = !player;
  m=3;
  do {
    this_move = 1 + random()% 9;
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Choose next move and return it
*/
int agent_next_move( int prev_move )
{
  int this_move;
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
  m++;
  do {
    this_move = 1 + random()% 9;
  } while( board[prev_move][this_move] != EMPTY );
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Receive last move and mark it on the board
*/
void agent_last_move( int prev_move )
{
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
}

/*********************************************************//*
   Called after each game
*/
void agent_gameover(
                    int result,// WIN, LOSS or DRAW
                    int cause  // TRIPLE, ILLEGAL_MOVE, TIMEOUT or FULL_BOARD
                   )
{
  // nothing to do here
}

/*********************************************************//*
   Called after the series of games
*/
void agent_cleanup()
{
  // nothing to do here
}
