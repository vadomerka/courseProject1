#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <vector>

class Player {
public:
  std::string _name = "";
  bool _isPlayer = true;

public:
  Player() {}

  Player(std::string name) : _name(name) {}

  Player(std::string name, std::string intension) : _name(name) {}

  virtual std::pair<int, int> makeMove(Board& board, const std::vector<int>& fTurns) {
    setlocale(LC_ALL, "Russian");
    std::cout << "player move\n";
    std::string input = "";
    int r = 0;
    int c = 0;
    do {
      std::wcout << L"Введите координаты ячейки, для хода: ряд колонка\n";
      std::wcout << L"Или напишите pass, чтобы пропустить ход.\n";
      std::cin >> input;
      if (input == "pass") {return {-1, -1};}
      r = std::stoi(input);
      std::cin >> c;
    } while (!canMakeMove(r, c, board));
    board.decrease(r, c);
    return {r, c};
  }

  bool canMakeMove(int r, int c, Board& board) {
    return board.getValue(r, c) > 0;
  }
};

#endif