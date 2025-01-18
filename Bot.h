#ifndef BOT_H
#define BOT_H

#include <iostream>
#include <vector>
#include "Board.h"
#include "Player.h"


class Bot : public Player {
public:
  bool _isPlayer = false;
  std::string _winCond = "";

public:
  Bot() {}

  Bot(std::string name, std::string winCond = "row") : Player(name) {
    _winCond = winCond;
  }

  void makeMove(Board& board) override {
    std::string input = "";
    int r = findBestRow(board);
    int c = findBestCol(board);
    std::vector<std::pair<size_t, int>> indSum;
    if (_winCond == "row") {
      // c = findWorstCol(board);
      indSum = board.getColsIndSums();
      int k = 0;
      c = indSum[k].first;
      while (board._board[r][c] == 0) {
        k++;
        c = indSum[k].first;
      }
    } else {
      indSum = board.getRowsIndSums();
      int k = 0;
      r = indSum[k].first;
      while (board._board[r][c] == 0) {
        k++;
        r = indSum[k].first;
      }
    }
    std::cout << "Бот уменьшает ячейку (" << r << ", " << c << ")!\n";
    board.decrease(r, c);
  }

  int findWorstRow(const Board& board) {
    int maxSum = 0;
    int maxInd = 0;
    for (int i = 0; i < board.getHeight(); i++) {
      int sum = 0;
      for (int j = 0; j < board.getWidth(); j++) {
        sum += board._board[i][j];
      }
      if (sum > maxSum) {
        maxSum = sum;
        maxInd = i;
      }
    }
    return maxInd;
  }

  int findBestRow(const Board& board) {
    int minSum = 2000000000;
    int minInd = 0;
    for (int i = 0; i < board.getHeight(); i++) {
      int sum = 0;
      for (int j = 0; j < board.getWidth(); j++) {
        sum += board._board[i][j];
      }
      if (sum < minSum) {
        minSum = sum;
        minInd = i;
      }
    }
    return minInd;
  }

  int findWorstCol(const Board& board) {
    int maxSum = 0;
    int maxInd = 0;
    for (int j = 0; j < board.getWidth(); j++) {
      int sum = 0;
      for (int i = 0; i < board.getHeight(); i++) {
        sum += board._board[i][j];
      }
      if (sum > maxSum) {
        maxSum = sum;
        maxInd = j;
      }
    }
    return maxInd;
  }

  int findBestCol(const Board& board) {
    int minSum = 2000000000;
    int minInd = 0;
    for (int j = 0; j < board.getWidth(); j++) {
      int sum = 0;
      for (int i = 0; i < board.getHeight(); i++) {
        sum += board._board[i][j];
      }
      if (sum < minSum) {
        minSum = sum;
        minInd = j;
      }
    }
    return minInd;
  }
};

#endif