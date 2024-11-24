#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include <algorithm>
#include <random>
#include <vector>

class Deck {
public:
  Deck() {
    reshuffle();
  }

  void reshuffle() {
    makeNewDeck();
    shuffle_deck();
  }

  void makeNewDeck() {
    int max_rank = 13;
    int max_suit = 4;
    _arr_deck.clear();
    for (int suit = 1; suit <= max_suit; suit++) {
      // Cards from 6 to King.
      for (int rank = 6; rank <= max_rank; rank++) {
        Card new_card {rank, suit};
        addCard(new_card);
      }
      // Aces.
      Card new_card {1, suit};
      addCard(new_card);
    }
    _size = _arr_deck.size();
  }

  void addCard(Card& card) {
    if (card.get_rank() == jocker_rank &&
        card.get_suit() == jocker_suit) {
      card.makeJocker();
    }
    _arr_deck.emplace_back(card);
  }

  void shuffle_deck() {
    std::mt19937 gen(std::random_device{}());
    std::shuffle(std::begin(_arr_deck), std::end(_arr_deck), gen);
  }

  Card giveCard() {
    Card c = _arr_deck.back();
    _arr_deck.pop_back();
    return c;
  }

  size_t getSize() const { return _arr_deck.size(); }

  void printDeck() const {
    for (size_t i = 0; i < _arr_deck.size(); i++) {
      std::cout << _arr_deck[i].toString() << "\n";
    }
    std::cout << '\n';
  }

private:
  std::vector<Card> _arr_deck;
  size_t _size = 0;
  int jocker_rank = 6;
  int jocker_suit = 3;
};

#endif