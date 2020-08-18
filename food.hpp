#ifndef FOOD_H
#define FOOD_H

#include <SFML/Graphics.hpp>

#include "entity.hpp"

class Food : public Entity {
public:
	Food(sf::Vector2i pos);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void setPosition(sf::Vector2i pos);
	Entity* clone();

private:
	sf::CircleShape circle;
};

#endif
