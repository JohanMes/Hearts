#include <algorithm>
#include <iostream>
#include <cstdio>
#include <vector>
#include <iomanip>

#include "jcmes.h"

#define PRINT

void jcmes::setPlayerId(int id) {
	d_player_id = id;
}

int jcmes::receiveCards(std::vector<Card*>* stapel) {
	
	// Krijgen we geen imaginaire stapel?
	if(stapel) {
		
		// Vervang de kaarten (hand zou leeg moeten zijn, maar toch)
		hand.clear();
		for(unsigned int i = 0;i < stapel->size();i++) {
			hand.push_back(stapel->at(i));
		}
	}
	return 0; // ???
}

std::vector<Card*>* jcmes::passThreeCards(void) {
	std::sort(hand.begin(),hand.end(),PrintColorSort);
#ifdef PRINT
	std::cout << "Current hand (" << hand.size() << " cards):\n";
	for(unsigned int i = 0;i < hand.size();i++) {
		PrintKaart(hand[i]);
	}
#endif	
	// We beginnen opnieuw
	heartsbroken = false;
	playedcards.clear();
	
	unsigned int givencount = 0;

#ifdef PRINT
	std::cout << "\n\nPassing three cards...\n";
#endif
	// Pass de eerste drie kaarten
	std::vector<Card*>* passhand = new std::vector<Card*>;
	
#ifdef PRINT
	std::cout << "Giving spades cards higher than queen...\n";
#endif
	for(unsigned int i = VALUE_ACE;i > VALUE_QUEEN;i--) {
		int index = FindValueColor(&hand,(value)i,COLOR_SPADES);
		if(index != -1) {
			givencount += ExchangeCard(passhand,&hand,index);
			if(givencount == 3) {
				return passhand;
			}
		}
	}
	
	int sqindex = FindValueColor(&hand,VALUE_QUEEN,COLOR_SPADES);
	if(sqindex != -1) {
		
		int spadecount = CountColorBelowValue(&hand,VALUE_QUEEN,COLOR_SPADES);
#ifdef PRINT
		std::cout << "We have " << spadecount << " spade cards below queen...\n";
#endif
		if(spadecount >= 4) {
#ifdef PRINT
			std::cout << "Keeping queen of spades...\n";
#endif
		} else {
#ifdef PRINT
			std::cout << "Giving queen of spades...\n";
#endif
			givencount += ExchangeCard(passhand,&hand,sqindex);
			if(givencount == 3) {
				return passhand;
			}
		}
	}
	
	// Probeer deze kaart kwijt te raken, want dan kunnen we zelf bepalen wat we opgooien in de 1ste ronde
#ifdef PRINT
	std::cout << "Giving two of clubs...\n";
#endif
	givencount += ExchangeCard(passhand,&hand,FindValueColor(&hand,VALUE_TWO,COLOR_CLUBS));
	if(givencount == 3) {
		return passhand;
	}

#ifdef PRINT
	std::cout << "Trying to void diamonds and clubs...\n";
#endif
	WeightedColor tempstruct = {COLOR_CLUBS,0,0}; // nul niks noppes
	
	tempstruct.thiscolor = COLOR_DIAMONDS;
	tempstruct.value1 = CountColor(&hand,COLOR_DIAMONDS);
	tempstruct.value2 = VALUE_ACE - GetMaxValueOfColor(&hand,COLOR_DIAMONDS); // lager is beter!
	playingsort.push_back(tempstruct);
	
	tempstruct.thiscolor = COLOR_CLUBS;
	tempstruct.value1 = CountColor(&hand,COLOR_CLUBS);
	tempstruct.value2 = VALUE_ACE - GetMaxValueOfColor(&hand,COLOR_CLUBS);
	playingsort.push_back(tempstruct);
	
	PassingVoidColorPriority(passhand,&givencount);
	
	if(givencount == 3) {
		return passhand;
	}
#ifdef PRINT
	std::cout << "Giving hearts above seven...\n";
#endif
	for(unsigned int i = VALUE_ACE;i > VALUE_SEVEN;i--) {
		int index = FindValueColor(&hand,(value)i,COLOR_HEARTS);
		if(index != -1) { // houd een paar lage hearts ter verdediging
			givencount += ExchangeCard(passhand,&hand,index);
			if(givencount == 3) {
				return passhand;
			}
		}
	}
#ifdef PRINT
	std::cout << "Giving other highest possible cards (not spades)...\n";
#endif
	for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
		 // lage spades niet wegdoen, hoge spades al gegeven, queen niet wegdoen
		int index = FindValueNotColor(&hand,(value)i,COLOR_SPADES);
		if(index != -1) {
			givencount += ExchangeCard(passhand,&hand,index);
			if(givencount == 3) {
				return passhand;
			}
		}
	}

	return passhand;
}

int jcmes::receiveThreeCards(std::vector<Card*>* passhand) {
#ifdef PRINT
	std::cout << "\n\nReceived the following cards...\n";
#endif	
	for(unsigned int i = 0;i < passhand->size();i++) {
		PrintKaart(passhand->at(i));
		hand.push_back(passhand->at(i));
	}
	
	// Een keertje sorteren, hoeft niet bij elke playCard!
	std::sort(hand.begin(),hand.end(),PrintColorSort);

	Next(true);
	return 0; // ???
}

bool jcmes::hasTwoOfClubs(void) {
	beginwithtwoofclubs = (FindValueColor(&hand,VALUE_TWO,COLOR_CLUBS) != -1);
	return beginwithtwoofclubs;
}

void jcmes::playCard(Trick* stapel) {
	if(stapel) {
		
		std::vector<Card*> cards = stapel->cards();
#ifdef PRINT
		if(hand.size() == 1) {
			std::cout << "Current hand (" << hand.size() << " card):\n";		
		} else {
			std::cout << "Current hand (" << hand.size() << " cards):\n";
		}
		for(unsigned int i = 0;i < hand.size();i++) {
			PrintKaart(hand[i]);
		}
#endif
		playedcardindex = cards.size();
		
		WeightedColor tempstruct = {COLOR_CLUBS,0,0}; // nul niks noppes
		
		if(cards.size() == 0) {
#ifdef PRINT
			std::cout << "\n\nStarting pile...\n";
#endif
			// Uitzondering: we moeten beginnen met klaver twee
			if(beginwithtwoofclubs) {
#ifdef PRINT
				std::cout << "Passing two of clubs...\n";
#endif
				MoveIndexToStapel(stapel,FindValueColor(&hand,VALUE_TWO,COLOR_CLUBS));
				
				beginwithtwoofclubs = false;
				
				return;
			}
#ifdef PRINT
			std::cout << "Opening with diamonds or clubs...\n";
#endif
			// Doe niet moeilijk over beginnen met diamonds of clubs (geen/weinig risico's)
			tempstruct.value1 = GetMaxValueOfColor(&hand,COLOR_DIAMONDS); // begin met hoogste kaart
			tempstruct.thiscolor = COLOR_DIAMONDS;
			playingsort.push_back(tempstruct);
			
			tempstruct.value1 = GetMaxValueOfColor(&hand,COLOR_CLUBS);
			tempstruct.thiscolor = COLOR_CLUBS;
			playingsort.push_back(tempstruct);
			
			// Open met een beetje lage spades of alle spades als de queen gespeeld is
			if(!CardIsPlayable(VALUE_QUEEN,COLOR_SPADES) || GetMaxValueOfColor(&hand,COLOR_SPADES) < VALUE_QUEEN) {
#ifdef PRINT
				std::cout << "Opening with spades too...\n";
#endif
				tempstruct.value1 = GetMaxValueOfColor(&hand,COLOR_SPADES);
				tempstruct.thiscolor = COLOR_SPADES;
				playingsort.push_back(tempstruct);
			} else {
#ifdef PRINT
				std::cout << "Not opening with spades, keeping them as defence...\n";
#endif				
			}
#ifdef PRINT
			std::cout << "Opening with high risky cards...\n";
#endif
			// Speel van de veiligste kaart als het kan de hoogste waarde
			if(ProcessFirstCardPriority(stapel,true)) {
				return;
			}
			
			tempstruct.value1 = EnemyColorCount(COLOR_DIAMONDS); // begin met veiligste kaart
			tempstruct.thiscolor = COLOR_DIAMONDS;
			playingsort.push_back(tempstruct);
			
			tempstruct.value1 = EnemyColorCount(COLOR_CLUBS);
			tempstruct.thiscolor = COLOR_CLUBS;
			playingsort.push_back(tempstruct);

			// Open alleen met spades onder de queen of alles als queen al gespeeld is!
			if(!CardIsPlayable(VALUE_QUEEN,COLOR_SPADES) || GetMinValueOfColor(&hand,COLOR_SPADES) < VALUE_QUEEN) {
#ifdef PRINT
				std::cout << "Opening with spades too...\n";
#endif
				tempstruct.value1 = EnemyColorCount(COLOR_SPADES);
				tempstruct.thiscolor = COLOR_SPADES;
				playingsort.push_back(tempstruct);
			} else {
#ifdef PRINT
				std::cout << "Not opening with spades, keeping them as defence...\n";
#endif
			}

#ifdef PRINT
			std::cout << "Opening with low safe cards instead...\n";
#endif			
			// Speel van de redelijk veilige kaart de laagste waarde
			if(ProcessFirstCardPriority(stapel,false)) {
				return;
			}
			
			if(heartsbroken) {
#ifdef PRINT
				std::cout << "Opening with low hearts as an alternative...\n";
#endif
				for(unsigned int i = VALUE_TWO;i <= VALUE_ACE;i++) {
					int index = FindValueColor(&hand,(value)i,COLOR_HEARTS);
					if(index != -1) {
						index = SuggestHigherCard(COLOR_HEARTS,index);

						MoveIndexToStapel(stapel,index);
						return;
					}
				}
			}
#ifdef PRINT
			std::cout << "Opening with spades as an alternative...\n";
#endif
			int sqindex = FindValueColor(&hand,VALUE_QUEEN,COLOR_SPADES);
			for(unsigned int i = VALUE_TWO;i <= VALUE_ACE;i++) {
				int index = FindValueColor(&hand,(value)i,COLOR_SPADES);
				if(index != -1 && index != sqindex) {
#ifdef PRINT
					std::cout << "Passing lowest allowed spades (not queen)...\n";
#endif
					index = SuggestHigherCard(COLOR_SPADES,index);

					MoveIndexToStapel(stapel,index);
					return;
				}
			}
#ifdef PRINT
			std::cout << "Passing queen of spades as very last option...\n";
#endif
			if(sqindex != -1) {
				if(MoveIndexToStapel(stapel,sqindex)) {
					return;
				}
			}
#ifdef PRINT
			std::cout << "Opening with hearts (against rules)...\n";
#endif
			for(unsigned int i = VALUE_TWO;i <= VALUE_ACE;i++) {
				int index = FindValueColor(&hand,(value)i,COLOR_HEARTS);
				if(index != -1) {
					index = SuggestHigherCard(COLOR_HEARTS,index);

					MoveIndexToStapel(stapel,index);
					return;
				}
			}
		} else { // end of starting pile
#ifdef PRINT
			if(stapel->penalty() == 1) {
				std::cout << "\n\nCurrent pile (" << stapel->penalty() << " point):\n";
			} else {
				std::cout << "\n\nCurrent pile (" << stapel->penalty() << " points):\n";				
			}
			for(unsigned int i = 0;i < cards.size();i++) {
				PrintKaart(cards[i]);
			}
#endif
			int validplayindex = FindFirstColor(&hand,stapel->color());
			
			// We moeten kleur volgen...
			if(validplayindex != -1) {
#ifdef PRINT
				std::cout << "Following starting color (" << stapel->color() << ")...\n";
#endif
				int sqindex = FindValueColor(&hand,VALUE_QUEEN,COLOR_SPADES);
	
				// Uitzondering: eerste ronde zo hoog mogelijk meegeven, maar GEEN punten
				if(hand.size() == 13) {
#ifdef PRINT
					std::cout << "Following pile of first round...\n";
#endif
					for(unsigned int i = VALUE_ACE;i > VALUE_TWO;i--) {
						int index = FindValueColor(&hand,(value)i,stapel->color()); // clubs
						if(index != -1) { // binnen deze loop slagen we altijd!
#ifdef PRINT
							std::cout << "Passing highest possible zero points card of color...\n";
#endif
							MoveIndexToStapel(stapel,index);
							return;
						}
					}
				}

				// Er ligt wat spul van waarde op de stapel, probeer te omzeilen
				if(stapel->penalty() > 0) {
#ifdef PRINT					
					std::cout << "Trying to avoid penalties (" << stapel->penalty() << ")...\n";
#endif
					// Geef altijd van de huidige kleur de kaart weg die lager is dan de huidige hoogste kaart
					Value maxstapelvalue = GetMaxValueOfColor(&cards,stapel->color());
					for(unsigned int i = maxstapelvalue-1;i >= VALUE_TWO;i--) {
						int index = FindValueColor(&hand,(value)i,stapel->color());
						if(index != -1) { // mag ook de queen of spades zijn!
#ifdef PRINT
							std::cout << "Avoided penalty using highest possible card below current max...\n";
#endif
							MoveIndexToStapel(stapel,index);
							return;
						}
					}
					
					// Als het de laatste is of als het toch een hopeloze zaak is...
					if(cards.size() == 3 || EnemyColorCount(stapel->color()) == 0) { // iedereen gaat toch punten spelen, dus doe niet moeilijk
						
						// Als we toch met de laatste kaart bezig zijn en ducken kan niet, hoogst meegeven
						for(unsigned int i = VALUE_ACE;i > maxstapelvalue;i--) {
							int index = FindValueColor(&hand,(value)i,stapel->color());
							if(index != -1 && index != sqindex) { // geen queen of spades geven als het kan
#ifdef PRINT
								std::cout << "Taking stapel using highest possible card (not queen)...\n"; // we zijn toch de lul
#endif
								MoveIndexToStapel(stapel,index);
								return;
							}
						}
					} else {
						// Hebben we die zalige kaarten niet gevonden? Geef iets hoger mee
						for(unsigned int i = maxstapelvalue+1;i <= VALUE_ACE;i++) {
							int index = FindValueColor(&hand,(value)i,stapel->color());
							if(index != -1 && index != sqindex) { // geen queen of spades geven als het kan
#ifdef PRINT
								std::cout << "Taken penalty using higher card than pile (not queen)...\n";
#endif
								index = SuggestHigherCard(stapel->color(),index);

								MoveIndexToStapel(stapel,index);
								return;
							}
						}
					}
					
					// Hebben we niks tactisch kunnen geven? Doe het dan zo maar...
					if(MoveIndexToStapel(stapel,sqindex)) {
#ifdef PRINT
						std::cout << "Taking stapel using queen of spades...\n"; // we zijn toch de lul
#endif
						return;
					}
				} else { // Stapel is tot nu toe waardeloos, we moeten kleur volgen
#ifdef PRINT
					std::cout << "Don't have to avoid penalties...\n";
#endif
					if(cards.size() == 3) { // geen punten op stapel, kleur volgen...
#ifdef PRINT
						std::cout << "Passing last card of pile...\n";
#endif
						if(stapel->color() == COLOR_SPADES) {
#ifdef PRINT
							std::cout << "Current color is spades...\n";
#endif
							// Raak de queen of spades kwijt...
							int sqindex = FindValueColor(&hand,VALUE_QUEEN,COLOR_SPADES);
							if(sqindex != -1) {
								if(GetMaxValueOfColor(&cards,COLOR_SPADES) > VALUE_QUEEN) { // queen kan er nog niet liggen!
#ifdef PRINT
									std::cout << "Dumping queen of spades...\n";
#endif									
									MoveIndexToStapel(stapel,sqindex);
									return;
								}
							}
							
							// Geef daarna van spades de zo hoog mogelijke kaart weg
							for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
								if(i != VALUE_QUEEN) {
									int index = FindValueColor(&hand,(value)i,stapel->color());
									if(index != -1) {
#ifdef PRINT										
										std::cout << "Passing highest allowed card (not queen)...\n";
#endif										
										MoveIndexToStapel(stapel,index);
										return;
									}
								}
							}
#ifdef PRINT							
							std::cout << "Passing queen of spades...\n";
#endif							
							MoveIndexToStapel(stapel,sqindex); // jammer
						} else { // er liggen geen spades, laatste kaart
							
							// Geef van de huidige kleur de zo hoog mogelijke kaart weg
							for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
								int index = FindValueColor(&hand,(value)i,stapel->color());
								if(index != -1) {
#ifdef PRINT									
									std::cout << "Passing highest allowed card...\n";
#endif									
									MoveIndexToStapel(stapel,index);
									return;
								}
							}
						}
					} else { // er liggen minder dan 3 kaarten, tot nu toe waardeloze stapel, kleur volgen
					
						// Raak de queen of spades kwijt...
						int sqindex = FindValueColor(&hand,VALUE_QUEEN,COLOR_SPADES);
						if(stapel->color() == COLOR_SPADES) {
							if(sqindex != -1) {
								if(GetMaxValueOfColor(&cards,COLOR_SPADES) > VALUE_QUEEN) { // queen kan er nog niet liggen!
#ifdef PRINT								
									std::cout << "Dumping queen of spades...\n";
#endif									
									MoveIndexToStapel(stapel,sqindex);
									return;
								}
							}
						}
					
						unsigned int enemycolorcount = EnemyColorCount(stapel->color());
#ifdef PRINT
						std::cout << "Enemy has " << enemycolorcount << " pile color cards...\n";
#endif
						// Elke speler moet er minstens eentje hebben...
						if(enemycolorcount >= 3*cards.size()) {
#ifdef PRINT							
							std::cout << "Assuming noone will play penalty cards...\n";
#endif							
							if(stapel->color() == COLOR_SPADES) {
								if(EnemyHasCard(VALUE_QUEEN,COLOR_SPADES)) {
#ifdef PRINT									
									std::cout << "Queen of spades can still be played...\n";
#endif
									for(unsigned int i = VALUE_QUEEN-1;i >= VALUE_TWO;i--) {
										int index = FindValueColor(&hand,(value)i,stapel->color());
										if(index != -1) {
#ifdef PRINT											
											std::cout << "Passing highest possible card below queen of spades...\n";
#endif											
											MoveIndexToStapel(stapel,index);
											return;
										}
									}
#ifdef PRINT
									std::cout << "Passing higher than queen of spades...\n";
#endif
									for(unsigned int i = VALUE_QUEEN+1;i <= VALUE_ACE;i++) {
										int index = FindValueColor(&hand,(value)i,stapel->color());
										if(index != -1) {
#ifdef PRINT
											std::cout << "Passing lowest possible card above queen...\n";
#endif
											// Kan nog een ace opleveren i.p.v. een king...
											index = SuggestHigherCard(stapel->color(),index);
											MoveIndexToStapel(stapel,index);
											return;
										}
									}
								} else if(sqindex != -1) {
#ifdef PRINT
									std::cout << "Not passing queen of spades because pile value is too low...\n";
#endif
									for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
										if(i != VALUE_QUEEN) {
											int index = FindValueColor(&hand,(value)i,stapel->color());
											if(index != -1) {
#ifdef PRINT									
												std::cout << "Passing highest possible card (not queen)...\n";
#endif									
												MoveIndexToStapel(stapel,index);
												return;
											}
										}
									}
								}
							}
							
							for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
								int index = FindValueColor(&hand,(value)i,stapel->color());
								if(index != -1) {
#ifdef PRINT									
									std::cout << "Passing highest possible card...\n";
#endif									
									MoveIndexToStapel(stapel,index);
									return;
								}
							}
						} else if(enemycolorcount > 0) {
#ifdef PRINT							
							std::cout << "Assuming someone will play penalty cards...\n";
#endif							
							for(unsigned int i = GetMaxValueOfColor(&cards,stapel->color())-1;i >= VALUE_TWO;i--) {
								int index = FindValueColor(&hand,(value)i,stapel->color());
								if(index != -1) {
#ifdef PRINT									
									std::cout << "Passing highest possible card below current max...\n";
#endif									
									MoveIndexToStapel(stapel,index);
									return;
								}
							}
							
							for(unsigned int i = GetMaxValueOfColor(&cards,stapel->color())+1;i <= VALUE_ACE;i++) {
								int index = FindValueColor(&hand,(value)i,stapel->color());
								if(index != -1) {
#ifdef PRINT									
									std::cout << "Passing lowest possible card above current max...\n";
#endif									
									index = SuggestHigherCard(stapel->color(),index);	
									MoveIndexToStapel(stapel,index);
									return;
								}
							}
						} else if(enemycolorcount == 0) {
#ifdef PRINT							
							std::cout << "Everyone will play penalty cards...\n";
#endif								
							for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
								int index = FindValueColor(&hand,(value)i,stapel->color());
								if(index != -1) {
#ifdef PRINT									
									std::cout << "Passing highest possible card...\n";
#endif										
									MoveIndexToStapel(stapel,index);
									return;
								}
							}
						}
					}
				}
			} else { // we hoeven de kleur niet te volgen
#ifdef PRINT
				std::cout << "Not following starting color...\n";
#endif
				// Uitzondering: eerste ronde zo hoog mogelijk troep meegeven, maar GEEN punten
				if(hand.size() == 13) {
#ifdef PRINT
					std::cout << "Not following pile of first round...\n";
#endif
					for(unsigned int i = VALUE_ACE;i > VALUE_TWO;i--) {
						int index = FindValueNotColor(&hand,(value)i,COLOR_HEARTS); 
						if(index != -1 && !(hand[index]->color() == COLOR_SPADES && i <= VALUE_QUEEN)) { // geen spades op of onder queen weggeven
#ifdef PRINT
							std::cout << "Passing highest possible zero points card excluding low and queen of spades...\n";
#endif
							MoveIndexToStapel(stapel,index);
							return;
						}
					}
					
					for(unsigned int i = VALUE_ACE;i > VALUE_TWO;i--) {
						int index = FindValueColor(&hand,(value)i,COLOR_SPADES); 
						if(index != -1 && i != VALUE_QUEEN) { // geen queen of spades meegeven
#ifdef PRINT
							std::cout << "Passing any spades (not queen)...\n";
#endif
							MoveIndexToStapel(stapel,index);
							return;
						}
					}
					
#ifdef PRINT					
					std::cout << "Passing hearts in first round because we have to...\n";
#endif
					for(unsigned int i = VALUE_ACE;i > VALUE_TWO;i--) {
						int index = FindValueColor(&hand,(value)i,COLOR_HEARTS); 
						if(index != -1) { // geen queen of spades meegeven
#ifdef PRINT
							std::cout << "Passing highest possible hearts card...\n";
#endif
							MoveIndexToStapel(stapel,index);
							return;
						}
					}
#ifdef PRINT					
					std::cout << "Passing queen of spades in first round because we have to...\n";
#endif					
					// Als we hier terecht komen, hebben we alleen de queen of spades maar. Komt toch niet voor...
					if(MoveIndexToStapel(stapel,FindValueColor(&hand,VALUE_QUEEN,COLOR_SPADES))) {
						return;
					}
				}

				// Probeer als eerste queen of spades te dumpen
				int sqindex = FindValueColor(&hand,VALUE_QUEEN,COLOR_SPADES);
				if(sqindex != -1) {
#ifdef PRINT					
					std::cout << "Dumping queen of spades...\n";
#endif				
					MoveIndexToStapel(stapel,sqindex);
					return;
				}

				// Of gooi hoge hearts weg (lage voor verdediging gebruiken)
				for(unsigned int i = VALUE_ACE;i > VALUE_SEVEN;i--) {
					int index = FindValueColor(&hand,(value)i,COLOR_HEARTS);
					if(index != -1) {
#ifdef PRINT
						std::cout << "Passing hearts above seven...\n";
#endif
						MoveIndexToStapel(stapel,index);

						return;
					}
				}
				
				WeightedColor tempstruct = {COLOR_CLUBS,0,0}; // nul niks noppes
#ifdef PRINT
				std::cout << "Trying to void diamonds or clubs...\n";
#endif
				// Raak van de kaarten met de laagste aantallen alles kwijt...
				tempstruct.thiscolor = COLOR_DIAMONDS;
				tempstruct.value1 = CountColor(&hand,COLOR_DIAMONDS); // sorteren of aantal
				tempstruct.value2 = VALUE_ACE - GetMaxValueOfColor(&hand,COLOR_DIAMONDS); // en als dat gelijk is, sorteren op kaarthoogte
				playingsort.push_back(tempstruct);
				
				tempstruct.thiscolor = COLOR_CLUBS;
				tempstruct.value1 = CountColor(&hand,COLOR_CLUBS);
				tempstruct.value2 = VALUE_ACE - GetMaxValueOfColor(&hand,COLOR_CLUBS); // hoger is beter, dus even omdraaien
				playingsort.push_back(tempstruct);
				
				// Dump hoge spades of alle spades als gevaar weg is
				if(GetMaxValueOfColor(&hand,COLOR_SPADES) > VALUE_QUEEN || !CardIsPlayable(VALUE_QUEEN,COLOR_SPADES)) {
#ifdef PRINT
					std::cout << "Trying to void spades too, it has dangerous cards...\n";
#endif
					tempstruct.thiscolor = COLOR_SPADES;
					tempstruct.value1 = CountColor(&hand,COLOR_SPADES);
					tempstruct.value2 = VALUE_ACE - GetMaxValueOfColor(&hand,COLOR_SPADES);
					playingsort.push_back(tempstruct);
				} else {
#ifdef PRINT
					std::cout << "Keeping low spades as defence...\n";
#endif					
				}
				
				if(VoidColorPriority(stapel)) {
					return;
				}
				
				// Of gooi alle hearts weg
				for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
					int index = FindValueColor(&hand,(value)i,COLOR_HEARTS);
					if(index != -1) {
#ifdef PRINT
						std::cout << "Passing highest hearts card...\n";
#endif
						MoveIndexToStapel(stapel,index);

						return;
					}
				}
				
				// Of gooi iets weg
				for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
					int index = FindFirstValue(&hand,(value)i);
					if(index != -1) {
#ifdef PRINT
						std::cout << "Passing any highest card...\n";
#endif
						MoveIndexToStapel(stapel,index);

						return;
					}
				}
			}
		}
	}
}

void jcmes::storeTrick(Trick* stapel) {
	if(stapel) {
				
		// Houd bij wat er allemaal gespeeld is
		for(unsigned int i = 0;i < stapel->cards().size();i++){
			playedcards.push_back(stapel->cards()[i]);
		}
		
		// Kijk of we al met harten mogen beginnen
		if(!heartsbroken) {
			if(FindFirstColor(&playedcards,COLOR_HEARTS) != -1) {
				heartsbroken = true;
			}
		}
		
		// En sla score op als de zooi naar ons toegaat
		if(stapel->winningPlayer() == playedcardindex) {
#ifdef PRINT
			if(stapel->penalty() == 1) {
				std::cout << "\n\nWon cards (" << stapel->penalty() << " point):\n";
			} else {
				std::cout << "\n\nWon cards (" << stapel->penalty() << " points):\n";
			}
			for(unsigned int i = 0;i < stapel->cards().size();i++) {
				PrintKaart(stapel->cards()[i]);
			}
#endif				
			totalscore += stapel->penalty();
		}
		
		if(stapel->penalty() > 0 && stapel->winningPlayer() == playedcardindex) {
			Next(true);
		} else {
			Next(false);
		}
	}
}

int jcmes::getScore(void) {
	return totalscore;
}

////////////////////////////////////////////////////////////////////////////////
/*
	Utils
*/
int jcmes::CountColorBelowValue(std::vector<Card*>* stapel,value valuein,color colorin) {
	int count = 0;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->color() == colorin && stapel->at(i)->value() < valuein) {
				count++;
			}
		}
	}
	return count;
}

int jcmes::CountColor(std::vector<Card*>* stapel,color colorin) {
	int count = 0;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->color() == colorin) {
				count++;
			}
		}
	}
	return count;
}

int jcmes::ExchangeCard(std::vector<Card*>* to,std::vector<Card*>* from,int index) {
	if(index != -1) {
		PrintKaart(from->at(index));
		
		to->push_back(from->at(index));
		from->erase(from->begin() + index);
		return 1;
	}
	
	return 0;
}

bool jcmes::MoveIndexToStapel(Trick* stapel,int index) {
	if(index < (int)hand.size() && index >= 0) {
		PrintKaart(hand[index]);
		
		stapel->addCard(hand[index]);
		hand.erase(hand.begin() + index); // 0 based!
		return true;
	} else {
		return false;
	}
}

int jcmes::FindFirstColor(std::vector<Card*>* stapel,color colorin) {
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->color() == colorin) {
				return i;
			}
		}
	}
	return -1;
}

int jcmes::FindFirstNotColor(std::vector<Card*>* stapel,color colorin) {
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->color() != colorin) {
				return i;
			}
		}
	}
	return -1;
}

int jcmes::FindFirstValue(std::vector<Card*>* stapel,value valuein) {
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() == valuein) {
				return i;
			}
		}
	}
	return -1;
}

int jcmes::FindFirstNotValue(std::vector<Card*>* stapel,value valuein) {
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() != valuein) {
				return i;
			}
		}
	}
	return -1;
}

int jcmes::FindMaxValue(std::vector<Card*>* stapel) {
	int maxvalueindex = -1;
	int maxvalue = 0;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() > maxvalue) {
				maxvalueindex = i;
				maxvalue = stapel->at(i)->value();
			}
		}
	}
	return maxvalueindex;
}

int jcmes::FindMinValue(std::vector<Card*>* stapel) {
	int maxvalueindex = -1;
	int maxvalue = 0xFFFFFFFF;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() < maxvalue) {
				maxvalueindex = i;
				maxvalue = stapel->at(i)->value();
			}
		}
	}
	return maxvalueindex;
}

int jcmes::FindMaxValueColor(std::vector<Card*>* stapel,color colorin) {
	int maxvalueindex = -1;
	int maxvalue = 0;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() > maxvalue && stapel->at(i)->color() == colorin) {
				maxvalueindex = i;
				maxvalue = stapel->at(i)->value();
			}
		}
	}
	return maxvalueindex;
}

int jcmes::FindMinValueColor(std::vector<Card*>* stapel,color colorin) {
	int maxvalueindex = -1;
	int maxvalue = 0xFFFFFFFF;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() < maxvalue && stapel->at(i)->color() == colorin) {
				maxvalueindex = i;
				maxvalue = stapel->at(i)->value();
			}
		}
	}
	return maxvalueindex;
}

int jcmes::FindValueColor(std::vector<Card*>* stapel,value valuein,color colorin) {
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() == valuein && stapel->at(i)->color() == colorin) {
				return i;
			}
		}
	}
	return -1;
}

int jcmes::FindValueNotColor(std::vector<Card*>* stapel,value valuein,color colorin) {
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() == valuein && stapel->at(i)->color() != colorin) {
				return i;
			}
		}
	}
	return -1;
}

int jcmes::FindValueNot2Color(std::vector<Card*>* stapel,value valuein,color colorin1,color colorin2) {
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->value() == valuein && stapel->at(i)->color() != colorin1 && stapel->at(i)->color() != colorin2) {
				return i;
			}
		}
	}
	return -1;
}

value jcmes::GetMaxValue(std::vector<Card*>* stapel) {
	value result = VALUE_TWO;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			result = max(result,stapel->at(i)->value());
		}
	}
	return result;
}

value jcmes::GetMaxValueOfColor(std::vector<Card*>* stapel,color colorin) {
	value result = VALUE_TWO;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->color() == colorin) {
				result = max(result,stapel->at(i)->value());
			}
		}
	}
	return result;
}

value jcmes::GetMinValue(std::vector<Card*>* stapel) {
	value result = VALUE_ACE;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			result = min(result,stapel->at(i)->value());
		}
	}
	return result;
}

value jcmes::GetMinValueOfColor(std::vector<Card*>* stapel,color colorin) {
	value result = VALUE_ACE;
	if(stapel) {
		for(unsigned int i = 0;i < stapel->size();i++) {
			if(stapel->at(i)->color() == colorin) {
				result = min(result,stapel->at(i)->value());
			}
		}
	}
	return result;
}

bool jcmes::EnemyHasCard(value valuein,color colorin) {
	// Hij mag niet al gespeeld zijn
	bool result = (FindValueColor(&playedcards,valuein,colorin) == -1);
	if(result) { // ligt hij NIET op de al gespeelde stapel...
		result = (FindValueColor(&hand,valuein,colorin) == -1);
	}
	return result;
}

bool jcmes::CardIsPlayable(value valuein,color colorin) {
	// Hij mag niet al gespeeld zijn
	return (FindValueColor(&playedcards,valuein,colorin) == -1);
}

int jcmes::EnemyColorCount(color colorin) {
	return 13 - CountColor(&playedcards,colorin) - CountColor(&hand,colorin);
}

int jcmes::SuggestHigherCard(color colorin,int initindex) {
	
	int result = initindex;

	// Voorbeeld:
	// Huidige stapel heeft 6 als max. Zelf hebben we 7 en 9. 8 is al gespeeld.
	// Dan moet er een 9 worden gespeeld!
	
	while(1) {
		
		Card* prevcard = hand[result];
		int nextindex = -1;

		// Get next playable card in hand...
		for(unsigned int i = prevcard->value()+1;i <= GetMaxValueOfColor(&hand,colorin);i++) {
			nextindex = FindValueColor(&hand,(value)i,colorin);
			if(nextindex != -1) {
				break;
			}
		}
		
		// Stop if we can't pass any higher...
		if(nextindex == -1) {
			return result;
		}
		
		Card* nextcard = hand[nextindex];
		
		// See if we can safely pass the next one found in the previous loop
		for(unsigned int j = prevcard->value()+1;j < nextcard->value();j++) {
			if(EnemyHasCard((value)j,colorin)) { // kaart speelbaar? niet proberen
				return result;
			}
		}
		
		// Is deze kaart speelbaar? resultaat verhogen
		result = nextindex;
	}
	return result;
}

////////////////////////////////////////////////////////////////////////////////
/*
	Gesorteerde opdrachten afhandelen...
*/
bool jcmes::PassHighestColorPriority(Trick* stapel,WeightedColor* priority) {
	for(unsigned int i = VALUE_ACE;i >= VALUE_TWO;i--) {
		int index = FindValueColor(&hand,(value)i,priority->thiscolor);
		if(index != -1 && !(i == VALUE_QUEEN && priority->thiscolor == COLOR_SPADES)) { // Nooit met queen of spades openen
#ifdef PRINT
			std::cout << "Assuming noone will play penalty cards (" << EnemyColorCount(priority->thiscolor) << " cards left)...\n";

			std::cout << "Passing highest possible " << priority->thiscolor << " card...\n";
#endif
			return MoveIndexToStapel(stapel,index);
		}
	}
	return false;
}

bool jcmes::PassLowestColorPriority(Trick* stapel,WeightedColor* priority) {
	for(unsigned int i = VALUE_TWO;i <= VALUE_ACE;i++) {
		int index = FindValueColor(&hand,(value)i,priority->thiscolor);
		if(index != -1) {
#ifdef PRINT
			std::cout << "Assuming someone will play penalty cards (" << EnemyColorCount(priority->thiscolor) << " cards left)...\n";
	
			std::cout << "Passing lowest possible " << priority->thiscolor << " card...\n";
#endif
			index = SuggestHigherCard(priority->thiscolor,index);
			return MoveIndexToStapel(stapel,index);
		}
	}
	return false;
}

bool jcmes::ProcessFirstCardPriority(Trick* stapel,bool playhigh) {
	
	// Sorteer!
	std::sort(playingsort.begin(),playingsort.end(),OpenColorSort);

	bool result = false;
	for(unsigned int i = 0;i < playingsort.size();i++) {
		if(playhigh) { // pass zo hoog mogelijk
			if(EnemyColorCount(playingsort[i].thiscolor) > 6) { // (5 of 6?) maat voor 'niemand speelt punten'
				if(PassHighestColorPriority(stapel,&playingsort[i])) {
					result = true;
					break;
				}
			} else {
#ifdef PRINT
				std::cout << "Not opening with " << playingsort[i].thiscolor << ", too dangerous (" << EnemyColorCount(playingsort[i].thiscolor) << " cards left)...\n";
#endif
			}
		} else { // speel gewoon alles (laatste alternatief)
			if(PassLowestColorPriority(stapel,&playingsort[i])) {
				result = true;
				break;
			}
		}
	}
	playingsort.clear();
	return result;
}

bool jcmes::VoidColorPriority(Trick* stapel) {
	
	// Sorteer!
	std::sort(playingsort.begin(),playingsort.end(),VoidColorSort);
	
	bool result = false;
	
	// Geef van de kleur met de minste kaarten over de hoogste kaart weg!
	for(unsigned int i = 0;i < playingsort.size();i++) {
		int index = FindMaxValueColor(&hand,playingsort[i].thiscolor);
		if(index != -1) {
#ifdef PRINT
			std::cout << "Trying to void suit of color " << playingsort[i].thiscolor << "...\n";
#endif
			MoveIndexToStapel(stapel,index);
			result = true;
			break;
		}
	}
	playingsort.clear();
	return result;
}

void jcmes::PassingVoidColorPriority(std::vector<Card*>* passhand,unsigned int* givencount) {
	
	// Sorteer!
	std::sort(playingsort.begin(),playingsort.end(),VoidColorSort);
	
	// Begin met de kleur met het minste aantal kaarten
	for(unsigned int i = 0;i < playingsort.size();i++) {
		if(3 - *givencount >= playingsort[i].value1) { // Geef iedere kleur die we kunnen legen weg
		
#ifdef PRINT
			std::cout << "Voiding suit of color " << playingsort[i].thiscolor << "...\n";
#endif
			for(unsigned int j = VALUE_ACE;j >= VALUE_TWO;j--) {
				int index = FindValueColor(&hand,(value)j,playingsort[i].thiscolor);
				if(index != -1) {

					*givencount += ExchangeCard(passhand,&hand,index);
					if(*givencount == 3) {
						break;
					}
				}
			}
		}
	}
	playingsort.clear();
}

////////////////////////////////////////////////////////////////////////////////
/*
	Printing helpers
*/
void jcmes::PrintKaart(Card* kaart) {
#ifdef PRINT
	std::cout << setw(8) << kaart->value() << " of " << kaart->color() << ": " << (int)kaart->value() << "\n";
#endif
}

void jcmes::Next(bool pause) {
#ifdef PRINT
	if(pause) {
		cin.ignore();
	}
	
	// Moet ook op Linux werken...
	if(system("cls")) {
		system("clear");
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////
/*
	Sorteerders
*/
bool PrintColorSort(Card* a,Card* b) {
	// Kleuren en waarden van laag naar hoog presenteren!
	if(a->color() == b->color()) {
		return a->value() < b->value();
	} else {
		return a->color() < b->color();
	}
}

bool OpenColorSort(WeightedColor a,WeightedColor b) {
	// Van hoog naar laag sorteren m.b.v. kaarthoogte of veiligheid
	return a.value1 > b.value1;
}

bool VoidColorSort(WeightedColor a,WeightedColor b) {
	// Sorteer eerst op value1, als die gelijk zijn aan value2 beginnen
	if(a.value1 == b.value1) {
		return a.value2 < b.value2;
	} else {
		return a.value1 < b.value1;
	}
}
