#ifndef COMPILER_H_
#define COMPILER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

#include "GameConstants.h"

class Compiler
{
public:

	enum Opcode {
		invalid = -1,
		label,
		goto_command,
		if_command,
		emitPheromone,
		faceRandomDirection,
		rotateClockwise,
		rotateCounterClockwise,
		moveForward,
		bite,
		pickupFood,
		dropFood,
		eatFood,
		generateRandomNumber
	};

	enum Condition
	{
		invalid_if = -1,
		i_smell_danger_in_front_of_me,		// 0
		i_smell_pheromone_in_front_of_me,	// 1
		i_was_bit,							// 2
		i_am_carrying_food,					// 3
		i_am_hungry,						// 4
		i_am_standing_on_my_anthill,		// 5
		i_am_standing_on_food,				// 6
		i_am_standing_with_an_enemy,		// 7
		i_was_blocked_from_moving,			// 8
		last_random_number_was_zero			// 9
	};

	struct Command
	{
		Opcode	opcode;
		std::string operand1;
		std::string operand2;
		std::string text;
		int lineNum;
	};

	Compiler()
	{
		m_colonyName = "--------";
	}

	std::string getColonyName() const { return m_colonyName; }

	bool compile(std::string sourceFile, std::string& firstError)
	{
		m_labelToLine.clear();
		m_outputProgram.clear();

		std::ifstream inf;
		for (auto suffix : { "", ".bug", ".txt", ".bug.txt" })
		{
			inf.open(sourceFile + suffix);
			if (inf)
				break;
		}
		if ( ! inf)
		{
			firstError = "Cannot open file";
			return false;
		}

		std::string colonyName;
		if ( ! getline(inf, colonyName))
		{
			firstError = "File is empty";
			return false;
		}
		std::vector<std::string> tokens = tokenize(colonyName);
		if (tokens.size() < 2 || tokens[0] != "colony:")
		{
			firstError = "Invalid colony specification at top of bug program";
			return false;
		}
		m_colonyName = (tokens[1].size() <= 8 ? tokens[1] : tokens[1].substr(0,8));

		int lineNum = 0;
		std::string line;
		while (getline(inf, line))
		{
			lineNum++;
			Command c;
			ParseResult pr = parseLine(line, c, lineNum, firstError);

			if (pr == error_line)
				return false;
			else if (pr == valid_line)
			{
				if (c.opcode != Opcode::label)
					m_outputProgram.push_back(c);
				else
				{
					auto p = m_labelToLine.find(c.operand1);
					if (p != m_labelToLine.end())
					{
						firstError = "Line ";
						firstError += std::to_string(lineNum);
						firstError += " has label ";
						firstError += c.operand1;
						firstError += " duplicating label on line ";
						firstError += std::to_string(p->second);
						return false;
					}
					m_labelToLine[c.operand1] = m_outputProgram.size(); // map each label to its proper location
				}
			}
		}

		// replace goto target labels with command numbers

		for (size_t i = 0; i < m_outputProgram.size(); ++i)
		{
			if (m_outputProgram[i].opcode == Opcode::goto_command)
			{
				auto p = m_labelToLine.find(m_outputProgram[i].operand1);
				if (p != m_labelToLine.end())
					m_outputProgram[i].operand1 = std::to_string(p->second);
				else
				{
					firstError = "Line ";
					firstError += std::to_string(m_outputProgram[i].lineNum);
					firstError += " has goto to unknown label ";
					firstError += m_outputProgram[i].operand1;
					return false;
				}
			}
			else if (m_outputProgram[i].opcode == if_command)
			{
				auto p = m_labelToLine.find(m_outputProgram[i].operand2);
				if (p != m_labelToLine.end())
					m_outputProgram[i].operand2 = std::to_string(p->second);
				else
				{
					firstError = "Line ";
					firstError += std::to_string(m_outputProgram[i].lineNum);
					firstError += " has goto to unknown label ";
					firstError += m_outputProgram[i].operand2;
					return false;
				}
			}
		}

		return true;
	}

	bool getCommand(int lineNumber, Command& c) const
	{
		if (lineNumber < 0  ||  lineNumber >= static_cast<int>(m_outputProgram.size()))
			return false;

		c = m_outputProgram[lineNumber];
		return true;
	}

private:

	enum ParseResult {
		empty_line, valid_line, error_line
	};

	ParseResult parseLine(std::string line, Command& c, int lineNum, std::string& firstError)
	{
		c.lineNum = lineNum;
		c.text = line;

		std::vector<std::string> tokens = tokenize(line);
		if (tokens.empty())
			return empty_line;

		if (tokens.size() < MIN_TOKENS_PER_LINE)
		{
			firstError = "Line ";
			firstError += std::to_string(lineNum);
			firstError += " has an incomplete command";
			return error_line;
		}

		bool validNumberOfOperands = false;
		c.opcode = getOpcodeForCommand(tokens[0], tokens.size(), validNumberOfOperands);

		if (c.opcode == invalid)
		{
			firstError = "Line ";
			firstError += std::to_string(lineNum);
			firstError += " has an invalid command: ";
			firstError += tokens[0];
			return error_line;
		}

		if (!validNumberOfOperands)
		{
			firstError = "Line ";
			firstError += std::to_string(lineNum);
			firstError += " has the wrong number of operands for the command ";
			firstError += tokens[0];
			return error_line;
		}

		if (c.opcode == if_command)
		{
			Condition cond = getIfOpcode(tokens[1]);
			if (cond == Condition::invalid_if)
			{
				firstError = "Line ";
				firstError += std::to_string(lineNum);
				firstError += " has invalid if condition: ";
				firstError += tokens[1];
				return error_line;
			}
			c.operand1 = std::to_string(cond); // condition number as string
			c.operand2 = tokens[4]; // target label
		}
		else if (c.opcode == goto_command)
		{
			c.operand1 = tokens[1];
		}
		else if (c.opcode == generateRandomNumber)
		{
			if (stoi(tokens[1]) > 0)
				c.operand1 = tokens[1];
			else
			{
				firstError = "Line ";
				firstError += std::to_string(lineNum);
				firstError += " operand must be an integer greater than zero";
				return error_line;
			}
		}
		else if (c.opcode == label)
		{
			// remove colon from label
			c.operand1 = tokens[0].substr(0, tokens[0].size() - 1);
		}

		return valid_line;
	}

	std::vector<std::string> tokenize(const std::string& line)
	{
		std::string curToken;
		size_t curOff = 0;
		std::vector<std::string> tokens;

		do
		{
			size_t startOfNextToken = line.find_first_not_of(" \t,\r\n", curOff);
			if (startOfNextToken == std::string::npos)
				break;
			if (line.compare(startOfNextToken, 2, "//") == 0)
				break;
			size_t startOfWhiteSpace = line.find_first_of(" \t,\r\n", startOfNextToken + 1);
			if (startOfWhiteSpace == std::string::npos)
				startOfWhiteSpace = line.size();

			tokens.push_back(line.substr(startOfNextToken, startOfWhiteSpace - startOfNextToken));
			std::string& data = tokens.back();
			std::transform(data.begin(), data.end(), data.begin(), ::tolower);

			curOff = startOfWhiteSpace + 1;
		} while (curOff < line.size());

		return tokens;
	}

	Opcode getOpcodeForCommand(const std::string& c, size_t numTokens, bool& validNumberOfOperands)
	{
		validNumberOfOperands = false;

		if (c == "moveforward")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::moveForward;
		}
		else if (c == "emitpheromone")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::emitPheromone;
		}
		else if (c == "facerandomdirection")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::faceRandomDirection;
		}
		else if (c == "rotateclockwise")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::rotateClockwise;
		}
		else if (c == "rotatecounterclockwise")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::rotateCounterClockwise;
		}
		else if (c == "bite")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::bite;
		}
		else if (c == "pickupfood")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::pickupFood;
		}
		else if (c == "dropfood")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::dropFood;
		}
		else if (c == "eatfood")
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::eatFood;
		}
		else if (c == "if")
		{
			validNumberOfOperands = (numTokens == 5);
			return Opcode::if_command;
		}
		else if (c == "goto")
		{
			validNumberOfOperands = (numTokens == 2);
			return Opcode::goto_command;
		}
		else if (c == "generaterandomnumber")
		{
			validNumberOfOperands = (numTokens == 2);
			return Opcode::generateRandomNumber;
		}
		else if (c.size() >= 2 && c.back() == ':')
		{
			validNumberOfOperands = (numTokens == 1);
			return Opcode::label;
		}

		return Opcode::invalid;
	}

	Condition getIfOpcode(const std::string& s)
	{
		if (s == "i_smell_pheromone_in_front_of_me")
			return i_smell_pheromone_in_front_of_me;
		if (s == "i_smell_danger_in_front_of_me")
			return i_smell_danger_in_front_of_me;
		if (s == "i_was_bit")
			return i_was_bit;
		if (s == "i_am_carrying_food")
			return i_am_carrying_food;
		if (s == "i_am_hungry")
			return i_am_hungry;
		if (s == "i_am_standing_on_my_anthill")
			return i_am_standing_on_my_anthill;
		if (s == "i_am_standing_on_food")
			return i_am_standing_on_food;
		if (s == "i_am_standing_with_an_enemy")
			return i_am_standing_with_an_enemy;
		if (s == "i_was_blocked_from_moving")
			return i_was_blocked_from_moving;
		if (s == "last_random_number_was_zero")	
			return last_random_number_was_zero;

		return Condition::invalid_if;
	}

private:
	static const int				MIN_TOKENS_PER_LINE = 1;
	std::map<std::string, size_t>	m_labelToLine;
	std::vector<Command>			m_outputProgram;
	std::string						m_colonyName;
};

#endif // COMPILER_H_
