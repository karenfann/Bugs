#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"
#include <string>
#include <list>
using namespace std;

class Actor;
class Compiler;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool canMoveTo(int x, int y) const;
    void addActor(Actor* a);
    Actor* getEdibleAt(int x, int y) const;
    Actor* getPheromoneAt(int x, int y, int colony) const;
    bool isEnemyAt(int x, int y, int colony) const;
    bool isDangerAt(int x, int y, int colony) const;
    bool isAntHillAt(int x, int y, int colony) const;
    bool biteEnemyAt(Actor* me, int colony, int biteDamage);
    bool poisonAllPoisonableAt(int x, int y);
    bool stunAllStunnableAt(int x, int y);
    void increaseScore(int colony);
    
private:
    list<Actor*> field[VIEW_WIDTH][VIEW_HEIGHT];
    int elapsedTicks;
    int lead;
    int scores[4];
    string colonyNames[4];
    
    // helper functions
    bool loadField(Field& field);
    string formatStatText(int ticks, int scores[], string colonyNames[], int winning);
};

#endif // STUDENTWORLD_H_
