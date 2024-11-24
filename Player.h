#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <vector>
#include "Card.h"

class Player {
public:
  std::string _name = "";
  float _balance = 0.0;
  float _starting_balance = 0.0;
  std::vector<Card> _hand;

public:
  Player() {}

  Player(std::string name) : _name(name) {}

  Player(std::string name, float balance, float start_b)
      : _name(name), _balance(balance), _starting_balance(start_b) {
    if (balance < 0) {
      _balance = 0.0;
    }
    if (start_b < 0) {
      _starting_balance = 0.0;
    }
  }

  void addCard(const Card& card) {
    _hand.emplace_back(card);
  }

  size_t get_hand_size() const { return _hand.size(); }

  int count_Hand() {
    int res = 0;
    int x = 0;
    // If suits are equal.
    for (size_t suit = 1; suit <= 4; suit++) {
      x = 0;
      for (int i = 0; i < _hand.size(); i++) {
        if (_hand[i].get_suit() == suit || _hand[i].isJocker()) {
          x += _hand[i].get_value();
        }
      }
      res = std::max(res, x);
    }
    // If ranks are equal.
    for (size_t rank = 1; rank <= 13; rank++) {
      x = 0;
      for (int i = 0; i < _hand.size(); i++) {
        if (_hand[i].get_rank() == rank || _hand[i].isJocker()) {
          x += _hand[i].get_value();
        }
      }
      res = std::max(res, x);
    }
    return res;
  }
};

#endif