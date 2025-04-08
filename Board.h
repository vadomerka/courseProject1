#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <sstream>

class Board {
public:
  size_t _height = 0;
  size_t _width = 0;
  std::vector<std::vector<int>> _board;
  long long _max_num = 0;
  int _passStreak = 0;

public:
  Board() {}

  Board(size_t h, size_t w) {
    setSize(h, w);
  }

  Board(std::vector<std::vector<int>> arr) {
    _board = arr;
    _height = arr.size();
    _width = _height != 0 ? arr[0].size() : 0;
  }

  void setSize(size_t h, size_t w) {
    _board.clear();
    _max_num = 0;
    _height = h;
    _width = w;
    for (size_t i = 0; i < h; i++) {
      _board.push_back(std::vector<int> (_width, 0));
    }
  }

  void fillBoard() {
    _max_num = 0;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> rint(1, 9);
    for (size_t i = 0; i < _height; i++) {
      for (size_t j = 0; j < _width; j++) {
        _board[i][j] = rint(gen);
        _max_num += _board[i][j];
      }
    }
  }

  void fillBoard(const std::vector<std::vector<int>>& matrix) {
    _max_num = 0;
    for (size_t i = 0; i < _height; i++) {
      for (size_t j = 0; j < _width; j++) {
        _board[i][j] = matrix[i][j];
        _max_num += _board[i][j];
      }
    }
  }

  bool fillFromString(const std::string& dataString) {
    try
    {
      std::vector<int> input_arr = parseArr(dataString);
      std::vector<std::vector<int>> nmatrix;
      if (input_arr.size() != _height * _width) {
        return false;
      }
      for (size_t i = 0; i < _height; i++) {
        nmatrix.push_back(std::vector<int> (_width, 0));
      }
      for (size_t k = 0; k < input_arr.size(); k++) {
        if (input_arr[k] < 0) {
          return false;
        }
        nmatrix[k / _height][k % _width] = input_arr[k];
      }
      
      fillBoard(nmatrix);
      return true;
    }
    catch(const std::exception& e)
    {
      return false;
    }
  }

  static std::vector<int> parseArr(std::string dataString) {
    std::istringstream iss(dataString);
    std::string s;
    std::vector<int> input_arr;
    std::vector<std::vector<int>> nmatrix;
    while (std::getline(iss, s, ' ')) {
      int intput = std::stoi(s);
      input_arr.push_back(intput);
    }
    return input_arr;
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
      // pass
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