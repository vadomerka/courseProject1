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

class SekiTests {
public:
  static void tests() {

    std::cout << "Запуск тестов\n";
    runBoardTests();
    runGameTests();
  }

  static void runBoardTests() {
    
  }

  static void runGameTests() {
    testGame("game_test_01", {{1, 1, 4}, {2, 2, 3}, {6, 1, 5}});
    testGame("game_test_02_4", {{2, 6, 6, 6, 6}, {5, 6, 4, 4, 5}, {6, 5, 3, 2, 5}, {1, 5, 6, 6, 3}, {5, 3, 3, 1, 4}}, 4);
    testGame("game_test_02_5", {{2, 6, 6, 6, 6}, {5, 6, 4, 4, 5}, {6, 5, 3, 2, 5}, {1, 5, 6, 6, 3}, {5, 3, 3, 1, 4}}, 5);
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

    static std::ofstream outFile;
    if (!outFile.is_open()) {
      outFile.open(testName + ".txt");
    }
    Game g;
    g.runGame(b, players, tq, turnDepth, &outFile);
    outFile.close();
  }
};

#endif