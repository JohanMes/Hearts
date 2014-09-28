#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <windows.h>

#include "hearts.h"
#include "default_player.h"
#include "human_player.h"
#include "jcmes.h"
#include "douwe.h"

using namespace std;

int numruns = 1000;

int main()
{
    // Initialise the random number generator
	srand(static_cast<unsigned>(time(NULL)));
	
	// Hearts simulator
	Hearts h = Hearts();
	
	unsigned int player1total = 0;
	unsigned int player2total = 0;
	unsigned int player3total = 0;
	unsigned int player4total = 0;
	
	unsigned int player1wins = 0;
	unsigned int player2wins = 0;
	unsigned int player3wins = 0;
	unsigned int player4wins = 0;
	
	char title[100] = "";
	
	sprintf(title,"%d of %d (0%%)",0,numruns);

	SetConsoleTitle(title);

	for(int i = 0;i < numruns;i++) {
		
		// Hearts players
		Player* player1 = new DefaultPlayer();
		Player* player2 = new DefaultPlayer();
		Player* player3 = new DefaultPlayer();
		Player* player4 = new jcmes();
			
		h.setPlayer(player1, 0);
		h.setPlayer(player2, 1);
		h.setPlayer(player3, 2);
		h.setPlayer(player4, 3);
		
		h.playGame();
		
		sprintf(title,"%d of %d (%.0f%%)",i+1,numruns,(i+1)/(float)numruns*100.0f);
		
		SetConsoleTitle(title);
		
		unsigned int player1score = player1->getScore();
		unsigned int player2score = player2->getScore();
		unsigned int player3score = player3->getScore();
		unsigned int player4score = player4->getScore();
		
		delete player1;
		delete player2;
		delete player3;
		delete player4;

		// Add to total
		player1total += player1score;
		player2total += player2score;
		player3total += player3score;
		player4total += player4score;

		// Determine winner
		unsigned int winindex = 0;
		unsigned int lowestscore = 0xFFFFFFFF;
		if(player1score < lowestscore) {
			winindex = 1;
			lowestscore = player1score;
		}
		if(player2score < lowestscore) {
			winindex = 2;
			lowestscore = player2score;	
		}
		if(player3score < lowestscore) {
			winindex = 3;
			lowestscore = player3score;	
		}
		if(player4score < lowestscore) {
			winindex = 4;
			lowestscore = player4score;	
		}
		switch(winindex) {
			case 1: {
				player1wins++;
				break;
			}
			case 2: {
				player2wins++;
				break;
			}
			case 3: {
				player3wins++;
				break;
			}
			case 4: {
				player4wins++;
				break;
			}
		}
	}
	
	std::cout << "\nTotaal: " << player1total << ",\tGemiddeld: " << player1total/(float)numruns << ",\tWins: " << player1wins;
	std::cout << "\nTotaal: " << player2total << ",\tGemiddeld: " << player2total/(float)numruns << ",\tWins: " << player2wins;
	std::cout << "\nTotaal: " << player3total << ",\tGemiddeld: " << player3total/(float)numruns << ",\tWins: " << player3wins;
	std::cout << "\nTotaal: " << player4total << ",\tGemiddeld: " << player4total/(float)numruns << ",\tWins: " << player4wins;
	
	return 0;
}
