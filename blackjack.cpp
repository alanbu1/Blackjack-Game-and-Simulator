// blackjack game under the assumption of multiple decks

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
using namespace std;

#include "globals.h"

// helper functions
void playBlackJack(double &balance);
void dealHand(string &card1, char &suit1, string &card2, char &suit2);
void calculateHandValue(int &value, string cardArray[]);
void printHand(string cArray[], char sArray[]);
int nextFreeIndex(string cArray[]);
void hit(string &card, char &suit);
void printDealer(string cardArray[], char suitArray[], int value);
void printPlayer(string cardArray[], char suitArray[], int value);
void simulate();
int calculateTop(string cArray[]);

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;
// main
int main() {
  srand(time(NULL));
  double startingBal = 0;
  double balance = 0;
  string line;
  // Get starting balance
  cout << "Welcome to BlackJack." << endl;
  // Prompt for input
  while (startingBal == 0 || cin.fail()) {
    cout << "Enter the balance to start with: " << endl;
    cout << "> ";
    cin >> startingBal;
    if (cin.fail() || startingBal <= 0) {
      cout << "Error: Invalid Value." << endl;
      cin.clear();
    }
  }
  balance = startingBal;
  bool cashedOut = false;
  char command;
  getline(cin, line); // Get a line from standard input
  cout << "\n\n"
       << "Your balance: " << balance << endl;
  cout << "Enter a command:" << endl;
  cout << "Play or Cash out or Simulate (P/C/S)" << endl;
  cout << "> ";
  while (!cin.eof() && (cashedOut == false) && balance != 0) {

    // Command deal or Cash out
    cin >> command;
    //
    // If command is deal: play game
    if ((command == 'p') || (command == 'P')) {
      playBlackJack(balance);
    } else if (command == 'c' || command == 'C') {
      cashedOut = true;
    } else if (command == 's' || command == 'S') {
      simulate();
    } else {
      cout << "Invalid command. Re-enter." << endl;
    }
    sleep_for(250ms);
    getline(cin, line);

    if (!cashedOut && balance != 0) {
      cout << "\n\n"
           << "Your balance: " << balance << endl;
      cout << "Enter a command:" << endl;
      cout << "Play or Cash out or Simulate (P/C/S)" << endl;
      cout << "> "; // Prompt for input
    }
  }

  if (balance == 0) {
    cout << "\n\n"
         << "Game Over! You lost!" << endl;
    sleep_for(1000ms);
  } else if (cashedOut == true) {
    double profit = balance - startingBal;
    cout << "You cashed out " << balance << " dollars. You profited: " << profit
         << " dollars"
         << "." << endl;
  }

  // Put the line in a linestream for parsing
  // Making a new sstream for each line so flags etc. are cleared
  // Read from string stream into the command
  // The only way this can fail is if the eof is encountered
  return 0;
}

// helper functions:
void playBlackJack(double &balance) {
  // Starting hand has a top and bottom card for both player and dealer.
  cout << "\n";
  double bet = 0;
  cout << "Please enter the amount you would like to bet" << endl;
  cout << "> ";
  while (bet == 0 || bet > balance || cin.fail()) {
    cin.clear();
    cin >> bet;
    if (bet > balance) {
      cout << "Error: not enough chips" << endl;
      cout << "You have " << balance << " chips." << endl;
    }
    if (cin.fail()) {
      cout << "Error: Invalid input." << endl;
    }
  }
  // If bet goes through, we play
  if (bet > 0 && bet <= balance) {
    cout << "Your bet: " << bet << " chips." << endl;
    balance = balance - bet;
    cout << "Your balance: " << balance << " chips." << endl;
    // Deal hand to player and dealer
    // Maximum amount of possible cards is 11.
    string dCA[MAX_CARDS] = {"0", "0", "0", "0", "0", "0",
                             "0", "0", "0", "0", "0"};
    char dSA[MAX_CARDS] = {'0', '0', '0', '0', '0', '0',
                           '0', '0', '0', '0', '0'};
    int dValue = 0;
    string pCA[MAX_CARDS] = {"0", "0", "0", "0", "0", "0",
                             "0", "0", "0", "0", "0"};
    char pSA[MAX_CARDS] = {'0', '0', '0', '0', '0', '0',
                           '0', '0', '0', '0', '0'};
    int pValue = 0;
    // deal first 2 cards for dealer and player
    // dealer
    dealHand(dCA[0], dSA[0], dCA[1], dSA[1]);
    calculateHandValue(dValue, dCA);
    // player
    dealHand(pCA[0], pSA[0], pCA[1], pSA[1]);
    calculateHandValue(pValue, pCA);
    // Logic hit or stand
    string command = "0";
    while (command == "0" || command == "H" || command == "h") {
      // First display player and dealer cards:
      cout << "Dealer: " << dCA[0] << dSA[0] << " ??" << endl;
      cout << "Player: ";
      printHand(pCA, pSA);
      cout << "\n"
           << "Player Total: " << pValue << endl;
      // If blackjack:
      if (pValue == MAX_NUMBER && pValue != dValue) {
        printDealer(dCA, dSA, dValue);
        printPlayer(pCA, pSA, pValue);
        double blackjack = bet + bet * 1.5;
        cout << "Blackjack! You won " << blackjack << "." << endl;
        balance += blackjack;
        break;
      } else if (pValue == MAX_NUMBER && pValue == dValue) {
        // prompt user for hit or stand
        printDealer(dCA, dSA, dValue);
        printPlayer(pCA, pSA, pValue);
        cout << "Push!" << endl;
        cout << "You won: " << bet << endl;
        balance += bet;
      } else {
        cout << "Hit or Stand? (H/S)" << endl;
        cout << "> ";
        cin >> command;
        cout << "\n";
        // check for error
        if (cin.fail() && command != "H" && command != "h" && command != "S" &&
            command != "s") {
          cout << "\n"
               << "Error: Invalid command" << endl;
        } else if (command == "S" || command == "s") { // stand
          if (dValue <= MAX_DEALER) {
            while (dValue <= MAX_DEALER) {
              cout << "\n";
              int freeIndex = nextFreeIndex(dCA);
              printDealer(dCA, dSA, dValue);
              printPlayer(pCA, pSA, pValue);
              hit(dCA[freeIndex], dSA[freeIndex]);
              calculateHandValue(dValue, dCA);
            }
          }
          cout << "\n";
          printDealer(dCA, dSA, dValue);
          printPlayer(pCA, pSA, pValue);
          if (dValue > pValue && dValue <= MAX_NUMBER) {
            cout << "You lost!" << endl;
            sleep_for(1000ms);
          } else {
            if (dValue == pValue) {
              cout << "Push!" << endl;
              cout << "You won: " << bet << endl;
              balance += bet;
            } else {
              if (dValue > MAX_NUMBER) {
                cout << "Dealer bust!" << endl;
              }
              cout << "You won!" << endl;
              sleep_for(1000ms);
              balance += bet * 2;
              cout << "Your New Balance is: " << balance << endl;
              sleep_for(1000ms);
            }
          }
          break;
        } else { // hit
          int freeIndex = nextFreeIndex(pCA);
          // hit(pCA[freeIndex], pSA[freeIndex], pValue, pCA);
          hit(pCA[freeIndex], pSA[freeIndex]);
          calculateHandValue(pValue, pCA);
          if (pValue > MAX_NUMBER) {
            printDealer(dCA, dSA, dValue);
            printPlayer(pCA, pSA, pValue);
            cout << "Bust! You lost" << endl;
            sleep_for(1000ms);
            break;
          }
        }
      }
    }
  }
}
void dealHand(string &card1, char &suit1, string &card2, char &suit2) {
  card1 = cardArray[rand() % NUM_CARDS];
  suit1 = suitArray[rand() % NUM_SUITS];
  card2 = cardArray[rand() % NUM_CARDS];
  suit2 = suitArray[rand() % NUM_SUITS];
}

void calculateHandValue(int &value, string cArray[]) {
  value = 0;
  int aceCounter = 0;
  for (int i = 0; i < MAX_CARDS; i++) {
    for (int j = 0; j < NUM_CARDS; j++) {
      if (cArray[i] == cardArray[j]) {
        if (j == 0) {
          aceCounter++;
        } else if (j <= 9)
          value += j + 1;
        else
          value += 10;
      }
    }
  }
  while (aceCounter > 0) {
    if (value + 11 <= MAX_NUMBER) {
      value += 11;
      aceCounter--;
    } else {
      value += 1;
      aceCounter--;
    }
  }
}
void printHand(string cArray[], char sArray[]) {
  for (int i = 0; i < MAX_CARDS; i++) {
    if (cArray[i] != "0") {
      cout << cArray[i] << sArray[i] << " ";
    }
  }
  cout << "\n";
}
// finds the next array index that is 0
int nextFreeIndex(string cArray[]) {
  for (int i = 0; i < MAX_CARDS; i++) {
    if (cArray[i] == "0") {
      return i;
    }
  }
  return -1;
}
void hit(string &card, char &suit) {
  card = cardArray[rand() % NUM_CARDS];
  suit = suitArray[rand() % NUM_SUITS];
  // sleep_for(1000ms);
  // calculateHandValue(value, cardA);
}

void printDealer(string cardA[], char suitA[], int value) {
  cout << "Dealer: ";
  printHand(cardA, suitA);
  cout << "Dealer total: " << value << endl;
  sleep_for(500ms);
}

void printPlayer(string cardA[], char suitA[], int value) {
  cout << "Player: ";
  printHand(cardA, suitA);
  cout << "Player total: " << value << endl;
  sleep_for(500ms);
}

void simulate() {

  int numSimulations = 0;
  int numWins = 0;
  int numLoss = 0;
  int numDraw = 0;
  while (numSimulations < 10000000) {
    string dCA[MAX_CARDS] = {"0", "0", "0", "0", "0", "0",
                             "0", "0", "0", "0", "0"};
    char dSA[MAX_CARDS] = {'0', '0', '0', '0', '0', '0',
                           '0', '0', '0', '0', '0'};
    int dValue = 0;
    string pCA[MAX_CARDS] = {"0", "0", "0", "0", "0", "0",
                             "0", "0", "0", "0", "0"};
    char pSA[MAX_CARDS] = {'0', '0', '0', '0', '0', '0',
                           '0', '0', '0', '0', '0'};
    int pValue = 0;
    dealHand(dCA[0], dSA[0], dCA[1], dSA[1]);
    calculateHandValue(dValue, dCA);
    // player
    dealHand(pCA[0], pSA[0], pCA[1], pSA[1]);
    calculateHandValue(pValue, pCA);
    // If blackjack:
    // Hit if dealer top card is 7+ and player is under 16
    if (pValue == MAX_NUMBER && pValue != dValue) {
      numWins++;
    } else if (pValue == MAX_NUMBER && pValue == dValue) {
      numDraw++;
    } else {
      // hit depending on dealer's topcard:
      if (calculateTop(dCA) > 7 && pValue <= 16) {
        int freeIndex = nextFreeIndex(pCA);
        hit(pCA[freeIndex], pSA[freeIndex]);
        calculateHandValue(pValue, pCA);
      } else if (pValue <= 11) { // hit if 11 or less
        int freeIndex = nextFreeIndex(pCA);
        hit(pCA[freeIndex], pSA[freeIndex]);
        calculateHandValue(pValue, pCA);
      }
      if (pValue > MAX_NUMBER) {
        numLoss++;
      } else {
        if (dValue <= MAX_DEALER) {
          while (dValue <= MAX_DEALER) {
            int freeIndex = nextFreeIndex(dCA);
            hit(dCA[freeIndex], dSA[freeIndex]);
            calculateHandValue(dValue, dCA);
          }
        }
        if (dValue > pValue && dValue <= MAX_NUMBER) {
          numLoss++;
        } else {
          if (dValue == pValue) {
            numDraw++;
          } else {
            numWins++;
          }
        }
      }
    }
    cout << "Simulation: " << numSimulations << endl;
    numSimulations++;
  }
  cout << "Number of wins: " << numWins << endl;
  cout << "Number of draws: " << numDraw << endl;
  cout << "Number of losses: " << numLoss << endl;
}

int calculateTop(string cArray[]) {
  int value = 0;
  int i = 0;
  for (int j = 0; j < NUM_CARDS; j++) {
    if (cArray[i] == cardArray[j]) {
      if (j == 0) {
        value += 11;
      } else if (j <= 9)
        value += j + 1;
      else
        value += 10;
    }
  }
  return value;
}
