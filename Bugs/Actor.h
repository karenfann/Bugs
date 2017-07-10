#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Compiler;

/**************************************/
/*************** ACTOR ****************/
/**************************************/
class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int startX, int startY, Direction startDir, int imageID, int depth);
    virtual void doSomething() = 0;
    virtual bool isDead() const;
    virtual bool blocksMovement() const;
    virtual void getBitten(int amt);
    virtual void getPoisoned();
    virtual void getStunned();
    virtual bool isEdible() const;
    virtual bool isPheromone(int colony) const;
    virtual bool isEnemy(int colony) const;
    virtual bool isDangerous(int colony) const;
    virtual bool isAntHill(int colony) const;
    
    // helper functions
    StudentWorld* getWorld() const;
    Direction randDirection() const;
    bool didMove() const;
    void setMove(bool value);
    void setStunned(bool value);
    bool isStunned() const;
    void setPoisoned(bool value);
    bool isPoisoned() const;

private:
    StudentWorld* m_world;
    bool moved;
    bool stunned;
    bool poisoned;
};

/**************************************/
/*************** PEBBLE ***************/
/**************************************/
class Pebble : public Actor
{
public:
    Pebble(StudentWorld* sw, int startX, int startY);
    virtual void doSomething();
    virtual bool blocksMovement() const;
};

/**************************************/
/************ ENERGY HOLDER ***********/
/**************************************/
class EnergyHolder : public Actor
{
public:
    EnergyHolder(StudentWorld* sw, int startX, int startY, Direction startDir, int energy, int imageID, int depth);
    virtual bool isDead() const;
    
    // helper functions
    int getEnergy() const;
    void updateEnergy(int amt);
    void addFood(int amt);
    int pickupFood(int amt);
    bool pickupAndEatFood(int amt);
    
private:
    int m_energy;
};

/**************************************/
/**************** FOOD ****************/
/**************************************/
class Food : public EnergyHolder
{
public:
    Food(StudentWorld* sw, int startX, int startY, int energy);
    virtual void doSomething();
    virtual bool isEdible() const;
};

/**************************************/
/************** ANT HILL **************/
/**************************************/
class AntHill : public EnergyHolder
{
public:
    AntHill(StudentWorld* sw, int startX, int startY, int colony, Compiler* program);
    virtual void doSomething();
    virtual bool isMyHill(int colony) const;
private:
    Compiler* m_program;
    int m_colony;
};

/**************************************/
/************* PHEROMONE **************/
/**************************************/
class Pheromone : public EnergyHolder
{
public:
    Pheromone(StudentWorld* sw, int startX, int startY, int colony, int imageID);
    virtual void doSomething();
    virtual bool isPheromone(int colony) const;
    
    // helper function
    void increaseStrength();
private:
    int cn;
};

/**************************************/
/************ TRIGGERABLE *************/
/**************************************/
class TriggerableActor : public Actor
{
public:
    TriggerableActor(StudentWorld* sw, int x, int y, int imageID);
    virtual bool isDangerous(int colony) const;
};

/**************************************/
/*********** POOL OF WATER ************/
/**************************************/
class WaterPool : public TriggerableActor
{
public:
    WaterPool(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};

/**************************************/
/************** POISON ****************/
/**************************************/
class Poison : public TriggerableActor
{
public:
    Poison(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};

/**************************************/
/************** INSECT ****************/
/**************************************/
class Insect : public EnergyHolder
{
public:
    Insect(StudentWorld* world, int startX, int startY, int energy, int imageID);
    virtual void getBitten(int amt);
    virtual void getPoisoned();
    virtual void getStunned();
    virtual bool isEnemy(int colony) const;
    virtual bool isDangerous(int colony) const;
    virtual bool moveForwardIfPossible();
    
    // helper functions
    void getXYInFrontOfMe(int& x, int& y) const;
    void increaseSleepTicks(int amt);
    int getSleepTicks() const;
    void setSleepTicks(int value);
private:
    int sleepTicks;
};

/**************************************/
/***************** ANT ****************/
/**************************************/
class Ant : public Insect
{
public:
    Ant(StudentWorld* sw, int startX, int startY, int colony, Compiler* program, int imageID);
    virtual void doSomething();
    virtual void getBitten(int amt);
    virtual bool isEnemy(int colony) const;
    virtual bool isDangerous(int colony) const;
    virtual bool moveForwardIfPossible();
private:
    Compiler* compiler;
    bool blocked;
    bool bitten;
    int counter;
    int lastRand;
    int cn;
    int food;
    void interpreter(int& ic);
};

/**************************************/
/************* GRASSHOPPER ************/
/**************************************/
class Grasshopper : public Insect
{
public:
    Grasshopper(StudentWorld* sw, int startX, int startY, int energy, int imageID);
    virtual void doSomething();
private:
    void getNumbers(int& x, int& y);
    int distance;
};

/**************************************/
/********** BABY GRASSHOPPER **********/
/**************************************/
class BabyGrasshopper : public Grasshopper
{
public:
    BabyGrasshopper(StudentWorld* sw, int startX, int startY);
    virtual bool isDangerous(int colony) const;
};

/**************************************/
/********** ADULT GRASSHOPPER *********/
/**************************************/
class AdultGrasshopper : public Grasshopper
{
public:
    AdultGrasshopper(StudentWorld* sw, int startX, int startY);
    virtual void getBitten(int amt);
    virtual void getStunned();
    virtual void getPoisoned();
};

#endif // ACTOR_H_
