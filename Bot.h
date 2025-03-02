#ifndef BOT_H
#define BOT_H

#include "Board.h"
#include "Player.h"
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <fstream>


const double WIN_EVAL = 0.99;
// const int megaint = 1000000;
long long inf = 1000000;
std::map<std::string, double> evalMap;
std::set<std::string> sekiSet;
std::map<std::string, std::vector<double>> sekiStates;
int dc1 = 0;
int dc2 = 0;



class Bot : public Player {
public:
  int PLAYER_1 = 0;
  int PLAYER_2 = 1;
  std::vector<int> turns;

  int player;

  Bot() {
    _isPlayer = false;
  }

  Bot(std::string name, std::string winCond = "row") : Bot() {
    _name = name;
    _winCond = winCond;
  }

  std::pair<int, int> makeMove(Board &board, const std::vector<int>& fTurns, int passStreak,
    std::ostream& log=std::cout) override {
    turns = fTurns;             // Доступные ходы
    Depth depth(turns.size());  // Глубина минимакса
    inf = board.max_num;
    std::pair<int, int> bestMove = minimax(board, depth, passStreak).first;
    // std::pair<int, int> bestMove = hint(board);

    board.decrease(bestMove.first, bestMove.second);
    return bestMove;
  }

  bool findEval(Board &board) {
    return evalMap.find(board.toString()) != evalMap.end();
  }


public:
  struct Depth {
    int start;
    int orig;
    int curr;

    Depth(int sstart) : start(sstart), orig(sstart), curr(sstart) {}

    Depth(int sstart, int oorig, int ccurr) : 
    start(sstart), orig(oorig), curr(ccurr) {}

    Depth next() const { return Depth{start, orig, curr - 1}; }  // TODO : ???

    Depth shallowing() const { return Depth{start, orig - 1, orig - 1}; }
  };

  double getBoardEval(Board &board, Depth depth, int passStreak) {
    double ret;
    if (findEval(board)) {
      ret = evalMap[board.toString()];
      dc1++;
    } else {
      ret = minimax(board, depth.next(), passStreak).second;
      dc2++;
    }
    return ret;
  }

  std::pair<std::pair<int, int>, double> minimax(Board &board, Depth depth, int passStreak = 0) {
    if (depth.curr <= 0 || board.hasWinner() != 0) {
      return {{-1, -1}, evaluateBoard(board)};
    }

    player = turns[depth.orig - depth.curr];
    std::pair<int, int> bestMove {-1, -1};
    double eval;
    double bestValue = (player == PLAYER_1)
                           ? -inf
                           : inf;

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
        if (board.toString() == "0 0 2 2 1 3 6 1 5 ") {
          std::cout << "";
        }
        eval = getBoardEval(board, depth, 0);
        // board.printBoard();
        // std::cout << eval << "\n";
        board.increase(x, y);
        player = turns[depth.orig - depth.curr];
        
        if ((player == PLAYER_1 && (eval > bestValue)) ||
            (player == PLAYER_2 && (eval < bestValue))) {
          bestValue = eval;
          bestMove = {x, y};
        }
      }
    }
    // Если все ходы бота ведут к поражению:
    if (bestMove.first == -1 && bestMove.second == -1 &&
    // доп условие, чтобы оценка глубокого хода была полной (если проиграл, то проиграл)
        depth.orig == depth.curr) {
      // Тогда уменьшаем количество просматриваемых ходов вперед.
      // Чтобы бот не боялся смерти, он должен "поглупеть".
      // Depth new_depth(depth.orig - 1);
      Depth new_depth = depth.shallowing();
      std::pair<std::pair<int, int>, double> res = minimax(board, new_depth, passStreak);

      bestMove = res.first;
      bestValue = res.second;
    }

    // pass turn
    if (passStreak < 2) {
      eval = getBoardEval(board, depth, ++passStreak);
      player = turns[depth.orig - depth.curr];
      if ((player == PLAYER_1 && (eval > bestValue)) ||
          (player == PLAYER_2 && (eval < bestValue))) {
        bestValue = eval;
        bestMove = {-2, -2};
      }
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
    double eur = 0;

    eur = (minCol - minRow);
    // возможный баг: если колонка равна строке, то 
    // оценка позиции не зависит от суммы колонки строки

    if (minRow == 0 && minCol == 0) {
      eur = koef * inf;
    } else {
      if (minRow == 0) {
        eur = inf;
      } else if (minCol == 0) {
        eur = -inf;
      } else {
        eur /= (minCol + minRow);
      }
      // Запись только если не зависит от того, кто выиграл.
      // evalMap[board.toString()] = eur;
    }
    return eur;
  }

public:
  void logEvals() {
    std::ofstream logFile;
    logFile.open("bot_eval_log.txt");
    logFile << dc1 << " " << dc2 << "\n";
    for (auto entry : evalMap) {
      logFile << entry.first << ": " << entry.second << '\n';
    }
    logFile.close();

    std::ofstream logExtraFile;
    logExtraFile.open("bot_extra_log.txt");
    for (auto entry : sekiStates) {
      logExtraFile << entry.first << ": ";
      for (auto ev : entry.second) {
        logExtraFile << ev << " ";
      }
      logExtraFile << '\n';
    }
    logExtraFile.close();
  }
};

#endif