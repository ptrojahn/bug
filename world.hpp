#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <Box2D/Box2D.h>

class Entity;

class World {
public:
	World();
	void addEntity(Entity* ent);
	void removeEntity(Entity* ent);
	void simVisual();
	void sim();
	void exit();

	b2World world;

private:
	std::vector<Entity*> entities;
	bool run;
};

#endif
