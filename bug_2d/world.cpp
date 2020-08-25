#include "world.hpp"

#include "entity.hpp"

#include <iostream>

World::World() {

}

World::World(World &world) {
	entities.reserve(world.entities.size());
	for (std::unique_ptr<Entity*> &ent : world.entities) {
		entities.push_back(std::make_unique<Entity*>((*ent)->clone())); // TODO How deep do we need to go?
	}
}

void World::addEntity(Entity* ent) {
	entities.push_back(std::make_unique<Entity*>(ent));
}

void World::removeEntity(Entity* ent) {
	for (int i = 0; i < entities.size(); i++) {
		if (*entities[i] == ent) {
			entities.erase(entities.begin() + i);
		}
	}
}

std::optional<Entity*> World::raycast(Ray ray) {
	std::optional<std::tuple<Entity*, float>> result = std::nullopt;
	for (auto& ent : entities) {
		if (std::optional<float> i = ray.intersection((*ent)->shape)) {
			if (result.has_value() && std::get<1>(result.value()) > i.value()) {
			} else {
				result = std::make_tuple(*ent, i.value());
			}
		}
	}
	return result.has_value() ? std::optional<Entity*>(std::get<0>(result.value())) : std::nullopt;
}

void World::visual(std::function<bool(World&)> update) {
	sf::RenderWindow window(sf::VideoMode(800, 800), "Bug");
	window.setFramerateLimit(4);

	while (window.isOpen() && update(*this))
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		for (auto& ent : entities) {
			(*ent)->update(this);
		}
		for (auto& ent : entities) {
			window.draw(**ent);
		}
		window.display();
	}
}

void World::step() {
	for (auto& ent : entities) {
		(*ent)->update(this);
	}
}
