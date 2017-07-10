#include "GameWorld.h"
#include "StudentWorld.h"
#include "Actor.h"
#include "Field.h"
#include "Compiler.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

class Compiler;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir): GameWorld(assetDir)
{
    elapsedTicks = 0;
    for (int i = 0; i < 4; i++)
        scores[i] = 0;
    lead = -1;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}
    
int StudentWorld::init()
{
    for (int c = 0; c < VIEW_WIDTH; c++)
    {
        for (int r = 0; r < VIEW_HEIGHT; r++)
        {
            field[c][r] = list<Actor*>();
        }
    }
    Field f;
    Compiler *compilerForEntrant0, *compilerForEntrant1,
    *compilerForEntrant2, *compilerForEntrant3;
    std::vector<std::string> fileNames = getFilenamesOfAntPrograms();
    string error;
    if (!loadField(f))
        return GWSTATUS_LEVEL_ERROR;
    for (int c = 0; c < VIEW_WIDTH; c++)
    {
        for (int r = 0; r < VIEW_HEIGHT; r++)
        {
            Field::FieldItem item = f.getContentsOf(c, r);
            switch(item)
            {
                case(Field::FieldItem::rock):
                    field[c][r].push_back(new Pebble(this, c, r));
                    break;
                case(Field::FieldItem::grasshopper):
                    field[c][r].push_back(new BabyGrasshopper(this, c, r));
                    break;
                case(Field::FieldItem::food):
                    field[c][r].push_back(new Food(this, c, r, 1600));
                    break;
                case(Field::FieldItem::water):
                    field[c][r].push_back(new WaterPool(this, c, r));
                    break;
                case(Field::FieldItem::poison):
                    field[c][r].push_back(new Poison(this, c, r));
                    break;
                case(Field::FieldItem::anthill0):
                    compilerForEntrant0 = new Compiler;
                    if ( ! compilerForEntrant0->compile(fileNames[0], error) )
                    {
                        setError(fileNames[0] + " " + error);
                        return GWSTATUS_LEVEL_ERROR;
                    }
                    field[c][r].push_back(new AntHill(this, c, r, 0, compilerForEntrant0));
                    colonyNames[0] = compilerForEntrant0->getColonyName();
                    break;
                case(Field::FieldItem::anthill1):
                    compilerForEntrant1 = new Compiler;
                    if ( ! compilerForEntrant1->compile(fileNames[1], error) )
                    {
                    setError(fileNames[1] + " " + error);
                    return GWSTATUS_LEVEL_ERROR;
                    }
                    field[c][r].push_back(new AntHill(this, c, r, 1, compilerForEntrant1));
                    colonyNames[1] = compilerForEntrant1->getColonyName();
                    break;
                case(Field::FieldItem::anthill2):
                    compilerForEntrant2 = new Compiler;
                    if ( ! compilerForEntrant2->compile(fileNames[2], error) )
                    {
                    setError(fileNames[2] + " " + error);
                    return GWSTATUS_LEVEL_ERROR;
                    }
                    field[c][r].push_back(new AntHill(this, c, r, 2, compilerForEntrant2));
                    colonyNames[2] = compilerForEntrant2->getColonyName();
                    break;
                case(Field::FieldItem::anthill3):
                    compilerForEntrant3 = new Compiler;
                    if ( ! compilerForEntrant3->compile(fileNames[3], error) )
                    {
                    setError(fileNames[3] + " " + error);
                    return GWSTATUS_LEVEL_ERROR;
                    }
                    field[c][r].push_back(new AntHill(this, c, r, 3, compilerForEntrant3));
                    colonyNames[3] = compilerForEntrant3->getColonyName();
                    break;
                default:
                    break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    elapsedTicks++;
    list<Actor*>::iterator it;
    for (int c = 1; c < VIEW_WIDTH-1; c++)
    {
        for (int r = 1; r < VIEW_HEIGHT-1; r++)
        {
            it = field[c][r].begin();
            while (it != field[c][r].end())
            {
                if (!(*it)->didMove() && !(*it)->isDead())
                {
                    (*it)->doSomething();
                    if ((*it)->didMove())
                    {
                        field[(*it)->getX()][(*it)->getY()].push_back(*it);
                        it = field[c][r].erase(it);
                    }
                }
                it++;
            }
        }
    }
    
    // set move to false
    for (int c = 1; c < VIEW_WIDTH-1; c++)
    {
        for (int r = 1; r < VIEW_HEIGHT-1; r++)
        {
            it = field[c][r].begin();
            while (it != field[c][r].end())
            {
                if ((*it)->didMove())
                    (*it)->setMove(false);
                it++;
            }
        }
    }
    
    // remove dead actors
    for (int c = 1; c < VIEW_WIDTH-1; c++)
    {
        for (int r = 1; r < VIEW_HEIGHT-1; r++)
        {
            it = field[c][r].begin();
            while (it != field[c][r].end())
            {
                if ((*it)->isDead())
                {
                    delete *it;
                    it = field[c][r].erase(it);
                }
                else
                    it++;
            }
        }
    }
    
    for (int i = 0; i < 4; i++)
    {
        if (scores[i] > scores[lead] && scores[i] >= 6)
            lead = i;
    }
    setGameStatText(formatStatText(elapsedTicks, scores, colonyNames, lead));
    
    if (elapsedTicks == 2000)
    {
        if (lead != -1)
        {
            setWinner(colonyNames[lead]);
            return GWSTATUS_PLAYER_WON;
        }
        return GWSTATUS_NO_WINNER;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it;
    for (int c = 0; c < VIEW_WIDTH; c++)
    {
        for (int r = 0; r < VIEW_HEIGHT; r++)
        {
            it = field[c][r].begin();
            while (it != field[c][r].end())
            {
                delete (*it);
                it = field[c][r].erase(it);
            }
        }
    }
}

bool StudentWorld::canMoveTo(int x, int y) const
{
    list<Actor*>::const_iterator it = field[x][y].begin();
    while(it != field[x][y].end())
    {
        if ((*it)->blocksMovement())
        {
            return false;
        }
        it++;
    }
    return true;
}

void StudentWorld::addActor(Actor* a)
{
    int x = a->getX();
    int y = a->getY();
    field[x][y].push_back(a);
}


Actor* StudentWorld::getEdibleAt(int x, int y) const
{
    list<Actor*>::const_iterator it = field[x][y].begin();
    while (it != field[x][y].end())
    {
        Food* ptr = dynamic_cast<Food*>(*it);
        if (ptr != nullptr)
            return *it;
        it++;
    }
    return nullptr;
}

Actor* StudentWorld::getPheromoneAt(int x, int y, int colony) const
{
    list<Actor*>::const_iterator it = field[x][y].begin();
    while (it != field[x][y].end())
    {
        Pheromone* ptr = dynamic_cast<Pheromone*>(*it);
        if (ptr != nullptr)
            return *it;
        it++;
    }
    return nullptr;
}

bool StudentWorld::isEnemyAt(int x, int y, int colony) const
{
    list<Actor*>::const_iterator it = field[x][y].begin();
    while (it != field[x][y].end())
    {
        if ((*it)->isEnemy(colony))
            return true;
        it++;
    }
    return false;
}

bool StudentWorld::isDangerAt(int x, int y, int colony) const
{
    list<Actor*>::const_iterator it = field[x][y].begin();
    while (it != field[x][y].end())
    {
        if ((*it)->isDangerous(colony))
            return true;
        it++;
    }
    return false;
}

bool StudentWorld::isAntHillAt(int x, int y, int colony) const
{
    if (field[x][y].empty())
        return false;
    list<Actor*>::const_iterator it = field[x][y].begin();
    while (it != field[x][y].end())
    {
        AntHill* ptr = dynamic_cast<AntHill*>(*it);
        if (ptr != nullptr)
        {
            AntHill* anthill = static_cast<AntHill*>(ptr);
            return anthill->isMyHill(colony);
        }
        it++;
    }
    return false;
}

bool StudentWorld::biteEnemyAt(Actor* me, int colony, int biteDamage)
{
    int x = me->getX();
    int y = me->getY();
    if (!isEnemyAt(x, y, colony))
        return false;
    vector<Actor*> enemies;
    list<Actor*>::const_iterator it = field[x][y].begin();
    while (it != field[x][y].end())
    {
        if ((*it)->isEnemy(colony))
            enemies.push_back(*it);
        it++;
    }
    Actor* enemyToBite = enemies[randInt(0, enemies.size()-1)];
    enemyToBite->getBitten(biteDamage);
    return true;
}

bool StudentWorld::poisonAllPoisonableAt(int x, int y)
{
    if (field[x][y].empty())
        return false;
    list<Actor*>::iterator it = field[x][y].begin();
    while (it != field[x][y].end())
    {
        (*it)->getPoisoned();
        it++;
    }
    return true;
}

bool StudentWorld::stunAllStunnableAt(int x, int y)
{
    list<Actor*>::iterator it = field[x][y].begin();
    if (field[x][y].empty())
        return false;
    while (it != field[x][y].end())
    {
        (*it)->getStunned();
        it++;
    }
    return true;
}

void StudentWorld::increaseScore(int colony)
{
    switch(colony)
    {
        case 0:
            scores[0]++;
            break;
        case 1:
            scores[1]++;
            break;
        case 2:
            scores[2]++;
            break;
        default:
            scores[3]++;
            break;
    }
}

bool StudentWorld::loadField(Field& f)
{
    string fieldFileName = getFieldFilename();
    if (f.loadField(fieldFileName) != Field::load_success)
        return false;
    return true;
}

string StudentWorld::formatStatText(int ticks, int scores[], string colonyNames[], int winning)
{
    ostringstream oss;
    oss << "Ticks: ";
    oss << setw(5) << 2000 - ticks << " -";
    for (int i = 0; i < getFilenamesOfAntPrograms().size() ; i++)
    {
        oss << "  ";
        oss << colonyNames[i];
        if (i == lead)
            oss << "*";
        oss.fill('0');
        oss << ": " << setw(2) << scores[i] << " ants";
    }
    string s = oss.str();
    return s;
}
