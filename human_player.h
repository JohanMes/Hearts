#ifndef HUMAN_PLAYER_H_INCLUDED
#define HUMAN_PLAYER_H_INCLUDED

#include <vector>
#include "player.h"
#include "card.h"
#include "trick.h"

#define NUM_OF_PLAYERS 4
#define NUM_OF_CARDS 52
#define NUM_OF_CARDS_PER_PLAYER 13

using namespace std;

class HumanPlayer : public Player {
private:
	int h_player_id;
	vector<Card *> h_hand;
	int h_score;

    void printCards(vector<Card *> *);
    Card *chooseCard(vector<Card *> *);
	int findCardIndex(vector<Card *> *, Color color, Value value);
	int countCardsOfColor(vector<Card *> *, Color color);

	// Addition for when player gets all penalty cards
	int h_roundScore;
public:
	HumanPlayer();
	void setPlayerId(int);
	int receiveCards(vector<Card *> *);
	vector<Card *> *passThreeCards(void);
	int receiveThreeCards(vector<Card *> *);
	bool hasTwoOfClubs();
	void playCard(Trick *);
	void storeTrick(Trick *);
	int getScore();


    string h_player_name;

	// Addition for when player gets all penalty cards
	int getRoundScore();
};

#endif // PLAYER_H_INCLUDED
