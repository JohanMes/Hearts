#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include "card.h"
#include "default_player.h"

#define NUM_OF_PLAYERS 4
#define NUM_OF_CARDS 52
#define NUM_OF_CARDS_PER_PLAYER 13

class Deck {
private:
	Card d_deck[NUM_OF_CARDS];

public:
	Deck();
	void dealCards(Player *players[NUM_OF_PLAYERS]);
};
#endif /* DECK_H_INCLUDED*/
