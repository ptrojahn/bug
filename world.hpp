#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "geometry.hpp"

class Entity;

class World {
public:
	World();
	void addEntity(Entity* ent);
	void removeEntity(Entity* ent);
	std::optional<Entity*> raycast(Ray ray);
	void worldVisual();
	void exit();
	void worldStep();

private:
	std::vector<std::unique_ptr<Entity*>> entities;
	bool run;
};

#endif
