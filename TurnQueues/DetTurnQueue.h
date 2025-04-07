#ifndef IDETTURNQUEUE_H
#define IDETTURNQUEUE_H

#include <vector>
#include <random>
// #include <ITurnQueue.h>
#include "ITurnQueue.h"


struct DetTurnQueue : public ITurnQueue
{
  std::vector<int> beginning;
  std::vector<int> period;
  int turn;
  bool isInPeriod = false;

  DetTurnQueue() {
    setStartPeriod({0}, {1, 0});
    turn = 0;
  }

  DetTurnQueue(const std::vector<int> &bbegining, const std::vector<int> &pperiod) : DetTurnQueue()
  {
    setStartPeriod(bbegining, pperiod);
  }

  bool setStartPeriod(const std::vector<int> &bbegining, const std::vector<int> &pperiod) {
    for (int n: bbegining) {
      if (n < 0 || n > 1) {
        return false;
      }
    }
    for (int n: pperiod) {
      if (n < 0 || n > 1) {
        return false;
      }
    }
    beginning = bbegining;
    period = pperiod;
    return true;
  }

  int getCurrTurn()
  {
    if (isInPeriod)
    {
      return period[turn];
    }
    else
    {
      return beginning[turn];
    }
  }

  std::vector<int> getNextTurns(int n)
  {
    bool oldFlag = isInPeriod;
    int oldTurn = turn;
    std::vector<int> ret;
    for (int i = 0; i < n; i++)
    {
      ret.push_back(getCurrTurn());
      nextTurn();
    }
    isInPeriod = oldFlag;
    turn = oldTurn;
    return ret;
  }

  void nextTurn()
  {
    if (isInPeriod)
    {
      turn = (turn + 1) % period.size();
    }
    else
    {
      turn++;
      if (turn >= beginning.size())
      {
        isInPeriod = true;
        turn = 0;
      }
    }
  }
};

#endif