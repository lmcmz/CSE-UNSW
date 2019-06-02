/*********************************************************
 *  agent.c
 *  Nine-Board Tic-Tac-Toe Agent
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 *
 *  Team: ❤️💀🤖
 *  Member:  Jingqi Wang - z5169247
 *			 Hao Fu      - z5102511
 */

/*********************************************************
	@Question		Briefly describe how your program works, including any algorithms and data structures employed, 
					and explain any design decisions you made along the way.
					
	
	@Answer			The program use min-max search and alpha-beta purning to find the most optimal step for the player
					The pre-defined step determine how deep the search tree will go
					Heuristic value is used to see if we should choose the step or not for the leaf node
					The way of calulating heuristic value is similar as what we used for Week5 lab
					The detailed data structure and function we used is explained by comments in detail
					
					The idea for winnig the game:
						if one player can win the game by place a token on a certain board, the other player cannot take step which leads to this board
						so in order to win the game, a player should tries to make this kind of board as much as possible.
						And in order to prevent enemy from winning, a player should tried to stop enemy has such a board.
					So by calculating the heuristic value of the nine boards, the larger the sum is, the more winning opportunity we can have.
					Special case:
						if the agent can win by takes a step, he will take that step
						If the agent's enemy can win if the agent takes a step, he will not take that step
						If the agent's enemy can will whichever step the agent takes, the agent will take an random available step 
					
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
#include <limits.h>

#include "common.h"
#include "agent.h"
#include "game.h"

#define MAX_MOVE 81

// Maximum and minimum heuristic value
#define MAX_HEURISTIC 800
#define MIN_HEURISTIC -800

// Macro util function
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int board[10][10];
int move[MAX_MOVE + 1];
int player;
int m;

// Max search layer in our algorithm
int step = 9;

// All win case in Tic Tac Toc
const int win_case[8][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {3, 6, 9}, {1, 4, 7}, {2, 5, 8}, {1, 5, 9}, {3, 5, 7}};

// The heuristic value as Tutorial week 5.
const int heuristic_table[4][4] = {{0, -1, -10, -100},
                                   {1, 0, 0, 0},
                                   {10, 0, 0, 0},
                                   {100, 0, 0, 0}};

/*********************************************************/ /*
	This function can help check if a player have the opportunity to win when it takes to his tern
	
	@parameter 	board_num		  the id of the board waiting for next step
	@parameter 	target_player	  the agent or the enemy (0 or 1) 
	
	@return 	the position which target_player can win, -1 if no such position
*/
int can_win(int board_num, int target_player)
{
    // Assume i is the next move
    for (int i = 1; i < 10; i++)
    {

        // Filter not EMPTY move
        if (board[board_num][i] != EMPTY)
        {
            continue;
        }

        // Assume we move to i
        board[board_num][i] = target_player;

        // Check all win case
        for (int j = 0; j < 8; j++)
        {
            bool win = false;
            // Each case check 3 position
            if (board[board_num][win_case[j][0]] == target_player && board[board_num][win_case[j][1]] == target_player && board[board_num][win_case[j][2]] == target_player)
            {
                win = true;
            }
            // All 3 position is player, win this game
            if (win)
            {
                // Reset move i to EMPTY
                board[board_num][i] = EMPTY;
                return i;
            }
        }

        // Reset move i to EMPTY
        board[board_num][i] = EMPTY;
    }

    return -1; // Can't win
}

/*********************************************************/ /*

  This function can help find heuristic value for the min_max searching algorithm
  The heuristic function is similar to what we learned on week 5 turtorial
  
  @parameter 	current_player	  the agent or the enemy (0 or 1) 
  
  @retrun 		heuristic value of the function 
*/
int get_score(int current_player)
{
    int score = 0;

    // Sum all 9 subBoard heuristic value as final heuristic value
    for (int board_id = 1; board_id <= 9; board_id++)
    {

        // Check all win case
        for (int i = 0; i < 8; i++)
        {
            int mine = 0;
            int oppo = 0;

            // Each win case, calculate the heuristic value in subBoard
            for (int j = 0; j < 3; j++)
            {
                if (board[board_id][win_case[i][j]] == current_player)
                {
                    mine++;
                }
                else if (board[board_id][win_case[i][j]] == !current_player)
                {
                    oppo++;
                }
            }
            // Current subBoard heuristic value
            score += heuristic_table[mine][oppo];
        }
    }
    return score;
}

/*********************************************************/ /*
  This function using min max search to find next move
  Alpha-beta purning is implemented

  @parameter 	next_board		  		  id of the board waiting for next step 
  @parameter 	alpha			  		  alpha value for alpha beta purning 
  @parameter 	beta	  		  		  beta value for alpha beta purning
  @parameter 	remaining_search_step	  destance from current node to the left node of min max search tree
	
  @retrun 		heuristic value of the node 
*/
int min_max_search(int next_board, int alpha, int beta, int remaining_search_step)
{

    int heuristic_value = INT_MIN;
    int temp_score = 0;

    // If it reached the deepest layer, calculate the value.
    if (remaining_search_step == step)
    {
        return get_score(player);
    }

    // If the step is odd, then it is player move, we need maximizing the value
    if (remaining_search_step % 2 != 0)
    {

        // If agent can win in this move, return a maximun heuristic_value in this node
        int result = can_win(next_board, player);
        if (result != -1)
        {
            return MAX_HEURISTIC;
        }

        for (int next_step = 1; next_step <= 9; next_step++)
        {
            if (board[next_board][next_step] != EMPTY)
            {
                continue;
            }

            // Assume move to next_step in board
            board[next_board][next_step] = player;

            // Recursive call minmax in next layer
            temp_score = min_max_search(next_step, alpha, beta, remaining_search_step + 1);

            // Reset the next_step move to EMPTY
            board[next_board][next_step] = EMPTY;

            // Update min heuristic_value
            heuristic_value = MAX(heuristic_value, temp_score);

            // Update alpha
            alpha = MAX(alpha, heuristic_value);

            // Pruning the tree
            if (beta <= alpha)
            {
                break;
            }
        }
    }
    else
    {
        // If the step is even, then it is enemy move, we need minimizing the value
        heuristic_value = INT_MAX;

        // If enemy can win in this move, return a minimun heuristic_value in this node
        int result = can_win(next_board, !player);
        if (result != -1)
        {
            return MIN_HEURISTIC;
        }

        for (int next_step = 1; next_step <= 9; next_step++)
        {
            if (board[next_board][next_step] != EMPTY)
            {
                continue;
            }

            // Assume move to next_step in board
            board[next_board][next_step] = !player;

            // Recursive call minmax in next layer
            temp_score = min_max_search(next_step, alpha, beta, remaining_search_step + 1);

            // Reset the next_step move to EMPTY
            board[next_board][next_step] = EMPTY;

            // Update min heuristic_value
            heuristic_value = MIN(heuristic_value, temp_score);

            // Update beta
            beta = MIN(beta, heuristic_value);

            // Pruning the tree
            if (beta <= alpha)
            {
                break;
            }
        }
    }

    return heuristic_value;
}

/*********************************************************/ /*
  Find next move of the agent, check all avaiable position and find the most optimal result
  If a position can leads the agent win, it will choose that position
  If a position can leads to enemy's victory, don't choose that position
  Choose remaing available position based on heuristic value
  If all the available position will leads to enemy's victory, choose a random position

  @parameter 	board_num		  id of the board waiting for next step 
  
  @return		the move our agent choosed
*/
int find_move(int board_num)
{
    // If we can win in next move, return win move
    int result = can_win(board_num, player);
    if (result != -1)
    {
        return result;
    }

    // Find the available move in current subBoard
    int available[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    for (int i = 1; i <= 9; i++)
    {

        // Filter not EMPTY move
        if (board[board_num][i] != EMPTY)
        {
            available[i] = 0;
            continue;
        }

        // If we move to i, enemy can win in next move.
        result = can_win(i, !player);
        if (result != -1 && i != board_num)
        {
            available[i] = 0;
        }
    }

    // Check all the move, if it have any available move
    bool no_place = true;
    for (int i = 1; i <= 9; i++)
    {
        if (available[i] == 0)
        {
            continue;
        }
        no_place = false;
        break;
    }

    // If not, agent 100% lose the game
    // Then choose a random move
    if (no_place)
    {
        do
        {
            result = 1 + random() % 9;
        } while (board[board_num][result] != EMPTY);
        return result;
    }

    // Find the best move in remaining position
    int maximum_heuristic_value = INT_MIN;
    int current_heuristic_value = 0;
    int return_position = 0;
    for (int i = 1; i <= 9; i++)
    {
        // Skip no available move
        if (available[i] != 1)
        {
            continue;
        }

        // Assume agent move to i
        board[board_num][i] = player;

        // Using minmax to find the best move
        current_heuristic_value = min_max_search(i, INT_MIN, INT_MAX, 0);

        // The move heuristic value is higher, update the optimal move
        if (current_heuristic_value > maximum_heuristic_value)
        {
            maximum_heuristic_value = current_heuristic_value;
            return_position = i;
        }

        // Reset agent move to EMPTY
        board[board_num][i] = EMPTY;
    }
    return return_position;
}

/*********************************************************/ /*
   Print usage information and exit
*/
void usage(char argv0[])
{
    printf("Usage: %s\n", argv0);
    printf("       [-p port]\n"); // tcp port
    printf("       [-h host]\n"); // tcp host
    exit(1);
}

/*********************************************************/ /*
   Parse command-line arguments
*/
void agent_parse_args(int argc, char *argv[])
{
    int i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            if (i + 1 >= argc)
            {
                usage(argv[0]);
            }
            port = atoi(argv[i + 1]);
            i += 2;
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            if (i + 1 >= argc)
            {
                usage(argv[0]);
            }
            host = argv[i + 1];
            i += 2;
        }
        else
        {
            usage(argv[0]);
        }
    }
}

/*********************************************************/ /*
   Called at the beginning of a series of games
*/
void agent_init()
{
    struct timeval tp;

    // generate a new random seed each time
    gettimeofday(&tp, NULL);
    srandom((unsigned int)(tp.tv_usec));
}

/*********************************************************/ /*
   Called at the beginning of each game
*/
void agent_start(int this_player)
{
    reset_board(board);
    m = 0;
    move[m] = 0;
    player = this_player;
}

/*********************************************************/ /*
   Choose second move and return it
*/
int agent_second_move(int board_num, int prev_move)
{
    int this_move;
    move[0] = board_num;
    move[1] = prev_move;
    board[board_num][prev_move] = !player;
    m = 2;

    // New medthod
    this_move = find_move(prev_move);

    move[m] = this_move;
    board[prev_move][this_move] = player;
    return (this_move);
}

/*********************************************************/ /*
   Choose third move and return it
*/
int agent_third_move(
    int board_num,
    int first_move,
    int prev_move)
{
    int this_move;
    move[0] = board_num;
    move[1] = first_move;
    move[2] = prev_move;
    board[board_num][first_move] = player;
    board[first_move][prev_move] = !player;
    m = 3;

    // New medthod
    this_move = find_move(prev_move);

    move[m] = this_move;
    board[move[m - 1]][this_move] = player;
    return (this_move);
}

/*********************************************************/ /*
   Choose next move and return it
*/
int agent_next_move(int prev_move)
{
    int this_move;
    m++;
    move[m] = prev_move;
    board[move[m - 1]][move[m]] = !player;
    m++;

    // New medthod
    int board_num = move[m - 1];
    this_move = find_move(board_num);

    move[m] = this_move;
    board[move[m - 1]][this_move] = player;
    return (this_move);
}

/*********************************************************/ /*
   Receive last move and mark it on the board
*/
void agent_last_move(int prev_move)
{
    m++;
    move[m] = prev_move;
    board[move[m - 1]][move[m]] = !player;
}

/*********************************************************/ /*
   Called after each game
*/
void agent_gameover(
    int result, // WIN, LOSS or DRAW
    int cause   // TRIPLE, ILLEGAL_MOVE, TIMEOUT or FULL_BOARD
)
{
    // nothing to do here
}

/*********************************************************/ /*
   Called after the series of games
*/
void agent_cleanup()
{
    // nothing to do here
}
