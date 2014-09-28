#ifndef DOUWE_H_INCLUDED
#define DOUWE_PLAYER_H_INCLUDED

#include <vector>
#include "player.h"
#include "card.h"
#include "trick.h"
#include <string>

#define NUM_OF_PLAYERS 4
#define NUM_OF_CARDS 52
#define NUM_OF_CARDS_PER_PLAYER 13

using namespace std;

class Douwe : public Player {
private:
	int d_player_id;
	vector<Card *> d_hand;
    int d_score;

	vector<int> score; 				// score 0 to 3 == player id
	vector<int> out_of;				// number of players that dont have the color (int)..
	vector<vector<int > > gespeeld; // vector met gespeelde kaarten
	
	bool hearts_broken;				// Open niet met harten tot...
	bool have_queen;				// If I have the queen
	bool first_trick;				
	bool q_gespeeld;				// If queen has been played!
	
	bool shoot_moon;				// Probeer een shoot-moon te halen
	int shoot_moon_color;			// Kleur waarvan veel kaarten!

public:
	Douwe();
	void setPlayerId(int);
	int receiveCards(vector<Card *> *);
	std::vector<Card *> *passThreeCards(void);
	int receiveThreeCards(std::vector<Card *> *);
	bool hasTwoOfClubs();
	void playCard(Trick *);
	void storeTrick(Trick *);
	int getScore();
	
	int offset(int);
	int playWorstCard();
	int findHighestCard(Trick *);
};

#endif // DOUWE_H_INCLUDED
