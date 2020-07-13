#include "food.hpp"

#include <iostream>

const float RADIUS = 10.f;

Food::Food(int x, int y)  {
	color = sf::Color::Blue;
	circle.setRadius(RADIUS);
	circle.setFillColor(sf::Color::Blue);
	initX = x;
	initY = y;
}

void Food::initPhysics(b2World* world) {
	b2BodyDef bodyDef;
	bodyDef.position.Set(initX / 100.f, initY / 100.f);
	body = world->CreateBody(&bodyDef);
	b2CircleShape shape_body;
	shape_body.m_radius = RADIUS / 100.f;
	body->CreateFixture(&shape_body, 1.0);
}

void Food::update(World* world) {
	b2Vec2 pos = body->GetPosition();
	circle.setPosition(pos.x * 100.f, pos.y * 100.f);
}

void Food::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(circle);
}
