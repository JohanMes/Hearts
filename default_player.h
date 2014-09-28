#ifndef DEFAULT_PLAYER_H_INCLUDED
#define DEFAULT_PLAYER_H_INCLUDED

#include <vector>
#include "player.h"
#include "card.h"
#include "trick.h"

#define NUM_OF_PLAYERS 4
#define NUM_OF_CARDS 52
#define NUM_OF_CARDS_PER_PLAYER 13

using namespace std;

class DefaultPlayer : public Player {
private:
	int d_player_id;
	vector<Card *> d_hand;
	int d_score;

public:
	DefaultPlayer();
	void setPlayerId(int);
	int receiveCards(vector<Card *> *);
	std::vector<Card *> *passThreeCards(void);
	int receiveThreeCards(std::vector<Card *> *);
	bool hasTwoOfClubs();
	void playCard(Trick *);
	void storeTrick(Trick *);
	int getScore();
};

#endif // PLAYER_H_INCLUDED
