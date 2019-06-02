/*********************************************************
 *  client.c
 *  Nine-Board Tic-Tac-Toe Client
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>

#include "common.h"
#include "agent.h"

int   port=31415;
char *local="localhost";
char *host;

int pipe_fd;

FILE* pipe_in_stream;
FILE* pipe_out_stream;

char client_buf[256];

/*********************************************************//*
   Close the network connection
*/
void client_cleanup()
{
  agent_cleanup();
  fclose(pipe_in_stream);
  fclose(pipe_out_stream);
  close(pipe_fd);
  exit(0);// must exit immediately, to avoid "zombie" process
}

/*********************************************************//*
   Read from pipe input stream to buffer
*/
void pipe_read(char *buf)
{
  fflush(pipe_in_stream);

  buf[0] = '\0';
  fscanf(pipe_in_stream, "%s", buf);

  if( buf[0] == '\0' ) {
    client_cleanup();
  }
}

/*********************************************************//*
   Get second move from agent and write it to output stream
*/
void client_second_move( int board_num, int prev_move )
{
  int this_move;
  this_move = agent_second_move( board_num,prev_move );
  fprintf(pipe_out_stream, "%d\n",this_move);
  fflush(pipe_out_stream);
}

/*********************************************************//*
   Get third move from agent and write it to output stream
*/
void client_third_move(
                       int board_num,
                       int first_move,
                       int prev_move
                      )
{
  int this_move;
  this_move=agent_third_move(board_num,first_move,prev_move);
  fprintf(pipe_out_stream, "%d\n",this_move);
  fflush(pipe_out_stream);
}

/*********************************************************//*
   Get next move from agent and write it to output stream
*/
void client_next_move( int prev_move )
{
  int this_move;
  this_move = agent_next_move( prev_move );
  fprintf(pipe_out_stream,"%d\n",this_move);
  fflush(pipe_out_stream);
}

/*********************************************************//*
   Set up tcp connection
*/
int tcpopen()
{   
  int sd, rc;
  struct hostent *h;
  struct sockaddr_in servAddr; // localAddr

  h = (struct hostent *)gethostbyname(host);
  if(h==NULL) {
    printf("unknown host '%s'\n",host);
    exit(1);
  }
  servAddr.sin_family = h->h_addrtype;
  memcpy((char *)&servAddr.sin_addr.s_addr,
         h->h_addr_list[0],h->h_length );
  servAddr.sin_port = htons(port);

  // create socket
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd<0) {
    perror("cannot open socket ");
    exit(1);
  }
  int tcp_no_delay = 1;
  if(setsockopt(sd, IPPROTO_TCP, TCP_NODELAY,
          (char *)&tcp_no_delay, sizeof(tcp_no_delay)) < 0) {
    perror ("tcpecho: TCP_NODELAY options");
    exit(1);
  }

  // connect to server
  rc=connect(sd,(struct sockaddr *)&servAddr,sizeof(servAddr));
  if(rc<0) {
    perror("cannot connect ");
    exit(1);
  }

  return sd;
}

/*********************************************************//*
   Determine cause for win, loss or draw
*/
int get_cause( char *buf )
{
  int cause=TRIPLE;
  if( strcmp(client_buf,"triple).") == 0) {
    cause = TRIPLE;
  }
  else if( strcmp(client_buf,"timeout).") == 0) {
    cause = TIMEOUT;
  }
  else if( strcmp(client_buf,"illegal_move).") == 0) {
    cause = ILLEGAL_MOVE;
  }
  else if( strcmp(client_buf,"full_board).") == 0) {
    cause = FULL_BOARD;
  }
  return( cause );
}

/*********************************************************/
int main(int argc, char** argv)
{
  int player=0;
  int result=DRAW;  // WIN, LOSS or DRAW
  int cause =TRIPLE;// TRIPLE, TIMEOUT, ILLEGAL_MOVE or FULL_BOARD
  int board_num,first_move,prev_move;
  int sd;
  char ch;

  host = local; // default
  agent_parse_args( argc, argv );

  sd = tcpopen(); // host,port );

  pipe_fd = sd;
  pipe_in_stream  = fdopen(sd,"r");
  pipe_out_stream = fdopen(sd,"w");

  client_buf[0] = '\0';
  pipe_read(client_buf);

  while( TRUE ) {    
    if(strcmp(client_buf,"init.") == 0) {
      agent_init();
    }
    else if(sscanf(client_buf,"start(%c).",&ch) == 1) {
      player = ( ch == 'x' ) ? 0 : 1 ;
      agent_start( player );
    }
    else if(sscanf(client_buf,"second_move(%d,%d).",
                   &board_num,&prev_move) == 2 ) {
      client_second_move( board_num,prev_move );
    }
    else if(sscanf(client_buf,"third_move(%d,%d,%d).",
                   &board_num,&first_move,&prev_move) == 3) {
      client_third_move( board_num,first_move,prev_move );
    }
    else if(sscanf(client_buf,"next_move(%d).",&prev_move)==1){
      client_next_move( prev_move );
    }
    else if(sscanf(client_buf,"last_move(%d).",&prev_move)==1){
      agent_last_move( prev_move );
    }
    else if(   strcmp(client_buf,"win(") > 0
            && strcmp(client_buf,"win)") < 0 ) {
      result = WIN;
      cause = get_cause(client_buf+4);
      agent_gameover(result,cause);
    }
    else if(   strcmp(client_buf,"loss(") > 0
            && strcmp(client_buf,"loss)") < 0 ) {
      result = LOSS;
      cause = get_cause(client_buf+5);
      agent_gameover(result,cause);
    }
    else if(   strcmp(client_buf,"draw(") > 0
            && strcmp(client_buf,"draw)") < 0 ) {
      result = DRAW;
      cause = get_cause(client_buf+5);
      agent_gameover(result,cause);
    }
    else if(strcmp(client_buf,"end") == 0) {
      client_cleanup();
      return 0;
    }        

    client_buf[0] = '\0';
    pipe_read(client_buf);
  }

  return 0;
}
