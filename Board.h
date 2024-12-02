#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <random>
#include <string>

class Board {
public:
  size_t _height = 0;
  size_t _width = 0;
  std::vector<std::vector<int>> _board;

public:
  Board() {}

  Board(size_t h, size_t w) : _height(h), _width(w) {
    for (size_t i = 0; i < h; i++) {
      _board.push_back(std::vector<int> (_width, 0));
    }
  }

  Board(std::vector<std::vector<int>> arr) {
    _board = arr;
    _height = arr.size();
    _width = _height != 0 ? arr[0].size() : 0;
  }

  void fillBoard() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> rint(0, 10);
    for (size_t i = 0; i < _height; i++) {
      for (size_t j = 0; j < _width; j++) {
        _board[i][j] = rint(gen);
      }
    }
  }

  int hasWinner() const {
    bool colWin = false;
    bool rowWin = false;

    for (size_t i = 0; i < _height; i++) {
      if (rowWin) break;
      rowWin = true;
      for (size_t j = 1; j < _width; j++) {
        if (_board[i][j] != _board[i][j - 1]) {
          rowWin = false;
        }
      }
    }
    for (size_t j = 0; j < _width; j++) {
      if (colWin) break;
      colWin = true;
      for (size_t i = 1; i < _height; i++) {
        if (_board[i][j] != _board[i - 1][j]) {
          colWin = false;
        }
      }
    }
    if (colWin && rowWin) return 3; // Draw
    if (rowWin) return 1;           // Player 1 won
    if (colWin) return 2;           // Player 2 won
    return 0;                       // No one won
  }

  void decrease(int r, int c) {
    if (r >= 0 && r < _height && c >= 0 && c < _width) {
      _board[r][c]--;
    }
  }

  int getValue(int r, int c) {
    if (r >= 0 && r < _height && c >= 0 && c < _width) {
      return _board[r][c];
    }
    return -1;
  }

  size_t getWidth() const {
    return _width;
  }

  size_t getHeight() const {
    return _height;
  }

  void printBoard() const {
    for (size_t i = 0; i < _height; i++) {
      for (size_t j = 0; j < _width; j++) {
        std::string output = std::to_string(_board[i][j]);
        std::cout << output;
        for (size_t _ = 0; _ < 4 - output.size(); _++) {
          std::cout << " ";
        }
        
      }
      std::cout << '\n';
    }
  }
};


#endif