#include "Board.h"
#include "Player.h"
#include "Bot.h"
#include <iostream>
#include <vector>
#include <random>
#include <fstream>

struct ITurnQueue {
  virtual int getCurrTurn() = 0;
  virtual std::vector<int> getNextTurns(int n) = 0;
  virtual void nextTurn() = 0;
};

struct RandTurnQueue : public ITurnQueue {
  std::random_device rd;
  std::mt19937 rng{rd()};
  std::vector<int> nextTurnVector;
  int minQueueSize = 10;

  RandTurnQueue(int n = 10) {

  }

  void generateQueue(int num) {
    for (size_t i = 0; i < num; i++) {
      std::uniform_int_distribution<int> uid(0, 1);
      nextTurnVector.push_back(uid(rng));
    }
  }

  virtual int getCurrTurn() {
    if (nextTurnVector.empty()) {
      generateQueue(minQueueSize);
    }
    return nextTurnVector[0];
  }

  std::vector<int> getNextTurns(int n) {
    if (nextTurnVector.size() < n) {
      generateQueue(n);
    }
    std::vector<int> ret;
    for (int i = 0; i < n; i++) {
      ret.push_back(nextTurnVector[i]);
    }
    return ret;
  }

  void nextTurn() {
    nextTurnVector.erase(std::begin(nextTurnVector));
  }
};

struct DetTurnQueue : public ITurnQueue {
  std::vector<int> begining;
  std::vector<int> period;
  int turn;
  bool isInPeriod = false;

  DetTurnQueue(const std::vector<int>& bbegining, const std::vector<int>& pperiod) {
    begining = bbegining;
    period = pperiod;
    turn = 0;
  }

  int getCurrTurn() {
    if (isInPeriod) {
      return period[turn];
    }
    else {
      return begining[turn];
    }
  }

  std::vector<int> getNextTurns(int n) {
    bool oldFlag = isInPeriod;
    int oldTurn = turn;
    std::vector<int> ret;
    for (int i = 0; i < n; i++) {
      ret.push_back(getCurrTurn());
      nextTurn();
    }
    isInPeriod = oldFlag;
    turn = oldTurn;
    return ret;
  }

  void nextTurn() {
    if (isInPeriod) {
      turn = (turn + 1) % period.size();
    }
    else {
      turn++;
      if (turn >= begining.size()) {
        isInPeriod = true;
        turn = 0;
      }
    }
  }
};


void runGame(Board& b, std::vector<Player*>& players, DetTurnQueue& tq, 
             int shownTurnsNum = 4, std::ostream& log = std::cout) {
  int winner = b.hasWinner();
  std::vector<int> tqv;

  while (winner == 0 && players.size() > 1) {
    // turns
    log << '\n';
    b.printBoard(log);
    int turn = tq.getCurrTurn();
    log << "Player " << turn + 1 << " turn.\n";
    tqv = tq.getNextTurns(shownTurnsNum);
    for (int i = 0; i < tqv.size(); i++) {
      log << tqv[i] << " ";
    }
    log << "\n";

    std::pair<int, int> hint = players[turn]->hint(b);

    std::pair<int, int> change = players[turn]->makeMove(b, tqv);

    std::string type = ((players[turn]->_isPlayer) ? "Player" : "Bot");
    log << type + " decreases cell (" << change.first << ", " << change.second << ")!\n";
    
    if (hint.first != change.first || hint.second != change.second) {
      log << "!!!";
    }
    log << "Optimal: " << hint.first << ", " << hint.second << '\n';

    winner = b.hasWinner();
    tq.nextTurn();
  }
  b.printBoard(log);
  if (winner == 3) {
    log << "Draw!";
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

void testGame() {
  Board b(3, 3);
  b.fillBoard({{0, 9, 1},
               {0, 9, 1},
               {1, 0, 2}});
  b.printBoard();
  Bot evaluer1 ("evaluer1", "row");
  Bot evaluer2 ("evaluer2", "col");
  std::vector<int> mokTurns {1, 1, 1};
  auto res = evaluer2.makeMove(b, mokTurns);
  std::cout << "result: " << res.first << " " << res.second << '\n';
}

int main() {
  std::vector<Player*> players{new Bot("b1", "row"), new Bot("b2", "col")};
  // Board b(4, 4);
  Board b(3, 3);
  // Board b(2, 2);
  // b.fillBoard();
  b.fillBoard({{1, 1, 4},
               {2, 2, 3},
               {6, 1, 5}});
  // b.fillBoard ({{0, 1, 2},
  //               {2, 0, 3},
  //               {6, 0, 5}});
  // b.fillBoard({{3, 6, 4},
  //              {6, 5, 5},
  //              {5, 2, 3}});
  // b.fillBoard({{3, 6, 4, 10},
  //              {6, 5, 5, 10},
  //              {5, 2, 3, 10}, 
  //              {5, 2, 3, 10}});
  // b.fillBoard({{2, 0},
  //              {0, 2}});
  // b.fillBoard({{10, 10, 10}});
  int turn = 0;
  
  DetTurnQueue tq {{0, 1}, {0, 0, 1, 1}};
  // RandTurnQueue tq {};
  
  // testGame();
  std::ofstream logFile;
  logFile.open("game_log.txt");
  runGame(b, players, tq, 4, logFile);

/*
3   6   4
6   5   5
5   2   3

6   3   1   
4   2   2
4   4   6

1   1   4
2   2   3   
6   1   5
*/

  return 0;
}
