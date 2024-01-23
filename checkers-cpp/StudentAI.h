#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#include <tuple>
#include <vector>
#include <list>
#include <string>
#pragma once

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
public:
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);
	struct node{
		int player;
		int is_explored;
		Move move;
		node* parent;
		int num_sims;
		int wins;
		vector<node*> children;
		list<node*> unexplored;
	};
private:
	int move_count = 0;
	node* root = nullptr;
	node* cur_node = nullptr;
	double uct(int wins, int num_sims, int par_sims);
	node* select(node* node);
	void backpropogate(node* node, int winner);
	node* expand(node* node);
	int simulate(node* node);
	Move MCTS();
	Move RandomMove(int player);
	
};

#endif //STUDENTAI_H
