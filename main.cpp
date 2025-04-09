#include <iostream>
#include <fstream>
#include "Game.h"
#include "Tests.h"


void menu() {
  std::cout << "Game Seki prototype start!\n";
  std::cout << "Run tests? y/n\n";
  char input = 'n';
  std::cin >> input;
  if (input == 'y') {
    SekiTests::tests();
  } else {
    std::cout << "Enter data for a new game? y/n\n";
    input = 'n';
    std::cin >> input;
    if (input == 'y') {
      Game g;
      g.makeGame();
    }
  }
  std::string exitstr;
  std::cout << "press 'Enter' to exit.\n";
  std::cin.ignore();
  std::getline(std::cin, exitstr);
}

int main() {
  menu();
  
  return 0;
}
