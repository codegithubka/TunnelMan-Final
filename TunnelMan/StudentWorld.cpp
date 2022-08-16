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

void StudentWorld::updateText()
{
	string txt = displayText(getScore(), getLevel(), getLives(), m_t->getHealth(),
		m_t->getAmmo(), m_t->getGold(), m_t->getSonar(), m_barrels);

	setGameStatText(txt);
}
//
string StudentWorld::displayText(int s, int lvl, int lives, int hlth, int sq, int g, int snr, int b)
{
	stringstream ss;

	ss.fill('0');
	ss << "Scr: " << setw(8) << s;

	ss.fill(' ');
	ss << " Lvl: " << setw(2) << lvl << " Lives: " << setw(1) << lives
		<< " Hlth: " << setw(3) << hlth * 10 << '%' << " Wtr: " << setw(2)
		<< sq << " Gld: " << setw(2) << g << " Sonar: " << setw(2) << snr
		<< " Oil Left: " << setw(2) << b;
	return ss.str();
}


void StudentWorld::addActor(Actor* a)
{
	m_actors.push_back(a);
}

void StudentWorld::addGoodie()
{

	int x, y;
	int G = (int)getLevel() + 300;

	if (int(rand() % G) + 1 == 1) {
		if (int(rand() % 5) + 1 == 1) {
			addActor(new Sonar(this, 0, 60));
		}
		else {
			do {
				x = rand() % 60 + 1;
				y = rand() % 60 + 1;
			} while (isEarth(x, y));
			addActor(new WaterPool(this, x, y));
		}
	}
}

bool StudentWorld::digEarth(int x, int y)
{
	bool empty = false;

	for (int i = x; i < x + 4; i++)
	{
		for (int j = y; j < y + 4; j++)
		{
			if (m_earth[i][j] != nullptr)
			{
				delete m_earth[i][j];
				m_earth[i][j] = nullptr;
				empty = true;
			}
		}
	}

	return empty;
}

void StudentWorld::decBarrel()
{
	m_barrels--;
}

bool StudentWorld::isWithinR(int x1, int y1, int x2, int y2, int r)
{
	if (r >= sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0)))
	{
		return true;
	}
	return false;
}

bool StudentWorld::actorWithinR(int x, int y, int r)
{
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if (isWithinR(x, y, (*it)->getX(), (*it)->getY(), r))
			return true;
	}
	return false;
}

void StudentWorld::addBGB(int& x, int& y)
{
	int i, j;
	int ax, ay;
	int dx, dy;

	bool cont = false;

	while (!cont)
	{
		x = rand() % 59 + 1;
		y = rand() % 59 + 1;
		if (m_actors.size() == 0)
			cont = true;

		for (size_t i = 0; i < m_actors.size(); i++)
		{
			ax = m_actors[i]->getX();
			ay = m_actors[i]->getY();

			dx = x - ax;
			dy = y - ay;

			int r = sqrt(pow(dx, 2.0) + pow(dy, 2.0));

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
	int a_t = max(25, 200 - (int)getLevel());
	int num = fmin(15, 2 + getLevel() * 1.5);

	if (m_firstTick == true || (m_ticksElapsed > a_t && m_protesters < num))
	{
		if (rand() % 100 + 1 > min(90, (int)getLevel() * 10 + 30))
			addActor(new RegularProtester(this));
		else
			addActor(new HardcoreProtester(this));

		m_ticksElapsed = 0;
		m_protesters++;
		m_firstTick = false;
	}
	m_ticksElapsed++;
}

bool StudentWorld::isAboveGround(int x, int y)
{
	for (int i = x; i < x + 4; i++)
	{
		if (m_earth[i][y] != nullptr)
			return true;
	}
	return false;
}

bool StudentWorld::isEarth(int x, int y)
{
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
	vector<Actor*>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->getID() == TID_BOULDER && isWithinR(x, y, (*it)->getX(), (*it)->getY(), r))
			return true;
	}

	return false;
}

bool StudentWorld::tManInR(int r, Actor* a)
{
	return isWithinR(a->getX(), a->getY(), m_t->getX(), m_t->getY(), r);
}

void StudentWorld::isClose(int x, int y, int r)
{
	int i, j;
	vector<Actor*>::iterator it;

	for (it = m_actors.begin(); it != m_actors.end(); i++)
	{
		if ((*it)->getID() == TID_GOLD || (*it)->getID() == TID_BARREL)
		{
			i = (*it)->getX();
			j = (*it)->getY();

			if (r >= sqrt(pow(x - i, 2.0) + pow(y - j, 2.0)))
				(*it)->setVisible(true);
		}
	}
}

void StudentWorld::decProtesters()
{
	m_protesters--;
}

bool StudentWorld::moveInD(GraphObject::Direction dir, int x, int y)
{
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
	return false;
}

Protester* StudentWorld::protesterInR(int r, Actor* a)
{
	vector<Actor*>::iterator it;

	for (it = m_actors.begin(); it != m_actors.end(); it++)
	{
		if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER)
			if (isWithinR(a->getX(), a->getY(), (*it)->getX(), (*it)->getY(), r))
				return dynamic_cast<Protester*>(*it);
	}

	return nullptr;

}

int StudentWorld::init()
{

	m_barrels = 0;
	m_firstTick = true;
	m_ticksElapsed = 0;
	m_protesters = 0;

	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 60; j++)
		{
			if (i < 30 || i > 33 || j < 4)
				m_earth[i][j] = new Earth(this, i, j);
		}
	}

	m_t = new Tunnelman(this);

	int B = min(int(getLevel() / 2 + 2), 9);

	int G = max(int(5 - getLevel() / 2), 2);

	int L = min(int(2 + getLevel()), 21);

	m_barrels = L;

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
	updateText();

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

	m_t->doSomething();

	addGoodie();

	addProtesters();

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

	queue <Grid> Q;
	Q.push(Grid(60, 60));

	m_field[60][60] = 1;

	while (!Q.empty())
	{
		Grid g = Q.front();
		Q.pop();

		int x = g.x;
		int y = g.y;

		//LEFT

		if (moveInD(GraphObject::left, x, y) && m_field[x - 1][y] == 0)
		{
			Q.push(Grid(x - 1, y));
			m_field[x - 1][y] = m_field[x][y] + 1;
		}

		//RIGHT

		if (moveInD(GraphObject::right, x, y) && m_field[x + 1][y] == 0)
		{
			Q.push(Grid(x + 1, y));
			m_field[x + 1][y] = m_field[x][y] + 1;
		}

		//UP

		if (moveInD(GraphObject::up, x, y) && m_field[x][y + 1] == 0)
		{
			Q.push(Grid(x, y + 1));
			m_field[x][y + 1] = m_field[x][y] + 1;
		}

		//DOWN

		if (moveInD(GraphObject::down, x, y) && m_field[x][y - 1] == 0)
		{
			Q.push(Grid(x, y - 1));
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