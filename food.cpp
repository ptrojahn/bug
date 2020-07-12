#include "food.hpp"

Food::Food(int x, int y) {
	bodyDef.position.Set(x / 100.f, y / 100.f);
	circle.setRadius(10);
	circle.setFillColor(sf::Color::Blue);
	shape = new b2CircleShape();
	shape->m_radius = 10 / 100.f;
}

void Food::update(World* world) {
	b2Vec2 pos = body->GetPosition();
	circle.setPosition(pos.x * 100.f, pos.y * 100.f);
}

void Food::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(circle);
}
