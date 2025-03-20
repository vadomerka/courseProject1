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
    
    if (hint.first != change.first || hint.second != change.second) {
      log << "!!!";
    }
    log << "Optimal: " << hint.first << ", " << hint.second << '\n';

    winner = b.hasWinner();
    tq.nextTurn();
    log << '\n';
  }
  b.printBoard(log);
  if (winner == 3) {
    // log << "Draw!";
    winner = 1 - tq.getCurrTurn();
    log << players[winner]->_name;
    log << " - won!";
  } else if (!players.empty()) {
    log << players[winner - 1]->_name;
    log << " - won!";
  }

  // DEBUG!!
  players[0]->logEvals();
  // DEBUG!!

  for (size_t i = 0; i < players.size(); i++) {
    delete players[i];
  }
}

void testGame(std::vector<std::vector<int>>& tb, std::vector<int>& mokTurns) {
  if (tb.empty()) return;

  Board b(tb.size(), tb[0].size());
  b.fillBoard (tb);
  b.printBoard();
  Bot evaluer1 ("evaluer1", "row");
  Bot evaluer2 ("evaluer2", "col");
  auto res1 = evaluer1.makeMove(b, mokTurns, 0);
  auto res2 = evaluer2.makeMove(b, mokTurns, 0);
  std::cout << "result 1: " << res1.first << " " << res1.second << '\n';
  std::cout << "result 2: " << res2.first << " " << res2.second << '\n';
}

int main() {
  std::vector<Player*> players{new Bot("b1", "row"), new Bot("b2", "col")};
  Board b(3, 3);
  // b.fillBoard({{1, 1, 4},
  //              {2, 2, 3},
  //              {6, 1, 5}});
  b.fillBoard();
               
  DetTurnQueue tq {{0, 1}, {0, 0, 1, 1}};
  // DetTurnQueue tq {{1}, {1, 0, 0, 1}};
  
  std::ofstream logFile ("game_log.txt");
  runGame(b, players, tq, 5, std::cout);
  logFile.close();
  return 0;
}
