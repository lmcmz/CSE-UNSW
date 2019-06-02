%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  random.pl
%  Nine-Board Tic-Tac-Toe Random Player
%  COMP3411/9414/9814 Artificial Intelligence
%  Alan Blair, CSE, UNSW

other(x,o).
other(o,x).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  mark(+P,+M,+SubBoard0,-SubBoard1)
%  mark move M for player P on SubBoard0 to produce SubBoard1
%
mark(P,1,[e|T],[P|T]).
mark(P,2,[A,e|T],[A,P|T]).
mark(P,3,[A,B,e|T],[A,B,P|T]).
mark(P,4,[A,B,C,e|T],[A,B,C,P|T]).
mark(P,5,[A,B,C,D,e|T],[A,B,C,D,P|T]).
mark(P,6,[A,B,C,D,E,e|T],[A,B,C,D,E,P|T]).
mark(P,7,[A,B,C,D,E,F,e|T],[A,B,C,D,E,F,P|T]).
mark(P,8,[A,B,C,D,E,F,G,e,I],[A,B,C,D,E,F,G,P,I]).
mark(P,9,[A,B,C,D,E,F,G,H,e],[A,B,C,D,E,F,G,H,P]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  play(+P,+L,+M,+Board0,-Board1)
%  mark move M for player P on board L of Board0 to produce Board1
%
play(_P,0,Board,Board).
play(P,1,M,[B|T],[B1|T]) :- mark(P,M,B,B1).
play(P,2,M,[C,B|T],[C,B1|T]) :- mark(P,M,B,B1).
play(P,3,M,[C,D,B|T],[C,D,B1|T]) :- mark(P,M,B,B1).
play(P,4,M,[C,D,E,B|T],[C,D,E,B1|T]) :- mark(P,M,B,B1).
play(P,5,M,[C,D,E,F,B|T],[C,D,E,F,B1|T]) :- mark(P,M,B,B1).
play(P,6,M,[C,D,E,F,G,B|T],[C,D,E,F,G,B1|T]) :- mark(P,M,B,B1).
play(P,7,M,[C,D,E,F,G,H,B|T],[C,D,E,F,G,H,B1|T]) :- mark(P,M,B,B1).
play(P,8,M,[C,D,E,F,G,H,I,B,K],[C,D,E,F,G,H,I,B1,K]) :- mark(P,M,B,B1).
play(P,9,M,[C,D,E,F,G,H,I,J,B],[C,D,E,F,G,H,I,J,B1]) :- mark(P,M,B,B1).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  empty(-M,+SubBoard)
%  check that cell M of SubBoard is empty
%
empty(1,[e|_]).
empty(2,[_,e|_]).
empty(3,[_,_,e|_]).
empty(4,[_,_,_,e|_]).
empty(5,[_,_,_,_,e|_]).
empty(6,[_,_,_,_,_,e|_]).
empty(7,[_,_,_,_,_,_,e|_]).
empty(8,[_,_,_,_,_,_,_,e,_]).
empty(9,[_,_,_,_,_,_,_,_,e]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  legal(+L,-M,+Board)
%  true if cell M of board L is legal
%
legal(1,M,[B|_]) :- empty(M,B).
legal(2,M,[_,B|_]) :- empty(M,B).
legal(3,M,[_,_,B|_]) :- empty(M,B).
legal(4,M,[_,_,_,B|_]) :- empty(M,B).
legal(5,M,[_,_,_,_,B|_]) :- empty(M,B).
legal(6,M,[_,_,_,_,_,B|_]) :- empty(M,B).
legal(7,M,[_,_,_,_,_,_,B|_]) :- empty(M,B).
legal(8,M,[_,_,_,_,_,_,_,B,_]) :- empty(M,B).
legal(9,M,[_,_,_,_,_,_,_,_,B]) :- empty(M,B).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  subwin(+P,SubBoard)
%  true if player P has achieved 3-in-a-row
%
subwin(P,[P,P,P|_]).
subwin(P,[_,_,_,P,P,P|_]).
subwin(P,[_,_,_,_,_,_,P,P,P]).
subwin(P,[P,_,_,P,_,_,P,_,_]).
subwin(P,[_,P,_,_,P,_,_,P,_]).
subwin(P,[_,_,P,_,_,P,_,_,P]).
subwin(P,[P,_,_,_,P,_,_,_,P]).
subwin(P,[_,_,P,_,P,_,P,_,_]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  winning(+P,+L,+Board)
%  true if player P has achieved 3-in-a-row on board L
%
winning(P,1,[B|_]) :- subwin(P,B).
winning(P,2,[_,B|_]) :- subwin(P,B).
winning(P,3,[_,_,B|_]) :- subwin(P,B).
winning(P,4,[_,_,_,B|_]) :- subwin(P,B).
winning(P,5,[_,_,_,_,B|_]) :- subwin(P,B).
winning(P,6,[_,_,_,_,_,B|_]) :- subwin(P,B).
winning(P,7,[_,_,_,_,_,_,B|_]) :- subwin(P,B).
winning(P,8,[_,_,_,_,_,_,_,B,_]) :- subwin(P,B).
winning(P,9,[_,_,_,_,_,_,_,_,B]) :- subwin(P,B).

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
   retractall(prev_move(_ )),
   assert(board(
   [[e,e,e,e,e,e,e,e,e],[e,e,e,e,e,e,e,e,e],[e,e,e,e,e,e,e,e,e],
    [e,e,e,e,e,e,e,e,e],[e,e,e,e,e,e,e,e,e],[e,e,e,e,e,e,e,e,e],
    [e,e,e,e,e,e,e,e,e],[e,e,e,e,e,e,e,e,e],[e,e,e,e,e,e,e,e,e]])),
   assert(player(P)),
   ttt.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  second_move(+K,+L)
%  assume first move is board K, cell L
%  choose second move and write it
%
second_move(K,L) :-
   retract(board(Board0)),
   player(P), other(P,Q),
   play(Q,K,L,Board0,Board1),
   print_board(Board1),
   search(P,L,Board1,M),
   play(P,L,M,Board1,Board2),
   print_board(Board2),
   assert(board(Board2)),
   assert(prev_move(M)),
   write_output(M),
   ttt.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  third_Move(+J,+K,+L)
%  assume first move is board K, cell L,
%           second move is board L, cell M
%  choose third move and write it
%
third_move(J,K,L) :-
   retract(board(Board0)),
   player(P),
   play(P,J,K,Board0,Board1),
   print_board(Board1),
   other(P,Q),
   play(Q,K,L,Board1,Board2),
   print_board(Board2),
   search(P,L,Board2,M),
   play(P,L,M,Board2,Board3),
   print_board(Board3),
   assert(board(Board3)),
   assert(prev_move(M)),
   write_output(M),
   ttt.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  next_move(+L)
%  assume opponent move is L
%  choose (our) next move and write it
%
next_move(L) :-
   retract(prev_move(K)),
   retract(board(Board0)),
   player(P), other(P,Q),
   play(Q,K,L,Board0,Board1),
   print_board(Board1),
   search(P,L,Board1,M),
   play(P,L,M,Board1,Board2),
   print_board(Board2),
   assert(board(Board2)),
   assert(prev_move(M)),
   write_output(M),
   ttt.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  search(+P,+L,+Board,-M)
%  choose Move M for player P on board L
%
search(_P,L,Board,Move) :-
   findall(M,legal(L,M,Board),List),
   random_member(List,Move).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  random_member(+List,-Item)
%  choose a random Item in the List
%
random_member(List,Item) :-
    length(List, Num),
    N is random(Num),
    nth0(N, List, Item).

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
   print3boards(A,B,C),
   write('------+-------+------'),nl,
   print3boards(D,E,F),
   write('------+-------+------'),nl,
   print3boards(G,H,I),nl.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  print_board()
%
print3boards([A1,A2,A3,A4,A5,A6,A7,A8,A9],
             [B1,B2,B3,B4,B5,B6,B7,B8,B9],
             [C1,C2,C3,C4,C5,C6,C7,C8,C9]) :-
   print_line(A1,A2,A3,B1,B2,B3,C1,C2,C3),
   print_line(A4,A5,A6,B4,B5,B6,C4,C5,C6),
   print_line(A7,A8,A9,B7,B8,B9,C7,C8,C9).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  print_line()
%
print_line(A,B,C,D,E,F,G,H,I) :-
   write(A),write(' '),write(B),write(' '),write(C),write(' | '),
   write(D),write(' '),write(E),write(' '),write(F),write(' | '),
   write(G),write(' '),write(H),write(' '),write(I),nl.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  last_move(+L)
%
last_move(L) :-
   retract(prev_move(K)),
   retract(board(Board0)),
   player(P), other(P,Q),
   play(Q,K,L,Board0,Board1),
   print_board(Board1),
   ttt.

win(_)  :- write('win'), nl,ttt.
loss(_) :- write('loss'),nl,ttt.
draw(_) :- write('draw'),nl,ttt.

end :- halt.
