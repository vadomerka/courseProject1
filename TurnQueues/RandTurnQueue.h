#ifndef IRANDTURNQUEUE_H
#define IRANDTURNQUEUE_H

#include <vector>
#include <random>
// #include <ITurnQueue.h>
#include "ITurnQueue.h"


struct RandTurnQueue : public ITurnQueue
{
  std::random_device rd;
  std::mt19937 rng{rd()};
  std::vector<int> nextTurnVector;
  int minQueueSize = 10;

  RandTurnQueue(int n = 10)
  {
  }

  void generateQueue(int num)
  {
    for (size_t i = 0; i < num; i++)
    {
      std::uniform_int_distribution<int> uid(0, 1);
      nextTurnVector.push_back(uid(rng));
    }
  }

  virtual int getCurrTurn()
  {
    if (nextTurnVector.empty())
    {
      generateQueue(minQueueSize);
    }
    return nextTurnVector[0];
  }

  std::vector<int> getNextTurns(int n)
  {
    if (nextTurnVector.size() < n)
    {
      generateQueue(n);
    }
    std::vector<int> ret;
    for (int i = 0; i < n; i++)
    {
      ret.push_back(nextTurnVector[i]);
    }
    return ret;
  }

  void nextTurn()
  {
    nextTurnVector.erase(std::begin(nextTurnVector));
  }
};

#endif