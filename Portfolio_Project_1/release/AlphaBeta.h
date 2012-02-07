#ifndef ALPHABETA_H
#define ALPHABETA_H

#include "ConnectFourBoard.h"

class AlphaBeta {
public:
	AlphaBeta() {
	} 

	//AlphaBetaSearch
	// acts as a wrapper function for intigating a minimax w/ alpha beta cutoffs
	// returns index of column CPU wishes to play in
	// currentBoard
	int AlphaBetaSearch(ConnectFourBoard* currentBoard) {
		int best_move;
		int childNum =0;
		srand(time(0));		
		currentBoard->minNode = false;		
		double stime, ftime,ttime,ptime=0.0;
		bool run = true;		
		int depth = 1;
		while(run && depth <= (42-currentBoard->filledSpaces) ){ //run && don't allow CPU to go deeper than possible
			stime = (1.0*clock())/CLOCKS_PER_SEC; //start time
			if (depth == 1)
			{
				int win = obviousWins(currentBoard); //find any obvious wins before initiating alpha-beta
				if (win != -1)
					return win;
				
			}
			else 
				AlphaBetaSearch(currentBoard, -2147483647, 2147483647, depth, &best_move); 
				
			ftime = (1.0*clock())/CLOCKS_PER_SEC; // finish time
			ttime = ftime - stime; //ttime = time it took (in seconds) to complete alpha beta
			//printf("ttime is %.3f\n",ttime);
			ptime+= ttime; // current total time run (in seconds)
			if (ptime + (ttime*7) <= 50.0)
				run = true;
			else{
				run = false;
			}
			if(depth>=8 && ttime>=3.000){
				run = false;
			}
			depth++;

		}
		printf("CPU reached depth of %d in %.6f seconds\n",depth-1,ptime);
		return best_move;
	}

private:

	//StaticEval
	// returns a point value for a static evaluation function of the board it is given
	int StaticEval(ConnectFourBoard *pos)
	{		
		int temp;
		char player, opponent;
		temp = pos->calcSef(COMPUTER) - pos->calcSef(HUMAN);

        return temp;
	}

	bool valid;
	
	//AlphaBetaSearch
	//the real heart of the minimax w/ alpha beta cutoffs
	int AlphaBetaSearch(ConnectFourBoard *currentBoard, int alpha, int beta, int depth, int *bestChild) {
		// board
		int val, node_val, i;
		int best;
		bool firstChild;
		char win = currentBoard->winningBoard();

		if(win == COMPUTER)
			return 2147483647;
		else if(win == HUMAN)
			return -2147483647;

		if(depth==0) {
			*bestChild = -1;
			return StaticEval(currentBoard);
		}
		 
		i=0;
		//find first child
		*bestChild=0;
		valid = currentBoard->insertPiece(i, (currentBoard->minNode)? HUMAN : COMPUTER)!=-1;
		while(i<6 && !valid) {
			++i;
			valid = currentBoard->insertPiece(i, (currentBoard->minNode)? HUMAN : COMPUTER)!=-1;	
		}
		if(valid)
			currentBoard->moves.push(i);
		if(!valid)
		{
			*bestChild = -1;
			return StaticEval(currentBoard);
		}
		else
		{
			firstChild = true;
			bool prune = false;
			while(!prune && valid)
			{
				val = AlphaBetaSearch(currentBoard, alpha, beta, depth-1, &best);
				currentBoard->undoMove(currentBoard->moves.top());
				currentBoard->moves.pop();

				if( firstChild )
				{ 
					firstChild = false;
					node_val = val;
					currentBoard->val = val;
					*bestChild = i;
				}
				if( currentBoard->minNode )
				{
					if( val < node_val )
					{
						node_val = val;
						currentBoard->val = val;
						*bestChild = i;
					}
					if( val < beta )
					{ 
						beta = val; 
					}
				}
				else
				{
					if( val > node_val )
					{
						node_val = val;
						currentBoard->val = val;
						*bestChild = i;
					}
					if( val > alpha )
					{ 
						alpha = val; 
					}
				}

				//pruning
				if(alpha > beta )
				{
					prune = true;
					return node_val;
				}
				else
				{ 
					// go to next child
					//tempNode = tempNode->next; 
					++i;
					valid = currentBoard->insertPiece(i, (currentBoard->minNode)? HUMAN : COMPUTER)!=-1;
					if(!valid)
						valid = false;
					while(i<6 && !valid) {
						++i;
						valid = currentBoard->insertPiece(i, (currentBoard->minNode)? HUMAN : COMPUTER)!=-1;	
					}
					if(valid)
						currentBoard->moves.push(i);
				}
			} // end while
		}//end else
		return node_val;
	}

	//obviousWins
	//looks for an obviousWin that is just one ply deep from the current board
	//to be used so that less time is spent waiting for CPU to finish alpha-beta before it takes a win
	int obviousWins(ConnectFourBoard *currentBoard)
	{
		int i=0;
		bool valid;
		//find first child
		valid = currentBoard->insertPiece(i, (currentBoard->minNode)? HUMAN : COMPUTER)!=-1;
		while(i<6 && !valid) {
			++i;
			valid = currentBoard->insertPiece(i, (currentBoard->minNode)? HUMAN : COMPUTER)!=-1;	
		}
		if(valid)
			currentBoard->moves.push(i);
		else
			return -1;

		while(valid)
		{
			if (currentBoard->winningBoard() == COMPUTER)
			{	i = currentBoard->moves.top();
				currentBoard->undoMove(currentBoard->moves.top());
				currentBoard->moves.pop();
				return i;
			}

			currentBoard->undoMove(currentBoard->moves.top());
			currentBoard->moves.pop();

				// go to next child
				++i;
				valid = currentBoard->insertPiece(i, (currentBoard->minNode)? HUMAN : COMPUTER)!=-1;
				while(i<6 && !valid) {
					++i;
					valid = currentBoard->insertPiece(i, (currentBoard->minNode)? HUMAN : COMPUTER)!=-1;	
				}
				if(valid)
					currentBoard->moves.push(i);

		} // end while
		return -1;
	}
};

#endif
