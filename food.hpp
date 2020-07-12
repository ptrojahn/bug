#ifndef FOOD_H
#define FOOD_H

#include <SFML/Graphics.hpp>

#include "entity.hpp"

class Food : public Entity {
public:
	Food(int x, int y);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(World* world);

private:
	sf::CircleShape circle;
};

#endif
