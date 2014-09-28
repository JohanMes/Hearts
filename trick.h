#ifndef TRICK_H_INCLUDED
#define TRICK_H_INCLUDED

#include <vector>

#include "card.h"

using namespace std;

class Trick {
private:
	vector<Card *> d_cards;
	int d_start_player;
	Color d_color;
	int d_penalty;
public:
	Trick(int);
	~Trick();

	void addCard(Card *);
	vector<Card *> cards();
	int startPlayer();
	void setColor(Color);
	Color color();
	int winningPlayer();
	int penalty();
};

#endif // TRICK_H_INCLUDED
