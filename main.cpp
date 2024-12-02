#include "Board.h"
#include "Player.h"
#include <iostream>
#include <vector>


// using namespace std;

int main() {
    std::vector<Player> players {Player("p1"), Player("p2")};
    Board b(3, 3);
    b.fillBoard();

    
    int turn = 0;
    int winner = b.hasWinner();
    
    while (winner == 0 && players.size() > 1) {
        // turns
        b.printBoard();

        std::cout << "Ходит " << turn + 1 << "й игрок.\n";
        std::cout << "winner = " << winner << "\n";
        players[turn].makeMove(b);

        winner = b.hasWinner();
        turn = (turn + 1) % players.size();
    }
    
    if (winner == 3) {
        std::cout << "Ничья!";
    } else if (!players.empty()) {
        std::cout << players[winner - 1]._name << " - выиграл!";
    }
    

    

//   int dealer_rank;
//   int player_rank;
//   float bet;
//   char choice;
//   Card card;
//   Player player = Player();

//   cout << "Player, what is your name? ";
//   cin >> player.name;

//   cout << "How much money would you like to start with? ";
//   cin >> player.starting_balance;

//   player.balance = player.starting_balance;

//   while (player.balance > 0 && (player.balance < player.starting_balance * 5)) {
//     cout << player.name << ", you have $" << player.balance << ". ";

//     while (bet == 0 || bet > player.balance) {
//       cout << "How much money do you want to bet? ";
//       cin >> bet;

//       if (bet > player.balance) {
//         cout << "Sorry, you don't have enough money to make that bet.\n";
//       }
//     }

//     card = Card();
//     dealer_rank = card.get_rank();

//     cout << "You draw a " << card.get_rank_string() << " of "
//          << card.get_suit_string() << ".\n";
//     cout << "Will the next card be higher or lower? "
//          << "Enter \"h\" for higher or \"l\" for lower: ";
//     cin >> choice;

//     card = Card();
//     player_rank = card.get_rank();

//     cout << "You draw a " << card.get_rank_string() << " of "
//          << card.get_suit_string() << ".\n";

//     if ((choice == 'h' && dealer_rank > player_rank) ||
//         (choice == 'l' && dealer_rank < player_rank)) {
//       player.balance -= bet;

//       cout << "LOSE!!! Too bad " + player.name << ". You lose $" << bet
//            << ".\n";
//     } else {
//       player.balance += bet;

//       cout << "WIN!!! Congratulations " + player.name << ". You win $" << bet
//            << "!\n";
//     }

//     bet = 0;
//   }

//   if (player.balance <= 0) {
//     cout << "Too bad " + player.name << ". "
//          << "You are out of money! You lose.\n";
//   } else {
//     cout << "You win the game " + player.name << "!";
//   }

  return 0;
}
