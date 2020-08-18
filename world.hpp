#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "geometry.hpp"

class Entity;

class World {
public:
	World();
	World(World &world);
	void addEntity(Entity* ent);
	void removeEntity(Entity* ent);
	std::optional<Entity*> raycast(Ray ray);
	void visual(std::function<bool(World&)> update);
	void step();
	template<typename T> std::vector<T*> getAll() {
		std::vector<T*> ents;

		for (std::unique_ptr<Entity*> &entity : entities) {
			T* e = dynamic_cast<T*>(*entity);
			if (e != nullptr) {
				ents.push_back(e);
			}
		}
		return ents;
	}

	std::vector<std::unique_ptr<Entity*>> entities;
};

#endif
