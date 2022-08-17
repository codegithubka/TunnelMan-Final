#ifndef STUDENTWORLD_H
#define STUDENTWORLD_H

#include "Gameworld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <vector>
#include <queue>

#include <iomanip>

class GraphObject;



class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);

	~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	void displayText(int s, int lvl, int lives, int hlth, int sq, int g, int snr, int b);


	Tunnelman* getPlayer();
	Protester* protesterInR(int r, Actor* a);

	void addGoodie();
	void addActor(Actor* a);
	void decBarrel();
	void addBGB(int& x, int& y);
	void isClose(int x, int y, int r);
	void addProtesters();
	void decProtesters();
	void findExit(Protester* p);

	bool removeDirt(int x, int y);
	bool isWithinR(int x1, int y1, int x2, int y2, int r);
	bool actorWithinR(int x, int y, int r);
	bool isAboveGround(int x, int y);
	bool isEarth(int x, int y);
	bool isBoulder(int x, int y, int r = 3);
	bool moveInD(GraphObject::Direction dir, int x, int y);
	bool tManInR(int r, Actor* a);


private:
	Tunnelman* m_t;

	Earth* m_earth[64][64];

	int m_barrels;
	bool m_firstTick;
	int m_ticksElapsed;
	int m_protesters;

	std::vector<Actor*> m_actors;

	int m_field[64][64];


	struct Maze
	{
		int x, y;
		Maze(int a, int b) : x(a), y(b) {}
	};

};

#endif //STUDENTWORLD_H