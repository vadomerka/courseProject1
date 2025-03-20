#ifndef ITURNQUEUE_H
#define ITURNQUEUE_H

#include <vector>
#include <random>

struct ITurnQueue
{
  virtual int getCurrTurn() = 0;
  virtual std::vector<int> getNextTurns(int n) = 0;
  virtual void nextTurn() = 0;
};

#endif