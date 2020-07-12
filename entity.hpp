#ifndef ENTITY_H
#define ENTITY_H

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

#include "world.hpp"

class Entity : public sf::Drawable {
public:
	b2Body* body;
	b2Shape* shape;
	b2BodyDef bodyDef;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	void update(World* world);

protected:
	Entity();
};

#endif

