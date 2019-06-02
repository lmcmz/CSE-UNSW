/*********************************************************
 *  servt9.c
 *  Nine-Board Tic-Tac-Toe Server
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/tcp.h>
#include <unistd.h>

#include "common.h"
#include "game.h"

#define  MAX_MOVE              81

FILE *agent_in[2];
FILE *agent_out[2];
int   agent_fd[2];
int  msec_left[2];
int   is_human[2]={FALSE,FALSE};

  // allow 30 secons initially, plus 2 seconds for each move
int seconds_initially = 30;
int seconds_per_move  =  2;


/*********************************************************//*
   Write message to specified player
*/
void write_agent(int i, char *str)
{
  if( !is_human[i] ) {
    fprintf(agent_in[i],"%s",str);
    fflush(agent_in[i]);
  }
}

/*********************************************************//*
   Write message to both players
*/
void write_all(char *str)
{
  int i;
  for(i = 0; i < 2; i++){
    write_agent(i, str);
  }
}

/*********************************************************//*
   Set up network connection(s)
*/
void server_init( int port )
{
  int i, slen, client, server;
  
  struct sockaddr_in servAddr;

  // create socket
  server = socket(AF_INET, SOCK_STREAM, 0);
  if( server < 0 ) {
    perror("cannot open socket ");
    exit(1);
  }    

  // server settings
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(port);

  // bind server port
  if(bind(server, (struct sockaddr *)&servAddr, sizeof(servAddr))<0) {
    perror("cannot bind port ");
    exit(1);
  }

  slen = sizeof(servAddr);
  if(getsockname(server,(struct sockaddr *)&servAddr,(socklen_t*)&slen) != 0) {
    perror("error retriving port number");
    exit(1);
  }
  printf("Connecting to port %d\n", ntohs(servAddr.sin_port));

  // server listen
  if(listen(server, 5) != 0) {
    perror("cannot listen ");
    exit(1);
  }
  
  // accept client connections
  for(i = 0; i < 2; i++) {
    if( !is_human[i] ) {
      int tcp_no_delay = 1;
      client = accept(server, NULL, NULL);
      if( client < 0 ) {
        perror("cannot accept connection ");
        return exit(1);
      }
      if(setsockopt(client, IPPROTO_TCP, TCP_NODELAY,
         (char *)&tcp_no_delay, sizeof(tcp_no_delay)) < 0) {
        perror ("tcpecho: TCP_NODELAY options");
        exit(1);
      }

      agent_fd[i]  = client;
      agent_in[i]  = fdopen(client,"w");
      agent_out[i] = fdopen(client,"r");
    }
  }
  printf("\n");

  close(server);

  write_all("init.\n");
}

/*********************************************************//*
   Print board and allow human player to enter next move
*/
int human_step(
               int player,
               int m,
               int move[],
               int board[10][10]
              )
{
  char line[256];
  int c=0,i;
  print_board( stdout,board,move[m-2],move[m-1] );
  while( c == 0 && !feof(stdin)) {
    printf("next move for %c ? ",sb[player]);
    fgets(line,256,stdin);
    i = 0;
    while( i < 256 && line[i] != '\0'
          &&( line[i] < '1' || line[i] > '9' ))
      i++;
    if( i < 256 && line[i] != '\0' ) {
      c = line[i] - '0';
      if( board[move[m-1]][c] != EMPTY ) {
        c = 0;
      }
    }
  }
  move[m] = c;
  return( make_move( player,m,move,board ));
}

/*********************************************************//*
   Invite computer player to send next move
*/
int server_step(
                int player,
                int m,
                int move[],
                int board[10][10]
               )
{
  int game_status;
  struct timeval tv;
  struct timeval tod_start, tod_fin;
  int move_msec;
  int move_scanned;
  fd_set fds;
  int i;
  if ( m == 2 ) { // second move
    fprintf(agent_in[player],"second_move(%d,%d).\n",
            move[0],move[1]);
  }
  else if( m == 3 ) { // third move
    fprintf(agent_in[player],"third_move(%d,%d,%d).\n",
            move[0],move[1],move[2]);
  }
  else {
    fprintf(agent_in[player],"next_move(%d).\n",move[m-1]);
  }
  fflush(agent_in[player]);

  FD_ZERO(&fds);
  FD_SET(agent_fd[player], &fds);

  msec_left[player] += 1000 * seconds_per_move;

  memset(&tv, 0, sizeof(struct timeval));
  tv.tv_sec = 1 + msec_left[player]/1000;

  gettimeofday( &tod_start, NULL );
  i = select(agent_fd[player] + 1, &fds, NULL, NULL, &tv);
  if( i > 0 ) {
    move_scanned = fscanf(agent_out[player], "%d", &move[m]);
    gettimeofday( &tod_fin, NULL );
    move_msec = 1 + (tod_fin.tv_sec -tod_start.tv_sec )*1000
                  + (tod_fin.tv_usec-tod_start.tv_usec)/1000;
    msec_left[player] -= move_msec;
    if( move_scanned ) {
      game_status = make_move( player,m,move,board );
    }
    else {
      game_status = TIMEOUT;
    }
  }
  else {
    game_status = TIMEOUT;
  }
  if(( msec_left[player] < 0 )&&( game_status == STILL_PLAYING) ) {
    game_status = TIMEOUT;
  }
  return( game_status );
}

/*********************************************************//*
   Play a series of games
*/
void play_games( int num_games, int move[] )
{
  int board[10][10];
  int game_status;
  int player, first_player;
  int game;
  int m;

  first_player = 0;

  for( game=0; game < num_games; game++ ) {
    reset_board( board );

    write_agent( first_player,"start(x).\n");
    write_agent(!first_player,"start(o).\n");

    msec_left[0] = 1000*(seconds_initially - seconds_per_move);
    msec_left[1] = 1000*(seconds_initially - seconds_per_move);

    if( game > 0 || move[0] == 0 ) {// choose first move randomly
      move[0] = 1 + random()% 9;
      move[1] = 1 + random()% 9;
    }
    m = 1;
    player = first_player;
    game_status = make_move( player,m,move,board );
    while( m < MAX_MOVE && game_status == STILL_PLAYING ) {
      //print_board( stdout,board,move[m-1],move[m] );
      m++;
      player = !player;
      if( is_human[player] ) {
        game_status =  human_step( player,m,move,board );
      }
      else {
        game_status = server_step( player,m,move,board );
      }
    }
    if(!is_human[!player] &&(game_status == WIN || game_status == DRAW)){
      fprintf(agent_in[!player],"last_move(%d).\n",move[m]);
    }

    print_board( stdout,board,move[m-1],move[m] );

    if( game_status == WIN ) {
      write_agent(  player, "win(triple).\n" );
      write_agent( !player,"loss(triple).\n" );
      printf("Player %c wins (triple)\n", sb[player]);
    }
    else if( game_status == DRAW ) {
      write_all("draw(full_board).\n" );
      printf( "draw (full_board)\n" );
    }
    else if( game_status == ILLEGAL_MOVE ) {
      write_agent(  player,"loss(illegal_move).\n" );
      write_agent( !player, "win(illegal_move).\n" );
      printf("Player %c wins (illegal_move)\n",sb[!player]);
    }
    else if( game_status == TIMEOUT ) {
      write_agent(  player,"loss(timeout).\n" );
      write_agent( !player, "win(timeout).\n" );
      printf( "Player %c wins (timeout)\n", sb[!player]);
    }
  }
  printf( "\n" );
}

/*********************************************************//*
   Close the network connection
*/
void cleanup()
{
  int i;

  for( i = 0; i < 2; i++) {
    if( !is_human[i] ) {
      write_agent(i, "end.\n");
      fclose(agent_in[i]);
      fclose(agent_out[i]);
      close(agent_fd[i]);
    }
  }
}    

/*********************************************************//*
   Print usage information and exit
*/
void usage( char argv0[] )
{
  printf("Usage: %s\n",argv0);
  printf("       [-x] [-o]\n");        // human plays X or O
  printf("       [-p port]\n");        // tcp port
  printf("       [-m board square]\n");// specify first move
  // number of seconds allocated initially, and per move
  printf("       [-t initial permove]\n");
  printf("       [-n num_games]\n");   // number of games
  exit(1);
}

/*********************************************************/
int main(int argc, char *argv[])
{
  struct timeval tp;
  int move[MAX_MOVE+1]={0};
  int port=31415;
  int num_games=1;
  int i=1;

  while( i < argc ) {
    if( strcmp( argv[i], "-p" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      port = atoi(argv[i+1]);
      i += 2;
    }
    else if( strcmp( argv[i], "-x" ) == 0 ) {
      is_human[0] = TRUE;
      i++;
    }
    else if( strcmp( argv[i], "-o" ) == 0 ) {
      is_human[1] = TRUE;
      i++;
    }
    else if( strcmp( argv[i], "-m" ) == 0 ) {
      if( i+2 >= argc ) {
        usage( argv[0] );
      }
      move[0] = atoi(argv[i+1]);
      move[1] = atoi(argv[i+2]);
      if(   move[0] < 1 || move[0] > 9
         || move[1] < 1 || move[1] > 9 ) {
        usage( argv[0] );
      }
      i += 3;
    }
    else if( strcmp( argv[i], "-t" ) == 0 ) {
      if( i+2 >= argc ) {
        usage( argv[0] );
      }
      seconds_initially = atoi(argv[i+1]);
      seconds_per_move  = atoi(argv[i+2]);
      if(   seconds_initially <= 0
         || seconds_per_move  <  0 ) {
        usage( argv[0] );
      }
      i += 3;
    }
    else if( strcmp( argv[i], "-n" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      num_games = atoi(argv[i+1]);
      i += 2;
    }
    else {
      usage( argv[0] );
    }
  }

  // generate a new random seed each time
  gettimeofday( &tp, NULL );
  srandom(( unsigned int )( tp.tv_usec ));

  if( !is_human[0] || !is_human[1] ) {
    server_init( port );
  }
  play_games( num_games,move );

  cleanup();

  return 0;
}
