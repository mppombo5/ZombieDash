#include "Actor.h"
#include "StudentWorld.h"
#include "provided/GameConstants.h"
#include <algorithm>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

////////////////////////
/// Global Functions ///
////////////////////////

// checks whether num is contained in between lower and upper inclusive
bool containedIn(double num, double lower, double upper) {
    return (lower <= num && num <= upper);
}

// checks whether two coordinates are within radius 10 of each other
bool intrudesRadius10(double firstX, double firstY, double secondX, double secondY) {
    double dX = secondX - firstX;
    double dY = secondY - firstY;

    return (dX * dX) + (dY * dY) <= 100;
}

bool intersectsBoundingBox(double destX, double destY, const Actor* otherActor) {
    // lower corners: destX,destY; otherX,otherY

    // checks:
    // otherX <=        destX         <= otherX+SPRITE_WIDTH-1
    // otherX <= destX+SPRITE_WIDTH-1 <= otherX+SPRITE_WIDTH-1
    // and
    // otherY <=         destY         <= otherY+SPRITE_HEIGHT-1
    // otherY <= destY+SPRITE_HEIGHT-1 <= otherY+SPRITE_HEIGHT-1

    double otherX     = otherActor->getX();
    double otherXEdge = otherActor->getX() + SPRITE_WIDTH-1;
    double otherY     = otherActor->getY();
    double otherYEdge = otherActor->getY() + SPRITE_HEIGHT-1;

    return (  (containedIn(destX, otherX, otherXEdge) || containedIn(destX + SPRITE_WIDTH-1, otherX, otherXEdge))
           && (containedIn(destY, otherY, otherYEdge) || containedIn(destY + SPRITE_HEIGHT-1, otherY, otherYEdge)));
}

////////////////////////////
/// Actor Implementation ///
////////////////////////////

/// Constructor/Destructor ///

Actor::Actor(int imageID, double startX, double startY, Direction startDir, int depth, StudentWorld* world)
        : GraphObject(imageID, startX, startY, startDir, depth) {
    m_world = world;
    m_dead = false;
    m_infected = false;
    m_infectionCount = 0;
}

/// Accessors ///

bool Actor::exits(const Actor* actor) const {
    return false;
    // for actors that can exit, just check if they're overlapping
}

// only the Exit class will ever return true, but so be it
bool Actor::detectsExits() const {
    return false;
}

bool Actor::isZombie() const {
    return false;
}

bool Actor::blocksFlames(double destX, double destY, const Actor *actor) const {
    return false;
}

bool Actor::isDead() const {
    return m_dead;
}

bool Actor::flameBlocked(double destX, double destY) const {
    return getWorld()->hasActorBlockingFlames(destX, destY);
}

bool Actor::overlaps(const Actor* other) const {
    return intrudesRadius10(getX(), getY(), other->getX(), other->getY());
}

// this is specifically different than overlaps because it's checking whether or not an actor
// WOULD overlap this actor if this actor were to move to destX,destY.
bool Actor::wouldOverlap(double destX, double destY, double actorX, double actorY) const {
    return intrudesRadius10(destX, destY, actorX, actorY);
}

bool Actor::isInfected() const {
    return m_infected;
}

int Actor::infectionCount() const {
    return m_infectionCount;
}

double Actor::squareDistBetween(double coordX, double coordY, const Actor *other) const {
    double dX = coordX - other->getX();
    double dY = coordY - other->getY();

    return (dX * dX) + (dY * dY);
}

/*double Actor::potentialSquareDistBetween(double destX, double destY, const Actor *other) const {
    double dX = destX - other->getX();
    double dY = destY
}*/

StudentWorld* Actor::getWorld() const {
    return m_world;
}

/// Mutators ///

void Actor::increaseInfection() {
    m_infectionCount++;
}

void Actor::setm_dead() {
    m_dead = true;
}

void Actor::infect() {}

void Actor::setm_infected() {
    m_infected = true;
}

void Actor::cureInfection() {
    m_infectionCount = 0;
    m_infected = false;
}

////////////////////////////////////
/// SentientActor Implementation ///
////////////////////////////////////

/// Constructor/Destructor ///

SentientActor::SentientActor(int imageID, double startX, double startY, Direction startDir, int depth, StudentWorld* world)
        : Actor(imageID, startX, startY, startDir, depth, world) {
}

/// Accessors ///

bool SentientActor::fallsIntoPits() const {
    return true;
}

bool SentientActor::damagedByFlame() const {
    return true;
}

bool SentientActor::triggersLandmines() const {
    return true;
}

/// Mutators ///

void SentientActor::setDead() {
    if (!isDead())
        setm_dead();
}

bool SentientActor::blocksMovement(double destX, double destY, const Actor *actor) const {
    // IMPORTANT: make the actor parameter this, because this is the thing you're checking destX and destY against
    return intersectsBoundingBox(destX, destY, this);
}

//////////////
/// Humans ///
//////////////

/// Constructor ///

Human::Human(int imageID, double startX, double startY, Direction startDir, int depth, StudentWorld *world)
: SentientActor(imageID, startX, startY, startDir, depth, world) {

}

/// Accessors ///

bool Human::isInfectible() const {
    return true;
}

bool Human::exits(const Actor *actor) const {
    if (actor->detectsExits())
        return overlaps(actor);
    return false;
}

/// Mutators ///

void Human::infect() {
    if (!isInfected())
        setm_infected();
}



///////////////////////////////
/// Penelope Implementation ///
///////////////////////////////

/// Constructor/Destructor ///

Penelope::Penelope(double startX, double startY, StudentWorld* world)
        : Human(IID_PLAYER, startX, startY, right, 0, world) {
    m_flames = 0;
    m_landmines = 0;
    m_vaccines = 0;
}

/// Accessors ///

int Penelope::getVaccines() const {
    return m_vaccines;
}

int Penelope::getLandmines() const {
    return m_landmines;
}

int Penelope::getFlames() const {
    return m_flames;
}

/// Mutators ///

void Penelope::setDead() {
    SentientActor::setDead();
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

// makes it easier to move somewhere by removing the block checks each time
void Penelope::safeMoveTo(double destX, double destY) {
    if (!getWorld()->hasActorBlockingMovement(destX, destY, this))
        moveTo(destX, destY);
}

void Penelope::addVaccine() {
    m_vaccines++;
}

void Penelope::addFlames() {
    m_flames += 5;
}

void Penelope::addLandmines() {
    m_landmines += 2;
}

void Penelope::doSomething() {
    // check if she's dead
    if (isDead())
        return;

    // check for infection, increase if she is and kill her if it reaches 500
    if (isInfected())
        increaseInfection();
    if (infectionCount() >= 500) {
        setDead();
        return;
    }

    // check for user input, respond appropriately
    int key;
    if (getWorld()->getKey(key)) {
        switch (key) {
            // directional key input
            case KEY_PRESS_RIGHT: {
                setDirection(right);
                double destX = getX() + 4;
                double destY = getY();
                safeMoveTo(destX, destY);
                break;
            }
            case KEY_PRESS_DOWN: {
                setDirection(down);
                double destX = getX();
                double destY = getY() - 4;
                safeMoveTo(destX, destY);
                break;
            }
            case KEY_PRESS_LEFT: {
                setDirection(left);
                double destX = getX() - 4;
                double destY = getY();
                safeMoveTo(destX, destY);
                break;
            }
            case KEY_PRESS_UP: {
                setDirection(up);
                double destX = getX();
                double destY = getY() + 4;
                safeMoveTo(destX, destY);
                break;
            }
            // flamethrower: spawn three flames in the direction that the player is facing
            case KEY_PRESS_SPACE: {
                if (getFlames() >= 1) {
                    m_flames--;
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    // make multipliers by which to multiply the x and y directions, so
                    // everything can be condensed into one generalized function
                    int multX = 0;
                    int multY = 0;
                    switch (getDirection()) {
                        case up:
                            multY = 1;
                            break;
                        case right:
                            multX = 1;
                            break;
                        case down:
                            multY = -1;
                            break;
                        case left:
                            multX = -1;
                            break;
                        default: break;
                    }
                    for (int i = 1; i <= 3; i++) {
                        double destX = getX() + (i * (multX * SPRITE_WIDTH));
                        double destY = getY() + (i * (multY * SPRITE_HEIGHT));
                        if (!flameBlocked(destX, destY))
                            getWorld()->addActor(new Flame(destX, destY, getDirection(), getWorld()));
                        else
                            break;
                    }
                }
                break;
            }
            // vaccine: use a vaccine and cure the user
            case KEY_PRESS_ENTER: {
                if (m_vaccines >= 1) {
                    cureInfection();
                    m_vaccines--;
                }
                break;
            }
            // landmine: spawn a landmine where the player is standing
            case KEY_PRESS_TAB: {
                if (m_landmines >= 1) {
                    getWorld()->addActor(new Landmine(getX(), getY(), getWorld()));
                    m_landmines--;
                }
                break;
            }
            default: break;
        }
    }
}



//////////////////////////////
/// Citizen Implementation ///
//////////////////////////////

Citizen::Citizen(double startX, double startY, StudentWorld *world)
: Human(IID_CITIZEN, startX, startY, right, 0, world) {
    m_paralyzed = false;
}

void Citizen::infect() {
    if (!isInfected())
        getWorld()->playSound(SOUND_CITIZEN_INFECTED);
    Human::infect();
}

void Citizen::setDead() {
    SentientActor::setDead();
    getWorld()->decCitsLeft();
}

void Citizen::paralyze() {
    m_paralyzed = true;
}

void Citizen::doSomething() {
    if (isDead())
        return;

    // check and respond for infection
    if (isInfected()) {
        increaseInfection();
        if (infectionCount() >= 500) {
            setDead();
            getWorld()->playSound(SOUND_ZOMBIE_BORN);
            getWorld()->increaseScore(-1000);
            int X = randInt(1, 10);
            if (X <= 7)
                getWorld()->addActor(new DumbZombie(getX(), getY(), getWorld()));
            else
                getWorld()->addActor(new SmartZombie(getX(), getY(), getWorld()));
            return;
        }
    }

    // paralysis check
    if (m_paralyzed) {
        m_paralyzed = false;
        return;
    }

    Penelope* p = getWorld()->getPlayer();
    double distP = squareDistBetween(getX(), getY(), getWorld()->getPlayer());
    Zombie* z = getWorld()->getClosestZombieToCitizen(this);
    // distZ gets -1 if there are no zombies in the world
    double distZ = (z == nullptr ? -1 : squareDistBetween(getX(), getY(), z));

    // if the player is the closest entity and <= 80 pixels away, move the citizen closer
    if ((distP < distZ || distZ == -1) && distP <= (80 * 80)) {
        // citizen is on the same column as Penelope
        if (getX() == p->getX()) {
            double destX = getX();
            double destY = getY() + (getY() > p->getY() ? -2 : 2);
            if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                    && !getWorld()->playerBlocksMovement(destX, destY)) {
                setDirection(destX > getX() ? down : up);
                moveTo(destX, destY);
                paralyze();
                return;
            }
        }
        // citizen is on the same row as Penelope
        else if (getY() == p->getY()) {
            double destX = getX() + (getX() > p->getX() ? -2 : 2);
            double destY = getY();
            if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                    && !getWorld()->playerBlocksMovement(destX, destY)) {
                setDirection(destX > getX() ? right : left);
                moveTo(destX, destY);
                paralyze();
                return;
            }
        }
        // citizen is not exactly in the row/column, so just move it closer
        else {
            int X = randInt(1, 2);
            switch (X) {
                case 1: {
                    double destX = getX() + (getX() > p->getX() ? -2 : 2);
                    double destY = getY();
                    if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                            && !getWorld()->playerBlocksMovement(destX, destY)) {
                        setDirection(destX > getX() ? right : left);
                        moveTo(destX, destY);
                        paralyze();
                        return;
                    }
                    // if at this point the function has not returned, try going in the y direction
                    destX = getX();
                    destY = getY() + (getY() > p->getY() ? -2 : 2);
                    if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                            && !getWorld()->playerBlocksMovement(destX, destY)) {
                        setDirection(destY > getY() ? up : down);
                        moveTo(destX, destY);
                        paralyze();
                        return;
                    }
                }
                    break;
                case 2: {
                    double destX = getX();
                    double destY = getY() + (getY() > p->getY() ? -2 : 2);
                    if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                            && !getWorld()->playerBlocksMovement(destX, destY)) {
                        setDirection(destY > getY() ? up : down);
                        moveTo(destX, destY);
                        paralyze();
                        return;
                    }
                    // same thing: if it hasn't returned, try the x direction
                    destX = getX() + (getX() > p->getX() ? -2 : 2);
                    destY = getY();
                    if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                            && !getWorld()->playerBlocksMovement(destX, destY)) {
                        setDirection(destX > getX() ? right : left);
                        moveTo(destX, destY);
                        paralyze();
                        return;
                    }
                }
                    break;
                default: break;
            }
        }
    }

    // we don't need an else if, because if nothing above returned out then
    // we continue onto whether there's a zombie within 80 pixels
    if (z != nullptr && !z->isDead() && distZ <= (80 * 80)) {
        // now we need to calculate the potential difference for the citizen moving in each direction
        // if that direction is blocked, leave the difference at -1
        double dUp = -1;
        double dDown = -1;
        double dRight = -1;
        double dLeft = -1;
        double maxDist = distZ;

        double destX;
        double destY;

        // dUp check
        destX = getX();
        destY = getY() + 2;
        if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                && !getWorld()->playerBlocksMovement(destX, destY)) {
            // make a temporary citizen at (destX,destY) and find the nearest zombie to that one
            Citizen* temp = new Citizen(destX, destY, getWorld());
            Zombie* closestZ = getWorld()->getClosestZombieToCitizen(temp);
            // don't need to check if closestZ is nullptr, because we already know there's one nearby
            dUp = squareDistBetween(destX, destY, closestZ);
            maxDist = (dUp > maxDist ? dUp : maxDist);
            // no memory leaks in my town
            delete temp;
        }

        // dDown check
        destY = getY() - 2;
        if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                && !getWorld()->playerBlocksMovement(destX, destY)) {
            Citizen* temp = new Citizen(destX, destY, getWorld());
            Zombie* closestZ = getWorld()->getClosestZombieToCitizen(temp);
            dDown = squareDistBetween(destX, destY, closestZ);
            maxDist = (dDown > maxDist ? dDown : maxDist);
            delete temp;
        }

        // dRight check
        destX = getX() + 2;
        destY = getY();
        if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                && !getWorld()->playerBlocksMovement(destX, destY)) {
            Citizen* temp = new Citizen(destX, destY, getWorld());
            Zombie* closestZ = getWorld()->getClosestZombieToCitizen(temp);
            dRight = squareDistBetween(destX, destY, closestZ);
            maxDist = (dRight > maxDist ? dRight : maxDist);
            delete temp;
        }

        // dLeft check
        destX = getX() - 2;
        if (!getWorld()->hasActorBlockingMovement(destX, destY, this)
                && !getWorld()->playerBlocksMovement(destX, destY)) {
            Citizen* temp = new Citizen(destX, destY, getWorld());
            Zombie* closestZ = getWorld()->getClosestZombieToCitizen(temp);
            dLeft = squareDistBetween(destX, destY, closestZ);
            maxDist = (dLeft > maxDist ? dLeft : maxDist);
            delete temp;
        }

        // if maxDist never increased, return immediately because it shouldn't move
        if (maxDist == distZ) {
            paralyze();
            return;
        }
        // otherwise, move the citizen in the direction that most increased distance
        if (maxDist == dUp) {
            setDirection(up);
            moveTo(getX(), getY() + 2);
            paralyze();
            return;
        }
        if (maxDist == dDown) {
            setDirection(down);
            moveTo(getX(), getY() - 2);
            paralyze();
            return;
        }
        if (maxDist == dRight) {
            setDirection(right);
            moveTo(getX() + 2, getY());
            paralyze();
            return;
        }
        if (maxDist == dLeft) {
            setDirection(left);
            moveTo(getX() - 2, getY());
            paralyze();
            return;
        }
    }

    // paralyze the citizen, should it reach this point
    paralyze();
}



/////////////////////////////
/// Zombie Implementation ///
/////////////////////////////

/// Constructor/Destructor ///

Zombie::Zombie(double startX, double startY, StudentWorld *world)
: SentientActor(IID_ZOMBIE, startX, startY, right, 0, world) {
    m_mvtPlan = 0;
    m_paralyzed = false;
}

/// Accessors ///

bool Zombie::isInfectible() const {
    return false;
}

bool Zombie::isZombie() const {
    return true;
}

int Zombie::mvtPlan() const {
    return m_mvtPlan;
}

/// Mutators ///

void Zombie::decMvtPlan() {
    m_mvtPlan--;
}

void Zombie::newMvtPlan() {
    m_mvtPlan = randInt(3, 10);
}

void Zombie::delMvtPlan() {
    m_mvtPlan = 0;
}

void Zombie::doSomething() {
    if (isDead())
        return;

    // don't do anything if it's paralyzed
    if (m_paralyzed) {
        m_paralyzed = false;
        return;
    }

    // compute vomit coordinates in the direction it's facing
    int multX = 0;
    int multY = 0;
    switch (getDirection()) {
        case up:
            multY = 1;
            break;
        case down:
            multY = -1;
            break;
        case right:
            multX = 1;
            break;
        case left:
            multX = -1;
            break;
        default: break;
    }

    // set vomit destinations (vDest) for x and y
    double vDestX = getX() + (multX * SPRITE_WIDTH);
    double vDestY = getY() + (multY * SPRITE_HEIGHT);
    if (getWorld()->actorWouldGetPukedOn(vDestX, vDestY)) {
        // randInt to determine 1/3 chance that vomit is spawned
        if (randInt(1, 3) == 1) {
            getWorld()->addActor(new Vomit(vDestX, vDestY, getDirection(), getWorld()));
            getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
    }

    // run the differentiated zombie movement between dumb and smart zombies
    determineNextDir();

    // copy and paste code to set multX and multY
    multX = 0;
    multY = 0;
    switch (getDirection()) {
        case up:
            multY = 1;
            break;
        case down:
            multY = -1;
            break;
        case right:
            multX = 1;
            break;
        case left:
            multX = -1;
            break;
        default: break;
    }
    // now move in the direction it's facing
    //if (!movementBlocked(getX() + multX, getY() + multY)) {
    double destX = getX() + multX;
    double destY = getY() + multY;
    if (!getWorld()->playerBlocksMovement(destX, destY) && !getWorld()->hasActorBlockingMovement(destX, destY, this)) {
        moveTo(destX, destY);
        decMvtPlan();
    }
    else
        delMvtPlan();

    // this goes at the end to switch its paralyzed state
    m_paralyzed = true;
}



////////////////////////////////////////
/// Dumb/Smart Zombie Implementation ///
////////////////////////////////////////

/// Dumb Zombie ///

DumbZombie::DumbZombie(double startX, double startY, StudentWorld *world)
: Zombie(startX, startY, world) {

}

void DumbZombie::setDead() {
    SentientActor::setDead();
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(1000);
    // attempt to have the zombie throw a vaccine one spot away
    if (randInt(1, 10) == 5) {
        // the zombie was carrying a vaccine, so try and fling it away
        // select a new random direction
        int multX = 0;
        int multY = 0;
        int X = randInt(1,4);
        switch (X) {
            case 1:
                // up
                multY = 1;
                break;
            case 2:
                // down
                multY = -1;
                break;
            case 3:
                // right
                multX = 1;
                break;
            case 4:
                // left
                multX = -1;
                break;
            default: break;
        }
        // select a new destination SPRITE_HEIGHT or WIDTH away from the deceased
        double destX = getX() + (multX * SPRITE_WIDTH);
        double destY = getY() + (multY * SPRITE_HEIGHT);
        if (!getWorld()->actorWouldOverlap(destX, destY))
            getWorld()->addActor(new Vaccine(destX, destY, getWorld()));
    }
}

void DumbZombie::determineNextDir() {
    // needs a new movement plan
    if (mvtPlan() <= 0) {
        newMvtPlan();
        int X = randInt(1,4);
        switch (X) {
            case 1:
                setDirection(up);
                return;
            case 2:
                setDirection(down);
                return;
            case 3:
                setDirection(left);
                return;
            case 4:
                setDirection(right);
                return;
            default: return;
        }
    }
}

/// Smart Zombie ///

SmartZombie::SmartZombie(double startX, double startY, StudentWorld *world)
        : Zombie(startX, startY, world) {

}

void SmartZombie::setDead() {
    SentientActor::setDead();
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(2000);
}

void SmartZombie::determineNextDir() {
    // needs a new movement plan
    if (mvtPlan() <= 0) {
        newMvtPlan();
        Human* closest = getWorld()->getClosestPersonToZombie(this);
        // now we set the zombie's direction accordingly.
        // check if closest is more than 80 pixels away:
        if (squareDistBetween(getX(), getY(), closest) > (80 * 80)) {
            int X = randInt(1, 4);
            switch (X) {
                case 1:
                    setDirection(up);
                    return;
                case 2:
                    setDirection(down);
                    return;
                case 3:
                    setDirection(left);
                    return;
                case 4:
                    setDirection(right);
                    return;
                default: return;
            }
        }
        else {
            // if it's in the same overlap column as closest, set its direction to
            // up or down depending on its relative y position
            if (getX() == closest->getX()) {
                setDirection(getY() > closest->getY() ? down : up);
                return;
            }
            // if it's contained in the same row, set left or right relative to x position
            else if (getY() == closest->getY()) {
                setDirection(getX() > closest->getX() ? left : right);
                return;
            }
            else {
                int X = randInt(1, 2);
                switch (X) {
                    case 1:
                        setDirection(getX() > closest->getX() ? left : right);
                        return;
                    case 2:
                        setDirection(getY() > closest->getY() ? down : up);
                        return;
                    default: return;
                }
            }
        }
    }
}



/////////////////////////////////////////
/// EnvironmentalActor Implementation ///
/////////////////////////////////////////

/// Constructor/Destructor ///

EnvironmentalActor::EnvironmentalActor(int imageID, double startX, double startY, Direction startDir, int depth, StudentWorld* world)
        : Actor(imageID, startX, startY, startDir, depth, world) {

}

/// Accessors ///

bool EnvironmentalActor::fallsIntoPits() const {
    return false;
}

// Environmental actors (except landmines) are not damaged by flames, but goodies are
bool EnvironmentalActor::damagedByFlame() const {
    return false;
}

bool EnvironmentalActor::triggersLandmines() const {
    return false;
}

bool EnvironmentalActor::blocksMovement(double destX, double destY, const Actor *actor) const {
    return false;
}

bool EnvironmentalActor::isInfectible() const {
    return false;
}

/// Mutators ///

void EnvironmentalActor::setDead() {
    if (!isDead())
        setm_dead();
}



///////////////////////////
/// Wall Implementation ///
///////////////////////////

/// Constructor/Destructor ///

Wall::Wall(double startX, double startY, StudentWorld* world)
        : EnvironmentalActor(IID_WALL, startX, startY, right, 0, world) {}

/// Accessors ///

bool Wall::blocksFlames(double destX, double destY, const Actor* actor) const {
    return wouldOverlap(destX, destY, getX(), getY());
}

bool Wall::blocksMovement(double destX, double destY, const Actor *actor) const {
    return intersectsBoundingBox(destX, destY, this);
}

/// Mutators ///

void Wall::doSomething() {}

// walls should never be set to dead
void Wall::setDead() {}



///////////////////////////
/// Exit Implementation ///
///////////////////////////

/// Constructor/Destructor ///

Exit::Exit(double startX, double startY, StudentWorld *world)
:EnvironmentalActor(IID_EXIT, startX, startY, right, 1, world) {

}

/// Accessors ///

bool Exit::playerExits() const {
    return getWorld()->getPlayer()->exits(this);
}

// just denotes that it detects when something exits
bool Exit::detectsExits() const {
    return true;
}

/// Mutators ///

void Exit::doSomething() {
    getWorld()->removeExitedCitizens(this);

    if (playerExits() && getWorld()->citsLeft() == 0) {
        getWorld()->setFinished();
    }
}

// exits should never be dead
void Exit::setDead() {}

bool Exit::blocksFlames(double destX, double destY, const Actor* actor) const {
    return wouldOverlap(destX, destY, getX(), getY());
}



//////////////////////////
/// Pit Implementation ///
//////////////////////////

/// Constructor/Destructor ///

Pit::Pit(double startX, double startY, StudentWorld *world)
: EnvironmentalActor(IID_PIT, startX, startY, right, 0, world) {

}

/// Accessors ///

bool Pit::damagedByFlame() const {
    return false;
}

/// Mutators ///

void Pit::doSomething() {
    getWorld()->killActorsInPits(this);
}

// pits should never be dead
void Pit::setDead() {}



////////////////////////////
/// Flame Implementation ///
////////////////////////////

/// Constructor/Destructor ///

Flame::Flame(double startX, double startY, Direction dir, StudentWorld *world)
: EnvironmentalActor(IID_FLAME, startX, startY, dir, 0, world) {
    m_ticksAlive = 0;
}

/// Mutators ///

void Flame::doSomething() {
    m_ticksAlive++;

    if (isDead())
        return;

    if (m_ticksAlive > 2) {
        setDead();
        return;
    }

    getWorld()->killBurnedActors(this);
}



////////////////////////////
/// Vomit Implementation ///
////////////////////////////

/// Constructor ///

Vomit::Vomit(double startX, double startY, Direction dir, StudentWorld *world)
: EnvironmentalActor(IID_VOMIT, startX, startY, dir, 0, world) {

}

/// Mutators ///

void Vomit::doSomething() {
    m_ticksAlive++;

    if (isDead())
        return;

    if (m_ticksAlive > 2) {
        setDead();
        return;
    }

    getWorld()->infectOverlappingActors(this);
}



///////////////////////////////
/// Landmine Implementation ///
///////////////////////////////

/// Constructor ///

Landmine::Landmine(double startX, double startY, StudentWorld *world)
: EnvironmentalActor(IID_LANDMINE, startX, startY, right, 1, world) {
    m_active = false;
    m_safetyTicks = 30;
}

/// Accessors ///

bool Landmine::damagedByFlame() const {
    return true;
}

/// Mutators ///

void Landmine::setDead() {
    EnvironmentalActor::setDead();
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);

    // nested for loops to add flames in a 3x3 pattern
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // go from -1 to 1, so that would be -1 + i or j
            int multX = -1 + i;
            int multY = -1 + j;
            double destX = getX() + (SPRITE_WIDTH * multX);
            double destY = getY() + (SPRITE_HEIGHT * multY);
            if (!flameBlocked(destX, destY))
                getWorld()->addActor(new Flame(destX, destY, up, getWorld()));
        }
    }
    getWorld()->addActor(new Pit(getX(), getY(), getWorld()));
}

void Landmine::doSomething() {
    if (isDead())
        return;

    if (!m_active) {
        m_safetyTicks--;
        if (m_safetyTicks <= 0) {
            m_active = true;
        }
        return;
    }

    if (m_active && getWorld()->actorTriggersLandmine(this)) {
        setDead();
    }
}



/////////////////////////////
/// Goodie Implementation ///
/////////////////////////////

/// Constructor ///

Goodie::Goodie(int imageID, double startX, double startY, StudentWorld *world)
: EnvironmentalActor(imageID, startX, startY, right, 1, world) {

}

/// Accessors ///

bool Goodie::damagedByFlame() const {
    return true;
}

/// Mutators ///

// the goodies' implementations of doSomething would be nearly identical – so they're all in one with
// a single differentiated function.
void Goodie::doSomething() {
    if (isDead())
        return;

    Penelope* player = getWorld()->getPlayer();
    if(!player->isDead() && player->overlaps(this)) {
        getWorld()->increaseScore(50);
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        // THIS is what differentiates the goodies from each other – what they add to the player's inventory
        addToInventory();
    }
}



//////////////////////////////
/// Vaccine Implementation ///
//////////////////////////////

/// Constructor ///

Vaccine::Vaccine(double startX, double startY, StudentWorld *world)
: Goodie(IID_VACCINE_GOODIE, startX, startY, world) {

}

/// Mutators ///

void Vaccine::addToInventory() {
    getWorld()->getPlayer()->addVaccine();
}



/////////////////////////////
/// GasCan Implementation ///
/////////////////////////////

/// Constructor ///

GasCan::GasCan(double startX, double startY, StudentWorld *world)
: Goodie(IID_GAS_CAN_GOODIE, startX, startY, world) {

}

/// Mutators ///

void GasCan::addToInventory() {
    getWorld()->getPlayer()->addFlames();
}



/////////////////////////////////////
/// LandmineGoodie Implementation ///
/////////////////////////////////////

/// Constructor ///

LandmineGoodie::LandmineGoodie(double startX, double startY, StudentWorld *world)
: Goodie(IID_LANDMINE_GOODIE, startX, startY, world) {

}

/// Mutators ///

void LandmineGoodie::addToInventory() {
    getWorld()->getPlayer()->addLandmines();
}