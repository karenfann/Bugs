#ifndef FIELD_H_
#define FIELD_H_

#include "GameConstants.h"

#include <iostream>
#include <fstream>
#include <string>

class Field
{
public:
    
    enum FieldItem {
        empty, anthill0, anthill1, anthill2, anthill3, food, grasshopper, water, rock, poison
    };
    enum LoadResult {
        load_success, load_fail_file_not_found, load_fail_bad_format
    };
    
    Field()
    {
        for (int i = 0; i < VIEW_HEIGHT; i++)
            for (int j = 0; j < VIEW_WIDTH; j++)
                m_grid[i][j] = empty;
    }
    
    // This overload exists for backward compatibility with the original
    // skelton code.
    LoadResult loadField(std::string filename)
    {
        std::string error;
        return loadField(filename, error);
    }
    
    LoadResult loadField(std::string filename, std::string& error)
    {
        std::ifstream inf(filename);
        if ( ! inf)
        {
            error = "Cannot open file";
            return load_fail_file_not_found;
        }
        
        std::string line;
        bool atLeastOneAnthill = false;
        
        // now read in the grid
        int lineNum = 0;
        int row = VIEW_HEIGHT - 1;
        for ( ; row >= 0  &&  getline(inf, line); row--)
        {
            lineNum++;
            if (!line.empty()  &&  line.back() == '\r')
                line.pop_back();
            if (line.size() < VIEW_WIDTH)
            {
                error = "Line ";
                error += std::to_string(lineNum);
                error += " has length less than ";
                error += std::to_string(VIEW_WIDTH);
                return load_fail_bad_format;
            }
            
            for (int col = 0; col < VIEW_WIDTH; col++)
            {
                switch (line[col])
                {
                    case '0':
                        m_grid[row][col] = anthill0;
                        atLeastOneAnthill = true;
                        break;
                    case '1':
                        m_grid[row][col] = anthill1;
                        atLeastOneAnthill = true;
                        break;
                    case '2':
                        m_grid[row][col] = anthill2;
                        atLeastOneAnthill = true;
                        break;
                    case '3':
                        m_grid[row][col] = anthill3;
                        atLeastOneAnthill = true;
                        break;
                    case '*':
                        m_grid[row][col] = rock;
                        break;
                    case 'g':
                    case 'G':
                        m_grid[row][col] = grasshopper;
                        break;
                    case 'f':
                    case 'F':
                        m_grid[row][col] = food;
                        break;
                    case 'w':
                    case 'W':
                        m_grid[row][col] = water;
                        break;
                    case 'p':
                    case 'P':
                        m_grid[row][col] = poison;
                        break;
                    case ' ':
                        m_grid[row][col] = empty;
                        break;
                    default:
                        error = "Line ";
                        error += std::to_string(lineNum);
                        error += " column ";
                        error += std::to_string(col+1);
                        error += " has bad character ";
                        error += line[col];
                        return load_fail_bad_format;
                }
            }
        }
        if (row >= 0)  // too few lines
        {
            error = "Field file has fewer than ";
            error += std::to_string(VIEW_HEIGHT);
            error += " lines";
            return load_fail_bad_format;
        }
        
        if ( ! atLeastOneAnthill)
        {
            error = "Field has no anthills";
            return load_fail_bad_format;
        }
        
        if (! validEdges())
        {
            error = "Field is not bordered by pebbles";
            return load_fail_bad_format;
        }
        
        return load_success;
    }
    
    FieldItem getContentsOf(int x, int y)
    {
        if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT)
            return empty;
        
        return m_grid[y][x];
    }
    
private:
    FieldItem	m_grid[VIEW_HEIGHT][VIEW_WIDTH];
    
    bool validEdges() const
    {
        for (int i = 0; i < VIEW_HEIGHT; i++)
            if (m_grid[i][0] != rock || m_grid[i][VIEW_WIDTH - 1] != rock)
                return false;
        for (int i = 0; i < VIEW_WIDTH; i++)
            if (m_grid[0][i] != rock || m_grid[VIEW_HEIGHT - 1][i] != rock)
                return false;
        
        return true;
    }
};

#endif // FIELD_H_
