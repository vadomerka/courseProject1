#include "Card.h"
#include "Player.h"
#include "Deck.h"
#include <iostream>
#include <vector>


// using namespace std;

int main() {
    std::vector<Player> players {Player("p1", 10, 5), Player("p2", 20, 5)};
    
    Deck deck;
    deck.printDeck();
    std::cout << deck.getSize();

    Card given {deck.giveCard()};
    std::cout << given.toString();
    
    size_t turn = 0;
    size_t max_cards_in_hand = 3;
    // for (int i = 0; i < players.size() * max_cards_in_hand; i++) {
    while (players[turn].get_hand_size() < max_cards_in_hand) {
        players[turn].addCard(deck.giveCard());
        turn = (turn + 1) % players.size();
    }

    turn = 0;
    while (deck.getSize() > 0 && players.size() > 1) {
        // turns

        turn = (turn + 1) % players.size();
    }
    if (!players.empty()) {
        std::cout << players[0]._name << " - выиграл!";
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
