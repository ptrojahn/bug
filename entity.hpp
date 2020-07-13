#ifndef ENTITY_H
#define ENTITY_H

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

#include "world.hpp"

class Entity : public sf::Drawable {
public:
	virtual void initPhysics(b2World* world) = 0;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	virtual void update(World* world);

	b2Body* body;
	sf::Color color;

protected:
	Entity();
};

#endif

