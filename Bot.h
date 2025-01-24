#ifndef BOT_H
#define BOT_H

#include "Board.h"
#include "Player.h"
#include <iostream>
#include <limits>
#include <vector>

const double WIN_EVAL = 0.99;
const int megaint = 1000000;

class Bot : public Player {
public:
  int PLAYER_1 = 0;
  int PLAYER_2 = 1;
  std::vector<int> turns;

  bool _isPlayer = false;
  int player;
  std::string _winCond = "";

  Bot() {}

  Bot(std::string name, std::string winCond = "row")
      : Player(name), _winCond(winCond) {}

  std::pair<int, int> makeMove(Board &board, const std::vector<int>& fTurns) override {
    turns = fTurns;             // Доступные ходы
    // PLAYER_1 = turns[0];        // Сам бот
    // PLAYER_2 = 1 - PLAYER_1;    // Его противник
    Depth depth(turns.size());  // Глубина минимакса
    std::pair<int, int> bestMove = minimax(board, depth).first;
    board.decrease(bestMove.first, bestMove.second);
    return bestMove;
  }

public:
  struct Depth {
    int orig;
    int curr;

    Depth(int oorig) : orig(oorig), curr(oorig) {}

    Depth(int oorig, int ccurr) : orig(oorig), curr(ccurr) {}

    Depth next() const { return Depth{orig, curr - 1}; }  // TODO : wtf???
  };

  std::pair<std::pair<int, int>, double> minimax(Board &board, Depth depth) {
    if (depth.curr <= 0 || board.hasWinner() != 0) {
      return {{-1, -1}, evaluateBoard(board)};
    }

    player = turns[depth.orig - depth.curr];
    std::pair<int, int> bestMove {-1, -1};
    double bestValue = (player == PLAYER_1)
                           ? -std::numeric_limits<double>::infinity()
                           : std::numeric_limits<double>::infinity();

    auto priorities =
        (player == PLAYER_1) ? calcRowPriority(board) : calcColPriority(board);

    for (int p : priorities) {
      for (int i = 0;
           i < ((player == PLAYER_1) ? board.getWidth() : board.getHeight());
           ++i) {
        int x = (player == PLAYER_1) ? p : i;
        int y = (player == PLAYER_1) ? i : p;
        // if (player == PLAYER_2) {
        //   std::swap(x, y);
        // }

        if (board.getValue(x, y) == 0)
          continue;

        board.decrease(x, y);
        // std::cout << "x, y: " << x << " " << y << "\t";
        double eval = minimax(board, depth.next()).second;
        // board.printBoard();
        board.increase(x, y);
        player = turns[depth.orig - depth.curr];
        
        if ((player == PLAYER_1 && eval > bestValue) ||
            (player == PLAYER_2 && eval < bestValue)) {
          bestValue = eval;
          bestMove = {x, y};
          // if (std::abs(bestValue) == std::numeric_limits<double>::infinity()) {
          //   return {bestMove, bestValue};
          // }
        }
      }
    }
    if (bestMove.first == -1 && bestMove.second == -1) {
      Depth new_depth(depth.orig - 1);  // Глубина минимакса
      std::pair<std::pair<int, int>, double> res = minimax(board, new_depth);
      bestMove = res.first;
      bestValue = res.second;
    }
    return {bestMove, bestValue};
  }

  double evaluateBoard(const Board &board) { return heuristics(board); }

  double heuristics(const Board &board) {
    std::vector<std::pair<size_t, int>> riSums = board.getRowsIndSums();
    std::vector<std::pair<size_t, int>> ciSums = board.getColsIndSums();
    double minRow =
        (*std::min_element(riSums.begin(),
                          riSums.end(),
                          [](auto &a, auto &b) { return a.second < b.second; }))
             .second;
    double minCol =
        (*std::min_element(ciSums.begin(),
                          ciSums.end(),
                          [](auto &a, auto &b) { return a.second < b.second; }))
             .second;
    int koef = (player == PLAYER_1) ? 1 : -1;
    // int koef = 1;
    double eur = 0;
    // if (minRow != minCol) {
    eur = (minCol - minRow);
    
    // }
    if (minRow == 0 && minCol == 0) {
      eur = koef * std::numeric_limits<double>::infinity();
    } else if (minRow == 0) {
      eur = std::numeric_limits<double>::infinity();
      // eur = koef * (megaint + (minRow + minCol));
    } else if (minCol == 0) {
      eur = -std::numeric_limits<double>::infinity();
    }
    // std::cout << "minRow: " << minRow << '\t';
    // std::cout << "minCol: " << minCol << '\t';
    // std::cout << "eur: " << eur << '\n';
    return eur;
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
};

#endif