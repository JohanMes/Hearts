#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "card.h"
#include "trick.h"

class Player {
private:
protected:
	int d_player_id;

public:
	virtual void setPlayerId(int) = 0;
	virtual int receiveCards(std::vector<Card *> *) = 0;
	virtual std::vector<Card *> * passThreeCards(void) = 0;
	virtual int receiveThreeCards(std::vector<Card *> *) = 0;
	virtual bool hasTwoOfClubs(void) = 0;
	virtual void playCard(Trick *) = 0;
	virtual void storeTrick(Trick *) = 0;
	virtual int getScore(void) = 0;
};

#endif /* PLAYER_H_INCLUDED */
