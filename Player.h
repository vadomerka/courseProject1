#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <vector>
#include <fstream>

class Player {
public:
  std::string _name = "";
  bool _isPlayer = true;
  std::string _winCond;

public:
  Player() {}

  Player(std::string name) : _name(name), _winCond("row") {}

  Player(std::string name, std::string intension) : _name(name), _winCond(intension) {}
  
  virtual std::pair<int, int> makeMove(Board &board, const std::vector<int>& fTurns, bool canPass, int passStreak, 
    int turnDepth = 4, std::ostream& log=std::cout) {
    std::string input = "";
    int r = 0;
    int c = 0;
    do {
      log << "Enter coordinates of a non-zero cell in format: row col\n";
      if (canPass && passStreak < 2) log << "Or write \"pass\", to skip turn.\n";
      std::cin >> input;
      if (input == "pass" && canPass && passStreak < 2) { return {-2, -2}; }
      try {
        r = std::stoi(input);
        std::cin >> c;
      } 
      catch(const std::exception& e)
      {
        continue;
      }
    } while (!canMakeMove(r, c, board));
    board.decrease(r, c);
    return {r, c};
  }

  bool canMakeMove(int r, int c, Board& board) {
    return board.getValue(r, c) > 0;
  }

  std::vector<int> calcRowPriority(const Board &board) {
    auto rows = board.getRowsIndSums();
    std::sort(rows.begin(), rows.end(),
              [](auto &a, auto &b) { return a.second < b.second; });
    std::vector<int> priorities;
    for (auto &row : rows)
      priorities.push_back(row.first);
    return priorities;
  }

  std::vector<int> calcColPriority(const Board &board) {
    auto cols = board.getColsIndSums();
    std::sort(cols.begin(), cols.end(),
              [](auto &a, auto &b) { return a.second < b.second; });
    std::vector<int> priorities;
    for (auto &col : cols)
      priorities.push_back(col.first);
    return priorities;
  }

  std::pair<int, int> hint(const Board &board) {
    if (board.hasWinner() != 0) {
      return {-1, -1};
    }
    std::string input = "";
    int r = calcRowPriority(board)[0];
    int c = calcColPriority(board)[0];
    std::vector<std::pair<size_t, int>> indSum;
    if (_winCond == "row") {
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
    return {r, c};
  }

public:
  virtual void logEvals() {}
};

#endif