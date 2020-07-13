#ifndef FOOD_H
#define FOOD_H

#include <SFML/Graphics.hpp>

#include "entity.hpp"

class Food : public Entity {
public:
	Food(int x, int y);
	void initPhysics(b2World* world);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(World* world);

private:
	int initX, initY;
	sf::CircleShape circle;
};

#endif
