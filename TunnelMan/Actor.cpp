#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
#include <random>
#include "GraphObject.h"

using namespace std;

Actor::Actor(StudentWorld* world, int imageID, int x, int y, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, x, y, dir, size, depth), m_world(world), m_alive(true)
{
	setVisible(true);
}

Actor::~Actor()
{
	setVisible(false); //Set invisible?
}

StudentWorld* Actor::getWorld()
{
	return m_world;
}

bool Actor::isAlive()
{
	return m_alive;
}

void Actor::setDead()
{
	m_alive = false;
}

void Actor::moveTo(int x, int y)
{
	//keep actpor within grid bounds
	if (x < 0)
		x = 0;
	if (x > 60)
		x = 60;
	if (y < 0)
		y = 0;
	if (y > 60)
		y = 60;
	GraphObject::moveTo(x, y);
}

/////////////////////////////////////////////////////////////
///PERSON ////////////////////////////////////


Person::Person(StudentWorld* world, int imageID, int x, int y, Direction dir, int hp)
	:Actor(world, imageID, x, y, dir, 1.0, 0)
{
	m_hp = hp;
}

int Person::getHealth() const
{
	return m_hp;
}

void Person::damage(int h)
{
	m_hp -= h; //decrease health
}

/////////////////////////////////////////////////////////////
///// TUNNELMAN /////////////////////////////////////////////

Tunnelman::Tunnelman(StudentWorld* world)
	:Person(world, TID_PLAYER, 30, 60, right, 10)
{
	m_ammo = 5;
	m_sonar = 1;
	m_gold = 0;
}

void Tunnelman::add(int ID)
{
	if (ID == TID_WATER_POOL)
		m_ammo += 5;
	if (ID == TID_SONAR)
		m_sonar += 2;
	if (ID == TID_GOLD)
		m_gold++;

}

void Tunnelman::doSomething()
{
	//check if alive
	if (!isAlive())
		return;

	//if can remove dirt, play sound
	if (getWorld()->removeDirt(getX(), getY()))
		getWorld()->playSound(SOUND_DIG);

	//get key pressed
	int key;
	if (getWorld()->getKey(key))
	{
		switch (key)
		{
		case KEY_PRESS_RIGHT:
			moveInDirection(right);
			break;
		case KEY_PRESS_LEFT:
			moveInDirection(left);
			break;
		case KEY_PRESS_UP:
			moveInDirection(up);
			break;
		case KEY_PRESS_DOWN:
			moveInDirection(down);
			break;
		case KEY_PRESS_SPACE:
			if (m_ammo > 0) //if tman has ammo, shoot water
			{
				m_ammo--;
				shootWater();
			}
			break;
		case 'z':
		case 'Z':
			if (m_sonar > 0)
			{
				m_sonar--;
				getWorld()->isClose(getX(), getY(), 12);
				getWorld()->playSound(SOUND_SONAR);

			}
			break;
		case KEY_PRESS_TAB:
			if (m_gold > 0)
			{
				getWorld()->addActor(new Gold(getWorld(), getX(), getY(), true, true));
				m_gold--;
			}
			break;
		}
	}
}

void Tunnelman::isAnnoyed(int h)
{
	damage(h); //decrease health
	if (getHealth() <= 0) //if health == 0, then set status to dead
		setDead();
	getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
}

void Tunnelman::moveInDirection(Direction d)
{
	switch (d)
	{
		//When pressing a key to move
			//if Tman is facing in that direction
				//if there is no boulder blocking the way in that direction
					//move 1 block to that direction
				//else
					//stay in the same spot
			//else
				//set direction to match key pressed
	case right:
		if (getDirection() == right)
		{
			if (!getWorld()->isBoulder(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			else
				moveTo(getX(), getY());
		}
		else
			setDirection(right);
		break;

	case left:
		if (getDirection() == left)
		{
			if (!getWorld()->isBoulder(getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			else
				moveTo(getX(), getY());
		}
		else
			setDirection(left);
		break;
	case up:
		if (getDirection() == up)
		{
			if (!getWorld()->isBoulder(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			else
				moveTo(getX(), getY());
		}
		else
			setDirection(up);
		break;
	case down:
		if (getDirection() == down)
		{
			if (!getWorld()->isBoulder(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			else
				moveTo(getX(), getY());
		}
		else
			setDirection(down);
		break;
	default:
		return;
	}

}

void Tunnelman::shootWater()
{
	//if there is no boulder/earth in the specified direction
		//add new actor: Squirt at position +- 4 in the direction Tman is facing
	switch (getDirection())
	{
	case right:
		if (!getWorld()->isBoulder(getX() + 4, getY()) && !getWorld()->isEarth(getX() + 4, getY()))
			getWorld()->addActor(new Squirt(getWorld(), getX() + 4, getY(), getDirection()));
	case left:
		if (!getWorld()->isBoulder(getX() - 4, getY()) && !getWorld()->isEarth(getX() + 4, getY()))
			getWorld()->addActor(new Squirt(getWorld(), getX() - 4, getY(), getDirection()));
	case up:
		if (!getWorld()->isBoulder(getX(), getY() + 4) && !getWorld()->isEarth(getX() + 4, getY()))
			getWorld()->addActor(new Squirt(getWorld(), getX(), getY() + 4, getDirection()));
	case down:
		if (!getWorld()->isBoulder(getX() + 4, getY() - 4) && !getWorld()->isEarth(getX() + 4, getY()))
			getWorld()->addActor(new Squirt(getWorld(), getX(), getY() - 4, getDirection()));
	}
	getWorld()->playSound(SOUND_PLAYER_SQUIRT); //play sound for squirt
}

////////////////////////////////////////////////////////////////
/////// EARTH ////////////////////////////////////////


Earth::Earth(StudentWorld* world, int x, int y)
	:Actor(world, TID_EARTH, x, y, right, 0.25, 3) {}

void Earth::doSomething() {} //does not do anything


///////////////////////////////////////////////////////////////
//////// BOULDER ////////////////////////////////////////

Boulder::Boulder(StudentWorld* world, int x, int y)
	:Actor(world, TID_BOULDER, x, y, down, 1.0, 1)
{
	world->removeDirt(x, y); //FIXME:
	m_stable = true;
	m_falling = false;
	m_ticks = 0;
}

void Boulder::doSomething()
{
	//check if alive
	if (!isAlive())

		return;
	//if boulder is stable
	if (m_stable)
	{
		if (getWorld()->isAboveGround(getX(), getY() - 1))
			return;
		else
			//if there is no earth below
				//set stable status to false
			m_stable = false;
	}

	if (m_ticks == 30)
	{
		m_falling = true;
		getWorld()->playSound(SOUND_FALLING_ROCK);
	}
	m_ticks++;

	//if boulder is falling
	if (m_falling)
	{
		//if y-coord is 0, another boulder, is +1-y above ground
			//set status to dead
		if (getY() == 0 || getWorld()->isBoulder(getX(), getY() - 4, 0) || getWorld()->isAboveGround(getX(), getY() - 1))
			setDead();
		else
		{
			moveTo(getX(), getY() - 1); //move one step down
			annoy(); //annoy tman if in range
		}
	}
}

void Boulder::annoy()
{
	//check if tman is in range
		//if yes, decrease health
	if (getWorld()->tManInR(3, this))
		getWorld()->getPlayer()->isAnnoyed(100);

	Protester* p;
	//check if protester in range
		//if yes, then annoy protester
	p = getWorld()->protesterInR(3, this);

	if (p != nullptr)
		p->isAnnoyed(100);
}

//////////////////////////////////////////////////////////////
////////// SQUIRT //////////////////////////////

Squirt::Squirt(StudentWorld* world, int x, int y, Direction dir)
	:Actor(world, TID_WATER_SPURT, x, y, dir, 1.0, 1)
{
	m_move = 0;
}

void Squirt::doSomething()
{
	//check if alive
	if (!isAlive())
		return;

	//if in range with protesters or has travelled full dist
		//set status to dead
	if (annoyProtesters() || m_move == 4)
	{
		setDead();
		return;
	}

	//given its current-facing direction
	// 
	//if the target location is occupied by earth/boulder
		//set status to dead and return
	if (getDirection() == left)
	{
		if (getWorld()->isBoulder(getX() - 1, getY()) || getWorld()->isEarth(getX() - 1, getY()))
		{
			setDead();
			return;
		}
	}

	else if (getDirection() == right)
	{
		if (getWorld()->isBoulder(getX() + 1, getY()) || getWorld()->isEarth(getX() + 1, getY()))
		{
			setDead();
			return;
		}
	}

	else if (getDirection() == up)
	{
		if (getWorld()->isBoulder(getX(), getY() + 1) || getWorld()->isEarth(getX(), getY() + 1))
		{
			setDead();
			return;
		}
	}

	else if (getDirection() == down)
	{
		if (getWorld()->isBoulder(getX(), getY() - 1) || getWorld()->isEarth(getX(), getY() - 1))
		{
			setDead();
			return;
		}
	}

	//increase distance travelled
	m_move++; //change m_move to sth else //FIXME!!!!

}

bool Squirt::annoyProtesters()
{

	Protester* p;
	p = getWorld()->protesterInR(3, this);

	//if protester is not in range
		//return false
	if (p == nullptr)
		return false;
	//else
		//annoy and return true
	else
	{
		p->isAnnoyed(2);
		return true;
	}
}


////////////////////////////////////////////////////////////////
//////////// POWERUP /////////////////////////////////////////


PowerUp::PowerUp(StudentWorld* world, int imageID, int x, int y)
	:Actor(world, imageID, x, y, right, 1.0, 2) {}

void PowerUp::vanish(int ticks)
{
	if (m_ticks == ticks)
		setDead();
	else
		m_ticks++;
}

//////////////////////////////////////////////////////////////////\
///////////////// BARREL ////////////////////////////////////////

Barrel::Barrel(StudentWorld* world, int x, int y)
	:PowerUp(world, TID_BARREL, x, y)
{
	setVisible(false);
}

void Barrel::doSomething()
{
	//check if alive
	if (!isAlive())
		return;

	//if tman is in range and the barrel is not visible yet
		//set visible and return
	if (getWorld()->tManInR(4, this) && !isVisible())
	{
		setVisible(true);
		return;
	}

	//if tman is in range to activate barrel
		//set state to dead
		//decrease # of barrels in game
	if (getWorld()->tManInR(3, this))
	{
		setDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decBarrel();

		return;
	}
}

//////////////////////////////////////////////////////////////
//////////////GOLD /////////////////////////////////////////

Gold::Gold(StudentWorld* world, int x, int y, bool isVisible, bool drop)
	:PowerUp(world, TID_GOLD, x, y), isDropped(drop)
{
	setVisible(isVisible); //status depends on context

}

void Gold::doSomething()
{
	//check if alive
	if (!isAlive())
		return;

	//if not visible and tman is in range
		//set visible
	if (!isVisible() && getWorld()->tManInR(4, this))
	{

		setVisible(true);
		return;
	}
	//if it has not been dropped and tman is in range to claim
		//set dead and add gold
	if (!isDropped && getWorld()->tManInR(3, this))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->add(getID());
		getWorld()->increaseScore(10);
		return;
	}

	//if tman dropped the gold
	if (isDropped)
	{
		Protester* p;
		//if protester is in range
			//then set to dead and bribe protester
		p = getWorld()->protesterInR(3, this);

		if (p != nullptr)
		{
			setDead();
			p->getBribed();
		}
		//check if lifetime ticks have elapsed
		vanish(100);
	}
}

/////////////////////////////////////////////////////////////
/////////// GOODIE /////////////////////////////////////////


Goodie::Goodie(StudentWorld* world, int imageID, int x, int y)
	:PowerUp(world, imageID, x, y) {}

void Goodie::doSomething()
{
	//check if alive
	if (!isAlive())
		return;

	//check if tman is in radius
		//if yes, then claim
	if (getWorld()->tManInR(3, this))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->add(getID());
		getWorld()->increaseScore(75);
		return;
	}
	//check if lifetime ticks have elapsed
	vanish(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
}

/////////////////////////////////////////////////////////////////
///////////////// SONAR ////////////////////////////////////////

Sonar::Sonar(StudentWorld* world, int startX, int startY)
	:Goodie(world, TID_SONAR, startX, startY) {}

//////////////////////////////////////////////////////////////////
////////////////// WATER POOL //////////////////////////////////

WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
	:Goodie(world, TID_WATER_POOL, startX, startY) {}

/////////////////////////////////////////////////////////////////
////////////// PROTESTER ///////////////////////////////////////


Protester::Protester(StudentWorld* world, int imageID, int hp)
	:Person(world, imageID, 60, 60, left, hp)
{
	m_exit = false; //leave the oil field state
	m_ticksFromTurn = 200; //ticks from perpendicular turn
	m_yellTicks = 15;

	m_nTMove = rand() % 53 + 8;

	m_restTicks = max(0, 3 - (int)getWorld()->getLevel() / 4);
}

void Protester::doSomething()
{

	//check if alive
	if (!isAlive())
		return;
	//check if in rest state
		//decrease rest ticks and return
	if (m_restTicks > 0)
	{
		m_restTicks--;
		return;
	}

	else
	{
		//reset rest ticks
		m_restTicks = max(0, 3 - (int)getWorld()->getLevel() / 4);
		m_ticksFromTurn++; //update ticks from last turn
		m_yellTicks++;
	}

	//if in an "leave the oil field state"
	if (m_exit)
	{
		//if at exit point
		if (getX() == 60 && getY() == 60)
		{
			//set dead and decrease # of protesters
			setDead();
			getWorld()->decProtesters();
			return;
		}
		//move one step closer to exit

		getWorld()->findExit(this);
		return;

	}

	//check if in range of 4 from tman and facing tman
		//if p has not shouted
			//annoy tman and protester yell
			//set ticks from last yell to 0
	if (getWorld()->tManInR(4, this) && faceTMan())
	{
		if (m_yellTicks > 15)
		{
			getWorld()->getPlayer()->isAnnoyed(2);
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			m_yellTicks = 0;

			return;
		}
	}

	//if protester 
		//1.is in straight line to tman
		//2.is in range of 4 with tman
		//3. can move to tman without being blocked

	Direction d = directToTMan();
	if (d != none && pathToTMan(d) && (!getWorld()->tManInR(4, this)))
	{
		//change d to face tman
		setDirection(d);
		//take one step to tman
		moveInDirection(d);
		//set numSquares to 0
		m_nTMove = 0;
		return;
	}

	//if protester cannot directly see tman
	//decrement numSquares by 1
	m_nTMove--;

	if (m_nTMove <= 0)
	{
		//pick random new direction
		Direction d = none;
		d = randomDir();


		while (true)
		{
			//pick new direction
			d = randomDir();
			//if protester can move in that d
				//break
			if (getWorld()->moveInD(d, getX(), getY()))
				break;
		}

		setDirection(d);
		//pick new direction for numSquares
		m_nTMove = rand() % 53 + 8;
	}

	//if p has not made a perpendicular turn
	// && can turn and move one square
	else if (m_ticksFromTurn > 200 && intersection())
	{
		//determine viable direction
		viableTurn();
		//set turn ticks to 0
		m_ticksFromTurn = 0;
		//pick numSquares to move
		m_nTMove = rand() % 53 + 8;
	}

	//attempt to take one step in cur dir

	//if blocked
		//set numSquares to zero, get new direction at next tick
	moveInDirection(getDirection());

	if (!getWorld()->moveInD(getDirection(), getX(), getY()))
		m_nTMove = 0;



}

void Protester::viableTurn()
{
	//given a direction
		//if both perpendicular directions are viable
			//set randomly (random % 2)
		//else if direction (A) direction is not viable
			//set (B)
		//else
			//set (A)
	if (getDirection() == up)
	{
		if (getWorld()->moveInD(left, getX(), getY()) &&
			getWorld()->moveInD(right, getX(), getY()))
		{
			int k = rand() % 2;
			if (k == 0)
				setDirection(right);
			else
				setDirection(left);
		}

		else if (!getWorld()->moveInD(right, getX(), getY()))
			setDirection(left);

		else
			setDirection(right);
	}
	if (getDirection() == down)
	{
		if (getWorld()->moveInD(left, getX(), getY()) &&
			getWorld()->moveInD(right, getX(), getY()))
		{
			int k = rand() % 2;
			if (k == 0)
				setDirection(right);
			else
				setDirection(left);
		}

		else if (!getWorld()->moveInD(right, getX(), getY()))
			setDirection(left);

		else
			setDirection(right);
	}
	if (getDirection() == right)
	{
		if (getWorld()->moveInD(up, getX(), getY()) &&
			getWorld()->moveInD(down, getX(), getY()))
		{
			int k = rand() % 2;
			if (k == 0)
				setDirection(down);
			else
				setDirection(up);
		}

		else if (!getWorld()->moveInD(up, getX(), getY()))
			setDirection(down);

		else
			setDirection(up);
	}
	if (getDirection() == left)
	{
		if (getWorld()->moveInD(up, getX(), getY()) &&
			getWorld()->moveInD(down, getX(), getY()))
		{
			int k = rand() % 2;
			if (k == 0)
				setDirection(down);
			else
				setDirection(up);
		}

		else if (!getWorld()->moveInD(up, getX(), getY()))
			setDirection(down);

		else
			setDirection(up);
	}
}

GraphObject::Direction Protester::randomDir()
{
	//to get a random direction
		//get a random number % 4

	//each case corresponds to one direction
	int k = rand() % 4;
	switch (k)
	{
	case 0: return right;
	case 1: return left;
	case 2: return up;
	case 3: return down;

	default: return none;
	}
}


void Protester::isAnnoyed(int hp)
{
	//if protester is in leave state, then return
	if (m_exit == true)
		return;

	//otherwise

	//decrease hp
	damage(hp);
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);

	//protester stunned
		//place in resting state for N ticks
	m_restTicks = max(50, 100 - (int)getWorld()->getLevel() * 10);

	//if fully annoyed
	if (getHealth() <= 0)
	{
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_restTicks = 0;
		//set to leave state
		m_exit = true;

		//bonked by boulder
		if (hp == 100)
			getWorld()->increaseScore(500);
		//squirted
		else if (getID() == TID_PROTESTER)
			getWorld()->increaseScore(100);
		//FIXME: !!!
		else
			getWorld()->increaseScore(250);
	}
}

void Protester::getBribed()
{
	//Picks up Gold Nugget


	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	if (getID() == TID_PROTESTER)
	{
		//set to leave state
		getWorld()->increaseScore(25);
		m_exit = true;
	}
	else
	{
		//FIXME: !!!
		//for hardcore protester
		getWorld()->increaseScore(50);
		m_restTicks = max(50, 100 - (int)getWorld()->getLevel() * 10);
	}
}

void Protester::moveInDirection(Direction dir)
{
	//if protester faces in the given dir
		//if is at end of grid
			//switch to opposite direction
			// 
		//otherwise, take one step in that direction
	//else
		//set protester's dir to the given direction
	switch (dir)
	{
	case right:
		if (getDirection() == right)
		{
			if (getX() == 60)
			{
				setDirection(left);
			}
			moveTo(getX() + 1, getY());
		}
		else setDirection(right);
		break;
	case left:
		if (getDirection() == left)
		{
			if (getX() == 0)
				setDirection(right);
			moveTo(getX() - 1, getY());

		}
		else setDirection(left);
		break;
	case up:
		if (getDirection() == up)
		{
			if (getY() == 60)
				setDirection(down);
			moveTo(getX(), getY() + 1);

		}
		else setDirection(up);
		break;
	case down:
		if (getDirection() == down)
		{
			if (getY() == 0)
				setDirection(up);
			moveTo(getX(), getY() - 1);

		}
		else setDirection(up);
		break;

	default:
		return;
	}
}

bool Protester::pathToTMan(Direction dir)
{

	//Given a direction:
		//starting from the protster's position
			//moving until tman's position
			//if earth/boulder is not blocking the way in that direction
				//continue
			//else
				//path is blocked, so return false
		//return true if not blocked

	switch (dir)
	{
	case left:
		for (int i = getX(); i >= getWorld()->getPlayer()->getX(); i--)
		{
			if (!getWorld()->isEarth(i, getY()) && !getWorld()->isBoulder(i, getY()))
				continue;
			else
				return false;
		}
		return true;
		break;
	case right:
		for (int i = getX(); i <= getWorld()->getPlayer()->getX(); i++)
		{
			if (!getWorld()->isEarth(i, getY()) && !getWorld()->isBoulder(i, getY()))
				continue;
			else
				return false;
		}
		return true;
		break;

	case up:
		for (int i = getY(); i <= getWorld()->getPlayer()->getY(); i++)
		{
			if (!getWorld()->isEarth(getX(), i) && !getWorld()->isBoulder(getX(), i))
				continue;
			else
				return false;
		}
		return true;
		break;

	case down:
		for (int i = getY(); i >= getWorld()->getPlayer()->getY(); i--)
		{
			if (!getWorld()->isEarth(getX(), i) && !getWorld()->isBoulder(getX(), i))
				continue;
			else
				return false;
		}
		return true;
		break;
	default:
		return false;
	}
}

bool Protester::faceTMan()
{
	//Given a direction:
	//Determine if Protester is facing the Tunnelman

	switch (getDirection())
	{
		//left
	case left:
		if (!(getWorld()->getPlayer()->getX() > getX()))
			return true;
		//compare tx and px
		//if tx > px, then the protester faces in opposite direction
		//otherwise, protester faces tunnelman
		return false;
		//right
	case right:
		if (!(getWorld()->getPlayer()->getX() < getX()))
			return true;
		return false;
		//up
	case up:
		if (!(getWorld()->getPlayer()->getY() < getY()))
			return true;
		return false;
		//down
	case down:
		if (!(getWorld()->getPlayer()->getX() > getY()))
			return true;
		return false;
	default:
		return false;
	}
}

bool Protester::intersection()
{
	//given a direction
		//return if true if the protester can move in each of the
		//perpendicular directions from the given one

	if (getDirection() == left)
		return (getWorld()->moveInD(up, getX(), getY())) ||
		getWorld()->moveInD(down, getX(), getY());

	else if (getDirection() == right)
		return (getWorld()->moveInD(up, getX(), getY())) ||
		getWorld()->moveInD(down, getX(), getY());

	else if (getDirection() == up)
		return (getWorld()->moveInD(left, getX(), getY())) ||
		getWorld()->moveInD(right, getX(), getY());

	else
		return (getWorld()->moveInD(left, getX(), getY())) ||
		getWorld()->moveInD(right, getX(), getY());
}


//FIXME: Place directToTMan in Protester::doSomething() method

GraphObject::Direction Protester::directToTMan()
{
	//get tman's position
	int x = getWorld()->getPlayer()->getX();
	int y = getWorld()->getPlayer()->getY();


	//if at same position, return same direction
	if (getX() == x)
	{
		if (getY() == y)
			return getDirection();
	}

	//if at same y-coord
		//if at right
			//set dir to right
		//else
			//set dir to left
	if (getY() == y)
	{
		if (getX() < x)
			return right;

		else if (getX() > x)
			return left;
	}

	//if at same x-coord
		//if up
			//set dir to up
		//else
			//set dir to down

	if (getX() == x)
	{
		if (getY() < y)
			return up;

		else if (getY() > y)
			return down;
	}

	return none;
}

/////////////////////////////////////////////////////////////
///////////// REGULAR PROTESTER //////////////////////////

RegularProtester::RegularProtester(StudentWorld* world)
	:Protester(world, TID_PROTESTER, 5) {}

//////////////////////////////////////////////////////////
////////////// HARDCORE PROTESTER //////////////////////////

HardcoreProtester::HardcoreProtester(StudentWorld* world)
	:Protester(world, TID_HARD_CORE_PROTESTER, 20) {}


// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
