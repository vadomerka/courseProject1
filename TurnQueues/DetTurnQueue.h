#ifndef IDETTURNQUEUE_H
#define IDETTURNQUEUE_H

#include <vector>
#include <random>
// #include <ITurnQueue.h>
#include "ITurnQueue.h"


struct DetTurnQueue : public ITurnQueue
{
  std::vector<int> begining;
  std::vector<int> period;
  int turn;
  bool isInPeriod = false;

  DetTurnQueue(const std::vector<int> &bbegining, const std::vector<int> &pperiod)
  {
    begining = bbegining;
    period = pperiod;
    turn = 0;
  }

  int getCurrTurn()
  {
    if (isInPeriod)
    {
      return period[turn];
    }
    else
    {
      return begining[turn];
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
      if (turn >= begining.size())
      {
        isInPeriod = true;
        turn = 0;
      }
    }
  }
};

#endif