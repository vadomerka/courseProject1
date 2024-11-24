#ifndef CARD_H
#define CARD_H

#include <ctime>
#include <iostream>
#include <string>

using namespace std;

class Card {
private:
  int _rank = 0;
  int _suit = 0;
  int _value = 0;
  bool _is_jocker = false;

public:
  Card() {}

  Card(int rank, int suit) : _rank(rank), _suit(suit) { 
    if (rank > 10) { _value = 10; }
    else if (rank == 1) { _value = 11; }
    else { _value = rank; }
  }

  Card(const Card& other) { 
    _rank = other._rank;
    _suit = other._suit;
    _value = other._value;
    _is_jocker = other._is_jocker;
  }

  Card(const Card&& other) { 
    _rank = other._rank;
    _suit = other._suit;
    _value = other._value;
    _is_jocker = other._is_jocker;
  }

  Card operator=(const Card& other) {
    if (this == &other) {
      return *this;
    }
    _rank = other._rank;
    _suit = other._suit;
    _value = other._value;
    _is_jocker = other._is_jocker;
    return *this;
  }

  Card operator=(const Card&& other) {
    _rank = other._rank;
    _suit = other._suit;
    _value = other._value;
    _is_jocker = other._is_jocker;
    return *this;
  }

  int get_rank() const { return _rank; }

  string get_rank_string() const {
    if (_rank == 1)
      return "Ace";
    if (_rank == 2)
      return "Two";
    if (_rank == 3)
      return "Three";
    if (_rank == 4)
      return "Four";
    if (_rank == 5)
      return "Five";
    if (_rank == 6)
      return "Six";
    if (_rank == 7)
      return "Seven";
    if (_rank == 8)
      return "Eight";
    if (_rank == 9)
      return "Nine";
    if (_rank == 10)
      return "Ten";
    if (_rank == 11)
      return "Jack";
    if (_rank == 12)
      return "Queen";
    if (_rank == 13)
      return "King";
    else
      return "Uknown _rank";
  }

  int get_suit() const { return _suit; }

  string get_suit_string() const {
    if (_suit == 1)
      return "♦️";
    if (_suit == 2)
      return "♥️";
    if (_suit == 3)
      return "♠️";
    if (_suit == 4)
      return "♣️";
    else
      return "Unknown _suit";
  }

  int get_value() const { return _value; }

  std::string get_value_string() const { return std::to_string(_value); }

  bool isJocker() const {
    return _is_jocker;
  }

  std::string toString() const {
    std::string res = get_rank_string() + "\t" + get_suit_string() + "\t" + get_value_string();
    res += _is_jocker ? ")" : "";
    return res;
  }

  void makeJocker() {
    _is_jocker = true;
    _value = 11;
  }
};

#endif