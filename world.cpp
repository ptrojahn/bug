#include "world.hpp"

#include "entity.hpp"

World::World() : world(b2Vec2(0.0f, 0.f)) {
}

void World::addEntity(Entity* ent) {
	ent->body = world.CreateBody(&ent->bodyDef);
	ent->body->CreateFixture(ent->shape, 0.0);
	entities.push_back(ent);
}

void World::removeEntity(Entity* ent) {
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i] == ent) {
			entities.erase(entities.begin() + i);
		}
	}
}

void World::exit() {
	run = false;
}

void World::simVisual() {
	sf::RenderWindow window(sf::VideoMode(400, 400), "Bug");
	window.setFramerateLimit(60);

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
		world.Step(1.f / 60.f, 6, 2);
		for (auto ent : entities) {
			ent->update(this);
		}
		for (auto ent : entities) {
			window.draw(*ent);
		}
		window.display();
	}
}

void World::sim() {
	run = true;
	while (run) {
		world.Step(1.f / 60.f, 6, 2);
		for (auto ent : entities) {
			ent->update(this);
		}
	}
}
