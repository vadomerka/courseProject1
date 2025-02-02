#include "Board.h"
#include "Player.h"
#include "Bot.h"
#include <iostream>
#include <vector>
#include <random>

// using namespace std;

// std::vector<int> makeTurnList(const std::vector<int>& begining, const std::vector<int>& period, int n = 10) {
//   std::vector<int> turnList {begining};
//   for (int i = 0; i < n; i++) {
//     turnList.emplace_back(period);
//   }
//   return turnList;
// }

struct ITurnQueue {
  virtual int getCurrTurn() = 0;
  virtual std::vector<int> getNextTurns(int n) = 0;
  virtual void nextTurn() = 0;
};

struct RandTurnQueue : public ITurnQueue {
  std::random_device rd;
  std::mt19937 rng{rd()};
  // std::uniform_int_distribution<int> uid(1, 6);
  // uid(rng);
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


void runGame(Board& b, std::vector<Player*>& players, DetTurnQueue& tq) {
  int winner = b.hasWinner();
  std::vector<int> tqv;

  while (winner == 0 && players.size() > 1) {
    // turns
    std::cout << '\n';
    b.printBoard();
    int turn = tq.getCurrTurn();
    std::cout << "Player " << turn + 1 << " turn.\n";
    tqv = tq.getNextTurns(4);
    for (int i = 0; i < tqv.size(); i++) {
      std::cout << tqv[i] << " ";
    }
    std::cout << "\n";

    // std::cout << "winner = " << winner << "\n";
    std::pair<int, int> change = players[turn]->makeMove(b, tqv);
    std::cout << "Bot decreases cell (" << change.first << ", " << change.second << ")!\n";

    winner = b.hasWinner();
    // turn = (turn + 1) % players.size();
    tq.nextTurn();
  }
  b.printBoard();
  if (winner == 3) {
    std::cout << "Draw!";
  } else if (!players.empty()) {
    std::cout << players[winner - 1]->_name;
    std::cout << " - won!";
  }

  for (size_t i = 0; i < players.size(); i++) {
    delete players[i];
  }
}

void testGame() {
  Board b(3, 3);
  b.fillBoard({{1, 1, 4},
               {2, 2, 3},
               {6, 0, 5}});
  b.printBoard();
  Bot evaluer1 ("evaluer1", "row");
  Bot evaluer2 ("evaluer2", "col");
  std::cout << "Evaluation: " << evaluer1.evaluateBoard(b) << '\n';
}

int main() {
  // setlocale(LC_ALL, "Russian");
  std::vector<Player*> players{new Bot("b1", "row"), new Bot("b2", "col")};
  Board b(3, 3);
  // b.fillBoard();
  b.fillBoard({{1, 1, 4},
               {2, 2, 3},
               {6, 1, 5}});
  int turn = 0;
  
  DetTurnQueue tq {{0, 1}, {0, 0, 1, 1}};
  // RandTurnQueue tq {};
  
  // testGame();
  runGame(b, players, tq);

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
