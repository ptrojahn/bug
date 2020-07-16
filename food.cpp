#include "food.hpp"

#include <iostream>

Food::Food(sf::Vector2i pos)  {
	color = sf::Color::Blue;
	circle.setRadius(10);
	circle.setFillColor(sf::Color::Blue);
	circle.setPosition(sf::Vector2f(pos));
	shape = new Circle(sf::Vector2i(pos.x, pos.y), 10);
}

void Food::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(circle);
}
