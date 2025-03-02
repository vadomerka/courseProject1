#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>

class Board {
public:
  size_t _height = 0;
  size_t _width = 0;
  std::vector<std::vector<int>> _board;
  long long max_num = 0;


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
    std::uniform_int_distribution<int> rint(1, 6);
    for (size_t i = 0; i < _height; i++) {
      for (size_t j = 0; j < _width; j++) {
        _board[i][j] = rint(gen);
      }
    }
  }

  void fillBoard(const std::vector<std::vector<int>>& matrix) {
    for (size_t i = 0; i < _height; i++) {
      for (size_t j = 0; j < _width; j++) {
        _board[i][j] = matrix[i][j];
        max_num += _board[i][j];
      }
    }
  }

  int hasWinner() const {
    bool colWin = false;
    bool rowWin = false;

    for (size_t i = 0; i < _height; i++) {
      if (rowWin) break;
      rowWin = true;
      for (size_t j = 0; j < _width; j++) {
        if (_board[i][j] != 0) {
          rowWin = false;
        }
      }
    }
    for (size_t j = 0; j < _width; j++) {
      if (colWin) break;
      colWin = true;
      for (size_t i = 0; i < _height; i++) {
        if (_board[i][j] != 0) {
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
    } else {
      throw 1;
    }
  }

  void increase(int r, int c) {
    if (r >= 0 && r < _height && c >= 0 && c < _width) {
      _board[r][c]++;
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

  int getRowSum(int row = 0) const {
    if (row < 0 || row >= _height) return 0;
    int sum = 0;
    for (int j = 0; j < _width; j++) {
      sum += _board[row][j];
    }
    return sum;
  }

  int getColSum(int col = 0) const {
    if (col < 0 || col >= _width) return 0;
    int sum = 0;
    for (int i = 0; i < _height; i++) {
      sum += _board[i][col];
    }
    return sum;
  }

  std::vector<std::pair<size_t, int>> getRowsIndSums() const {
    std::vector<std::pair<size_t, int>> res (_height, {0, 0});

    for (int i = 0; i < _height; i++) {
      res[i] = {i, getRowSum(i)};
    }
    std::sort(std::begin(res), std::end(res), [](auto &left, auto &right) {
      return left.second > right.second;
    });
    return res;
  }

  std::vector<std::pair<size_t, int>> getColsIndSums() const {
    std::vector<std::pair<size_t, int>> res (_width, {0, 0});

    for (int j = 0; j < _width; j++) {
      res[j] = {j, getColSum(j)};
    }
    std::sort(std::begin(res), std::end(res), [](auto &left, auto &right) {
      return left.second > right.second;
    });
    return res;
  }

  std::string toString() const {
    std::string res = "";
    for (size_t i = 0; i < _height; i++) {
      for (size_t j = 0; j < _width; j++) {
        res += std::to_string(_board[i][j]) + " ";
      }
    }
    return res;
  }

  void printBoard(std::ostream& log=std::cout) const {
    for (size_t i = 0; i < _height; i++) {
      for (size_t j = 0; j < _width; j++) {
        std::string output = std::to_string(_board[i][j]);
        log << output;
        for (size_t _ = 0; _ < 4 - output.size(); _++) {
          log << " ";
        }
        
      }
      log << '\n';
    }
  }
};


#endif