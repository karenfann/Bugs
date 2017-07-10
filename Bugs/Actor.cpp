
#include "GraphObject.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include "Compiler.h"
#include "Actor.h"

class StudentWorld;
class Compiler;


/**************************************/
/*************** ACTOR ****************/
/**************************************/
Actor::Actor(StudentWorld* world, int startX, int startY, Direction startDir, int imageID, int depth): GraphObject(imageID, startX, startY, startDir, depth)
{
    m_world = world;
    moved = false;
    stunned = false;
    poisoned = false;
}

bool Actor::isDead() const {
    return false;
}

bool Actor::blocksMovement() const {
    return false;
}

void Actor::getBitten(int amt) {
    return;
}

void Actor::getPoisoned() {
    return;
}

void Actor::getStunned() {
    return;
}

bool Actor::isEdible() const {
    return false;
}

bool Actor::isPheromone(int colony) const {
    return false;
}

bool Actor::isEnemy(int colony) const {
    return false;
}
    
bool Actor::isDangerous(int colony) const {
    return false;
}

bool Actor::isAntHill(int colony) const {
    return false;
}

StudentWorld* Actor::getWorld() const {
    return m_world;
}

Actor::Direction Actor::randDirection() const
{
    int dir = randInt(1, 4);
    switch (dir)
    {
        case 1:
            return up;
        case 2:
            return down;
        case 3:
            return left;
        default:
            return right;
    }
}

bool Actor::didMove() const
{
    return moved;
}

void Actor::setMove(bool value)
{
    moved = value;
}

void Actor::setStunned(bool value)
{
    stunned = value;
}
        
bool Actor::isStunned() const
{
    return stunned;
}

void Actor::setPoisoned(bool value)
{
    poisoned = value;
}

bool Actor::isPoisoned() const
{
    return poisoned;
}

/**************************************/
/*************** PEBBLE ***************/
/**************************************/
Pebble::Pebble(StudentWorld* sw, int startX, int startY): Actor(sw, startX, startY, right, IID_ROCK, 1) {}

void Pebble::doSomething() {
    return;
}

bool Pebble::blocksMovement() const {
    return true;
}

/**************************************/
/************ ENERGY HOLDER ***********/
/**************************************/
EnergyHolder::EnergyHolder(StudentWorld* sw, int startX, int startY, Direction startDir, int energy, int imageID, int depth): Actor(sw, startX, startY, startDir, imageID, depth), m_energy(energy) {}

bool EnergyHolder::isDead() const
{
    return getEnergy() <= 0;
}

int EnergyHolder::getEnergy() const
{
    return m_energy;
}

void EnergyHolder::updateEnergy(int amt)
{
    m_energy += amt;
}

void EnergyHolder::addFood(int amt)
{
    if (amt == 0)
        return;
    Actor* ptr = getWorld()->getEdibleAt(getX(), getY());
    if (ptr != nullptr)
    {
        EnergyHolder* food = static_cast<EnergyHolder*>(ptr);
        food->updateEnergy(amt);
    }
    else
    {
        Actor* food = new Food(getWorld(), getX(), getY(), amt);
        getWorld()->addActor(food);
    }
}
    
// Have this actor pick up an amount of food.
int EnergyHolder::pickupFood(int amt) {
    Actor* ptr = getWorld()->getEdibleAt(getX(), getY());
    if (ptr == nullptr)
        return 0;
    EnergyHolder* food = static_cast<EnergyHolder*>(ptr);
    if (food->getEnergy() < amt)
        amt = food->getEnergy();
    food->updateEnergy(-amt);
    return amt; // TODO
}
    
// Have this actor pick up an amount of food and eat it.
bool EnergyHolder::pickupAndEatFood(int amt) {
    Actor* ptr = getWorld()->getEdibleAt(getX(), getY());
    if (ptr == nullptr || ptr->isDead())
        return false;
    updateEnergy(pickupFood(amt));
    return true;
}

/**************************************/
/**************** FOOD ****************/
/**************************************/
Food::Food(StudentWorld* sw, int startX, int startY, int energy): EnergyHolder(sw, startX, startY, right, energy, IID_FOOD, 2) {}

void Food::doSomething() {
    return;
}

bool Food::isEdible() const {
    return true;
}

/**************************************/
/************** ANT HILL **************/
/**************************************/
AntHill::AntHill(StudentWorld* sw, int startX, int startY, int colony, Compiler* program): EnergyHolder(sw, startX, startY, right, 8999, IID_ANT_HILL, 2)
{
    m_program = program;
    m_colony = colony;
}

void AntHill::doSomething()
{
    updateEnergy(-1);
    if (isDead())
        return;
    if(pickupAndEatFood(10000))
        return;
    if (getEnergy() >= 2000)
    {
        Ant* ptr;
        switch(m_colony)
        {
            case 0:
            {
                ptr = new Ant(getWorld(), getX(), getY(), m_colony, m_program, IID_ANT_TYPE0);
                break;
            }
            case 1:
            {
                ptr = new Ant(getWorld(), getX(), getY(), m_colony, m_program, IID_ANT_TYPE1);
                break;
            }
            case 2:
            {
                ptr = new Ant(getWorld(), getX(), getY(), m_colony, m_program, IID_ANT_TYPE2);
                break;
            }
            default:
            {
                ptr = new Ant(getWorld(), getX(), getY(), m_colony, m_program, IID_ANT_TYPE3);
                break;
            }
        }
        getWorld()->addActor(ptr);
        updateEnergy(-1500);
        getWorld()->increaseScore(m_colony);
    }
}

bool AntHill::isMyHill(int colony) const
{
    return colony == m_colony;
}

/**************************************/
/************* PHEROMONE **************/
/**************************************/
Pheromone::Pheromone(StudentWorld* sw, int startX, int startY, int colony, int imageID): EnergyHolder(sw, startX, startY, right, 256, imageID, 2) {}

void Pheromone::doSomething()
{
    updateEnergy(-1);
}
bool Pheromone::isPheromone(int colony) const
{
    return (cn == colony);
}
    
    // Increase the strength (i.e., energy) of this pheromone.
void Pheromone::increaseStrength()
{
    updateEnergy(256);
    if (getEnergy() > 768)
        updateEnergy(-(getEnergy()-768));
}

/**************************************/
/************ TRIGGERABLE *************/
/**************************************/
TriggerableActor::TriggerableActor(StudentWorld* sw, int x, int y, int imageID): Actor(sw, x, y, right, imageID, 2) {}

bool TriggerableActor::isDangerous(int colony) const
{
    return true;
}

/**************************************/
/*********** POOL OF WATER ************/
/**************************************/
WaterPool::WaterPool(StudentWorld* sw, int x, int y): TriggerableActor(sw, x, y, IID_WATER_POOL) {}

void WaterPool::doSomething()
{
    getWorld()->stunAllStunnableAt(getX(), getY());
}

/**************************************/
/************** POISON ****************/
/**************************************/
Poison::Poison(StudentWorld* sw, int x, int y): TriggerableActor(sw, x, y, IID_POISON) {}

void Poison::doSomething()
{
    getWorld()->poisonAllPoisonableAt(getX(), getY());
}

/**************************************/
/************** INSECT ****************/
/**************************************/
Insect::Insect(StudentWorld* world, int startX, int startY, int energy, int imageID): EnergyHolder(world, startX, startY, randDirection(), energy, imageID, 1), sleepTicks(0) {}

void Insect::getBitten(int amt)
{
    updateEnergy(-amt);
    if (isDead())
        addFood(100);
}

void Insect::getPoisoned()
{
    if (!isPoisoned())
    {
        Insect* ptr = static_cast<Insect*>(this);
        ptr->updateEnergy(-50);
        setPoisoned(true);
    }
}

void Insect::getStunned()
{
    if (!isStunned())
    {
        Insect* ptr = static_cast<Insect*>(this);
        ptr->increaseSleepTicks(2);
        setStunned(true);
    }
}

bool Insect::isEnemy(int colony) const
{
    return true;
}

bool Insect::isDangerous(int colony) const
{
    return true;
}
    
    // Set x,y to the coordinates of the spot one step in front of this insect.
void Insect::getXYInFrontOfMe(int& x, int& y) const
{
    switch(getDirection())
    {
        case up:
            y++;
            break;
        case down:
            y--;
            break;
        case right:
            x++;
            break;
        default:
            x--;
            break;
    }
}
    
    // Move this insect one step forward if possible, and return true;
    // otherwise, return false without moving.
bool Insect::moveForwardIfPossible()
{
    int x = getX();
    int y = getY();
    getXYInFrontOfMe(x, y);
    if (getWorld()->canMoveTo(x, y))
    {
        moveTo(x, y);
        return true;
    }
    return false;
}

    // Increase the number of ticks this insect will sleep by the indicated amount.
void Insect::increaseSleepTicks(int amt)
{
    sleepTicks += amt;
}

int Insect::getSleepTicks() const
{
    return sleepTicks;
}

void Insect::setSleepTicks(int value)
{
    sleepTicks = value;
}

/**************************************/
/***************** ANT ****************/
/**************************************/
Ant::Ant(StudentWorld* sw, int startX, int startY, int colony, Compiler* program, int imageID): Insect(sw, startX, startY, 1500, imageID), cn(colony), compiler(program), bitten(false), food(0), counter(0), blocked(false), lastRand(0) {}

void Ant::doSomething()
{
    updateEnergy(-1);
    if (isDead())
    {
        addFood(100);
        return;
    }
    if (getSleepTicks() != 0)
    {
        increaseSleepTicks(-1);
        return;
    }
    interpreter(counter);
}
void Ant::getBitten(int amt)
{
    updateEnergy(-amt);
    bitten = true;
    if (isDead())
        addFood(100);
}

bool Ant::isEnemy(int colony) const
{
    return cn != colony;
}

bool Ant::isDangerous(int colony) const
{
    return cn != colony;
}

bool Ant::moveForwardIfPossible()
{
    setMove(Insect::moveForwardIfPossible());
    if (didMove())
    {
        blocked = false;
        bitten = false;
        setPoisoned(false);
    }
    else
        blocked = true;
    return didMove();
}

void Ant::interpreter(int& ic)
{
    Compiler::Command cmd;
    bool stateChanged = false;
    int cmdCount = 0;
    while (cmdCount <= 10 && !stateChanged)
    {
        // get the command from element ic of the vector
        if ( ! compiler->getCommand(ic, cmd) )
        {
            updateEnergy(-getEnergy());
            stateChanged = true;
        }// error - no such instruction!
        switch (cmd.opcode)
        {
            case (Compiler::Opcode::moveForward):
                moveForwardIfPossible();
                ++ic;
                stateChanged = true;
                break;
            case (Compiler::Opcode::generateRandomNumber):
                lastRand = randInt(0, stoi(cmd.operand1));
                ++ic;
                break;
            case (Compiler::Opcode::if_command):
                // if the condition of the if command is
                // is true, then go to the target position
                // in the vector; otherwise fall through to
                // the next position
                /*0: i_smell_danger_in_front_of_me
                 1: i_smell_pheromone_in_front_of_me
                 2: i_was_bit
                 3: i_am_carrying_food
                 4: i_am_hungry
                 5: i_am_standing_on_my_anthill
                 6: i_am_standing_on_food
                 7: i_am_standing_with_an_enemy
                 8: i_was_blocked_from_moving
                 9: last_random_number_was_zero*/
            {
                bool trigger;
                switch(stoi(cmd.operand1))
                {
                    case (Compiler::Condition::i_smell_danger_in_front_of_me):
                    {
                        int x = getX();
                        int y = getY();
                        getXYInFrontOfMe(x, y);
                        trigger = getWorld()->isDangerAt(x, y, cn);
                        break;
                    }
                    case (Compiler::Condition::i_smell_pheromone_in_front_of_me):
                    {
                        int x = getX();
                        int y = getY();
                        getXYInFrontOfMe(x, y);
                        Actor* ptr = getWorld()->getPheromoneAt(x, y, cn);
                        trigger = (ptr != nullptr);
                        break;
                    }
                    case (Compiler::Condition::i_was_bit):
                        trigger = bitten;
                        break;
                    case (Compiler::Condition::i_am_carrying_food):
                        trigger = (food > 0);
                        break;
                    case (Compiler::Condition::i_am_hungry):
                        trigger = (getEnergy() <= 25);
                        break;
                    case (Compiler::Condition::i_am_standing_on_my_anthill):
                        trigger = getWorld()->isAntHillAt(getX(), getY(), cn);
                        break;
                    case (Compiler::Condition::i_am_standing_on_food):
                    {
                        Actor* ptr = getWorld()->getEdibleAt(getX(), getY());
                        trigger = (ptr != nullptr);
                        break;
                    }
                    case (Compiler::Condition::i_am_standing_with_an_enemy):
                        trigger = getWorld()->isEnemyAt(getX(), getY(), cn);
                        break;
                    case (Compiler::Condition::i_was_blocked_from_moving):
                        trigger = blocked;
                        break;
                    default:
                        trigger = (lastRand == 0);
                        break;
                }
                if (trigger)
                    ic = stoi(cmd.operand2);
                else
                    ++ic; // just advance to the next line
                break;
            }
            case (Compiler::Opcode::eatFood):
                if (food < 100)
                {
                    updateEnergy(food);
                    food = 0;
                }
                else
                {
                    updateEnergy(100);
                    food -= 100;
                }
                ic++;
                stateChanged = true;
                break;
            case (Compiler::Opcode::dropFood):
                addFood(food);
                food = 0;
                ic++;
                stateChanged = true;
                break;
            case (Compiler::Opcode::bite):
                getWorld()->biteEnemyAt(this, cn, 15);
                ic++;
                stateChanged = true;
                break;
            case (Compiler::Opcode::pickupFood):
                food += pickupFood(400);
                if (food > 1800)
                {
                    addFood(food - 1800);
                    food = 1800;
                }
                ic++;
                stateChanged = true;
                break;
            case (Compiler::Opcode::emitPheromone):
            {
                Actor* ptr = getWorld()->getPheromoneAt(getX(), getY(), cn);
                if (ptr != nullptr)
                {
                    Pheromone* pher = static_cast<Pheromone*>(ptr);
                    pher->increaseStrength();
                }
                else
                {
                    Pheromone* pher;
                    switch(cn)
                    {
                        case 0:
                            pher = new Pheromone(getWorld(), getX(), getY(), cn, IID_PHEROMONE_TYPE0);
                            break;
                        case 1:
                            pher = new Pheromone(getWorld(), getX(), getY(), cn, IID_PHEROMONE_TYPE1);
                            break;
                        case 2:
                            pher = new Pheromone(getWorld(), getX(), getY(), cn, IID_PHEROMONE_TYPE2);
                            break;
                        default:
                            pher = new Pheromone(getWorld(), getX(), getY(), cn, IID_PHEROMONE_TYPE3);
                            break;
                    }
                    getWorld()->addActor(pher);
                }
                ic++;
                stateChanged = true;
                break;
            }
            case (Compiler::Opcode::faceRandomDirection):
                setDirection(randDirection());
                ic++;
                blocked = false;
                stateChanged = true;
                break;
            case (Compiler::Opcode::rotateClockwise):
            {
                switch(getDirection())
                {
                    case (Direction::up):
                        setDirection(right);
                        break;
                    case (Direction::down):
                        setDirection(down);
                        break;
                    case (Direction::right):
                        setDirection(down);
                        break;
                    default:
                        setDirection(up);
                        break;
                }
                ic++;
                blocked = false;
                stateChanged = true;
                break;
            }
            case (Compiler::Opcode::rotateCounterClockwise):
            {
                switch(getDirection())
                {
                    case (Direction::up):
                        setDirection(left);
                        break;
                    case (Direction::down):
                        setDirection(right);
                        break;
                    case (Direction::right):
                        setDirection(up);
                        break;
                    default:
                        setDirection(down);
                        break;
                }
                ic++;
                blocked = false;
                stateChanged = true;
                break;
            }
            default:
                ic = stoi(cmd.operand1);
                break;
        }
        cmdCount++;
    }
}

/**************************************/
/************* GRASSHOPPER ************/
/**************************************/
Grasshopper::Grasshopper(StudentWorld* sw, int startX, int startY, int energy, int imageID): Insect(sw, startX, startY, energy, imageID), distance(randInt(2, 10)) {}

void Grasshopper::doSomething()
{
    updateEnergy(-1);
    if (isDead())
    {
        addFood(100);
        return;
    }
    if (getSleepTicks() != 0)
    {
        increaseSleepTicks(-1);
        return;
    }
    BabyGrasshopper* pp = dynamic_cast<BabyGrasshopper*>(this);
    if (pp != nullptr)
    {
        if (getEnergy() >= 1600)
        {
            getWorld()->addActor(new AdultGrasshopper(getWorld(), getX(), getY()));
            updateEnergy(-getEnergy());
            addFood(100);
            return;
        }
    }
    else
    {
        if (randInt(0, 2) == 0)
        {
            if(getWorld()->biteEnemyAt(this, 0, 50))
            {
                setSleepTicks(2);
                return;
            }
        }
        if (randInt(0, 9) == 0)
        {
            int x, y;
            getNumbers(x, y);
            if (getWorld()->canMoveTo(x, y))
                moveTo(x, y);
            if (didMove())
            {
                setSleepTicks(2);
                return;
            }
        }
    }
    if (pickupAndEatFood(200))
    {
        if (randInt(0, 1) == 0)
        {
            setSleepTicks(2);
            return;
        }
    }
    if (distance == 0)
    {
        setDirection(randDirection());
        distance = randInt(2, 10);
    }
    setMove(moveForwardIfPossible());
    if (didMove())
    {
        setStunned(false);
        setPoisoned(false);
        distance--;
    }
    else
        distance = 0;
    setSleepTicks(2);
}

void Grasshopper::getNumbers(int &x, int &y)
{
    double radius = randInt(1, 10);
    double angle = rand();
    x = getX() + round(radius * cos(angle));
    y = getY() + round(radius * sin(angle));
    if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT)
        getNumbers(x, y);
}

/**************************************/
/********** BABY GRASSHOPPER **********/
/**************************************/
BabyGrasshopper::BabyGrasshopper(StudentWorld* sw, int startX, int startY): Grasshopper(sw, startX, startY, 500, IID_BABY_GRASSHOPPER) {};

bool BabyGrasshopper::isDangerous(int colony) const
{
    return false;
}

/**************************************/
/********** ADULT GRASSHOPPER *********/
/**************************************/
AdultGrasshopper::AdultGrasshopper(StudentWorld* sw, int startX, int startY): Grasshopper(sw, startX, startY, 1600, IID_ADULT_GRASSHOPPER) {};

void AdultGrasshopper::getBitten(int amt)
{
    updateEnergy(-amt);
    if (isDead())
    {
        addFood(100);
        return;
    }
    if (randInt(1,2) == 1)
        getWorld()->biteEnemyAt(this, -1, 50);
}

void AdultGrasshopper::getStunned()
{
    return;
}

void AdultGrasshopper::getPoisoned()
{
    return;
}
