#ifndef GAMEWORLD_H_
#define GAMEWORLD_H_

#include "GameConstants.h"
#include <string>
#include <vector>

const int START_PLAYER_LIVES = 1;	// usually 3, but this is a simulation, not a video game

class GameController;

class GameWorld
{
public:

	GameWorld(std::string assetDir)
	 : m_lives(START_PLAYER_LIVES), m_score(0), m_level(0),
	   m_controller(nullptr), m_assetDir(assetDir)
	{
		m_winnerColony = "No winner!";
	}

	virtual ~GameWorld()
	{
	}
	
	virtual int init() = 0;
	virtual int move() = 0;
	virtual void cleanUp() = 0;

	void setGameStatText(std::string text);

	bool getKey(int& value);
	void playSound(int soundID);

	std::string getFieldFilename() const
	{
		if (m_params.empty())
			return "";

		return m_params[0];
	}

	std::vector<std::string> getFilenamesOfAntPrograms() const
	{
        std::vector<std::string> names = m_params;

		if (names.empty())
			return names;

		names.erase(names.begin());

		return names;
	}

	void setWinner(const std::string& winner)
	{
		m_winnerColony = winner;
	}
	
	 // The following should be used by only the framework, not the student

	bool isGameOver() const
	{
		return m_lives == 0;
	}

	void advanceToNextLevel() 
	{
		++m_level;
	}
   
	void setController(GameController* controller)
	{
		m_controller = controller;
	}

	std::string assetDirectory() const
	{
		return m_assetDir;
	}

	void addParameter(const std::string& p)
	{
		m_params.push_back(p);
	}

	std::string getWinnerName() const
	{
		return m_winnerColony;
	}

	void setError(const std::string& error)
	{
		m_error = error;
	}

	std::string getError() const { return m_error;	}

	
private:
	unsigned int	m_lives;
	unsigned int	m_score;
	unsigned int	m_level;
	GameController* m_controller;
	std::string		m_assetDir;
	std::vector<std::string> m_params;
	std::string		m_winnerColony;
	std::string		m_error;
};

#endif // GAMEWORLD_H_
