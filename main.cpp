#include "Board.h"
#include "Player.h"
#include "Bot.h"
#include <iostream>
#include <vector>
#include "TurnQueues/RandTurnQueue.h"
#include "TurnQueues/DetTurnQueue.h"
#include <fstream>


// Главный цикл игры.
// Аргументы: Board& - доска; std::vector<Player*>& - игроки;
// ITurnQueue& - очередь (либо детерминированная либо случайная);
// shownTurnsNum - сколько ходов вперед видят игроки.
// std::ostream& - куда выводить результаты.
void runGame(Board& b, std::vector<Player*>& players, ITurnQueue& tq, 
             int shownTurnsNum = 4, std::ostream& log = std::cout) {
  int winner = b.hasWinner();
  std::vector<int> tqv;
  int allTurns = 0;

  while (winner == 0 && players.size() > 1) {
    // Выводим доску.
    b.printBoard(log);
    // Получаем текущего игрока.
    int turn = tq.getCurrTurn();
    log << "Player " << turn + 1 << " turn.\n";
    // Получаем слудующие n ходов.
    tqv = tq.getNextTurns(shownTurnsNum);
    for (int i = 0; i < tqv.size(); i++) {
      log << tqv[i] << " ";
    }
    log << "\n";

    // Оптимальный ход по алгоритму 1.
    // std::pair<int, int> hint = players[turn]->hint(b);

    int turnDepth = std::min(shownTurnsNum, 40 + allTurns / 2);

    // Ход игрока. Бот ходит по алгоритму минимакса.
    // b.passStreak - сколько ходов было пропущено до этого.
    std::pair<int, int> change;
    if (players[turn]->_isPlayer) {
      change = players[turn]->makeMove(b, tqv, b.passStreak);
    } else {
      change = (dynamic_cast<Bot*>(players[turn]))->makeTimedMove(b, tqv, b.passStreak, turnDepth);
    }

    std::string type = ((players[turn]->_isPlayer) ? "Player" : "Bot");
    // Если игрок пропускает ход.
    if (change.first == -2) {
      log << type + " passes their turn!\n";
      b.passStreak++;
    } else if (change.first == -1) {
      log << type + " error!\n";
    } else {
      log << type + " decreases cell (" << change.first << ", " << change.second << ")!\n";
      b.passStreak = 0;
    }

    winner = b.hasWinner();
    tq.nextTurn();
    log << '\n';
    allTurns++;
  }

  b.printBoard(log);
  if (winner == 3) {
    // Если ничья, считаем кто последний занулил.
    winner = 1 - tq.getCurrTurn();
    log << players[winner]->_name;
    log << " - won!";
  } else if (!players.empty()) {
    log << players[winner - 1]->_name;
    log << " - won!";
  }

  // DEBUG!!
  players[0]->logEvals();
  // players[1]->logEvals();
  // DEBUG!!

  for (size_t i = 0; i < players.size(); i++) {
    delete players[i];
  }
}

int main() {
  std::vector<Player*> players{new Bot("b1", "row"), new Bot("b2", "col")};
  Board b(5, 5);
  b.fillBoard();
  // b.fillBoard({{2, 6, 6, 6, 6},
  //              {5, 6, 4, 4, 5},
  //              {6, 5, 3, 2, 5},
  //              {1, 5, 6, 6, 3},
  //              {6, 3, 3, 1, 4}});
  // b.fillBoard({{1, 1, 4},
  //   {2, 2, 3},
  //   {6, 1, 5}});
  // b.fillBoard({{1, 5, 5, 6, 5},
  //              {3, 1, 6, 2, 1},
  //              {6, 3, 5, 5, 2},
  //              {5, 6, 3, 5, 1},
  //              {5, 5, 3, 1, 3}});
  b.fillBoard({{1, 5, 5, 6, 0}, 
               {0, 0, 2, 2, 1}, 
               {6, 3, 5, 5, 0}, 
               {5, 6, 3, 5, 1}, 
               {5, 5, 3, 1, 3}});
  int turn = 0;
  
  DetTurnQueue tq {{0, 1}, {0, 0, 1, 1}};
  // RandTurnQueue tq {};
  
  std::ofstream logFile;
  logFile.open("game_log.txt");
  runGame(b, players, tq, 5, logFile);
  logFile.close();
  // char str_out;
  // std::cin >> str_out;
  return 0;
}
