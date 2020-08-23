#include "bug.hpp"

#include <cmath>
#include <iostream>


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

	actionFunction = [](std::vector<sf::Color>){return Direction(false, false, false);};
}

void Bug::update(World* world) {
	std::vector<sf::Color> render(BUG_RESOLUTION);
	for (int i = 0; i < BUG_RESOLUTION; i++) {
		float angle = -(BUG_FOV / 2.f) + BUG_FOV / (float)(BUG_RESOLUTION - 1) * (float)i + rotation;
		Ray ray = Ray(sf::Vector2i(eye.getPosition() + sf::Vector2f(6.f, 6.f)), rotate(sf::Vector2i(0, -256), angle));
		std::optional<Entity*> ent = world->raycast(ray);
		if (ent.has_value()) {
			render[i] = ent.value()->color;
		}
	}

	Direction dir = actionFunction(render);
	std::cout << dir.forward << " " << dir.left << " " << dir.right << std::endl;;
	if (dir.forward) {
		position += rotate(sf::Vector2i(0, -10), rotation);
	}
	if (dir.left) {
		rotation -= 4;
	}
	if (dir.right) {
		rotation += 4;
	}

	dynamic_cast<Circle*>(shape)->pos = position;
	bug.setPosition(position.x - 20, position.y - 20);
	eye.setPosition(sf::Vector2f(position - sf::Vector2i(6, 6) + rotate(sf::Vector2i(0, -25), rotation)));
}

Entity *Bug::clone() {
	return new Bug(position, rotation);
}

void Bug::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(bug);
	target.draw(eye);

	for (int i = 0; i < BUG_RESOLUTION; i++) {
		float angle = -(BUG_FOV / 2.f) + BUG_FOV / (float)(BUG_RESOLUTION - 1) * (float)i + rotation;
		sf::Vertex line[] =
		{
			sf::Vertex(eye.getPosition() + sf::Vector2f(6.f, 6.f)),
			sf::Vertex(sf::Vector2f(sf::Vector2i(eye.getPosition()) + sf::Vector2i(6, 6) + rotate(sf::Vector2i(0, -256), angle)))
		};
		target.draw(line, 2, sf::Lines);
	}
}
