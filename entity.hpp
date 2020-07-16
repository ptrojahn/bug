#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>

#include "world.hpp"
#include "geometry.hpp"

class Entity : public sf::Drawable {
public:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	virtual void update(World* world);

	Shape* shape;
	sf::Color color;

protected:
	Entity();
};

#endif

