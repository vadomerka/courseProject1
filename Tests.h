#ifndef SEKI_TESTS_H
#define SEKI_TESTS_H

#include "Board.h"
#include "Bot.h"
#include "Game.h"
#include "Player.h"
#include "TurnQueues/DetTurnQueue.h"
#include "TurnQueues/RandTurnQueue.h"
#include <iostream>
#include <vector>
#include <fstream>

class SekiTests {
public:
  static void tests() {

    std::cout << "Запуск тестов\n";
    runBoardTests();
    // runGameTests();
  }

  static void runBoardTests() {
    std::vector<bool> results;
    results.push_back(testBoard("test 01", {{0}}, {0, 1}, {-1, -1}));

    for (bool res : results) {
      if (!res) {
        std::cout << "Error while running auto tests!";
      }
    }
    std::cout << "auto tests finished successfully!";
  }

  static bool testBoard(const std::string &testName,
                        const std::vector<std::vector<int>> &matrix, 
                        const std::vector<int> &qs, 
                        std::pair<int, int> check = {-3, -3},
                        bool canPass = true, int passStreak = 0) {
      if (qs.empty()) return false;
      std::vector<Player *> players{new Bot("b1", "row"), new Bot("b2", "col")};
      Board b(matrix.size(), matrix[0].size());
      b.fillBoard(matrix);

      std::pair<int, int> res = players[qs[0]]->makeMove(b, qs, canPass, passStreak, qs.size());
      if (check.first == -3) {
        check = players[qs[0]]->hint(b);
      }
      if (res.first == check.first && res.second == check.second) {
        return true;
      }
      return false;
    }


  static void runGameTests() {
    testGame("game_test_01", {{1, 1, 4}, {2, 2, 3}, {6, 1, 5}});
    testGame("game_test_02_4", {{2, 6, 6, 6, 6}, {5, 6, 4, 4, 5}, {6, 5, 3, 2, 5}, {1, 5, 6, 6, 3}, {5, 3, 3, 1, 4}}, 4);
    testGame("game_test_02_5", {{2, 6, 6, 6, 6}, {5, 6, 4, 4, 5}, {6, 5, 3, 2, 5}, {1, 5, 6, 6, 3}, {5, 3, 3, 1, 4}}, 5);
    testGame("game_test_02_6", {{2, 6, 6, 6, 6}, {5, 6, 4, 4, 5}, {6, 5, 3, 2, 5}, {1, 5, 6, 6, 3}, {5, 3, 3, 1, 4}}, 6);
    testGame("game_test_03", {{1, 5, 5, 6, 5}, {3, 1, 6, 2, 1}, {6, 3, 5, 5, 2}, {5, 6, 3, 5, 1}, {5, 5, 3, 1, 3}});
    testGame("game_test_04", {{0, 5, 5}, {1, 5, 5}, {2, 0, 0}});
    testGame("game_test_05", {{3, 4, 8, 4, 2}, {3, 2, 2, 5, 6}, {3, 7, 7, 8, 4}, {6, 7, 6, 5, 4}, {8, 1, 9, 9, 8}});
  }

  static void testGame(const std::string &testName,
                        const std::vector<std::vector<int>> &matrix, int turnDepth = 4,
                        const std::vector<int> &qs = {0, 1},
                        const std::vector<int> &qp = {0, 0, 1, 1}) {

    std::vector<Player *> players{new Bot("b1", "row"), new Bot("b2", "col")};
    Board b(matrix.size(), matrix[0].size());
    b.fillBoard(matrix);

    DetTurnQueue tq{qs, qp};

    std::ofstream testFile(testName + ".txt");
    if (!testFile.is_open()) {
      testFile.open(testName + ".txt");
    }
    Game g (false, false);
    g.runGame(b, players, tq, turnDepth, &testFile);  // &testFile
    testFile.close();
  }
};

#endif