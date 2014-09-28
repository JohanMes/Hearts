#ifndef CARD_H_included
#define CARD_H_included

#include <iostream>

using namespace std;

enum color
{
	COLOR_CLUBS = 0,
	COLOR_SPADES,
	COLOR_DIAMONDS,
	COLOR_HEARTS
};

enum value
{
	VALUE_TWO = 2,
	VALUE_THREE,
	VALUE_FOUR,
	VALUE_FIVE,
	VALUE_SIX,
	VALUE_SEVEN,
	VALUE_EIGHT,
	VALUE_NINE,
	VALUE_TEN,
	VALUE_JACK,
	VALUE_QUEEN,
	VALUE_KING,
	VALUE_ACE
};

typedef enum color Color;
typedef enum value Value;

Color operator++(Color &, int);
ostream& operator<< (ostream &out, Color color);
Value operator++(Value &, int);
ostream& operator<< (ostream &out, Value value);

class Card {
private:
    Color d_color;
	Value d_value;
	double d_penalty;

public:
	Card();
	Card(Color, Value, double);

	Color color();
	Value value();
	double penalty();

	void setColor(Color);
	void setValue(Value);
	void setPenalty(double);
};

ostream& operator<< (ostream &out, Card card);

#endif // CARD_H_included
