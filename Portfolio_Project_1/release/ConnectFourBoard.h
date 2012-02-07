#ifndef CONNECTFOURBOARD_H
#define CONNECTFOURBOARD_H

#include <fstream>
#include <iostream>
#include <ctime>
#include <stack>
using namespace std;


#define HUMAN 'h'
#define COMPUTER 'c'
#define EMPTY 'e'
// board symbols:
// 'h': human player's piece
// 'c': computer's piece
// 'e': empty square

class ConnectFourBoard {
public:
	// parameterless constructor
	// initializes all the board to EMPTY
	ConnectFourBoard() {
		clearBoard();
		this->minNode = false;
	}

	// copy constructor
	// deep copies entre board
	ConnectFourBoard(const ConnectFourBoard& rhs) {
		int r, c;
		for(r=0; r<6; ++r) {
			for(c=0; c<7; ++c)
				this->board[r][c] = rhs.board[r][c];
		}

		this->minNode = rhs.minNode;
		this->filledSpaces = rhs.filledSpaces;
	}

	// inserts player (which should be HUMAN or COMPUTER) to zero-indexed position.
	// returns the row where the piece lands and
	// returns -1 if the intended position is already full
	// precondition: player = HUMAN or player = COMPUTER
	int insertPiece(int position, char player) {
		if(position > 6 || position < 0)
			return -1;

		if(nextMove[position] > 5)
			return -1;

		lastMoveR = nextMove[position]++;
		lastMoveC = position;

		++filledSpaces;
		board[lastMoveR][lastMoveC] = player;
		
		if(minNode)
			minNode = false;
		else
			minNode = true;

		return lastMoveR;
	}


	// prints the board to fout
	void printBoard(ostream *fout) {
		int r, c;
		for(r=5; r>=0; --r) {
			for(c=0; c<7; ++c)
				(*fout) << board[r][c] << " ";
			(*fout) << endl;
		}
	}

	void undoMove(int pos) {
		--filledSpaces;

		board[--nextMove[pos]][pos] = EMPTY;

		if(minNode)
			minNode = false;
		else
			minNode = true;
	}

	// returns HUMAN if the human player has won, 
	// COMPUTER if the computer has won, EMPTY otherwise
	char winningBoard() {
		//return EMPTY;
		int i, pos=0, rtemp, ctemp, consHuman=0, consComp=0, r, c;
		
		r = lastMoveR;
		c = lastMoveC;

		// HUMAN, COMPUTER

		
		consHuman = consComp = 0;

		// horizontal detection
		for(i=0; i<7; ++i) {
			if(board[r][i] == HUMAN) {
				if(++consHuman==4){
					//cout << "HUMAN WINS HORIZONTAL" << endl;
					return HUMAN;
					
				}
				consComp = 0;
			} else if(board[r][i] == COMPUTER) {
				if(++consComp == 4){
					//cout << "COMPUTER WINS HORIZONTAL" << endl;
					return COMPUTER;
					
				}
				consHuman = 0;
			} else {
				consComp = consHuman = 0;
			}
		}

		consHuman = consComp = 0;

		// vertical detection
		for(i=0; i<6; ++i) {
			if(board[i][c] == HUMAN) {
				if(++consHuman==4) {
					//cout << "HUMAN WINS VERTICAL" << endl;
					return HUMAN;
				}
				consComp = 0;
			} else if(board[i][c] == COMPUTER) {
				if(++consComp == 4) {
					//cout << "COMPUTER WINS VERTICAL" << endl;
					return COMPUTER;
				} 
				consHuman = 0;
			} else {
				consComp = consHuman = 0;
			}
		}

		// diagonals
		// diagonal "/"
		consHuman = consComp = 0;
		rtemp = r;
		ctemp = c;
		while(rtemp > 0 && ctemp > 0) {
			--rtemp;
			--ctemp;
		}
		if(rtemp<3 && ctemp<4) {
			do {
				if(board[rtemp][ctemp] == HUMAN) {
					if(++consHuman == 4)
					{
						//cout << "HUMAN WINS DIAGONAL" << endl;
						return HUMAN;
					}
					consComp = 0;
				} else if(board[rtemp][ctemp] == COMPUTER) {
					if(++consComp == 4)
					{
						//cout << "COMPUTER WINS DIAGONAL" << endl;
						return COMPUTER;
					}
					consHuman = 0;
				} else 
					consComp = consHuman = 0;
				++rtemp;
				++ctemp;
			} while(rtemp<6 && ctemp < 7);
		}

		// diagonal "\"
		consComp = consHuman = 0;
		rtemp = r;
		ctemp = c;
		while(rtemp < 6 && ctemp > 0) {
			++rtemp;
			--ctemp;
		}
		if(rtemp>2 && ctemp<4) {
			do {
				if(board[rtemp][ctemp] == HUMAN) {
					if(++consHuman == 4) 
					{
						//cout << "HUMAN WINS DIAGONAL2" << endl;
						return HUMAN;
					}
					consComp = 0;
				} else if(board[rtemp][ctemp] == COMPUTER) {
					{
					if (++consComp == 4)
					{
						//cout << "COMPUTER WINS DIAGONAL2" << endl;
						return COMPUTER;
					}
					}
					consHuman = 0;
				} else {
					consComp = 0;
					consHuman = 0;
				}

				--rtemp;
				++ctemp;
			} while(rtemp>=0 && ctemp<7 );
		}

		return EMPTY;
	}

	bool fullBoard()
	{
		return filledSpaces == 42;
	}

	void clearBoard()
	{
		for (int i=0;i<6;++i)
			for (int j=0;j<7;++j)
				board[i][j] = EMPTY;

		for(int k=0; k<7; ++k)
			nextMove[k] = 0;

		filledSpaces =0;
	}

	int linearFours(int n1, int n2) {
		switch(n1) {
			case 3:
				return (n2+1);
				break;
			case 2:
				return n2;
				break;
			case 1:
				return ((n2-1)>0)? n2:0;
				break;
			case 0:
				return ((n2-2)>0)? n2:0;
				break;
		}
	}

	/*
	void squareSef(int r, int c, int &xp, int ind) {
		ssef = 0;
		currentSquare = board[lastr + r][lastc + c];

		if(!cont || currentSquare == opponent) {
				cont = false;
				return;
		} else {
			++xp;

			if(currentSquare != EMPTY) {
				if()
					pot1[
			}
		}
	}
	*/

	char opponent;
	char currentSquare;
	bool cont;
	int ssef;
	int lastr, lastc;
	int pot1[8], pot2[8], pot3[8], pot4[8];

	void analyzeFour(int r1, int c1, int r2, int c2, int r3, int c3, int r4, int c4) {
		count = 0;
		if(board[r1][c1] == opponent || board[r2][c2] == opponent || board[r3][c3] == opponent || board[r4][c4] == opponent)
			return;

		if(board[r1][c1] == play)
			++count;
		if(board[r2][c2] == play)
			++count;
		if(board[r3][c3] == play)
			++count;
		if(board[r4][c4] == play)
			++count;

		if(count == 4)
			++n4p;

		if(count == 3) {
			if(board[r1][c1] == EMPTY && r1 == nextMove[c1]) {
				++n3pa;
				return;
			}
			if(board[r2][c2] == EMPTY && r2 == nextMove[c2]) {
				++n3pa;
				return;
			}
			if(board[r3][c3] == EMPTY && r3 == nextMove[c3]) {
				++n3pa;
				return;
			}
			if(board[r4][c4] == EMPTY && r4 == nextMove[c4]) {
				++n3pa;
				return;
			}

			++n3p;
			return;
		}

		if(count == 2) {
			++n2p;
			return;
		}
	
		++n1p;
	}

	int r, c, count;
	int n4p, n3pa, n3p, n2p, n1p;
	char play;
	int calcSef(char player) {
		n3pa = n3p = n2p = n1p = 0;
		n4p = 0;
		play = player;
		if(player == HUMAN)
			opponent = COMPUTER;
		else
			opponent = HUMAN;

		for(c=0; c<7; ++c)
			for(r=0; r<nextMove[c]; ++r) {
				if(board[r][c] == player) {
					// east-west
					if(c>2)
						analyzeFour(r, c-3, r, c-2, r, c-1, r, c);
					if(c>1 && c<6)
						analyzeFour(r, c-2, r, c-1, r, c, r, c+1);
					if(c>0 && c <5)
						analyzeFour(r, c-1, r, c, r, c+1, r, c+2);
					if(c<4)
						analyzeFour(r, c, r, c+1, r, c+2, r, c+3);

					// north-south
					if(r>2)
						analyzeFour(r-3, c, r-2, c, r-1, c, r, c);
					if(r>1 && r<5)
						analyzeFour(r-2, c, r-1, c, r, c, r+1, c);
					if(r>0 && r <4)
						analyzeFour(r-1, c, r, c, r+1, c, r+2, c);
					if(r < 3)
						analyzeFour(r, c, r+1, c, r+2, c, r+3, c);

					// northeast-southwest
					if(r>2 && c>2)
						analyzeFour(r-3, c-3, r-2, c-2, r-1, c-1, r, c);
					if(r>1 && c>1 && r<5 && c<6)
						analyzeFour(r-2, c-2, r-1, c-1, r, c, r+1, c+1);
					if(r>0 && c>0 && r<4 && c<5)
						analyzeFour(r-1, c-1, r, c, r+1, c+1, r+2, c+2);
					if(r<3 && c<4)
						analyzeFour(r, c, r+1, c+1, r+2, c+2, r+3, c+3);

					// southeast-northwest
					if(r<3 && c>2)
						analyzeFour(r+3, c-3, r+2, c-2, r+1, c-1, r, c);
					if(r<4 && c>1 && r>0 && c<6)
						analyzeFour(r+2, c-2, r+1, c-1, r, c, r-1, c+1);
					if(r<5 && c>0 && r>1 && c<5)
						analyzeFour(r+1, c-1, r, c, r-1, c+1, r-2, c+2);
					if(r>2 && c<4)
						analyzeFour(r, c, r-1, c+1, r-2, c+2, r-3, c+3);
				}
			}

		if(n4p > 0) {
			sef = 2147483647;
			return sef;
		}

		if(n3pa/3 > 1) {
			sef = 2147483647;
			return sef;
		}
			
		sef = (int)(100*n1p + 1000*n2p/2.0 + 10000*(n3p + n3pa)/3.0);
		return sef;
	}

	int nextMove[7];

	/*
	int potentialFours(int lr, int lc, char player) {
		int e=0, w=0, n=0, s=0, ne=0, nw=0, se=0, sw=0, total=0;
		int ep=0, wp=0, np=0, sp=0, nep=0, nwp=0, sep=0, swp=0, totalp=0;
		
	
		lastr = lr;
		lastc = lc;
		
		if(player == HUMAN)
			opponant = COMPUTER;
		else
			opponent = HUMAN;

		cont = true;
		// east
		if(lastc<6)
			squareSef(0, 1, e, ep);

		if(lastc<5)
			squareSef(0, 2, e, ep);

		if(lastc<4) 
			squareSef(0, 3, e, ep);

		cont = true;
		// west
		if(lastc>0) 
			squareSef(0, -1, w, wp);
			
		if(lastc>1)
			squareSef(0, -2, w, wp);
			
		if(lastc>2)
			squareSef(0, -3, w, wp);

		cont =  true;
		// north
		if(lastr<6)
			squareSef(1, 0, n, np);

		if(lastr<5) 
			squareSef(2, 0, n, np);
		if(lastr<4)
			squareSef(3, 0, n, np);

		cont = true;
		// south
		if(lastr>0)
			squareSef(-1, 0, s, sp);
			
		if(lastr>1)
			squareSef(-2, 0, s, sp);

		if(lastr>2)
			squareSef(-3, 0, s, sp);

		cont = true;
		// northeast
		if(lastr<6 && lastc<6)
			squareSef(1, 1, ne, nep);

		if(lastr<5 && lastc<5)
			squareSef(2, 2, ne, nep);

		if(lastr<4 && lastc<4)
			squareSef(3, 3, ne, nep);

		cont = true;
		// northwest
		if(lastr<6 && lastc>0)
			squareSef(1, -1, nw, nwp);

		if(lastr<5 && lastc>1)
			squareSef(2, -2, nw, nwp);

		if(lastr<4 && lastc>2)
			squareSef(3, -3, nw, nwp);

		cont = true;
		// southwest
		if(lastr>0 && lastc>0)
			squareSef(-1, -1, sw, swp);

		if(lastr>1 && lastc>1)
			squareSef(-2, -2, sw, swp);

		if(lastr>2 && lastc>2)
			squareSef(-3, -3, sw, swp);

		cont = true;
		// southeast
		if(lastr>0 && lastc<6)
			squareSef(-1, 1, se, sep);

		if(lastr>1 && lastc<5)
			squareSef(-2, 2, se, sep);

		if(lastr>2 && lastc<4)
			squareSef(-3, 3, se, sep);

		if(
		
		return total;
	}*/
/*
	int blocking(){
		int e=0, w=0, n=0, s=0, ne=0, nw=0, se=0, sw=0, total=0;
		
		int score=0;
		int numPieces, numCompPieces;
		//loop through board

		for (int row = 0; row < 6; row++){
			for (int col = 0; col < 7; col++){

				// check if piece starts a horizontal row has other pieces
				if (col<4){
					numPieces = 0;

					for (int k=col; k < col +4; k++){
						if (board[row][k] == HUMAN)
							numPieces++;
						else if (board[row][k] == COMPUTER)
							numCompPieces++;
					}
					if (numPieces == 4)
						score = score -15,000;
					else if (numPieces == 3 && numCompPieces ==0) //3 and next move could win
						score = score - 7,500;
					else if (numPieces == 2 && numCompPieces == 0)// 2 and possible 4 could win
						score = score - 5,000;
					else if (numPieces == 2 && numCompPieces >0) // 2 but can't get 4
						score = score + 1,000;
				}
			 
				if (row <3){
					numPieces = 0;

					for (int k=row; k < row +4; k++){
						if (board[k][col] == HUMAN)
							numPieces++;
						else if (board[k][col] == COMPUTER)
							numCompPieces++;
					}
					if (numPieces == 4)
						score = score -15,000;
					else if (numPieces == 3 && numCompPieces ==0) //3 and next move could win
						score = score - 7,500;
					else if (numPieces == 2 && numCompPieces == 0)// 2 and possible 4 could win
						score = score - 5,000;
					else if (numPieces == 2 && numCompPieces >0) // 2 but can't get 4
						score = score + 1,000;
				}

				//check diagonally going to upper right
				if (col < 4 && row < 3){
					numPieces = 0;

					for (int k=col, l = row; k<col+4, l< row+4; k++, l++){
						if (board[l][k] == HUMAN)
							numPieces++;
						else if (board[l][k] == COMPUTER)
							numCompPieces++;
					}

					if (numPieces == 4)
						score = score -15,000;
					else if (numPieces == 3 && numCompPieces ==0) //3 and next move could win
						score = score - 7,500;
					else if (numPieces == 2 && numCompPieces == 0)// 2 and possible 4 could win
						score = score - 5,000;
					else if (numPieces == 2 && numCompPieces >0) // 2 but can't get 4
						score = score + 1,000;
				}
			 
				//check upper left
				if (col > 2 && row < 3){
					numPieces = 0;

					for (int k=col, l = row; k>col-4, l< row+4; k--, l++){
						if (board[l][k]  == HUMAN)
							numPieces++;
						else if (board[l][k] == COMPUTER)
							numCompPieces++;
					}

					if (numPieces == 4)
						score = score -15,000;
					else if (numPieces == 3 && numCompPieces ==0) //3 and next move could win
						score = score - 7,500;
					else if (numPieces == 2 && numCompPieces == 0)// 2 and possible 4 could win
						score = score - 5,000;
					else if (numPieces == 2 && numCompPieces >0) // 2 but can't get 4
						score = score + 1,000;
				}
			}
		}
		return score;
	}*/

	int sef;
	stack<int> moves;
	bool minNode;
	char board[6][7];
	int val;	
	int filledSpaces;
	int lastMoveR, lastMoveC;
};

#endif