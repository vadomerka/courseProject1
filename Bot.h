#ifndef BOT_H
#define BOT_H

#include "Board.h"
#include "Player.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include <string>

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
  size_t _maxVarietyDepth = 3;

  int player;

  double max_td = 0;
  double td_sum = 0;
  int64_t td_count = 0;

  std::vector<double> td_vector;

  Bot() { 
    _isPlayer = false;
  }

  Bot(std::string name, std::string winCond = "row", size_t maxVarietyDepth = 3) : Bot() {
    _name = name;
    _winCond = winCond;
    _maxVarietyDepth = maxVarietyDepth;
  }

  std::pair<int, int> makeMove(Board &board, const std::vector<int>& fTurns, bool canPass, int passStreak, 
    int turnDepth = 4, std::ostream& log=std::cout) override {
    turns = fTurns;                                         // Доступные ходы
    Depth depth(turnDepth == 0 ? turns.size() : turnDepth); // Глубина минимакса
    inf = board.max_num; // Сумма доски (наибольшее значение оценки)

    std::pair<int, int> bestMove = minimax(board, depth, canPass, passStreak).first;

    board.decrease(bestMove.first, bestMove.second);
    return bestMove;
  }

  // Функция для замера скорости алгоритма.
  std::pair<int, int> makeTimedMove(Board &board, const std::vector<int>& fTurns, bool canPass, int passStreak, 
    int turnDepth = 4, std::ostream& log=std::cout) {
    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();
    std::pair<int, int> change =
        makeMove(board, fTurns, canPass, passStreak, turnDepth, log);
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    double td =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count();

    max_td = std::max(max_td, td);
    td_sum += td;
    td_count++;
    td_vector.push_back(td);
    return change;
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
  double getBoardEval(Board &board, Depth depth, bool canPass, int passStreak) {
    return minimax(board, depth.next(), canPass, passStreak).second;
  }

  // Основная функция высчета лучшего хода.
  // Возвращает: лучший ход и оценку доски при этом ходе.
  std::pair<std::pair<int, int>, double> minimax(Board &board, Depth depth,
    bool canPass, int passStreak = 0) {
    // Если рекурсия достигла конца очереди ходов, или уже определен победитель
    // - заканчиваем оценку.
    if (depth.curr <= 0 || board.hasWinner() != 0) {
      double eur = 0;
      if (findEval(board)) {
        eur = evalMap[board.toString()];
      } else {
        eur = evaluateBoard(board);
      }
      return {{-1, -1}, eur};
    }

    // Получаем игрока.
    player = turns[depth.orig - depth.curr];
    // Инициализация искомых значений.
    std::pair<int, int> bestMove{-1, -1};
    double eval = 0;
    // Идея алгоритма:
    // Первый игрок пытается получить наибольшую оценку доски.
    // Второй игрок - наименьшую.
    double bestValue = (player == PLAYER_1) ? -inf : inf;
    // Расчет наиболее выгодных строк/столбцов.
    auto rowP = calcRowPriority(board);
    auto colP = calcColPriority(board);
    auto priorities1 = (player == PLAYER_1) ? rowP : colP;
    auto priorities2 = (player == PLAYER_1) ? colP : rowP;
    std::reverse(priorities2.begin(), priorities2.end());

    // Ограничиваем бота на просмотр лишь нескольких строк/столбцов, лучших по приоритету.
    int pCount = std::min(priorities1.size(), _maxVarietyDepth);
    for (int j = 0; j < pCount; j++) {
      int p1 = priorities1[j];
      for (int i = 0; i < priorities2.size(); ++i) {
        int p2 = priorities2[i];
        int x = (player == PLAYER_1) ? p1 : p2;
        int y = (player == PLAYER_1) ? p2 : p1;

        if (board.getValue(x, y) == 0)
          continue;

        board.decrease(x, y);

        eval = getBoardEval(board, depth, canPass, 0);

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
        // доп условие, чтобы оценка глубокого хода была полной (если проиграл,
        // то проиграл)
        depth.orig == depth.curr) {
      // Тогда уменьшаем количество просматриваемых ходов вперед.
      // Чтобы бот не боялся смерти, он должен "поглупеть".
      // Depth new_depth(depth.orig - 1);
      Depth new_depth = depth.shallowing();
      std::pair<std::pair<int, int>, double> res =
          minimax(board, new_depth, passStreak);

      bestMove = res.first;
      bestValue = res.second;
    } else {
      // pass turn
      if (canPass && passStreak < 2) {
        eval = getBoardEval(board, depth, canPass, ++passStreak);
        player = turns[depth.orig - depth.curr];
        if ((player == PLAYER_1 && (eval >= bestValue) && bestValue != inf) ||
            (player == PLAYER_2 && (eval <= bestValue) && bestValue != -inf)) {
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
        (*std::min_element(riSums.begin(), riSums.end(), [](auto &a, auto &b) {
          return a.second < b.second;
        })).second;
    double minCol =
        (*std::min_element(ciSums.begin(), ciSums.end(), [](auto &a, auto &b) {
          return a.second < b.second;
        })).second;
    int koef = (player == PLAYER_1) ? 1 : -1;
    double eur = 0;

    eur = (minCol - minRow);

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
    return eur;
  }

public:
  // Debug logs
  void logEvals() {
    std::ofstream logFile;
    logFile.open("bot_eval_log.txt");
    // logFile << dc1 << " " << dc2 << "\n";
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