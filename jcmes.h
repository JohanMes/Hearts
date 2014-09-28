#ifndef JOHAN_H_INCLUDED
#define JOHAN_H_INCLUDED

#include <queue>

#include "card.h"
#include "trick.h"
#include "player.h"
#include "hearts.h"
#include "deck.h"

// Sorteerder voor kaarten printen (laag -> hoog)
bool PrintColorSort(Card* a,Card* b); // moet static zijn

// Sorteerder voor openingskaarten spelen (hoog -> laag)
struct WeightedColor {
	color thiscolor;
	unsigned int value1;
	unsigned int value2;
};
bool OpenColorSort(WeightedColor a,WeightedColor b); // moet static zijn

// Sorteerder voor kleuren voiden (laag -> hoog)
bool VoidColorSort(WeightedColor a,WeightedColor b); // moet static zijn

class jcmes : public Player {
	protected:
		// Nieuw
		std::vector<Card*> hand;
		std::vector<Card*> playedcards;
		std::vector<WeightedColor> playingsort;
		
		int totalscore;
		bool heartsbroken;
		bool beginwithtwoofclubs;
		
		int playedcardindex;
	public:
		
		// Kaarten doorgeven
		int ExchangeCard(std::vector<Card*>* to,std::vector<Card*>* from,int index);
		bool MoveIndexToStapel(Trick* stapel,int index);
		
		// Van van alles en nog wat de incides vinden
		int FindFirstColor(std::vector<Card*>* stapel,color colorin);
		int FindFirstNotColor(std::vector<Card*>* stapel,color colorin);
		int FindFirstValue(std::vector<Card*>* stapel,value valuein);
		int FindFirstNotValue(std::vector<Card*>* stapel,value valuein);
		int FindValueColor(std::vector<Card*>* stapel,value valuein,color colorin);
		int FindValueNotColor(std::vector<Card*>* stapel,value valuein,color colorin);
		int FindValueNot2Color(std::vector<Card*>* stapel,value valuein,color colorin1,color colorin2);
		int FindMaxValue(std::vector<Card*>* stapel);
		int FindMinValue(std::vector<Card*>* stapel);
		int FindMaxValueColor(std::vector<Card*>* stapel,color colorin);
		int FindMinValueColor(std::vector<Card*>* stapel,color colorin);

		int CountColor(std::vector<Card*>* stapel,color colorin);
		int CountColorBelowValue(std::vector<Card*>* stapel,value valuein,color colorin);
		
		value GetMaxValue(std::vector<Card*>* stapel);
		value GetMinValue(std::vector<Card*>* stapel);
		value GetMaxValueOfColor(std::vector<Card*>* stapel,color colorin);
		value GetMinValueOfColor(std::vector<Card*>* stapel,color colorin);
		
		bool EnemyHasCard(value valuein,color colorin);
		bool CardIsPlayable(value valuein,color colorin);
		int EnemyColorCount(color colorin);

		int SuggestHigherCard(color colorin,int initindex);
		
		bool ProcessFirstCardPriority(Trick* stapel,bool playhigh);
		bool PassHighestColorPriority(Trick* stapel,WeightedColor* priority);
		bool PassLowestColorPriority(Trick* stapel,WeightedColor* priority);
		
		bool VoidColorPriority(Trick* stapel);
		
		void PassingVoidColorPriority(std::vector<Card*>* passhand,unsigned int* givencount);
		
		// Printing helpers
		void PrintKaart(Card* kaart);
		void Next(bool pause);
		
		// Inherited virtuals
		void setPlayerId(int);
		int receiveCards(std::vector<Card*>*);
		std::vector<Card*>* passThreeCards(void);
		int receiveThreeCards(std::vector<Card*>*);
		bool hasTwoOfClubs(void);
		void playCard(Trick*);
		void storeTrick(Trick*);
		int getScore(void);
};

#endif /* JOHAN_H_INCLUDED */
