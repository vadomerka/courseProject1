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
    
    log << "Game start!\n";
    log << "Rules: can pass: " + std::to_string(_canPass) + ", can draw: " + std::to_string(_canPass) + "\n";
    while (winner == 0 && players.size() > 1) {
      // Выводим доску.
      b.printBoard(log);
      // Получаем текущего игрока.
      int turn = tq.getCurrTurn();
      log << "Player " << turn + 1 << "'s turn.\n";
      // Получаем слудующие n ходов.
      tqv = tq.getNextTurns(shownTurnsNum);
      for (int i = 0; i < tqv.size(); i++) {
        log << tqv[i] << " ";
      }
      log << "\n";

      int turnDepth = std::min(shownTurnsNum, shownTurnsNum);

      // Ход игрока. Бот ходит по алгоритму минимакса.
      // b.passStreak - сколько ходов было пропущено до этого.
      std::pair<int, int> change;
      if (players[turn]->_isPlayer) {
        change = players[turn]->makeMove(b, tqv, _canPass, b._passStreak, turnDepth);
      } else {
        change = (dynamic_cast<Bot *>(players[turn]))
                     ->makeTimedMove(b, tqv, _canPass, b._passStreak, turnDepth);
      }

      std::string type = ((players[turn]->_isPlayer) ? "Player" : "Bot");
      // Если игрок пропускает ход.
      if (change.first == -2) {
        log << type + " passes the turn!\n";
        b._passStreak++;
      } else if (change.first == -1) {
        log << type + " error!\n";
      } else {
        log << type + " decreases cell (" << change.first << ", "
            << change.second << ")!\n";
        b._passStreak = 0;
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
        log << "The game ended in a draw. Friendship wins!\n";
      } else {
        winner = tq.getCurrTurn();
        log << players[winner]->_name;
        log << " wins!\n";
      }
    } else if (!players.empty()) {
      log << players[winner - 1]->_name;
      log << " wins!\n";
    }

    if (logStats) {
      if (players[0]->_isPlayer) players[0]->logEvals();
      if (players[1]->_isPlayer) players[1]->logEvals();
    }

    for (size_t i = 0; i < players.size(); i++) {
      delete players[i];
    }
  }

  bool getGameType() {
    std::cout << "Choose game type: 1/2\n";
    std::cout << "1. Seki (Last player to make a move wins)\n";
    std::cout << "2. D-Seki (If a zero row and column occur simultaneously, it's a draw):\n";
    char input = '1';
    std::cin >> input;
    _canDraw = false;
    if (input == '2') {
      _canDraw = true;
      std::cout << "D-Seki variant selected\n";
    } else {
      std::cout << "Seki variant selected\n";
    }
    return true;
  }

  bool getCanPass() {
    std::cout << "Can players pass their turn? y/n\n";
    _canPass = true;
    char input = 'n';
    std::cin >> input;
    if (input == 'n') {
      _canPass = false;
      std::cout << "Passing is disabled\n";
    } else {
      std::cout << "Passing is allowed\n";
    }
    return true;
  }

  bool getBoardSize() {
    std::cout << "Enter board size as number of rows followed by number of columns, separated by space.\n";
    std::cout << "Maximum board size is 10x10.\n";
    size_t rows;
    size_t cols;
    std::cin >> rows >> cols;
    if (rows > 10 || cols > 10 || rows * cols == 0) {
      std::cout << "Invalid input.\n";
      return false;
    }
    std::cout << "Board size: " << rows << " " << cols << ".\n";
    b.setSize(rows, cols);
    return true;
  }

  bool getBoardContents() {
    std::cout << "Fill the board with random values? y/n\n";
    char input = 'y';
    std::cin >> input;
    if (input != 'n') {
      b.fillBoard();
    } else {
      std::string boardString = "";
      std::cout << "Enter a string to initialize the board.\n";
      std::cout << "The string must contain " + std::to_string(b._height * b._width) + " positive numbers separated by space.\n";
      std::cin.ignore();
      std::getline(std::cin, boardString);
      if (!b.fillFromString(boardString)) {
        std::cout << "Invalid string format!\n";
        return false;
      }
    }
    std::cout << "Game board:\n";
    b.printBoard();
    return true;
  }

  bool getPlayerType(size_t ind) {
    std::cout << "Choose type of player " << ind + 1 << ": Player/Bot. 1/2\n";
    char input = '2';
    std::cin >> input;
    if (input == '1') {
      players[ind] = new Player("player " + std::to_string(ind + 1), "row");
      _arePlayers = true;
      std::cout << "Player created\n";
    } else {
      std::cout << "Bot created\n";
    }
    return true;
  }

  bool getTurnQueue() {
    char input = '1';
    std::cout << "Add normal, random, or deterministic turn queue? 1/2/3\n";
    std::cin >> input;
    if (input == '3') {
      std::cout << "Enter initial queue array, then period array. Arrays must contain only 0 and 1.\n";
      std::string start;
      std::string period;
      std::cin.ignore();
      std::getline(std::cin, start);
      std::getline(std::cin, period);
      auto sArr = Board::parseArr(start);
      auto pArr = Board::parseArr(period);
      auto dtq = DetTurnQueue();
      if (!dtq.setStartPeriod(sArr, pArr)) {
        std::cout << "Invalid queue format.\n";
        return false;
      }
      tq = new DetTurnQueue(sArr, pArr);
      std::cout << "Deterministic turn queue created.\n";
    } else if (input == '2') {
      tq = new RandTurnQueue();
      std::cout << "Random turn queue created.\n";
    } else {
      tq = new DetTurnQueue();
      std::cout << "Classic turn queue created.\n";
    }
    return true;
  }

  bool getTurnDepth() {
    _turnDepth = 1;
    do {
      std::cout << "Enter a positive integer - number of visible upcoming turns.\n";
      std::cin >> _turnDepth;
    } while (_turnDepth <= 0);
    return true;
  }

  bool showResult() {
    char input = '2';
    if (!_arePlayers) {
      std::cout << "Output results to file or console? 1/2\n";
      std::cin >> input;
    }
    std::ofstream logFile;
    if (input == '1') {
      std::cout << "Game results will be logged to \"game_log.txt\"\n";
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
    std::cout << "Game finished!";
    return true;
  }

  void errorMessage() {
    std::cout << "An error occured during game creation.\n";
  }

  void makeGame() {
    if (!getGameType()) { errorMessage(); return; };
    if (!getCanPass()) { errorMessage(); return; };

    if (!getBoardSize()) { errorMessage(); return; };
    if (!getBoardContents()) { errorMessage(); return; };

    if (!getPlayerType(0)) { errorMessage(); return; };
    if (!getPlayerType(1)) { errorMessage(); return; };

    if (!getTurnQueue()) { errorMessage(); return; };
    if (!getTurnDepth()) { errorMessage(); return; };

    showResult();
  }
};

#endif
