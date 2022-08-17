#include "StudentWorld.h"
#include <string>
#include <sstream>
#include <random>
#include <iostream>
#include <cmath>

using namespace std;

GameWorld* createStudentWorld(std::string assetDir)
{
	return new StudentWorld(assetDir);
}

//TODO:

StudentWorld::StudentWorld(std::string assetDir)
	:GameWorld(assetDir)
{
	m_protesters = 0;
	m_t = nullptr;
	m_barrels = 0;
	m_firstTick = true;
	m_ticksElapsed = 0;
}


StudentWorld::~StudentWorld()
{
	cleanUp();
}

Tunnelman* StudentWorld::getPlayer()
{
	return m_t;
}

void StudentWorld::displayText(int s, int lvl, int lives, int hlth, int sq, int g, int snr, int b)
{


	stringstream ss;

	ss.fill('0');
	ss << "Scr: " << setw(8) << s;

	ss.fill(' ');
	ss << " Lvl: " << setw(2) << lvl << " Lives: " << setw(1) << lives
		<< " Hlth: " << setw(3) << hlth * 10 << '%' << " Wtr: " << setw(2)
		<< sq << " Gld: " << setw(2) << g << " Sonar: " << setw(2) << snr
		<< " Oil Left: " << setw(2) << b;

	string text = ss.str();
	setGameStatText(text);
}


void StudentWorld::addActor(Actor* a)
{
	//push back actor to vector 
	m_actors.push_back(a);
}

void StudentWorld::addGoodie()
{

	//add goodies

	int x, y;
	int G = (int)getLevel() + 300;

	if (int(rand() % G) + 1 == 1) {
		if (int(rand() % 5) + 1 == 1) {
			addActor(new Sonar(this, 0, 60)); //adds sonar
		}
		else {
			do {
				x = rand() % 60 + 1;
				y = rand() % 60 + 1;
			} while (isEarth(x, y));
			addActor(new WaterPool(this, x, y)); //adds water pool
		}
	}
}

bool StudentWorld::removeDirt(int x, int y)
{
	bool empty = false;

	//Remove dirt at (x, y)
	for (int i = x; i < x + 4; i++)
	{
		for (int j = y; j < y + 4; j++)
		{
			if (m_earth[i][j] != nullptr) //if != to NULL
			{
				delete m_earth[i][j]; //delete and set to nullptr
				m_earth[i][j] = nullptr;
				empty = true;
			}
		}
	}

	return empty; //return true, if now empty
}

void StudentWorld::decBarrel()
{
	//decrement barrels
	m_barrels--;
}

bool StudentWorld::isWithinR(int x1, int y1, int x2, int y2, int r)
{
	//Determine if two sets of coords are within a given radius
	if (r < sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0)))
	{
		return false;
	}
	return true; //true if in given r
}

bool StudentWorld::actorWithinR(int x, int y, int r)
{
	//Determine if actor is within specified radius
	vector<Actor*>::iterator it;
	//Loop though the vector of actors
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		//if an actor is within specified r, then return true
		if (isWithinR(x, y, (*it)->getX(), (*it)->getY(), r))
			return true;
	}
	return false;
}

void StudentWorld::addBGB(int& x, int& y)
{
	//FIXME: Change for loop maybe?

	//add boulders, barrels, gold
	int i, j;
	int ax, ay;
	int dx, dy;

	bool cont = false;

	//makes sure that actors do not appear on top of each other
	while (!cont)
	{
		x = rand() % 59 + 1;
		y = rand() % 59 + 1;
		if (m_actors.size() == 0)
			cont = true;

		//Loop though vector of actors
		for (size_t i = 0; i < m_actors.size(); i++)
		{
			//get actor coords
			ax = m_actors[i]->getX();
			ay = m_actors[i]->getY();

			dx = x - ax;
			dy = y - ay;

			//distance between random coords (x, y) and actor coords
			int r = sqrt(pow(dx, 2.0) + pow(dy, 2.0));

			//if radius is less than 4, then re-generate coords and repeat
			if (r <= 4.0)
			{
				cont = false;
				break;
			}
			cont = true;

		}

	}

}

void StudentWorld::addProtesters()
{
	//Add protesters

	int T = max(25, 200 - (int)getLevel()); //T number of ticks have to pass
	int P = fmin(15, 2 + getLevel() * 1.5); //Target Number of Protesters

	//if is first tick or elapsed ticks > T and # of protestors < Target
	if (m_firstTick == true || (m_ticksElapsed > T && m_protesters < P))
	{
		//Get probability of hardcore
		if (rand() % 100 + 1 <= min(90, (int)getLevel() * 10 + 30))
			addActor(new HardcoreProtester(this)); //add HP
		else
			addActor(new RegularProtester(this)); //add RP

		m_ticksElapsed = 0; //ticks after adding protester
		m_protesters++; //increment # of protesters
		m_firstTick = false;
	}
	m_ticksElapsed++;
}

bool StudentWorld::isAboveGround(int x, int y)
{
	//Determine the space at x: x + 4 is empty
	for (int i = x; i < x + 4; i++)
	{
		if (m_earth[i][y] != nullptr)
			return true;
	}
	return false;
}

bool StudentWorld::isEarth(int x, int y)
{
	//For (x, y) determine if is earth
	for (int i = x; i < x + 4; i++)
	{
		for (int j = y; j < y + 4; j++)
		{
			if (m_earth[i][j] != nullptr)
				return true;
		}
	}
	return false;
}

bool StudentWorld::isBoulder(int x, int y, int r)
{
	//Determine if is Boulder
	vector<Actor*>::iterator it;
	//Loop though vector of actors 
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		//If ID = Boulder ID and is within radius r
			//return true
		if ((*it)->getID() == TID_BOULDER && isWithinR(x, y, (*it)->getX(), (*it)->getY(), r))
			return true;
	}

	return false; //not a boulder
}

bool StudentWorld::tManInR(int r, Actor* a)
{
	//Determine if Tunnelman is within specified radius of Actor a
	if (isWithinR(a->getX(), a->getY(), m_t->getX(), m_t->getY(), r))
		return true;
	return false;
}

void StudentWorld::isClose(int x, int y, int r)
{
	vector<Actor*>::iterator it;

	//Iterate through vector of actors
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		//If actor is gold or barrel
		if ((*it)->getID() == TID_GOLD || (*it)->getID() == TID_BARREL)
		{
			//get actor coords
			int i = (*it)->getX();
			int j = (*it)->getY();

			//if actor is within the specified radius from input coords
				//then set actor to visible (gold & barrel of oil)
			if (r >= sqrt(pow(x - i, 2.0) + pow(y - j, 2.0)))
				(*it)->setVisible(true);
		}
	}
}

void StudentWorld::decProtesters()
{
	//decrement # of protesters
	m_protesters--;
}

bool StudentWorld::moveInD(GraphObject::Direction dir, int x, int y)
{
	//Determine if step in specified direction is possible

	//For specified direction:
		//If the space one step in that direction has no boulder or earth
			//Then return true
	switch (dir)
	{
	case GraphObject::left:
		if (!isEarth(x - 1, y) && !isBoulder(x - 1, y) && x != 0)
			return true;
		break;
	case GraphObject::right:
		if (!isEarth(x + 1, y) && !isBoulder(x + 1, y) && x != 0)
			return true;
		break;
	case GraphObject::up:
		if (!isEarth(x, y + 1) && !isBoulder(x, y + 1) && y != 60)
			return true;
		break;
	case GraphObject::down:
		if (!isEarth(x, y - 1) && !isBoulder(x, y - 1) && y != 0)
			return true;
		break;
	default:
		return false;
	}
	return false; //Move not possible
}

Protester* StudentWorld::protesterInR(int r, Actor* a)
{
	//Determine if Protester if in specified radius from Actor

	//Loop though vector of actors
	vector<Actor*>::iterator it;

	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		//If ID matches that of either protester && is within radius
		if (((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER) && isWithinR(a->getX(), a->getY(), (*it)->getX(), (*it)->getY(), r))
				return dynamic_cast<Protester*>(*it); //Return p
	}

	return nullptr;

}

int StudentWorld::init()
{

	m_barrels = 0;
	m_firstTick = true;
	m_ticksElapsed = 0;
	m_protesters = 0;

	//Add Earth
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			if (i < 30 || i > 33 || j < 4)
				m_earth[i][j] = new Earth(this, i, j);
		}
	}

	//Add Tunnelman
	m_t = new Tunnelman(this);

	int B = min(int(getLevel() / 2 + 2), 9);

	int G = max(int(5 - getLevel() / 2), 2);

	int L = min(int(2 + getLevel()), 21);

	m_barrels = L; //# of barrels

	//Add Gold, Boulders && Barrels
	for (int i = 0; i < B; i++)
	{
		int x, y;
		addBGB(x, y);

		for (int j = x; j < x + 4; j++)
		{
			for (int k = y; k < y + 4; k++)
			{
				if (m_earth[j][k] != nullptr)
				{
					delete m_earth[j][k];
					m_earth[j][k] = nullptr;
				}

			}
		}
		Boulder* boulder = new Boulder(this, x, y);
		m_actors.push_back(boulder);
	}

	for (int i = 0; i < L; i++)
	{
		int x, y;
		addBGB(x, y);

		Barrel* barrel = new Barrel(this, x, y);
		m_actors.push_back(barrel);
	}

	for (int i = 0; i < G; i++)
	{
		int x, y;
		addBGB(x, y);

		Gold* gold = new Gold(this, x, y, false, false);
		m_actors.push_back(gold);
	}


	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	//Update Text

	displayText(getScore(), getLevel(), getLives(), m_t->getHealth(),
		m_t->getAmmo(), m_t->getGold(), m_t->getSonar(), m_barrels);

	//Ask all actors to do something
	vector<Actor*>::iterator it;

	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->isAlive())
			(*it)->doSomething();

		if (!m_t->isAlive())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}

		if (m_barrels == 0)
			return GWSTATUS_FINISHED_LEVEL;

	}

	//Ask Tunnelman to do something
	m_t->doSomething();

	//Add goodies
	addGoodie();

	//Add Protesters
	addProtesters();

	//Remove all dead actors from game
	vector<Actor*>::iterator it1;

	for (it1 = m_actors.begin(); it1 != m_actors.end();)
	{
		if (!(*it1)->isAlive())
		{
			delete (*it1);
			it1 = m_actors.erase(it1);
		}
		else
			it1++;
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			delete m_earth[i][j];
		}
	}

	vector<Actor*>::iterator it;

	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		delete (*it);
	}
	m_actors.clear();
	delete m_t;
}


void StudentWorld::findExit(Protester* p)
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j)
			m_field[i][j] = 0;
	}

	int px = p->getX();
	int py = p->getY();

	queue <Maze> Q;
	Q.push(Maze(60, 60));

	m_field[60][60] = 1;

	while (!Q.empty())
	{
		Maze g = Q.front();
		Q.pop();

		int x = g.x;
		int y = g.y;

		//LEFT

		if (moveInD(GraphObject::left, x, y) && m_field[x - 1][y] == 0)
		{
			Q.push(Maze(x - 1, y));
			m_field[x - 1][y] = m_field[x][y] + 1;
		}

		//RIGHT

		if (moveInD(GraphObject::right, x, y) && m_field[x + 1][y] == 0)
		{
			Q.push(Maze(x + 1, y));
			m_field[x + 1][y] = m_field[x][y] + 1;
		}

		//UP

		if (moveInD(GraphObject::up, x, y) && m_field[x][y + 1] == 0)
		{
			Q.push(Maze(x, y + 1));
			m_field[x][y + 1] = m_field[x][y] + 1;
		}

		//DOWN

		if (moveInD(GraphObject::down, x, y) && m_field[x][y - 1] == 0)
		{
			Q.push(Maze(x, y - 1));
			m_field[x][y - 1] = m_field[x][y] + 1;
		}
	}

	if (m_field[px - 1][py] < m_field[px][py] && moveInD(GraphObject::left, px, py))
		p->moveInDirection(GraphObject::left);

	if (m_field[px + 1][py] < m_field[px][py] && moveInD(GraphObject::right, px, py))
		p->moveInDirection(GraphObject::right);

	if (m_field[px][py - 1] < m_field[px][py] && moveInD(GraphObject::down, px, py))
		p->moveInDirection(GraphObject::down);

	if (m_field[px][py + 1] < m_field[px][py] && moveInD(GraphObject::up, px, py))
		p->moveInDirection(GraphObject::up);

	return;

}