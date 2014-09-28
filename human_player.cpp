#include <vector>

#include "deck.h"
#include "player.h"
#include "trick.h"
#include "human_player.h"

HumanPlayer::HumanPlayer()
{
	h_score = 0;
}

void HumanPlayer::setPlayerId(int playerId)
{
	h_player_id = playerId;
	cout << "Please enter your name:" << endl;
	cin >> h_player_name;
	cout << "Good luck " << h_player_name << endl << endl;
}

int HumanPlayer::receiveCards(vector<Card *> *cards)
{
	h_hand = *cards;

	// Addition for when player gets all penalty cards
	h_roundScore = 0;

	for(int current_index = 0; current_index < NUM_OF_CARDS_PER_PLAYER; current_index++)
	{
		for(int seek_index = current_index; seek_index < NUM_OF_CARDS_PER_PLAYER; seek_index++)
		{
			if(h_hand[current_index]->color() > h_hand[seek_index]->color())
			{
				Card *temp = h_hand[current_index];
				h_hand[current_index] = h_hand[seek_index];
				h_hand[seek_index] = temp;
			}
			else if(h_hand[current_index]->color() == h_hand[seek_index]->color())
			{
				if(h_hand[current_index]->value() > h_hand[seek_index]->value())
				{
					Card *temp = h_hand[current_index];
					h_hand[current_index] = h_hand[seek_index];
					h_hand[seek_index] = temp;
				}
			}
			else
			{
				/* Do nothing */
			}
		}
	}

	//cout << "Your hand:" << endl;
	//printCards(&h_hand);

	return 0;
}

vector<Card *> * HumanPlayer::passThreeCards(void)
{
	vector<Card *> *threeCards = new vector<Card *>();
	Card *card = NULL;

    cout << "Pass cards to other player" << endl;
	for(int index = 0; index < 3; index++)
	{
	    cout << "Select card " << index+1 << " of 3" << endl;
	    card = chooseCard(&h_hand);
		threeCards->push_back(card);
		h_hand.erase(h_hand.begin() + findCardIndex(&h_hand, card->color(), card->value()));
	}
	return threeCards;
}

int HumanPlayer::receiveThreeCards(vector<Card *> *threeCards)
{
    cout << "You cards you received: " << endl;
    printCards(threeCards);

	h_hand.insert(h_hand.end(), threeCards->begin(), threeCards->end());

	for(int current_index = 0; current_index < NUM_OF_CARDS_PER_PLAYER; current_index++)
	{
		for(int seek_index = current_index; seek_index < NUM_OF_CARDS_PER_PLAYER; seek_index++)
		{
			if(h_hand[current_index]->color() > h_hand[seek_index]->color())
			{
				Card *temp = h_hand[current_index];
				h_hand[current_index] = h_hand[seek_index];
				h_hand[seek_index] = temp;
			}
			else if(h_hand[current_index]->color() == h_hand[seek_index]->color())
			{
				if(h_hand[current_index]->value() > h_hand[seek_index]->value())
				{
					Card *temp = h_hand[current_index];
					h_hand[current_index] = h_hand[seek_index];
					h_hand[seek_index] = temp;
				}
			}
			else
			{
				/* Do nothing */
			}
		}
	}

	return 0;
}

bool HumanPlayer::hasTwoOfClubs()
{
	for(int cur_card = 0; cur_card < NUM_OF_CARDS_PER_PLAYER; cur_card++)
		if(h_hand[cur_card]->color() == COLOR_CLUBS && h_hand[cur_card]->value() == VALUE_TWO)
			return true;

	return false;
}

void HumanPlayer::playCard(Trick *trick)
{
	Card *card = NULL;
	vector<Card *> cards_in_trick = trick->cards();

	cout << endl << endl << "Trick number " << NUM_OF_CARDS_PER_PLAYER+1-h_hand.size() << endl;
	//cout << "Start player " << trick->startPlayer() << endl;

	if((h_player_id == trick->startPlayer()))
	{
        cout << "You get to go first" << endl;
	    card = chooseCard(&h_hand);
    }
	else
	{
	    cout << "Cards played:" << endl;
	    printCards(&cards_in_trick);
	    cout << endl;
	    if (countCardsOfColor(&h_hand, trick->color()) == 0)
	    {
	        cout << "Play a card" << endl;
            card = chooseCard(&h_hand);
	    }
	    else
	    {
	        do
	        {
                switch(trick->color())
                {
                    case COLOR_CLUBS: { cout << "Play a Club" << endl; break;};
                    case COLOR_SPADES: { cout << "Play a Spade" << endl; break;};
                    case COLOR_DIAMONDS: { cout << "Play a Diamond" << endl; break;};
                    case COLOR_HEARTS: { cout << "Play a Heart" << endl; break;};
                }
                card = chooseCard(&h_hand);
	        } while (card->color() != trick->color());

	    }
	}

	trick->addCard(card);
    h_hand.erase(h_hand.begin() + findCardIndex(&h_hand, card->color(), card->value()));
}

void HumanPlayer::storeTrick(Trick *trick)
{
    vector<Card *> cards_in_trick = trick->cards();
    printCards(&cards_in_trick);
	//cout << "Winning player " << trick->winningPlayer() << endl;
	//cout << "Start player " << trick->startPlayer() << endl;
    if (trick->winningPlayer() + trick->startPlayer() % 4 == h_player_id)
    {
        cout << "Won, penalty: " << trick->penalty() << endl << endl;
    }
    else
    {
        cout << "Lost (penalty: " << trick->penalty() << ")" << endl << endl;
    }

    // Fix for scoreTrick
    if (true)
    {
        if (trick->winningPlayer() + trick->startPlayer() % 4 == h_player_id)
        {
            h_score += trick->penalty();

            // Addition for when player gets all penalty cards
            h_roundScore += trick->penalty();
        }
    }
    else
    {
        if(trick->startPlayer() == h_player_id)
        {
            h_score += trick->penalty();

            // Addition for when player gets all penalty cards
            h_roundScore += trick->penalty();
        }
    }

    if (h_hand.size() == 0)
    {
        cout << "Points this round: " << h_roundScore << endl << endl;
    }
}

int HumanPlayer::getScore()
{
	return h_score;
}

// Addition for when player gets all penalty cards
int HumanPlayer::getRoundScore()
{
	return h_roundScore;
}

void HumanPlayer::printCards(vector<Card *> *cards)
{
    vector<Card *> _hand = *cards;

    for(unsigned int index = 0; index < _hand.size(); index++)
    {
        cout << "[";
        switch(_hand[index]->value())
        {
            case VALUE_TWO: { cout << " 2"; break;};
            case VALUE_THREE: { cout << " 3"; break;};
            case VALUE_FOUR: { cout << " 4"; break;};
            case VALUE_FIVE: { cout << " 5"; break;};
            case VALUE_SIX: { cout << " 6"; break;};
            case VALUE_SEVEN: { cout << " 7"; break;};
            case VALUE_EIGHT: { cout << " 8"; break;};
            case VALUE_NINE: { cout << " 9"; break;};
            case VALUE_TEN: { cout << "10"; break;};
            case VALUE_JACK: { cout << " J"; break;};
            case VALUE_QUEEN: { cout << " Q"; break;};
            case VALUE_KING: { cout << " K"; break;};
            case VALUE_ACE: { cout << " A"; break;};
        }
        switch(_hand[index]->color())
        {
            case COLOR_CLUBS: { cout << "C"; break;};
            case COLOR_SPADES: { cout << "S"; break;};
            case COLOR_DIAMONDS: { cout << "D"; break;};
            case COLOR_HEARTS: { cout << "H"; break;};
        }
        cout << "] ";
    }
    cout << endl;
}

Card *HumanPlayer::chooseCard(vector<Card *> *cards)
{
    vector<Card *> _hand = *cards;
    unsigned int cardIndex = -1;
    while (cardIndex < 0 || cardIndex >= cards->size())
    {
        cout << "Enter number of card you want to play: " << endl;
        printCards(cards);
        for(unsigned int index = 0; index < _hand.size(); index++)
        {
            if (index < 10)
            {
                cout << "  " << index+1 << "   ";
            }
            else
            {
                cout << " " << index+1 << "   ";
            }
        }
        cout << endl;
        cin >> cardIndex;
        cardIndex--;
        if (cardIndex < 0 || cardIndex >= _hand.size())
            cout << "Invalid card number, choose a number between 1 and " << cards->size() << endl;
    }
    return _hand[cardIndex];
}

int HumanPlayer::findCardIndex(vector<Card *> *cards, Color _color, Value _value)
{
    vector<Card *> _hand = *cards;
    int cardIndex = -1;
    for(unsigned int index = 0; index < _hand.size(); index++)
    {
        if (_hand[index]->color() == _color && _hand[index]->value() == _value)
            cardIndex = index;
    }

    return cardIndex;
}

int HumanPlayer::countCardsOfColor(vector<Card *> *cards, Color _color)
{
    vector<Card *> _hand = *cards;
    int numCards = 0;
    for(unsigned int index = 0; index < _hand.size(); index++)
    {
        if (_hand[index]->color() == _color)
            numCards++;
    }

    return numCards;
}

