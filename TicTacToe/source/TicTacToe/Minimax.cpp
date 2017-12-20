#include "pch.h"
#include "Minimax.h"
#include "Player.h"

using namespace std;

namespace TicTacToe
{

	int Minimax::Evaluation(char board[3][3])
	{
		char playerPiece = player;
		char aiPiece = ai;

		for (int i = 0; i < boardWidth; i++)
		{
			//// check if player wins
			// check horizontal
			if (board[0][i] == playerPiece && board[1][i] == playerPiece && board[2][i] == playerPiece)
				return 1;

			// check vertical
			if (board[i][0] == playerPiece && board[i][1] == playerPiece && board[i][2] == playerPiece)
				return 1;

			// check diagonal top left to bottom right
			if (board[0][0] == playerPiece && board[1][1] == playerPiece && board[2][2] == playerPiece)
				return 1;

			// check diagonal bottom left to top right
			if (board[0][2] == playerPiece && board[1][1] == playerPiece && board[2][0] == playerPiece)
				return 1;


			//// Check if AI wins
			// check horizontal
			if (board[0][i] == aiPiece && board[1][i] == aiPiece && board[2][i] == aiPiece)
				return -1;

			// check vertical
			if (board[i][0] == aiPiece && board[i][1] == aiPiece && board[i][2] == aiPiece)
				return -1;

			// check diagonal top left to bottom right
			if (board[0][0] == aiPiece && board[1][1] == aiPiece && board[2][2] == aiPiece)
				return -1;

			// check diagonal bottom left to top right
			if (board[0][2] == aiPiece && board[1][1] == aiPiece && board[2][0] == aiPiece)
				return -1;
		}
		return 0;
	}

	int Minimax::MaxFinder(char board[3][3], int alpha, int beta)
	{
		bool isPruned = false;
		int bestValue = INT_MIN;
		int tempValue = INT_MIN;
		int score = Evaluation(board);

		//// check if game over state
		// player wins, return their score
		if (score == 1)
			return score;

		// ai wins, return their score
		if (score == -1)
			return score;

		// game is a draw
		if (HasMovesLeft(board) == false)
			return 0;

		for (int i = 0; i < boardWidth; i++)
		{
			for (int j = 0; j < boardHeight; j++)
			{
				if (board[i][j] == '-')
				{
					board[i][j] = player;
					tempValue = MinFinder(board, alpha, beta);
					board[i][j] = '-';

					if (tempValue >= bestValue)
						bestValue = tempValue;
					if (bestValue >= alpha)
						alpha = bestValue;
					if (beta <= alpha)
					{
						isPruned = true;
						break;
					}
				}
			}
			if (isPruned)
				break;
		}
		return bestValue;
	}

	int Minimax::MinFinder(char board[3][3], int alpha, int beta)
	{
		bool isPruned = false;
		int bestValue = INT_MAX;
		int tempValue = INT_MAX;
		int score = Evaluation(board);

		//// check if game over state
		// player wins, return their score
		if (score == 1)
			return score;

		// ai wins, return their score
		if (score == -1)
			return score;

		// game is a draw
		if (HasMovesLeft(board) == false)
			return 0;

		for (int i = 0; i < boardWidth; i++)
		{
			for (int j = 0; j < boardHeight; j++)
			{
				if (board[i][j] == '-')
				{
					board[i][j] = ai;
					tempValue = MaxFinder(board, alpha, beta);
					board[i][j] = '-';

					if (tempValue <= bestValue)
						bestValue = tempValue;
					if (bestValue < beta)
						beta = bestValue;
					if (beta <= alpha)
					{
						isPruned = true;
						break;
					}
				}
			}
			if (isPruned)
				break;
		}
		return bestValue;
	}

	bool Minimax::HasMovesLeft(char board[3][3])
	{
		for (int i = 0; i < boardWidth; i++)
		{
			for (int j = 0; j < boardHeight; j++)
			{
				if (board[i][j] == '-')
					return true;
			}
		}
		return false;
	}

	bool Minimax::IsWinner(char board[3][3])
	{
		char playerPiece = player;
		char aiPiece = ai;

		for (int i = 0; i < boardWidth; i++)
		{
			//// check if player wins
			// check horizontal
			if (board[0][i] == playerPiece && board[1][i] == playerPiece && board[2][i] == playerPiece)
				return true;

			// check vertical
			if (board[i][0] == playerPiece && board[i][1] == playerPiece && board[i][2] == playerPiece)
				return true;

			// check diagonal top left to bottom right
			if (board[0][0] == playerPiece && board[1][1] == playerPiece && board[2][2] == playerPiece)
				return true;

			// check diagonal bottom left to top right
			if (board[0][2] == playerPiece && board[1][1] == playerPiece && board[2][0] == playerPiece)
				return true;


			//// Check if AI wins
			// check horizontal
			if (board[0][i] == aiPiece && board[1][i] == aiPiece && board[2][i] == aiPiece)
				return true;

			// check vertical
			if (board[i][0] == aiPiece && board[i][1] == aiPiece && board[i][2] == aiPiece)
				return true;

			// check diagonal top left to bottom right
			if (board[0][0] == aiPiece && board[1][1] == aiPiece && board[2][2] == aiPiece)
				return true;

			// check diagonal bottom left to top right
			if (board[0][2] == aiPiece && board[1][1] == aiPiece && board[2][0] == aiPiece)
				return true;
		}
		return false;
	}

	bool Minimax::GameOver(char board[3][3])
	{
		if (IsWinner(board))
			return true;
		if (!HasMovesLeft(board))
			return true;

		return false;
	}

	Move Minimax::PlayerMove(char board[3][3])
	{
		Move moveCoordinates;
		bool tryAgain = false;
		bool isValidMove = false;
		int xCoord = INT_MIN;
		int yCoord = INT_MIN;

		cout << endl << "---Enter your move coordinates for the 3x3 grid---" << endl;
		cout << "---Move coordinates x y: ";

		// loop while input is not within bounds of board
		while (xCoord < 0 || xCoord > 2 || yCoord < 0 || yCoord > 2 || isValidMove == false)
		{
			if (tryAgain)
				cout << "Try Again: ";

			cin >> xCoord >> yCoord;
			tryAgain = true;

			if (board[yCoord][xCoord] == '-')
				isValidMove = true;
		}

		moveCoordinates.row = xCoord;
		moveCoordinates.col = yCoord;

		return moveCoordinates;
	}

	Move Minimax::FindBestMove(char board[3][3])
	{
		Move bestMove;
		int bestValue = INT_MAX;
		int tempValue = INT_MAX;
		int alpha = INT_MIN;
		int beta = INT_MAX;
		bestMove.row = INT_MAX;
		bestMove.col = INT_MAX;

		for (int i = 0; i < boardWidth; i++)
		{
			for (int j = 0; j < boardHeight; j++)
			{
				if (board[i][j] == '-')
				{
					board[i][j] = ai; // set the piece for testing
					tempValue = MaxFinder(board, alpha, beta); // init call that starts minimax algorithm
					board[i][j] = '-'; // revert back to prev state

					if (tempValue <= bestValue)
					{
						bestMove.row = i;
						bestMove.col = j;
						bestValue = tempValue;
					}
				}
			}
		}
		return bestMove;
	}
}