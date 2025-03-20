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
    winner = 1 - tq.getCurrTurn();
    log << players[winner]->_name;
    log << " - won!";
  } else if (!players.empty()) {
    log << players[winner - 1]->_name;
    log << " - won!";
  }

  // DEBUG!!
  // players[0]->logEvals();
  // DEBUG!!

  for (size_t i = 0; i < players.size(); i++) {
    delete players[i];
  }
}

int main() {
  std::vector<Player*> players{new Bot("b1", "row"), new Bot("b2", "col")};
  Board b(3, 3);
  b.fillBoard();
  // b.fillBoard({{1, 1, 4},
  //              {2, 2, 3},
  //              {6, 1, 5}});
  int turn = 0;
  
  DetTurnQueue tq {{0, 1}, {0, 0, 1, 1}};
  // RandTurnQueue tq {};
  
  std::ofstream logFile;
  logFile.open("game_log.txt");
  runGame(b, players, tq, 5, logFile);
  logFile.close();

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
