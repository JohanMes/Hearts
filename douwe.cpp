#include <ctime>
#include <cstdlib>
#include <iostream>
#include <iostream>

#include "douwe.h"

Douwe::Douwe()
{
    d_score = 0;
    q_gespeeld = 0;
	shoot_moon = false;
}

void Douwe::setPlayerId(int my_number)
{
	d_player_id = my_number;
}

int Douwe::receiveCards(vector<Card *> * kaarten)
{
	if(kaarten != NULL)
	{
		// Reset all score counters
		score.clear();
		for(int i=0; i<NUM_OF_PLAYERS; i++)
		{	
			score.push_back(0);
		}
	    
	    // Bijhouden van gespeelde kaarten initialiseren
	    gespeeld.clear();
	    for(int j=0; j<4; j++)
	    {
			vector<int> s;
			for(int i=0; i<13; i++)
	    	{
				s.push_back(0);	
	    	}
	   		gespeeld.push_back(s);
		}	
	
		// Spelvariabelen init
	    have_queen  = false;
	    q_gespeeld  = false;
	    first_trick = true;
	    shoot_moon 	= false;
	    shoot_moon_color = -1;
	    
	    // Speler zonder kaarten van een kleur op nul zetten
	    out_of.clear();
	    out_of.push_back(0);
	    out_of.push_back(0);
	    out_of.push_back(0);
	    out_of.push_back(0);
	    
	    // Hand met kaarten aanmaken
	    d_hand.clear();	    
	    for(unsigned int j=0;j < (*kaarten).size(); j++)
	    {
	       d_hand.push_back(kaarten->at(j));
	    }
	}
    return 0;
}

std::vector<Card *> * Douwe::passThreeCards(void)
{
	if(d_hand.size() < 1)
		return 0;

	// Kijken hoeveel kaarten van bepaalde kleur in hand!
	vector<int> num_hand;
	num_hand.clear();
	num_hand.push_back(0);
	num_hand.push_back(0);
	num_hand.push_back(0);
	num_hand.push_back(0);

	for(unsigned int j=0; j<d_hand.size(); j++)
	{
		num_hand[(int)d_hand[j]->color()] += 1;
	}		
	
	// controleer of ik de koningin heb!
    for(unsigned int i = 0;i < d_hand.size();i++) 
	{
		if((int)d_hand[i]->value() == 12 && (int) d_hand[i]->color() == 1 )
		{
			have_queen = true;
		}
	}

	// Decide wheter to shoot the moon or not...
    // Check if many of one color (e.g. >7)
    // or many high hearts and Q and high spades
    // At least! harten aas!
    int counter = 0;
	for(unsigned int j=0; j<d_hand.size(); j++)
	{
		int color = d_hand[j]->color();
		int value = d_hand[j]->value();
		
		if(color == 3 && value == 14)
			counter += 1;
		if(color == 1 && value == 12)
			counter += 1;
	}
	if(counter > 1) // check further if queen spades and ace hearts
	{
		for(int i=0; i<4; i++)
		{
			if(num_hand[i] > 7)
			{
				shoot_moon = true;
				shoot_moon_color = i;
			}
		}
	}

    Card *card;
    vector<Card *> * kaarten_weg = new vector<Card *>;

    for(int i=0; i<3; i++)
    {
		if(shoot_moon) // Give away all lowest cards!
		{
			int lowest = 500;
	        int location = -1;
	
	        for(int j=0; unsigned(j) < d_hand.size(); j++)
	        {
	            int value = d_hand[j]->value();
	            int color = d_hand[j]->color();	            
				int compare = value;
	
				if(color == 3)
	            {
	                compare = value*3;    // Harten zijn best vervelend
	            }
	            if(color == 1 && value > 11)
	            {
	                compare += 30;     			// Schoppen kaarten boven queen zijn gevaarlijk!
	                if(value == 12)
	                {
	                    compare += 30; 			// Schoppen queen weg!
	                }
	            }
	            if(color == shoot_moon_color)
	            {
	            	compare += 10;
	            }
	            if(compare < lowest)
	            {
	                card     = d_hand[j];
	                location = j;
	                lowest   = compare;
	            }
	        }
	        if(location == -1)
	        {
	        	location = 0;
	        	card = d_hand[0];
	    	}
	        kaarten_weg->push_back(card);
	        d_hand.erase(d_hand.begin() + location);
		}
		else		// Give away all highest cards
		{
			int highest  = 0;
	        int location = -1;
	
	        for(int j=0; unsigned(j) < d_hand.size(); j++)
	        {
	            int value = d_hand[j]->value();
	            int color = d_hand[j]->color();
	            int compare = value;
	
				if(color == 3)
	            {
	                compare = (compare-1)*2;    // Harten zijn best vervelend
	            }
	            if(color == 1 && value > 11 && num_hand[1] <= 4)
	            {
	                compare += 30;     			// Schoppen kaarten boven queen zijn gevaarlijk!
	                if(value == 12)
	                {
	                    compare += 30; 			// Schoppen queen weg!
	                }
	            }
	            if(num_hand[1] == 5 && color == 1 && have_queen)
	            {
	            	value = 1;
	            }
	
	            if(compare > highest || (compare == highest && num_hand[color] > num_hand[(int)d_hand[location]->color()]))
	            {
	                card     = d_hand[j];
	                location = j;
	                highest  = compare;
	            }
	        }
	        if(location == -1)
	        {
	        	location = 0;
	        	card = d_hand[0];
	    	}
	        kaarten_weg->push_back(card);
	        d_hand.erase(d_hand.begin() + location);
		}
    }    
    return kaarten_weg;
}

int Douwe::receiveThreeCards(std::vector<Card *> * kaarten_terug)
{
	if(kaarten_terug != NULL)
	{
		hearts_broken = 0;
		
	    d_hand.push_back((*kaarten_terug)[0]);
	    d_hand.push_back((*kaarten_terug)[1]);
	    d_hand.push_back((*kaarten_terug)[2]);
	    
	    for(unsigned int i = 0;i < d_hand.size();i++) 
		{
			if((int)d_hand[i]->value() == 12 && (int) d_hand[i]->color() == 1 )
			{
				have_queen = true;
			}
		}
	}
    return 0;
}

bool Douwe::hasTwoOfClubs()
{
    if(d_hand.size() < 1)
    	return 0;
	
	for(unsigned int i=0; i < d_hand.size(); i++)
    {
        if( (int)(*d_hand[i]).value() == 2 && (int)(*d_hand[i]).color() == 0)
        {
            return 1;
        }
    }
    return 0;
}

void Douwe::playCard(Trick * slag)
{
	if(slag == NULL) 	// Controleer of de pointer wel bestaat!
		return;
		
	// Als klavertwee, speel die!
    for(unsigned int i=0; i < d_hand.size(); i++)
    {
        int color = d_hand[i]->color();
        int value = d_hand[i]->value();

        if(value == 2 && color == 0)
        {
            slag->addCard(d_hand[i]);
			d_hand.erase(d_hand.begin() + i);
            return;
        }
    }
    
    // Opslaan hoeveel kaarten er van een kleur gespeeld zijn!
    vector<int> num_played;
    num_played.clear();
	num_played.push_back(0);
	num_played.push_back(0);
	num_played.push_back(0);
	num_played.push_back(0);
	
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<13; j++)
    	{
    		if(gespeeld[i][j] == 1)
    		{
				num_played[i] += 1;
    		}
    	}
	}
	
	// Voeg hierbij ook de kaarten die al op tafel liggen!
	for(unsigned int i=0; i < slag->cards().size(); i++)
    {
		num_played[(int) slag->cards()[i]->color()] += 1;
    }
	
	// Sla aantal kaarten van één kleur in hand op!
	vector<int> num_hand;
	num_hand.push_back(0);
	num_hand.push_back(0);
	num_hand.push_back(0);
	num_hand.push_back(0);

	for(unsigned int j=0; j<d_hand.size(); j++)
	{
		num_hand[(int)d_hand[j]->color()] += 1;
	}	
	
	
	if(shoot_moon)
	{
		if(slag->cards().size() == 0)
		{
			int best_card  = -1;
		    int best_value = -1;
		    for(unsigned int i=0; i < d_hand.size(); i++)
		    {
		        int value = d_hand[i]->value();
		        int color = d_hand[i]->color();
		
				if(color == shoot_moon_color)
					value += 10;
					
				if(color == 3 && hearts_broken == false)
				{
					value = 0;
				}
		
				if(color == 1 && value == 12 && (gespeeld[1][11] == 0 || gespeeld[1][12] == 0))
				{
					value = 1;
				}
		
                if(value > best_value)
                {
                    best_card  = i;
                    best_value = value;
                }
		    }
		    slag->addCard(d_hand[best_card]);
			d_hand.erase(d_hand.begin() + best_card);
			return;
		}
		int highest_trickcard = findHighestCard(slag);
	
		int best_card  = -1;
		int best_value = 0;
		if(slag->cards().size() == 3)
		{
			if(slag->penalty() > 0)
			{
				// Speel laagste kaart boven hoogste trickkaart!
			    best_value = 100;
			    for(unsigned int i=0; i < d_hand.size(); i++)
			    {
			        int value = d_hand[i]->value();
			        int color = d_hand[i]->color();
			
					if(color == 1 && value == 12)
						value = 1;
						
			        if(color == (int)slag->color())
			        {
		                if(value < best_value && value > highest_trickcard)
		                {
		                    best_card  = i;
		                    best_value = value;
		                }
			        }
			    }
			    if(best_card == -1)  // Als niet bestaand -> laagste van trickkleur spelen
			    {
					for(unsigned int i=0; i < d_hand.size(); i++)
				    {
				        int value = d_hand[i]->value();
				        int color = d_hand[i]->color();
				
						if(color == 1 && value == 12)
							value = 1;
							
				        if(color == (int)slag->color())
				        {
			                if(value < best_value)
			                {
			                    best_card  = i;
			                    best_value = value;
			                }
				        }
				    }
			    }
			}
			else
			{
				best_value = 100;
			    for(unsigned int i=0; i < d_hand.size(); i++)
			    {
			        int value = d_hand[i]->value();
			        int color = d_hand[i]->color();
			
					if(color == 1 && value == 12 && highest_trickcard < 12)
						value = 1;
						
			        if(color == (int)slag->color())
			        {
		                if(value < best_value)
		                {
		                    best_card  = i;
		                    best_value = value;
		                }
			        }
			    }
			}
		}
		else
		{
		    // Kies de hoogste kaart van trickkleur
		    for(unsigned int i=0; i < d_hand.size(); i++)
		    {
		        int value = d_hand[i]->value();
		        int color = d_hand[i]->color();
		
				if(color == 1 && value == 12 && (gespeeld[1][11] == 0 || gespeeld[1][12] == 0))
						value = 1;
				
		        if(color == (int)slag->color())
		        {
	                if(value > best_value)
	                {
	                    best_card  = i;
	                    best_value = value;
	                }
		        }
		    }
		}
	    if(best_card != -1)     // Speel die kaart
	    {
	        slag->addCard(d_hand[best_card]);
			d_hand.erase(d_hand.begin() + best_card);
			return;
	    }
	
		// Als kleurbekennen niet kan, kies kaart met minste punten        
	    int lowest = 500;
        int location = -1;
        for(int j=0; unsigned(j) < d_hand.size(); j++)
        {
            int value = d_hand[j]->value();
            int color = d_hand[j]->color();	            
			int compare = value;

			if(color == 3)
            {
                compare = 100;    // Harten zijn best vervelend
            }
            if(color == 1 && value == 12)
            {
                compare += 120; 			// Schoppen queen weg!
            }
            if(color == shoot_moon_color)
            {
            	compare += 10;
            }
            if(compare < lowest)
            {
                location = j;
                lowest   = compare;
            }
        }
        if(location == -1)
        {
        	location = 0;
    	}	
	    slag->addCard(d_hand[location]); 
		d_hand.erase(d_hand.begin() + location);
		return;
	}
	else
	{
		// Eerste ronde ... Speel hoge kaart (zonder punten)!
		if(first_trick)
		{
			if(slag->color() != 0)
				cout << "Douwe says: Someone opened the first trick with a card not 2 clubs!" << endl;
			
			// Eerst kleurbekennen! --> hoogste
			int index = -1;
			int hoogste = 0;			
	
			for(unsigned int i=0; i<d_hand.size(); i++)
	        {
	            int color = d_hand[i]->color();
	            int value = d_hand[i]->value();
	            
	            if(color == (int)slag->color() && value > hoogste)
	            {
	                index   = i;
	                hoogste = value;
	            }
	        }
	        if(index != -1)
	        {
	        	slag->addCard(d_hand[index]);   
				d_hand.erase(d_hand.begin() + index);	        
	        	return;
	        }
	    	
	    	// Als kleurbekennen niet kan --> speel slechtste kaart!
	    	int highest = 0;
	        int location = -1;
	        for(unsigned int i=0; i<d_hand.size(); i++)
	        {
	            int value = d_hand[i]->value();
	            int color = d_hand[i]->color();
				
				int compare = value;
				
				if(color == 3 || (color == 1 && value == 12) ) 
	            {
	                compare = 1;    // Geen kaarten met punten!
	            }
	            if(color == 1 && value > 12)
	            {
	                compare += 30;  // Schoppen kaarten boven queen zijn gevaarlijk!
	            }
	
	            if(compare > highest || (compare == highest && num_hand[color] > num_hand[d_hand[location]->color()]))
	            {
	                location = i;
	                highest  = compare;
	            }
	        }
	        if(location == -1)
	        	location = 0;
	
	        slag->addCard(d_hand[location]);
			d_hand.erase(d_hand.begin() + location);	        
	        return;
		}
	    
		// Als er nog geen kaarten gespeeld zijn in slag
	    // Speel dan laagste kaart
	    if(slag->cards().size() == 0)
	    {
	    	// Bereken offset per kleur
			vector<int> color_offset;
			color_offset.clear();
			color_offset.push_back(offset(0));
			color_offset.push_back(offset(1));
			color_offset.push_back(offset(2));
			color_offset.push_back(offset(3));
	    	
	        int lowest_card = -1;
	        int best_value 	= 250;
	        for(unsigned int i=0; i<d_hand.size(); i++)
	        {
	            int value = d_hand[i]->value();
	            int color = d_hand[i]->color();
				
				if(!hearts_broken && color == 3)	// Harten hoge waarden geven wanneer nog niet gebroken
					value += 165;
								
				if(color == 1 && value == 12)		// Schoppen koningin hoge waarde geven!
					value += 150;
				
				if(color == 1 && value > 12 && !have_queen && !q_gespeeld)
					value += 120;
					
				value -= color_offset[color];
	
				if(color == 1 && have_queen)
					value += 13;
	
				if(num_played[color] + num_hand[color] > 12)
					value = 165;
				
				if(value < best_value || ( value == best_value && num_played[color] < num_played[d_hand[lowest_card]->color()]))
				{
					lowest_card = i;
	                best_value = d_hand[i]->value();
				}
	        }
			if(lowest_card == -1)
				lowest_card = 0;
	        slag->addCard(d_hand[lowest_card]);
	       	d_hand.erase(d_hand.begin() + lowest_card);		
			return;
	    }
	
	    // Kijk of er niet al 3 kaarten liggen zonder penalty
	    // Zoja, speel dan een tactische kaart
	    if(slag->cards().size() == 3)
	    {
	        if(slag->penalty() == 0) // speel hoogste kaart (die geen punten kost)
	        {
				// Eerst kleurbekennen! --> hoogste
				int index = -1;
				int hoogste = 0;			
				for(unsigned int i=0; i<d_hand.size(); i++)
	            {
	                int color = d_hand[i]->color();
	                int value = d_hand[i]->value();
	                
					if(color == 1 && value == 12)
	                {
	                	value = 1;
	                }
	                
	                if(color == (int)slag->color() && value > hoogste)
	                {
	                    index   = i;
	                    hoogste = value;
	                }
	            }
	            if(index != -1)
	            {
	            	slag->addCard(d_hand[index]);
		        	d_hand.erase(d_hand.begin() + index);	        
		        	return;
	            }
	        	
	        	// Als kleurbekennen niet kan --> speel slechtste kaart!
	        	int location = playWorstCard();
	        	
		        slag->addCard(d_hand[location]);
				d_hand.erase(d_hand.begin() + location);	        
		        return;
	        }
	    	
			if(slag->penalty() > 4)	// Q zit erin! --> niet nemen
	    	{
				// Vind hoogste kaart van slag
				int highest_trickcard = findHighestCard(slag);
			    
			    // kleur bekennen
				int hoogste = 0;
	            int index = -1;
	            for(unsigned int i=0; i<d_hand.size(); i++)
	            {
	                int color = d_hand[i]->color();
	                int value = d_hand[i]->value();
	                
	                if(color == (int)slag->color() && value > hoogste && value < highest_trickcard)
	                {
	                    index   = i;
	                    hoogste = value;
	                }
	            }
	            if(index != -1)
	            {
	            	slag->addCard(d_hand[index]);
					d_hand.erase(d_hand.begin() + index);
					return;
	            }
	            
	            // kijken naar hogere kaarten van slagkleur
	            for(unsigned int i=0; i<d_hand.size(); i++)
	            {
	                int color = d_hand[i]->color();
	                int value = d_hand[i]->value();
	                
	                if(value > hoogste && color == (int)slag->color())
	                {
	                    index   = i;
	                    hoogste = value;
	                }
	            }
	            if(index > -1)
	            {
	            	slag->addCard(d_hand[index]);
	            	d_hand.erase(d_hand.begin() + index);
					return;
	            }
	            
	            // Geen kaarten van slagkleur --> dus lekker hoog kaartje!
				int location = playWorstCard();
		        	
		        slag->addCard(d_hand[location]);
		       	d_hand.erase(d_hand.begin() + location);
				return;
	    	}
	    	
			if(slag->penalty() == 1) 	// niet zo erg. Neem deze als er 3 mensen zonder punten zijn!
	    	{
	    		int mooning = 0;
	    		for(int i=0; i<4; i++)
	    		{
	    			if(score[i] == 0)
	    			{
	    				mooning += 1;
	    			}
	    		}
	    		if(mooning == 3)
	    		{
	    			// zorg dat je de hand krijgt!
	    			int hoogste = 0;
		            int index = -1;
		            for(unsigned int i=0; i<d_hand.size(); i++)
		            {
		                int color = d_hand[i]->color();
		                int value = d_hand[i]->value();
		                
		                if(value == 12 && color == 1)
		                {
		                	value = 1;
		                }
		                
		                if(value > hoogste && color == (int)slag->color())
		                {
		                    index   = i;
		                    hoogste = value;
		                }
		            }
		            if(index > 0)
		            {
		            	slag->addCard(d_hand[index]);
		            	d_hand.erase(d_hand.begin() + index);
						return;
		            }    			
	    		}
	    	}
	    
			// Probeer kaart te ontwijken. Als het echt niet anders kan speel dan hoogste kaart!
			// Vind de hoogste kaart van slagkleur op tafel
		    int highest_trickcard = findHighestCard(slag);
		
		    // Kies de hoogste waarde van trickkleur onder highest_trickcard
		    int best_card  = -1;
		    int best_value = 0;
		    
			
			int mooning = 0;
    		int scorevalue = 0;
			for(int i=0; i<4; i++)
    		{
    			if(score[i] == 0)
    			{
    				mooning += 1;
    			}
    			else
    			{
    				scorevalue = score[i];
    			}    			
    		}
    		
    		if(mooning == 3 && scorevalue > 13)
    		{			
				for(unsigned int i=0; i < d_hand.size(); i++)
			    {
			        int value = d_hand[i]->value();
			        int color = d_hand[i]->color();
			
					if(color == 1 && value == 12)
						value = highest_trickcard + 1;
			
			        if(color == (int)slag->color())
			        {
			            if(value < highest_trickcard)
			            {
			                if(value > best_value)
			                {
			                    best_card  = i;
			                    best_value = value;
			                }
			            }
			        }
			    }
			}
		    if(best_card != -1)     // Speel die kaart
		    {
		        slag->addCard(d_hand[best_card]);
		       	d_hand.erase(d_hand.begin() + best_card);
				return;
		    }
		    
		    // Als die niet bestaat kies hoogste kaart van trickkleur
			int higher_card = -1;
		        best_value  = 0;
	        for(unsigned int i=0; i < d_hand.size(); i++)
	        {
	            int color = d_hand[i]->color();
	            int value = d_hand[i]->value();
				
				if(color == 1 && value == 12)
					value = 1;
				
	            if(color == (int)slag->color())
	            {
	                if(value > best_value)
	                {
	                    higher_card = i;
	                    best_value = value;
	                }
	            }
	        }	    
		    if(higher_card != -1)   // speel die kaart
		    {
		        slag->addCard(d_hand[higher_card]);
				d_hand.erase(d_hand.begin() + higher_card);
				return;
		    }
		    
			// Als kleurbekennen niet hoeft --> Speel slechtste kaart!
		    int location = playWorstCard();
		        	
	        slag->addCard(d_hand[location]);
			d_hand.erase(d_hand.begin() + location);
			return;
		}
	
	    // Vind de hoogste kaart van slagkleur op tafel
	    int highest_trickcard = findHighestCard(slag);
	
	    // Kies de hoogste waarde van trickkleur onder highest_trickcard
	    // tenzij iemand probeert shoot_moon te halen!
	    
	    int best_card  = -1;
	    int best_value = 0;
	    
		int mooning = 0;
		int scorevalue = 0;
		for(int i=0; i<4; i++)
		{
			if(score[i] == 0)
			{
				mooning += 1;
			}
			else
			{
				scorevalue = score[i];
			}    			
		}
		
		if(mooning == 3 && scorevalue > 13 && slag->penalty() > 0 && slag->penalty()<4 && (int)slag->color() != 3)
		{
			for(unsigned int i=0; i < d_hand.size(); i++)
		    {
		        int value = d_hand[i]->value();
		        int color = d_hand[i]->color();
		
				if(color == 1 && value == 12)
					value = 1;
					
		        if(color == (int)slag->color())
		        {
		            if(value > best_value)
	                {
	                    best_card  = i;
	                    best_value = value;
	                }
		        }
			}
		}
		else
		{
			for(unsigned int i=0; i < d_hand.size(); i++)
		    {
		        int value = d_hand[i]->value();
		        int color = d_hand[i]->color();
		
				if(color == 1 && value == 12)
					value = highest_trickcard + 1;
					
		        if(color == (int)slag->color())
		        {
		            if(value < highest_trickcard)
		            {
		                if(value > best_value)
		                {
		                    best_card  = i;
		                    best_value = value;
		                }
		            }
		        }
			}
	    }
	    if(best_card != -1)     // Speel die kaart
	    {
	        slag->addCard(d_hand[best_card]);
			d_hand.erase(d_hand.begin() + best_card);
			return;
	    }
	
		// Als die niet bestaat kies laagste kaart boven highest_trickcard
	    int higher_card = -1;
	    best_value  = 16;
	    for(unsigned int i=0; i < d_hand.size(); i++)
	    {
	        int color = d_hand[i]->color();
	        int value = d_hand[i]->value();
			
			if(color == 1 && value == 12)
			{
				value = 15;
			}		
			
	        if(color == (int)slag->color())
	        {
	            if(value < best_value)
	            {
	                higher_card = i;
	                best_value = value;
	            }
	        }
	    }
	    if(higher_card != -1)   // speel die kaart
	    {
	        slag->addCard(d_hand[higher_card]);
			d_hand.erase(d_hand.begin() + higher_card);
			return;
	    }
		
		// Als kleurbekennen niet kan, kies kaart met meeste punten        
		int location = playWorstCard();
	    	
	    slag->addCard(d_hand[location]); 
		d_hand.erase(d_hand.begin() + location);
		return;
	}
}

void Douwe::storeTrick(Trick * hele_slag)
{
	if(hele_slag)
	{
		int winning_player_id = (hele_slag->startPlayer() + hele_slag->winningPlayer()) % NUM_OF_PLAYERS;
		
	    if(winning_player_id == d_player_id)
	    {
			// Eigen score opslaan
	        d_score += hele_slag->penalty();
	    }
	    else
	    {
	    	if(hele_slag->penalty() > 0)
	    	{	
	    		// Penalty gemist --> stop met proberen shoot_moon te halen!
	    		shoot_moon = false;
	    	}
	    }
	    
	    score[winning_player_id] += hele_slag->penalty();
	    
	    for(int i=0; i<NUM_OF_PLAYERS; i++)
	    {
	    	if(score[i] == 26)
	    	{
				if(i == d_player_id)
				{
					d_score -= 26;
				}
				else
				{
	    			d_score += 26;
				}
	    	}
	    }
	    
	    // Sla op welke kaarten al gespeeld zijn!
	    for(unsigned int i=0; i<hele_slag->cards().size(); i++)	
	  	{
	  		int color = hele_slag->cards()[i]->color();
	  		int value = hele_slag->cards()[i]->value();
	  		
	  		// Opslaan als koningin gespeeld is
	  		if(color == 1 && value == 12)
	  			q_gespeeld = 1;
			
			// Hearst broken?
	  		if(color == 3 && !hearts_broken)
	  			hearts_broken = true;
			
			// Gespeelde kaarten opslaan
	  		gespeeld[color][value-2] = 1;
	  		
	  		// Opslaan wie er geen kaarten van bepaalde kleur meer heeft!
	  		if(color != (int)hele_slag->color())
	  		{
	  			out_of[color] = 2;
	  		}
	  	}
	  	
	  	// Als de hand niet de goede grootte heeft: geef error
	  	if(hele_slag->cards().size() != NUM_OF_PLAYERS)
	  	{
	  		cout << "Douwe says: ERROR! Number of cards in storeTrick is wrong: " << hele_slag->cards().size() << endl;
		}

		// Einde eerste ronde
		if(first_trick)
		{
			if(hele_slag->penalty() > 0)
			{
				cout << "Douwe says: Someone played a card with penalty in first trick! (Forbidden!)" << endl;
				cin.ignore();
			}
	    	first_trick = false;
		}
	}
}
	
int Douwe::getScore()
{
    return d_score;
}

int Douwe::offset(int color)
{
	// Tel gespeelde kaarten vanaf 2 tot er een "gat" komt in de rij
	int offset_count = 0;
	
	for(int i=0; i<13; i++) // De waarde van gespeeld (j) is gelijk aan waarde-2
	{
		offset_count += gespeeld[color][i];
		if(gespeeld[color][i] == 0 || i>=12)
		{
			return offset_count;
		}
	}	
	return 0;
}

int Douwe::playWorstCard(void)
{
	// Nog te implementeren --> ook lagere kaart spelen als er daar minder van in het spel zijn
	// Bijvoorbeeld als de enige kaart van die kleur in mijn hand zit!

	int highest  = 0;
    int location = -1;

    for(unsigned int j=0; j < d_hand.size(); j++)
    {
        int value = d_hand[j]->value();
        int color = d_hand[j]->color();
        
        int compare = value;
        
		// Harten wegen zwaarder
		if(color == 3)
        {
        	if(value > 10)
        	{
        		compare = compare*2;      // Harten zijn best vervelend
			}
			else
			{
				compare += 1;
			}                
        }
        
        // Hoge schoppen wegen nog zwaarder
        if(color == 1 && value > 11 && !q_gespeeld && !have_queen) // Als queen nog niet gespeeld
        {
            compare += 30;     // Schoppen kaarten boven queen zijn gevaarlijk!
            if(value == 12)
            {
                compare += 30; // Schoppen queen nog gevaarlijker!
            }
        }
        
		// Bewaar ten minste één schoppen kaart zolang queen niet gespeeld??

        if(compare > highest)
        {
            location = j;
            highest  = compare;
        }
    }
    
    // If no card found --> return first card in hand... (ERROR)
    if(location == -1)
    {
		return 0;
    }
    return location;
}

int Douwe::findHighestCard(Trick * slag)
{
	int highest_trickcard = 0;
	
	for(unsigned int i=0; i < slag->cards().size(); i++)            // Doorloop stapel
    {
        if((int) slag->cards()[i]->color() == (int)slag->color()) 	// Als kleur hetzelfde is
        {
            if(slag->cards()[i]->value() > highest_trickcard)       // Waarde hoger dan highest_kaart?
            {
                highest_trickcard = slag->cards()[i]->value();      // highest_trickcard -> nieuwe hoge waarde
            }
        }
    }
    return highest_trickcard;
}

