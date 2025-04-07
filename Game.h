#ifndef SEKI_GAME_H
#define SEKI_GAME_H

#include "Board.h"
#include "Bot.h"
#include "Player.h"
#include "TurnQueues/DetTurnQueue.h"
#include "TurnQueues/RandTurnQueue.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <format>

#define log (*slog)

class Game {
public:
  bool _canDraw;
  bool _canPass;
  Board b;
  bool _arePlayers;
  std::vector<Player*> players;
  ITurnQueue *tq;
  int _turnDepth;

  Game(bool canDraw = false, bool canPass = true) {
    _canDraw = canDraw;
    _canPass = canPass;
    _arePlayers = false;
    players = {new Bot("bot 1", "row"), new Bot("bot 2", "col")};
  }

  // Главный цикл игры.
  // Аргументы: Board& - доска; std::vector<Player*>& - игроки;
  // ITurnQueue& - очередь (либо детерминированная либо случайная);
  // shownTurnsNum - сколько ходов вперед видят игроки.
  // std::ostream& - куда выводить результаты.
  void runGame(Board &b, std::vector<Player *> &players, ITurnQueue &tq,
               int shownTurnsNum = 4, std::ostream *slog = &std::cout, bool logStats = false) {
    int winner = b.hasWinner();
    std::vector<int> tqv;
    int allTurns = 0;

    while (winner == 0 && players.size() > 1) {
      // Выводим доску.
      b.printBoard(log);
      // Получаем текущего игрока.
      int turn = tq.getCurrTurn();
      log << "Ход игрока " << turn + 1 << ".\n";
      // Получаем слудующие n ходов.
      tqv = tq.getNextTurns(shownTurnsNum);
      for (int i = 0; i < tqv.size(); i++) {
        log << tqv[i] << " ";
      }
      log << "\n";

      // Оптимальный ход по алгоритму 1.
      // std::pair<int, int> hint = players[turn]->hint(b);
      // log << hint.first << " " << hint.second << '\n';

      int turnDepth = std::min(shownTurnsNum, shownTurnsNum);

      // Ход игрока. Бот ходит по алгоритму минимакса.
      // b.passStreak - сколько ходов было пропущено до этого.
      std::pair<int, int> change;
      if (players[turn]->_isPlayer) {
        change = players[turn]->makeMove(b, tqv, b.passStreak, _canPass);
      } else {
        change = (dynamic_cast<Bot *>(players[turn]))
                     ->makeTimedMove(b, tqv, b.passStreak, turnDepth, _canPass);
        // change = (dynamic_cast<Bot*>(players[turn]))->makeMove("");
      }

      std::string type = ((players[turn]->_isPlayer) ? "Игрок" : "Бот");
      // Если игрок пропускает ход.
      if (change.first == -2) {
        log << type + " пропускает ход!\n";
        b.passStreak++;
      } else if (change.first == -1) {
        log << type + " ошибка!\n";
      } else {
        log << type + " уменьшает ячейку (" << change.first << ", "
            << change.second << ")!\n";
        b.passStreak = 0;
      }

      winner = b.hasWinner();
      if (!winner)
        tq.nextTurn();
      log << '\n';
      allTurns++;
    }

    b.printBoard(log);
    if (winner == 3) {
      if (_canDraw) {
        log << "Игра завершилась ничьей. Победила дружба!\n";
      } else {
        // Считаем кто последний занулил.
        winner = tq.getCurrTurn();
        log << players[winner]->_name;
        log << " - победил!\n";
      }
    } else if (!players.empty()) {
      log << players[winner - 1]->_name;
      log << " - победил!\n";
    }

    if (logStats) {
      if (players[0]->_isPlayer) players[0]->logEvals();
      if (players[1]->_isPlayer) players[1]->logEvals();
    }

    for (size_t i = 0; i < players.size(); i++) {
      delete players[i];
    }
  }

  void getGameType() {
    std::cout << "Выберете тип игры (чтобы выбрать, введите число):\n";
    std::cout << "1. Секи (Выигрывает тот, кто сделал последний ход)\n";
    std::cout << "2. Н-Секи (Если нулевые строка и столбец возникли одновременно, объявляется ничья):\n";
    char input = '1';
    std::cin >> input;
    _canDraw = false;
    if (input == '2') {
      _canDraw = true;
      std::cout << "Выбран вариант Н-Секи\n";
    } else {
      std::cout << "Выбран вариант Секи\n";
    }
  }

  void getCanPass() {
    std::cout << "Можно ли пропускать ход? y/n\n";
    _canPass = true;
    char input = 'n';
    std::cin >> input;
    if (input == 'n') {
      _canPass = false;
      std::cout << "Выбран вариант нельзя\n";
    } else {
      std::cout << "Выбран вариант можно\n";
    }
  }

  void getBoardSize() {
    std::cout << "Введите размер доски в формате число строк, затем число "
                 "столбцов через пробел.\n";
    std::cout << "Максимальный размер доски - 10x10.\n";
    size_t rows;
    size_t cols;
    std::cin >> rows >> cols;
    if (rows > 10 || cols > 10 || rows * cols == 0) {
      std::cout << "Неверные данные.\n";
      return;
    }
    b.setSize(rows, cols);
  }

  void getBoardContents() {
    std::cout << "Заполнить доску случайными значениями? y/n\n";
    char input = 'y';
    std::cin >> input;
    if (input == 'y') {
      b.fillBoard();
    } else {
      std::string boardString = "";
      std::cout << "Введите строку для инициализации доски.\n";
      std::cout << "Строка должна содержать " + std::to_string(b._height * b._width) +
                       " положительных чисел через пробел.\n";
      std::getline(std::cin, boardString);
      std::getline(std::cin, boardString);
      if (!b.fillFromString(boardString)) {
        std::cout << "Строка неверного формата!\n";
        return;
      }
    }
  }

  void getPlayerType(size_t ind) {
    std::cout << "Выберете тип первого игрока Player/Bot. 1/2\n";
    char input = '2';
    std::cin >> input;
    if (input == '1') {
      players[ind] = new Player("player 1", "row");
      _arePlayers = true;
      std::cout << "Создан игрок Player\n";
    } else {
      std::cout << "Создан игрок Bot\n";
    }
  }

  void getTurnQueue() {
    char input = '1';
    std::cout << "Добавить обычную, случайную или детерминированную очередь? 1/2/3\n";
    std::cin >> input;
    if (input == '1') {
      tq = new DetTurnQueue();
    } else if (input == '2') {
      tq = new RandTurnQueue();
    } else {
      std::cout << "Введите массив начала очереди, затем периода очереди. "
                   "Массивы должны содержать только 0 и 1.\n";
      std::string start;
      std::string period;
      std::getline(std::cin, start);
      std::getline(std::cin, start);
      std::getline(std::cin, period);
      auto sArr = Board::parseArr(start);
      auto pArr = Board::parseArr(period);
      auto dtq = DetTurnQueue();
      if (!dtq.setStartPeriod(sArr, pArr)) {
        std::cout << "Неверный формат очереди.\n";
        return;
      }
      tq = new DetTurnQueue(sArr, pArr);
    }
  }

  void getTurnDepth() {
    _turnDepth = 1;
    do {
      std::cout
          << "Введите натуральное число - количество просматриваемых ходов.\n";
      std::cin >> _turnDepth;
    } while (_turnDepth <= 0);
  }

  void showResult() {
    char input = '2';
    if (!_arePlayers) {
      std::cout << "Вывести результаты в файл или в консоль? 1/2\n";
      std::cin >> input;
    }
    std::ofstream logFile;
    std::cout << "Запуск игры.\n";
    if (input == '1') {
      std::ostream *logFile = &std::cout;
      static std::ofstream outFile;
      if (!outFile.is_open()) {
        outFile.open("game_log.txt");
      }
      logFile = &outFile;
      runGame(b, players, *tq, _turnDepth, logFile);
      outFile.close();
    } else {
      runGame(b, players, *tq, _turnDepth);
    }
  }

  void makeGame() {
    getGameType();
    getCanPass();

    getBoardSize();
    getBoardContents();
    
    getPlayerType(0);
    getPlayerType(1);

    getTurnQueue();
    getTurnDepth();
    
    showResult();
  }
};

#endif