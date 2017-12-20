#pragma once
#include "Move.h"

namespace TicTacToe
{
	class Minimax
	{
	public:
		int Evaluation(char board[3][3]);
		int MaxFinder(char board[3][3], int alpha, int beta);
		int MinFinder(char board[3][3], int alpha, int beta);

		bool HasMovesLeft(char board[3][3]);
		bool IsWinner(char board[3][3]);
		bool GameOver(char board[3][3]);

		Move PlayerMove(char board[3][3]);
		Move FindBestMove(char board[3][3]);

	private:
		const int boardWidth = 3;
		const int boardHeight = 3;
	};
}