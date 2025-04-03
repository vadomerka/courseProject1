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
#include <chrono>


// Дефолтное значение бесконечности, впоследствии заменяется.
long long inf = 1000000;
// Массив, хранящий уже оцененные состояния доски.
std::map<std::string, double> evalMap;
// Debug logs
std::map<std::string, std::vector<double>> sekiStates;


class Bot : public Player {
public:
  int PLAYER_1 = 0;
  int PLAYER_2 = 1;
  std::vector<int> turns;

  int player;

  // Time Statistics.
  double max_td = 0;
  double td_sum = 0;
  int64_t td_count = 0;
  std::vector<double> td_vector;

  Bot() {
    _isPlayer = false;
  }

  Bot(std::string name, std::string winCond = "row") : Bot() {
    _name = name;
    _winCond = winCond;
  }

  std::pair<int, int> makeMove(Board &board, const std::vector<int>& fTurns, int passStreak,
    int turnDepth = 0, std::ostream& log=std::cout) override {
    turns = fTurns;  // Доступные ходы
    Depth depth(turnDepth == 0 ? turns.size() : turnDepth);  // Глубина минимакса
    inf = board.max_num;  // Сумма доски (наибольшее значение оценки)

    std::pair<int, int> bestMove = minimax(board, depth, passStreak).first;

    board.decrease(bestMove.first, bestMove.second);
    return bestMove;
  }

private:
  // Структура для отслеживания глубины рекурсии и того, чей сейчас ход.
  struct Depth {
    int orig;
    int curr;

    Depth(int sstart) : orig(sstart), curr(sstart) {}

    Depth(int oorig, int ccurr) : orig(oorig), curr(ccurr) {}

    Depth next() const { return Depth{orig, curr - 1}; }

    Depth shallowing() const { return Depth{orig - 1, orig - 1}; }
  };

  // Вспомогательная функция для проверки была ли оценена доска.
  bool findEval(Board &board) {
    return evalMap.find(board.toString()) != evalMap.end();
  }

  // Вспомогательная функция получения оценки доски.
  double getBoardEval(Board &board, Depth depth, int passStreak) {
    double ret;
    if (findEval(board)) {
      // Если доска уже была оценена, возвращаем это значение.
      ret = evalMap[board.toString()];
    } else {
      // Иначе высчитываем это значение.
      ret = minimax(board, depth.next(), passStreak).second;
    }
    return ret;
  }

  // Основная функция высчета лучшего хода.
  // Возвращает: лучший ход и оценку доски при этом ходе.
  std::pair<std::pair<int, int>, double> minimax(Board &board, Depth depth, int passStreak = 0) {
    // Если рекурсия достигла конца очереди ходов, или уже определен победитель - заканчиваем оценку.
    if (depth.curr <= 0 || board.hasWinner() != 0) {
      return {{-1, -1}, evaluateBoard(board)};
    }

    // Получаем игрока.
    player = turns[depth.orig - depth.curr];
    // Инициализация искомых значений.
    std::pair<int, int> bestMove {-1, -1};
    double eval = 0;
    // Идея алгоритма:
    // Первый игрок пытается получить наибольшую оценку доски.
    // Второй игрок - наименьшую.
    double bestValue = (player == PLAYER_1)
                           ? -inf
                           : inf;
    // Расчет наиболее выгодных строк/столбцов.
    auto priorities =
        (player == PLAYER_1) ? calcRowPriority(board) : calcColPriority(board);
    
    int pCount = std::min(priorities.size(), (size_t)5);
    for (int j = 0; j < pCount; j++) {
      int p = priorities[j];
      for (int i = 0;
           i < ((player == PLAYER_1) ? board.getWidth() : board.getHeight());
           ++i) {
        int x = (player == PLAYER_1) ? p : i;
        int y = (player == PLAYER_1) ? i : p;

        if (board.getValue(x, y) == 0)
          continue;

        board.decrease(x, y);

        if (board.toString() == "1 5 5 6 0 0 0 2 2 1 6 3 5 4 0 5 6 3 5 1 5 5 3 1 3 ") {
          std::cout << "";
        }
        eval = getBoardEval(board, depth, 0);
        if (0.11 <= eval && eval <= 0.12) {
          std::cout << "";
        }

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
    } else {
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
        // eur *= inf / (minCol + minRow);
      }
      // Запись только если не зависит от того, кто выиграл.
      evalMap[board.toString()] = eur;
    }
    if (0.11 <= eur && eur <= 0.12) {
      std::cout << "";
    }
    return eur;
  }

public:
  // Debug logs
  void logEvals() {
    std::ofstream logFile;
    logFile.open("bot_eval_log.txt");
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

    std::ofstream logBotStats;
    logBotStats.open("bot_stats_log" + _name + ".txt");
    logBotStats << "Max solve time: " << max_td << '\n';
    double av_td = td_sum / td_count;
    logBotStats << "Average solve time: " << av_td << '\n';
    logBotStats << "Sum solve time: " << td_sum << '\n';
    logBotStats << "Count solve time: " << td_count << '\n';
    for (auto td : td_vector) {
      logBotStats << td << '\n';
    }
    logBotStats.close();
  }
};

#endif