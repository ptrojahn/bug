#include "bug.hpp"

#include <cmath>
#include <iostream>


const int FOV = 90;
const int EYES = 64;

Bug::Bug(sf::Vector2i pos, float rotation) {
	position = pos;
	this->rotation = rotation;
	color = sf::Color::Cyan;
	bug.setRadius(20.f);
	bug.setFillColor(sf::Color::Cyan);
	eye.setRadius(6.f);
	eye.setFillColor(sf::Color::Blue);

	shape = new Circle(sf::Vector2i(pos.x, pos.y), 20);
	bug.setPosition(position.x - 20, position.y - 20);
	eye.setPosition(sf::Vector2f(position - sf::Vector2i(6, 6) + rotate(sf::Vector2i(0, -25), rotation)));
}

void Bug::update(World* world) {
	sf::Color image[EYES];
	for (int i = 0; i < EYES; i++) {
		float angle = -(FOV / 2.f) + FOV / (float)(EYES - 1) * (float)i + rotation;
		Ray ray = Ray(sf::Vector2i(eye.getPosition() + sf::Vector2f(6.f, 6.f)), rotate(sf::Vector2i(0, -256), angle));
		std::optional<Entity*> ent = world->raycast(ray);
		if (ent.has_value()) {
			image[i] = ent.value()->color;
		}
	}

	/* DEEP REINFORCEMENT MAGIC */

	bool actionLeft = false;
	bool actionRight = false;
	bool actionForward = false;

	if (actionForward) {
		position += rotate(sf::Vector2i(0, -10), rotation);
	}
	if (actionLeft) {
		rotation -= 4;
	}
	if (actionRight) {
		rotation += 4;
	}

	dynamic_cast<Circle*>(shape)->pos = position;
	bug.setPosition(position.x - 20, position.y - 20);
	eye.setPosition(sf::Vector2f(position - sf::Vector2i(6, 6) + rotate(sf::Vector2i(0, -25), rotation)));
}

void Bug::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(bug);
	target.draw(eye);

	for (int i = 0; i < EYES; i++) {
		float angle = -(FOV / 2.f) + FOV / (float)(EYES - 1) * (float)i + rotation;
		sf::Vertex line[] =
		{
			sf::Vertex(eye.getPosition() + sf::Vector2f(6.f, 6.f)),
			sf::Vertex(sf::Vector2f(sf::Vector2i(eye.getPosition()) + sf::Vector2i(6, 6) + rotate(sf::Vector2i(0, -256), angle)))
		};
		target.draw(line, 2, sf::Lines);
	}
}
