% COMP9414 Assignment 1 - Prolog Programming
% Author : Hao Fu
% StudentID : z5102511
% Contact : lmcmze@gmail.com
% ----------------------------------------------------------------- %

%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   		Q1			  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%

sumsq_even([], 0).
sumsq_even([Head | Rest], Result) :-
	sumsq_even(Rest, RestResult),
	Head mod 2 =:= 0,
	Result is Head * Head + RestResult.

sumsq_even([Head | Rest], Result) :-
	sumsq_even(Rest, RestResult),
	Head mod 2 =\= 0,
	Result is RestResult.

%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   		Q2			  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%

% A - Ancestor
% P - Person

maleA(P, A) :-
	parent(A, P),
	male(A).
		
maleA(P, A) :-
	parent(P2, P),
	male(P2),
	maleAncestor(P2, A).


same_name(P1,P2) :-
	maleA(P1, P2).

same_name(P1,P2) :-
	maleA(P2, P1).

% they have common male ancestor
same_name(P1,P2) :- 
	maleA(P1, A),
	maleA(P2, A).

%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   		Q3			  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%

sqrt_list([], []).
sqrt_list(InputList, OutList) :-
	[Head|Rest] = InputList,
	[OutListHead|OutListRest] = OutList,
	Head >= 0,
	LogHead is sqrt(Head), 
	% add to list
	OutListHead = [Head | [LogHead]], 
	sqrt_list(Rest, OutListRest).
	
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   		Q4			  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%

%List = [8,-1,-3,0,2,0,-4]

% Same sign >= 0
is_same(A, B):-
	A >= 0,
	B >= 0.
	
% Same sign < 0
is_same(A, B):-
	A < 0,
	B < 0.

% reverse sub list.
sub_reverse([], []).
sub_reverse(A, B):-
	[AH|AR] = A,
	[BH|BR] = B,
	reverse(AH, BH),
	sub_reverse(AR, BR).

sign_runs([],[]).
sign_runs([H|R],Result):-
	sign_runs([H|R], R, [[H]], Result).

sign_runs(_, [], List,Result):-
	reverse(List, NewList),
	sub_reverse(NewList, Result).

% Same sign add to last list
sign_runs(InputList, SignList, OutList, Result) :-
	[Head|Rest] = InputList,
	[SHead|SRest] = SignList,
	[OutListHead|OutListRest] = OutList,
	is_same(Head, SHead),
	NewList = [ [SHead|OutListHead] | OutListRest],
	sign_runs(Rest, SRest, NewList, Result).

% Not same sign create a new list
sign_runs(InputList, SignList, OutList, Result) :-
	[Head|Rest] = InputList,
	[SHead|SRest] = SignList,
	\+is_same(Head, SHead),
	NewList = [[SHead] | OutList],
	sign_runs(Rest, SRest, NewList, Result).

%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   		Q5			  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%

is_heap(empty).
is_heap(tree(empty, _, empty)).

% Check Right leaf, when left leaf is empty
is_heap(tree(empty, Number, Node)):-
	Node = tree(_, NNumber, _),
	Number =< NNumber,
	is_heap(Node).

% Check left leaf, when right leaf is empty
is_heap(tree(Node, Number, empty)):-
	Node = tree(_, NNumber, _),
	Number =< NNumber,
	is_heap(Node).

% Check both leaf	
is_heap(tree(LNode, Number, RNode)):-
	LNode = tree(_, LNumber, _),
	RNode = tree(_, RNumber, _),
	Number =< LNumber,
	Number =< RNumber,
	is_heap(LNode),
	is_heap(RNode).