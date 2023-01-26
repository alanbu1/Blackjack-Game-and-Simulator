#ifndef globals_h
#define globals_h
#include <string>

// These arrays should make it easier to check that a name is not a reserved
// word and that an entered shape type is valid
#define NUM_CARDS 13
string cardArray[13] = {"A", "2", "3",  "4", "5", "6", "7",
                        "8", "9", "10", "J", "Q", "K"};
#define NUM_SUITS 4
char suitArray[4] = {'D', 'C', 'H', 'S'};

#define MAX_CARDS 11

// max number 1 less than bust
#define MAX_NUMBER 21

// max number dealer has to hit
#define MAX_DEALER 16
#endif /* globals_h */