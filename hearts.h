#ifndef HEARTS_H_INCLUDED
#define HEARTS_H_INCLUDED

#include "player.h"

#define NUM_OF_PLAYERS 4

class Hearts {
private:
	Player *d_players[NUM_OF_PLAYERS];

	void passThreeCards(int);
	bool playRound(int);
	int playTrick(int start_player);
	int getStartPlayer();
	int getMaxScore();
public:
	Hearts();
	void setPlayer(Player *, int);
	void playGame();
	friend ostream& operator<< (ostream &out, Hearts hearts);
};

ostream& operator<< (ostream &out, Hearts hearts);

#endif // HEARTS_H_INCLUDED
