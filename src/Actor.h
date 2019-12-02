#ifndef ACTOR_H_
#define ACTOR_H_

#include "provided/GraphObject.h"
#include <list>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

// Base class from which all actors will be derived
class Actor: public GraphObject {
public:
    // Constructor
    Actor(int imageID, double startX, double startY, Direction startDir, int depth, StudentWorld* world);

    // Accessors
    virtual bool fallsIntoPits() const = 0;
    virtual bool blocksMovement(double destX, double destY, const Actor *actor) const = 0;
    virtual bool damagedByFlame() const = 0;
    virtual bool triggersLandmines() const = 0;
    virtual bool isInfectible() const = 0;
    virtual bool blocksFlames(double destX, double destY, const Actor* actor) const;
    virtual bool detectsExits() const;
    virtual bool isZombie() const;
    bool isDead() const;
    bool overlaps(const Actor* actor) const;
    bool wouldOverlap(double destX, double destY, double actorX, double actorY) const;
    int infectionCount() const;
      // coordX and coordY have to be separately defined to check for potential distance as well
    double squareDistBetween(double coordX, double coordY, const Actor *other) const;

    // Mutators
      // doSomething() to be called by move() function.
      // return false if the actor died
    virtual void doSomething() = 0;
      // Each actor can specify whether or not they can be killed (true/false)
    virtual void setDead() = 0;
    virtual void infect();

protected:
    // Accessors
    virtual bool exits(const Actor* actor) const;
    bool flameBlocked(double destX, double destY) const;
    bool isInfected() const;
    StudentWorld* getWorld() const;
    void increaseInfection();

    // Mutators
      // setDead() will be different for sentient and environmental actors, but I still need a way to set m_dead to false
    void setm_dead();
    void setm_infected();
    void cureInfection();

private:
    StudentWorld* m_world;
    bool m_dead;
    bool m_infected;
    int m_infectionCount;
};



///////////////////////
/// Sentient Actors ///
///////////////////////

// Actors that can move. For our purposes, zombies are indeed sentient.
class SentientActor: public Actor {
public:
    // Constructor/Destructor
    SentientActor(int imageID, double startX, double startY, Direction startDir, int depth, StudentWorld* world);

    // Accessors
    virtual bool fallsIntoPits() const;
    virtual bool damagedByFlame() const;
    virtual bool triggersLandmines() const;
    virtual bool blocksMovement(double destX, double destY, const Actor *actor) const;

    // Mutators
    virtual void setDead();
};



/// Human ///

class Human: public SentientActor {
public:
    // Constructor
    Human(int imageID, double startX, double startY, Direction startDir, int depth, StudentWorld* world);

    // Accessors
    virtual bool isInfectible() const;
    virtual bool exits(const Actor* actor) const;

    // Mutators
    virtual void infect();
};



/// Penelope ///

class Penelope: public Human {
public:
    // Constructor/Destructor
    Penelope(double startX, double startY, StudentWorld* world);

    // Accessors
    int getVaccines() const;
    int getLandmines() const;
    int getFlames() const;

    // Mutators
    virtual void doSomething();
    virtual void setDead();
    void addVaccine();
    void addFlames();
    void addLandmines();

private:
    // Mutators
    void safeMoveTo(double destX, double destY);

    // Data members
    int m_vaccines;
    int m_landmines;
    int m_flames;
};



/// Citizen ///

class Citizen: public Human {
public:
    // Constructor
    Citizen(double startX, double startY, StudentWorld* world);

    // Mutators
    virtual void doSomething();
    virtual void infect();
    virtual void setDead();

private:
    void paralyze();
    bool m_paralyzed;
};



/// Zombie ///

class Zombie: public SentientActor {
public:
    // Constructor/Destructor
    Zombie(double startX, double startY, StudentWorld* world);

    // Accessors
    virtual bool isInfectible() const;
    virtual bool isZombie() const;

    // Mutators
    virtual void doSomething();

protected:
    // Accessors
    int mvtPlan() const;

    // Mutators
    virtual void determineNextDir() = 0;
    void decMvtPlan();
    void newMvtPlan();
    void delMvtPlan();

private:
    bool m_paralyzed;
    int m_mvtPlan;
};

/// Dumb Zombie ///

class DumbZombie: public Zombie {
public:
    // Constructor
    DumbZombie(double startX, double startY, StudentWorld* world);

    // Mutators
    virtual void setDead();

protected:
    // Mutators
    virtual void determineNextDir();
};

/// Smart Zombie ///

class SmartZombie: public Zombie {
public:
    // Constructor
    SmartZombie(double startX, double startY, StudentWorld* world);

    // Mutators
    virtual void setDead();

protected:
    // Mutators
    virtual void determineNextDir();
};



////////////////////////////
/// Environmental Actors ///
////////////////////////////

// Actors that do not move – e.g. walls, mines, flames, vomit, etc.
class EnvironmentalActor: public Actor {
public:
    // Constructor/Destructor
    EnvironmentalActor(int imageID, double startX, double startY, Direction startDir, int depth, StudentWorld* world);

    // Accessors
    virtual bool fallsIntoPits() const;
    virtual bool damagedByFlame() const;
    virtual bool triggersLandmines() const;
    virtual bool isInfectible() const;

    // Mutators
    virtual void setDead();

protected:
    // Accessors
    virtual bool blocksMovement(double destX, double destY, const Actor *actor) const;
};



/// Wall ///

class Wall: public EnvironmentalActor {
public:
    // Constructor/Destructor
    Wall(double startX, double startY, StudentWorld* world);

    // Accessors
    virtual bool blocksFlames(double destX, double destY, const Actor* actor) const;

    // Mutators
    virtual void doSomething();

protected:
    // Accessors
    virtual bool blocksMovement(double destX, double destY, const Actor *actor) const;

    // Mutators
    virtual void setDead();
};



/// Exit ///

class Exit: public EnvironmentalActor {
public:
    // Constructor/Destructor
    Exit(double startX, double startY, StudentWorld* world);

    // Accessors
    virtual bool detectsExits() const;

    // Mutators
    virtual void doSomething();
    virtual void setDead();
    virtual bool blocksFlames(double destX, double destY, const Actor* actor) const;

protected:
    // Accessors
    bool playerExits() const;
};



/// Pit ///

class Pit: public EnvironmentalActor {
public:
    // Constructor/Destructor
    Pit(double startX, double startY, StudentWorld* world);

    // Accessors
    virtual bool damagedByFlame() const;

    // Mutators
    virtual void doSomething();
    virtual void setDead();
};



/// Flame ///

// To be constructed whenever the flamethrower necessitates it
class Flame: public EnvironmentalActor {
public:
    // Constructor/Destructor
    Flame(double startX, double startY, Direction dir, StudentWorld* world);

    // Mutators
    virtual void doSomething();

private:
    int m_ticksAlive;
};



/// Vomit ///

// To be constructed whenever a Zombie necessitates it
class Vomit: public EnvironmentalActor {
public:
    // Constructor/Destructor
    Vomit(double startX, double startY, Direction dir, StudentWorld* world);

    // Mutators
    virtual void doSomething();

private:
    int m_ticksAlive;
};



/// Landmines ///

class Landmine: public EnvironmentalActor {
public:
    // Constructor/Destructor
    Landmine(double startX, double startY, StudentWorld* world);

    // Accessors
    virtual bool damagedByFlame() const;

    // Mutators
    virtual void doSomething();
    virtual void setDead();

private:
    bool m_active;
    int m_safetyTicks;
};



///////////////
/// Goodies ///
///////////////

class Goodie: public EnvironmentalActor {
public:
    // Constructor/Destructor
    Goodie(int imageID, double startX, double startY, StudentWorld* world);

    // Accessors
    virtual bool damagedByFlame() const;

    // Mutators
    virtual void doSomething();

protected:
    // Mutators
    virtual void addToInventory() = 0;
};



/// Vaccines ///

class Vaccine: public Goodie {
public:
    // Constructor/Destructor
    Vaccine(double startX, double startY, StudentWorld* world);

protected:
    virtual void addToInventory();
};



/// Gas Can ///

class GasCan: public Goodie {
public:
    // Constructor/Destructor
    GasCan(double startX, double startY, StudentWorld* world);

protected:
    virtual void addToInventory();
};



/// Landmine Goodie ///

class LandmineGoodie: public Goodie {
public:
    // Constructor/Destructor
    LandmineGoodie(double startX, double startY, StudentWorld* world);

protected:
    virtual void addToInventory();
};

#endif // ACTOR_H_