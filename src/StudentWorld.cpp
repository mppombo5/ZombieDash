#include "provided/GameWorld.h"
#include "StudentWorld.h"
#include "provided/GameConstants.h"
#include "Actor.h"
#include "provided/Level.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

int StudentWorld::init()
{
    Level level(assetPath());
    // Now to figure out which level to load: "level" followed by two digits, the first of which might be 0
    string levelFile = "level";
    ostringstream oss;
    oss.fill('0');
    // Make sure to fill up 6 spaces: "xx.txt"
    string lvltxt = to_string(getLevel()) + ".txt";
    oss << setw(6) << lvltxt;
    levelFile += oss.str();
    // I just really hope they don't make more than 99 levels.

    Level::LoadResult levelResult = level.loadLevel(levelFile);

    // couldn't find level file, so we assume they won
    if (levelResult == Level::load_fail_file_not_found || getLevel() > 99) {
        return GWSTATUS_PLAYER_WON;
    }
    // level file was improperly formatted
    else if (levelResult == Level::load_fail_bad_format) {
        return GWSTATUS_LEVEL_ERROR;
    }

    else if (levelResult == Level::load_success) {
        // level was loaded successfully, so we can go about our day
        m_citsLeft = 0;
        m_levelFinished = false;
        m_debugTicks = 0;

        // first we loop through and add each actor to m_actors according to the level file
        // nested for loops to check for each spot on the grid
        for (int i = 0; i < LEVEL_WIDTH; i++) {
            for (int k = 0; k < LEVEL_HEIGHT; k++) {
                double startX = i * SPRITE_WIDTH;
                double startY = k * SPRITE_HEIGHT;

                const Level::MazeEntry& objectAtCoords = level.getContentsOf(i, k);
                switch (objectAtCoords) {
                    case Level::empty:
                        break;
                    case Level::smart_zombie:
                        m_actors.push_back(new SmartZombie(startX, startY, this));
                        break;
                    case Level::dumb_zombie:
                        m_actors.push_back(new DumbZombie(startX, startY, this));
                        break;
                    case Level::player:
                        m_player = new Penelope(startX, startY, this);
                        break;
                    case Level::exit:
                        m_actors.push_back(new Exit(startX, startY, this));
                        break;
                    case Level::citizen:
                        m_actors.push_back(new Citizen(startX, startY, this));
                        m_citsLeft++;
                        break;
                    case Level::wall:
                        m_actors.push_back(new Wall(startX, startY, this));
                        break;
                    case Level::pit:
                        m_actors.push_back(new Pit(startX, startY, this));
                        break;
                    case Level::vaccine_goodie:
                        m_actors.push_back(new Vaccine(startX, startY, this));
                        break;
                    case Level::gas_can_goodie:
                        m_actors.push_back(new GasCan(startX, startY, this));
                        break;
                    case Level::landmine_goodie:
                        m_actors.push_back(new LandmineGoodie(startX, startY, this));
                        break;
                }
            }
        }
        return GWSTATUS_CONTINUE_GAME;
    }
    // I have no idea when this would be the case, but I figure this is as appropriate a response as any.
    else
        return GWSTATUS_LEVEL_ERROR;
}

// NOTE: to check which one to remove, just set it to dead
int StudentWorld::move()
{
    m_player->doSomething();
    list<Actor*>::iterator it;
    // have each actor do something and check if the player died
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if (! (*it)->isDead())
            (*it)->doSomething();
        if (m_player->isDead()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_levelFinished) {
            playSound(SOUND_LEVEL_FINISHED);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    // go through and delete/erase every actor that's dead
    it = m_actors.begin();
    while (it != m_actors.end()) {
        if ((*it)->isDead()) {
            delete *it;
            // sets iterator to item after erased item
            it = m_actors.erase(it);
            continue;
        }
        // otherwise, just keep traversing
        it++;
    }

    // update the game status line
    ostringstream oss;
    oss << "Score: ";
    oss.fill('0');
    if (getScore() >= 0)
        oss << setw(6) << getScore() << setw(0) << "  Level: ";
    else
        oss << "-" << setw(5) << getScore() * -1 << setw(0) << "  Level:";
    oss.fill(' ');
    oss << setw(1) << getLevel() << setw(0) << "  Lives: " << getLives() << "  Vaccines: "
        << setw(1) << m_player->getVaccines() << setw(0) << "  Flames: " << setw(1) << m_player->getFlames()
        << setw(0) << "  Mines: " << setw(1) << m_player->getLandmines() << setw(0) << "  Infected: " << m_player->infectionCount();
    setGameStatText(oss.str());



    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end()) {
        delete *it;
        it = m_actors.erase(it);
    }

    if (m_player != nullptr)
        delete m_player;
    m_player = nullptr;
}

// Functions that aren't the big three

// IMPORTANT: before doing something to an actor in m_actors, CHECK IF THEY'RE DEAD
// and do nothing if they are.

void StudentWorld::addActor(Actor *a) {
    m_actors.push_back(a);
}

//// These next few functions return whether or not there is
//// an actor in the world that fulfills a certain condition

// previously looped through m_actors in actor implementation, now moved to here
bool StudentWorld::hasActorBlockingMovement(double destX, double destY, const Actor* checker) const {
    list<Actor*>::const_iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* actor = *it;
        // check if each actor would block actor
        if (!actor->isDead() && actor != checker && actor->blocksMovement(destX, destY, actor))
            return true;
    }
    return false;
}

bool StudentWorld::playerBlocksMovement(double destX, double destY) const {
    Penelope* player = getPlayer();
    return (!player->isDead() && player->blocksMovement(destX, destY, player));
}

bool StudentWorld::hasActorBlockingFlames(double destX, double destY) const {
    list <Actor*>::const_iterator it;

    for (it = m_actors.begin(); it  != m_actors.end(); it++) {
        Actor* actor = *it;
        if (!actor->isDead() && actor->blocksFlames(destX, destY, actor))
            return true;
    }
    return false;
}

bool StudentWorld::actorWouldOverlap(double destX, double destY) const {
    Penelope* player = getPlayer();
    if (!player->isDead() && player->wouldOverlap(destX, destY, player->getX(), player->getY()))
        return true;

    list<Actor*>::const_iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* actor = *it;
        if (!actor->isDead() && actor->wouldOverlap(destX, destY, actor->getX(), actor->getY()))
            return true;
    }
    return false;
}

// check if an infectible actor would get attacked with vomit
bool StudentWorld::actorWouldGetPukedOn(double destX, double destY) const {
    Penelope* player = getPlayer();
    if (!player->isDead() && player->wouldOverlap(destX, destY, player->getX(), player->getY()))
        return true;

    list<Actor*>::const_iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* actor = *it;
        if (!actor->isDead() && actor->isInfectible() && actor->wouldOverlap(destX, destY, actor->getX(), actor->getY()))
            return true;
    }
    return false;
}

// check if actors are overlapping with a landmine
bool StudentWorld::actorTriggersLandmine(const Actor *landmine) const {
    Penelope* player = getPlayer();
    if (!player->isDead() && player->overlaps(landmine))
        return true;

    list<Actor*>::const_iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* actor = *it;
        if (!actor->isDead() && actor->overlaps(landmine) && actor->triggersLandmines())
            return true;
    }
    return false;
}


//// The following two functions kill actors overlapping with flames and pits,
//// to be called by the respective actors in their doSomething() methods

// check if actors are overlapping with a pit, and kill them if they are
void StudentWorld::killActorsInPits(const Actor *pit) {
    Penelope* player = getPlayer();
    if (!player->isDead() && player->overlaps(pit))
        player->setDead();

    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* actor = *it;
        if (!actor->isDead() && actor->overlaps(pit) && actor->fallsIntoPits()) {
            actor->setDead();
            // isInfectible doubles as a useful way to tell if it's a citizen.
            if (actor->isInfectible()) {
                playSound(SOUND_CITIZEN_DIE);
                increaseScore(-1000);
            }
        }
    }
}

// next overlap check for actors touching flames
void StudentWorld::killBurnedActors(const Actor *flame) {
    Penelope* player = getPlayer();
    if (!player->isDead() && player->overlaps(flame))
        player->setDead();

    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* actor = *it;
        if (!actor->isDead() && actor->overlaps(flame) && actor->damagedByFlame()) {
            actor->setDead();
            if (actor->isInfectible()) {
                playSound(SOUND_CITIZEN_DIE);
                increaseScore(-1000);
            }
        }
    }
}

void StudentWorld::removeExitedCitizens(const Actor *exit) {
    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* a = *it;
        if (!a->isDead() && a->isInfectible() && a->overlaps(exit)) {
            a->setDead();
            increaseScore(500);
            playSound(SOUND_CITIZEN_SAVED);
        }
    }
}

void StudentWorld::infectOverlappingActors(const Actor* vomit) {
    Penelope* player = getPlayer();
    if (!player->isDead() && player->overlaps(vomit))
        player->infect();

    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* actor = *it;
        if (!actor->isDead() && actor->overlaps(vomit))
            actor->infect();
    }
}

//// The following functions are proximity checks for zombies and citizens.

// used in Smart Zombies to get the closest person. Is there a better way to do this? Probably.
Human* StudentWorld::getClosestPersonToZombie(const Zombie* zombie) const {
    // initialize closestDist to the player, and then loop through and see if any citizens are closer
    Actor* closestPerson = m_player;
    double closestDist = zombie->squareDistBetween(zombie->getX(), zombie->getY(), m_player);

    list<Actor*>::const_iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* actor = *it;
        // important to note: only humans are infectible, so this is a reasonable check for humanity
        if (!actor->isDead() && actor->isInfectible()) {
            double currentDist = zombie->squareDistBetween(zombie->getX(), zombie->getY(), actor);
            if (currentDist < closestDist) {
                closestPerson = actor;
                closestDist = currentDist;
            }
        }
    }
    // we know that closestPerson will be a human, so we need to return as such
    return dynamic_cast<Human*>(closestPerson);
}

// used in Citizens to get the closest Zombie. basically a copy paste of the above function.
Zombie* StudentWorld::getClosestZombieToCitizen(const Citizen *citizen) const {
    // initialize closestZombie to nullptr, check if it's nullptr to see if there are no zombies
    Actor* closestZombie = nullptr;
    double closestDist = 0;
    bool found = false;

    list<Actor*>::const_iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* a = *it;
        if (a->isZombie() && !a->isDead()) {
            double currentDist = citizen->squareDistBetween(citizen->getX(), citizen->getY(), a);
            // extra case if
            if (!found) {
                found = true;
                closestDist = currentDist;
                closestZombie = a;
            }
            else {
                if (currentDist < closestDist) {
                    closestDist = currentDist;
                    closestZombie = a;
                }
            }
        }
    }
    // we know that closestZombie will be a zombie, so we can downcast
    if (closestZombie != nullptr)
        return dynamic_cast<Zombie*>(closestZombie);
    return nullptr;
}