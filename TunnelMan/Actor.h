#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm>

class StudentWorld;

class Actor : public GraphObject
{
private:
	bool m_alive;
	StudentWorld* m_world;
public:
	Actor(StudentWorld* world, int imageID, int x, int y, Direction dir = right, double size = 1.0, unsigned int depth = 0);
	virtual ~Actor();
	virtual void doSomething() = 0; //pure virtual
	StudentWorld* getWorld();
	bool isAlive(); //check if alive
	void setDead(); //set status to dead
	virtual void moveTo(int x, int y);
	virtual void isAnnoyed(int hp) {} //Actor is annoyed, calls damage
};

class Person : public Actor
{
public:
	Person(StudentWorld* world, int imageID, int x, int y, Direction dir, int hp);
	int getHealth() const;
	void damage(int hp); //decrease hp
	virtual void moveInDirection(Direction d) = 0; //pure virtual
	virtual void isAnnoyed(int h) = 0;
private:
	int m_hp;
};

class Tunnelman :public Person
{
public:
	Tunnelman(StudentWorld* world);
	void add(int ID);

	int getGold() const { return m_gold; } //get gold
	int getAmmo() const { return m_ammo; } //get ammo
	int getSonar() const { return m_sonar; } //get sonar

	virtual void doSomething();
	virtual void isAnnoyed(int h);
	virtual void moveInDirection(Direction d);
	void shootWater(); //shoot water when asked
private:
	int m_ammo;
	int m_sonar;
	int m_gold;
};


class Earth : public Actor
{
public:
	Earth(StudentWorld* world, int x, int y);
	virtual void doSomething();
};

class Boulder : public Actor
{
public:
	Boulder(StudentWorld* world, int x, int y);
	virtual void doSomething();
	void annoy();
private:
	int m_ticks;
	bool m_stable;
	bool m_falling;
};


class Squirt : public Actor
{
public:
	Squirt(StudentWorld* world, int x, int y, Direction dir);
	virtual void doSomething();
	bool annoyProtesters();
private:
	int m_move;
};


class PowerUp : public Actor
{
public:
	PowerUp(StudentWorld* world, int imageID, int x, int y);
	virtual void doSomething() {}
	void vanish(int ticks); //deletes item in #ticks indicated
private:
	int m_ticks;
};

class Barrel : public PowerUp
{
public:
	Barrel(StudentWorld* world, int x, int y);
	virtual void doSomething();
};

class Gold : public PowerUp
{
public:
	Gold(StudentWorld* world, int x, int y, bool isVisible, bool drop);
	virtual void doSomething();
private:
	bool isDropped; //mark if gold was dropped by tman
};

class Goodie : public PowerUp
{
public:
	Goodie(StudentWorld* world, int imageID, int x, int y);
	virtual void doSomething();
};

class Sonar : public Goodie
{
public:
	Sonar(StudentWorld* world, int x, int y);
};

class WaterPool : public Goodie
{
public:
	WaterPool(StudentWorld* world, int x, int y);
};


class Protester : public Person
{
public:
	Protester(StudentWorld* world, int imageID, int hp);

	virtual void doSomething();
	virtual void getBribed();
	virtual void moveInDirection(Direction dir);
	virtual void isAnnoyed(int h);
	void viableTurn();

	bool faceTMan();
	bool pathToTMan(Direction dir);
	bool intersection();

	Direction directToTMan();
	Direction randomDir();

private:
	int m_restTicks;
	int m_ticksFromTurn;
	int m_yellTicks;
	int m_nTMove;

	bool m_exit;

};

class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* world);
};


class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* world);
};


#endif //ACTOR_H