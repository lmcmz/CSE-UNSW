%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  alphabeta.pl
%  Tic-Tac-Toe Alpha-Beta Player
%  COMP3411/9414/9814 Artificial Intelligence
%  Alan Blair, CSE, UNSW
%
%  Code for alpha_beta(), eval_choose() and cutoff()
%  adapted from "The Art of Prolog" by Sterling & Shapiro.

other(x,o).
other(o,x).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  play(+P,+M,+Board0,-Board1)
%  play move M for player P on Board0 to produce Board1
%
play(_P,0,Board,Board).
play(P,1,[e|T],[P|T]).
play(P,2,[A,e|T],[A,P|T]).
play(P,3,[A,B,e|T],[A,B,P|T]).
play(P,4,[A,B,C,e|T],[A,B,C,P|T]).
play(P,5,[A,B,C,D,e|T],[A,B,C,D,P|T]).
play(P,6,[A,B,C,D,E,e|T],[A,B,C,D,E,P|T]).
play(P,7,[A,B,C,D,E,F,e|T],[A,B,C,D,E,F,P|T]).
play(P,8,[A,B,C,D,E,F,G,e,I],[A,B,C,D,E,F,G,P,I]).
play(P,9,[A,B,C,D,E,F,G,H,e],[A,B,C,D,E,F,G,H,P]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  legal(-M,+Board)
%  true if move M is legal for Board
%
legal(1,[e|_]).
legal(2,[_,e|_]).
legal(3,[_,_,e|_]).
legal(4,[_,_,_,e|_]).
legal(5,[_,_,_,_,e|_]).
legal(6,[_,_,_,_,_,e|_]).
legal(7,[_,_,_,_,_,_,e|_]).
legal(8,[_,_,_,_,_,_,_,e,_]).
legal(9,[_,_,_,_,_,_,_,_,e]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  winning(+P,+Board)
%  true if player P has achieved 3-in-a-row on Board
%
winning(P,[P,P,P|_]).
winning(P,[_,_,_,P,P,P|_]).
winning(P,[_,_,_,_,_,_,P,P,P]).
winning(P,[P,_,_,P,_,_,P,_,_]).
winning(P,[_,P,_,_,P,_,_,P,_]).
winning(P,[_,_,P,_,_,P,_,_,P]).
winning(P,[P,_,_,_,P,_,_,_,P]).
winning(P,[_,_,P,_,P,_,P,_,_]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  open socket and establish TCP read/write streams
%
connect(Port) :-
   tcp_socket(Socket),
   gethostname(Host),
   tcp_connect(Socket,Host:Port),
   tcp_open_socket(Socket,INs,OUTs),
   assert(connectedReadStream(INs)),
   assert(connectedWriteStream(OUTs)).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  read next command and execute it
%
ttt :-
   connectedReadStream(IStream),
   read(IStream,Command),
   Command.

init :- ttt.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  start(+P)
%  start a new game for player P
%
start(P) :-
   retractall(board(_ )),
   retractall(player(_ )),
   assert(board([e,e,e,e,e,e,e,e,e])),
   assert(player(P)),
   ttt.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  next_move(+L)
%  assume opponent move is L
%  choose (our) next move and write it
%
next_move(L) :-
   retract(board(Board0)),
   player(P), other(P,Q),
   play(Q,L,Board0,Board1),
   print_board(Board1),
   search(P,Board1,M),
   play(P,M,Board1,Board2),
   print_board(Board2),
   assert(board(Board2)),
   write_output(M),
   ttt.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  search(+P,+Board,-M)
%  choose Move M for player P, randomly
%
%search(P,Board,Move) :-
%  findall(M,legal(M,Board),List),
%  random_member(List,Move).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  random_member(+List,-Item)
%  choose a random Item in the List
%
random_member(List,Item) :-
   length(List, Num),
   N is random(Num),
   nth0(N, List, Item).


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  search(+P,+Board,-M)
%  choose Move M for player P, by alpha-beta search
%
search(P,Board,Move) :-
   alpha_beta(P,10,Board,-2000,2000,Move,_Value).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  alpha_beta(+P,+D,+Board,+Alpha,+Beta,-Move,-Value)
%  perform alpha-beta search to depth D for player P,
%  assuming P is about to move on Board. Return Value
%  of current Board position, and best Move for P.

% if other player has won, Value is -1000
alpha_beta(P,_D,Board,_Alpha,_Beta,0,-1000) :-
   other(P,Q),
   winning(Q,Board), ! .

% if depth limit exceeded, use heuristic estimate
alpha_beta(P,0,Board,_Alpha,_Beta,0,Value) :-
   value(P,Board,Value), ! .

% evaluate and choose all legal moves in this position
alpha_beta(P,D,Board,Alpha,Beta,Move,Value) :-
   D > 0,
   findall(M,legal(M,Board),Moves),
   Moves \= [], !,
   Alpha1 is -Beta,
   Beta1 is -Alpha,
   D1 is D-1,
   eval_choose(P,Moves,Board,D1,Alpha1,Beta1,0,Move,Value).

% if no available moves, it must be a draw
alpha_beta(_P,_D,_Board,_Alpha,_Beta,0,0).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  eval_choose(+P,+Moves,+Board,+D,+Alpha,+Beta,+BestMove
%              -ChosenMove,-Value)
%  Evaluate list of Moves and determine Value of position
%  as well as ChosenMove for this Board position
% (taking account of current BestMove for this position)

% if no more Moves, BestMove becomes ChosenMove and Value is Alpha
eval_choose(_P,[],_Board,_D,Alpha,_Beta,BestMove,BestMove,Alpha).

% evaluate Moves, find Value of Board Position, and ChosenMove for P
eval_choose(P,[M|Moves],Board,D,Alpha,Beta,BestMove,ChosenMove,Value) :-
   play(P,M,Board,Board1),
   other(P,Q),
   alpha_beta(Q,D,Board1,Alpha,Beta,_Move1,Value1),
   V is -Value1,
   cutoff(P,Moves,Board,D,Alpha,Beta,BestMove,M,V,ChosenMove,Value).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  cutoff(+P,+Moves,+Board,+D,+Alpha,+Beta,+BestMove,+M,+V,
%              -ChosenMove,-Value)
%  Compare move M (with value V) to Alpha and Beta,
%  and compute Value and ChosenMove appropriately.

% cut off the search, ChosenMove is M and Value is V
cutoff(_P,_Moves,_Board,_D,_Alpha,Beta,_Move0,M,V,M,V) :-
   V >= Beta.

% Alpha increases to V, BestMove is M, continue search
cutoff(P,Moves,Board,D,Alpha,Beta,_BestMove,M,V,ChosenMove,Value) :-
   Alpha < V, V < Beta,
   eval_choose(P,Moves,Board,D,V,Beta,M,ChosenMove,Value).

% keep searching, with same Alpha, Beta, BestMove
cutoff(P,Moves,Board,D,Alpha,Beta,BestMove,_M,V,ChosenMove,Value) :-
   V =< Alpha,
   eval_choose(P,Moves,Board,D,Alpha,Beta,BestMove,ChosenMove,Value).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  write_output(+M)
%  transmit the chosen move (M)
%
write_output(M) :-
   connectedWriteStream(OStream),
   write(OStream,M),
   nl(OStream), flush_output(OStream).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  print_board()
%
print_board([A,B,C,D,E,F,G,H,I]) :-
   write(A),write(' '),write(B),write(' '),write(C),nl,
   write(D),write(' '),write(E),write(' '),write(F),nl,
   write(G),write(' '),write(H),write(' '),write(I),nl,nl.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  last_move(+L)
%
last_move(L) :-
   retract(board(Board0)),
   player(P), other(P,Q),
   play(Q,L,Board0,Board1),
   print_board(Board1),
   ttt.

win(_)  :- write('win'), nl,ttt.
loss(_) :- write('loss'),nl,ttt.
draw(_) :- write('draw'),nl,ttt.

end :- halt.
