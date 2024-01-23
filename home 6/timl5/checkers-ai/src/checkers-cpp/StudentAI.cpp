#include "StudentAI.h"
#include <random>
#include <cmath>

//The following part should be completed by students.
//The students can modify anything except the class name and exisiting functions and varibles.
StudentAI::StudentAI(int col,int row,int p)
	:AI(col, row, p)
{
    board = Board(col,row,p);
    board.initializeGame();
    player = 2;
}

Move StudentAI::GetMove(Move move)
{
    // ofstream output;
    // output.open("/home/timl5/checkers-ai/Tools/test.txt", ios::app);
    // output << "new turn\n" << flush;
    if (move.seq.empty())
    {
        player = 1;
    } else{
        board.makeMove(move,player == 1?2:1);
        if(this->cur_node){
            for(auto i: this->cur_node->children){
                if(i->move.toString() == move.toString()){
                    // output << "found " << "player: " << player << " node player: " << i->player << '\n' << flush;
                    this->cur_node = i;
                    break;
                }
            }
        }
    }
    
    if (!this->cur_node){
        this->cur_node = new node;
        this->cur_node->player = player==1?2:1;
        this->cur_node->is_explored = 1;
        this->cur_node->num_sims = 0;
        this->cur_node->parent = nullptr;
        this->cur_node->wins = 0;
        this->root = this->cur_node;
        this->cur_node->children = vector<node*>();
        this->cur_node->unexplored = list<node*>();
    }
    Move res = this->MCTS();
    board.makeMove(res, player);
    return res;
}

Move StudentAI::MCTS(){
    int num_iterations = 250;
    for(int i = 0; i < num_iterations; i++){
        node* selected = select(this->cur_node);
        if (board.getAllPossibleMoves(selected->player==1?2:1).size() != 0){
            node* to_simulate = expand(selected);
            int winner = simulate(to_simulate);
            backpropogate(to_simulate, winner);
        }
        else{
            int winner = simulate(selected);
            backpropogate(selected, winner);
        }
        for(int j = 0; j < move_count; j++){
            board.Undo();
        }
        move_count = 0;
    }
    double max_winrate = -1;
    Move best_move;
    for (auto n: this->cur_node->children){
        double winrate = ((double)n->wins)/((double)n->num_sims);
        if (winrate >= max_winrate){
            max_winrate = winrate;
            best_move = n->move;
            this->cur_node = n;
        }
    }
    // ofstream output;
    // output.open("/home/timl5/checkers-ai/Tools/test.txt", ios::app);
    // output << "mcts move: " << best_move.toString() << ' '<< cur_node->player << '\n' << flush;
    return best_move;
}

StudentAI::node* StudentAI::select(node* n){
    // ofstream output;
    // output.open("/home/timl5/checkers-ai/Tools/test.txt", ios::app);
    if (n->unexplored.size() == 0 && n->children.size() == 0){
        return n;
    }
    if (n->unexplored.empty()){
        double max = 0;
        node* node_max;
        for (auto i: n->children){
            double val = uct(i->wins, i->num_sims, i->parent->num_sims);
            if (val >= max){
                max = val;
                node_max = i;
            }
        }
        // output << " select move: " << node_max->move.toString() << '\n' << flush;
        board.makeMove(node_max->move, node_max->player);
        move_count++;

        return select(node_max);
    }
    int i = rand() % n->unexplored.size();
    node* chosen = *(next(n->unexplored.begin(),i));
    n->unexplored.erase(next(n->unexplored.begin(),i));
    n->children.push_back(chosen);
    // output << "chosen move: " << chosen->move.toString() << chosen->player << '\n' << flush;
    board.makeMove(chosen->move, chosen->player);
    move_count++;
    return chosen;
}

Move StudentAI::RandomMove(int player){
    vector<vector<Move> > moves = board.getAllPossibleMoves(player);
    int i = rand() % (moves.size());
    vector<Move> checker_moves = moves[i];
    int j = rand() % (checker_moves.size());
    Move res = checker_moves[j];
    board.makeMove(res,player);
    return res;
}

double StudentAI::uct(int wins, int num_sims, int par_sims){
    return ((double) wins)/((double)num_sims) + sqrt(2)*sqrt(log((double)par_sims)/(double)num_sims);
}

StudentAI::node* StudentAI::expand(node* n){
    if (n->unexplored.size() == 0){
        vector<vector<Move>> moves = board.getAllPossibleMoves((n->player)==1?2:1);
        vector<node*> children;
        list<node*> unexplored;
        for (auto i: moves){
            for (auto j: i){
                node* new_child = new node;
                new_child->is_explored = 0;
                new_child->move = j;
                new_child->num_sims = 0;
                new_child->parent = n;
                new_child->player = (n->player)==1?2:1;
                new_child->wins = 0;
                new_child->children = vector<node*>();
                new_child->unexplored = list<node*>();
                unexplored.push_back(new_child);
            }
        }
        n->unexplored = unexplored;
        n->children = children;
    }
    int i = rand() % n->unexplored.size();
    
    node* chosen = *(next(n->unexplored.begin(),i));
    
    n->unexplored.erase(next(n->unexplored.begin(),i));
    n->children.push_back(chosen);
    return chosen;
}

int StudentAI::simulate(node* n){
    if (board.isWin(n->player)!=0){
        return board.isWin(n->player);
    }
    int cur_player = n->player;
    board.makeMove(n->move, n->player);
    move_count++;
    int win = board.isWin(n->player);
    cur_player = cur_player==1?2:1;
    while (!win){
        RandomMove(cur_player);
        move_count++;
        win = board.isWin(cur_player);
        cur_player = cur_player==1?2:1;
    }
    return win;
}

void StudentAI::backpropogate(node* n, int winner){
    winner = winner==-1?player:winner;
    while (n != cur_node){
        n->num_sims++;
        if (n->player == winner){
            n->wins++;
        }
        n = n->parent;
    }
    n->num_sims++;
}

