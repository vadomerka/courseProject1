#include "Board.h"
#include "Player.h"
#include "Bot.h"
#include <iostream>
#include <vector>
#include "TurnQueues/RandTurnQueue.h"
#include "TurnQueues/DetTurnQueue.h"
#include <fstream>


void runGame(Board& b, std::vector<Player*>& players, DetTurnQueue& tq, 
             int shownTurnsNum = 4, std::ostream& log = std::cout) {
  int winner = b.hasWinner();
  std::vector<int> tqv;

  while (winner == 0 && players.size() > 1) {
    // turns
    b.printBoard(log);
    int turn = tq.getCurrTurn();
    log << "Player " << turn + 1 << " turn.\n";
    tqv = tq.getNextTurns(shownTurnsNum);
    for (int i = 0; i < tqv.size(); i++) {
      log << tqv[i] << " ";
    }
    log << "\n";

    std::pair<int, int> hint = players[turn]->hint(b);
    log << "Optimal " << hint.first << " " << hint.second << '\n';
    // Ход бота.
    std::pair<int, int> change = players[turn]->makeMove(b, tqv, b.passStreak);

    std::string type = ((players[turn]->_isPlayer) ? "Player" : "Bot");
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
    if (!winner) tq.nextTurn();
    log << '\n';
  }
  b.printBoard(log);
  if (winner == 3) {
    winner = tq.getCurrTurn();
    log << players[winner]->_name;
    log << " - won!";
  } else if (!players.empty()) {
    log << players[winner - 1]->_name;
    log << " - won!";
  }

  players[0]->logEvals();

  for (size_t i = 0; i < players.size(); i++) {
    delete players[i];
  }
}

int main() {
  std::vector<Player*> players{new Bot("b1", "row"), new Bot("b2", "col")};
  Board b(3, 3);
  // b.fillBoard();
  b.fillBoard({{1, 1, 4},
               {2, 2, 3},
               {6, 1, 5}});
  // b.fillBoard({{1, 5, 5, 6, 5},
  //              {3, 1, 6, 2, 1},
  //              {6, 3, 5, 5, 2},
  //              {5, 6, 3, 5, 1},
  //              {5, 5, 3, 1, 3}});
  // b.fillBoard({{1, 5, 5, 6, 0}, 
  //              {0, 0, 2, 2, 1}, 
  //              {6, 3, 5, 5, 0}, 
  //              {5, 6, 3, 5, 1}, 
  //              {5, 5, 3, 1, 3}});

  int turn = 0;
  
  DetTurnQueue tq {{0, 1}, {0, 0, 1, 1}};
  // DetTurnQueue tq {{1}, {0, 0, 1, 1}};
  // RandTurnQueue tq {};
  
  std::ofstream logFile;
  logFile.open("game_log.txt");
  runGame(b, players, tq, 5, logFile);  // начиная с 3 не решает доску
  logFile.close();
  return 0;
}
