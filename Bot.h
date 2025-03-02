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

  std::pair<int, int> makeMove(Board &board, const std::vector<int>& fTurns, std::ostream& log=std::cout) override {
    turns = fTurns;             // Доступные ходы
    Depth depth(turns.size());  // Глубина минимакса
    inf = board.max_num;
    std::pair<int, int> bestMove = minimax(board, depth).first;
    // std::pair<int, int> bestMove = hint(board);
    board.decrease(bestMove.first, bestMove.second);
    return bestMove;
  }

  bool findEval(Board &board) {
    return evalMap.find(board.toString()) != evalMap.end();
  }


public:
  struct Depth {
    int orig;
    int curr;

    Depth(int oorig) : orig(oorig), curr(oorig) {}

    Depth(int oorig, int ccurr) : orig(oorig), curr(ccurr) {}

    Depth next() const { return Depth{orig, curr - 1}; }  // TODO : ???
  };

  double getBoardEval(Board &board, Depth depth) {
    double ret;
    if (findEval(board)) {
      ret = evalMap[board.toString()];
      dc1++;
    } else {
      ret = minimax(board, depth.next()).second;
      dc2++;
      // Поиск вызывает ошибку terminated returned int
      // if (sekiSet.find(board.toString()) == sekiSet.end()) {
      // вызывает баг при зависимости от хода игрока
      //   evalMap[board.toString()] = ret;
      // }
    }
    return ret;
  }

  std::pair<std::pair<int, int>, double> minimax(Board &board, Depth depth) {
    if (depth.curr <= 0 || board.hasWinner() != 0) {
      return {{-1, -1}, evaluateBoard(board)};
    }

    player = turns[depth.orig - depth.curr];
    std::pair<int, int> bestMove {-1, -1};
    double eval;
    double bestValue = (player == PLAYER_1)
                           ? -inf
                           : inf;
    // DEBUG
    bool bestValueUpd = false;

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
        eval = getBoardEval(board, depth);
        // board.printBoard();
        // std::cout << eval << "\n";
        board.increase(x, y);
        player = turns[depth.orig - depth.curr];
        
        if ((player == PLAYER_1 && (bestValueUpd || eval > bestValue)) ||
            (player == PLAYER_2 && (bestValueUpd || eval < bestValue))) {
          bestValue = eval;
          bestMove = {x, y};
          bestValueUpd = false;
          // if (std::abs(bestValue) == inf) {
          //   return {bestMove, bestValue};
          // }
        }
      }
    }
    // Если все ходы бота ведут к поражению:
    if (bestMove.first == -1 && bestMove.second == -1 &&
        depth.orig == depth.curr) {
      // Тогда уменьшаем количество просматриваемых ходов вперед.
      // Чтобы бот не боялся смерти, он должен "поглупеть".
      Depth new_depth(depth.orig - 1);  // Глубина минимакса
      std::pair<std::pair<int, int>, double> res = minimax(board, new_depth);

      // depth изменяется на глубине, но при этом очередь начинается с самого начала.
      // попробовать: в отупение заходить только в начале очереди
      //            : сдвигать очередь

      bestMove = res.first;
      bestValue = res.second;
      bestValueUpd = false;
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
    // _winCond = (player == PLAYER_1) ? "row" : "col";
    // int koef = 1;
    double eur = 0;
    // if (minRow != minCol) {
    eur = (minCol - minRow);
    
    // }
    if (minRow == 0 && minCol == 0) {
      eur = koef * inf;
      sekiStates[board.toString()].push_back(eur);
      sekiSet.insert(board.toString());
    } else if (minRow == 0) {
      eur = inf;
      // eur = koef * (megaint + (minRow + minCol));
      evalMap[board.toString()] = eur;
    } else if (minCol == 0) {
      eur = -inf;
      evalMap[board.toString()] = eur;
    } else {
      eur /= (minCol + minRow);
      evalMap[board.toString()] = eur;
    }
    // std::cout << "minRow: " << minRow << '\t';
    // std::cout << "minCol: " << minCol << '\t';
    // std::cout << "eur: " << eur << '\n';
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