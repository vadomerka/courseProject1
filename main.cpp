#include <iostream>
#include <fstream>
#include "Game.h"
#include "Tests.h"

void menu() {
  std::cout << "Вас приветсвует прототип игры Seki!\n";
  std::cout << "Запустить тесты? y/n\n";
  char input = 'n';
  std::cin >> input;
  if (input == 'y') {
    SekiTests::tests();
  } else {
    std::cout << "Ввести данные для своей игры? y/n\n";
    char input;
    std::cin >> input;
    if (input == 'y') {
      Game g;
      g.makeGame();
      return;
    }
  }
}

int main() {
  menu();

  return 0;
}
