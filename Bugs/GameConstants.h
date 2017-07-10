#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

#include <random>
#include <utility>

// IDs for the game objects

const int IID_ANT_TYPE0 = 0;	// up to four different competitor colonies
const int IID_ANT_TYPE1 = 1;
const int IID_ANT_TYPE2 = 2;
const int IID_ANT_TYPE3 = 3;
const int IID_ANT_HILL = 4;
const int IID_POISON = 5;
const int IID_FOOD = 6;
const int IID_WATER_POOL = 7;
const int IID_ROCK = 8;
const int IID_BABY_GRASSHOPPER = 9;
const int IID_ADULT_GRASSHOPPER = 10;
const int IID_PHEROMONE_TYPE0 = 11;
const int IID_PHEROMONE_TYPE1 = 12;
const int IID_PHEROMONE_TYPE2 = 13;
const int IID_PHEROMONE_TYPE3 = 14;

// sounds

const int SOUND_THEME = 0;


const int SOUND_NONE				   = -1;

// keys the user can hit

const int KEY_PRESS_LEFT  = 1000;
const int KEY_PRESS_RIGHT = 1001;
const int KEY_PRESS_UP	  = 1002;
const int KEY_PRESS_DOWN  = 1003;
const int KEY_PRESS_SPACE = ' ';
const int KEY_PRESS_ESCAPE = '\x1b';
const int KEY_PRESS_TAB	  = '\t';

// # of registers

const int NUM_ANT_MEMORY_CELLS = 3;

// # ant colonies

const int MAX_ANT_COLONIES = 4;
const int INVALID_COLONY_NUMBER = -1;

// board and sprite dimensions

const int VIEW_WIDTH = 64; //256;
const int VIEW_HEIGHT = 64;	 //256;

const int SPRITE_WIDTH = VIEW_WIDTH/16;
const int SPRITE_HEIGHT = VIEW_HEIGHT/16;

const double SPRITE_WIDTH_GL = .5; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .5; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming

// status of each tick (did the player die?)

const int GWSTATUS_CONTINUE_GAME	= 0;
const int GWSTATUS_PLAYER_WON		= 1;
const int GWSTATUS_NO_WINNER		= 2;
const int GWSTATUS_LEVEL_ERROR		= 3;

// test parameter constants

const int NUM_TEST_PARAMS			  = 1;

  // Return a uniformly distributed random int from min to max, inclusive
inline
int randInt(int min, int max)
{
	if (max < min)
		std::swap(max, min);
	static std::random_device rd;
	static std::mt19937 generator(rd());
	std::uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

#endif // GAMECONSTANTS_H_
