#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "provided/GameWorld.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Human;
class Penelope;
class Citizen;
class Zombie;

class StudentWorld: public GameWorld
{
public:
    // Constructor/Destructor
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();

    // Mutators
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addActor(Actor* a);
    void decCitsLeft();
    void setFinished();
    void killActorsInPits(const Actor* pit);
    void killBurnedActors(const Actor* flame);
    void removeExitedCitizens(const Actor *exit);
    void infectOverlappingActors(const Actor* vomit);

    // Accessors
    int citsLeft() const;
    Penelope* getPlayer() const;
      // always pass in "this" to hasActorBlockingMovement, so it won't compare against itself
    bool hasActorBlockingMovement(double destX, double destY, const Actor* checker) const;
    bool playerBlocksMovement(double destX, double destY) const;
    bool hasActorBlockingFlames(double destX, double destY) const;
    bool actorWouldOverlap(double destX, double destY) const;
    bool actorWouldGetPukedOn(double destX, double destY) const;
    bool actorTriggersLandmine(const Actor* landmine) const;
    Human* getClosestPersonToZombie(const Zombie* zombie) const;
    Zombie* getClosestZombieToCitizen(const Citizen* citizen) const;

private:
    std::list<Actor*> m_actors;
    Penelope* m_player;
    int m_citsLeft;
    bool m_levelFinished;
      // debug counter so I can have events occur at specified times
    int m_debugTicks;
};



/// Inline Declarations ///

inline
StudentWorld::StudentWorld(std::string assetPath)
: GameWorld(assetPath) {}

inline
StudentWorld::~StudentWorld() {
    cleanUp();
}

inline
Penelope* StudentWorld::getPlayer() const {
    return m_player;
}

inline
int StudentWorld::citsLeft() const {
    return m_citsLeft;
}

inline
void StudentWorld::decCitsLeft() {
    if (m_citsLeft > 0)
        m_citsLeft--;
}

// only to be called by the Exit class when the player has completed the level
inline
void StudentWorld::setFinished() {
    if (!m_levelFinished)
        m_levelFinished = true;
}

#endif // STUDENTWORLD_H_