#include "world.hpp"

#include "entity.hpp"

#include <iostream>

World::World() {
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

void World::exit() {
	run = false;
}

void World::worldVisual() {
	sf::RenderWindow window(sf::VideoMode(600, 600), "Bug");
	window.setFramerateLimit(4);

	run = true;
	while (window.isOpen() && run)
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

void World::worldStep() {
	for (auto& ent : entities) {
		(*ent)->update(this);
	}
}
