#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <vector>

class Player {
public:
  std::string _name = "";

public:
  Player() {}

  Player(std::string name) : _name(name) {}

  void makeMove(Board& board) {
    std::string input = "";
    int r = 0;
    int c = 0;
    do {
      std::cout << "Введите координаты ячейки, для хода: ряд колонка\n";
      std::cout << "Или напишите pass, чтобы пропустить ход.\n";
      std::cin >> input;
      if (input == "pass") {return;}
      r = std::stoi(input);
      std::cin >> c;
    } while (!canMakeMove(r, c, board));
    board.decrease(r, c);
  }

  bool canMakeMove(int r, int c, Board& board) {
    return board.getValue(r, c) > 0;
  }
};

#endif