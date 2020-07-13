#include "bug.hpp"

#include <cmath>
#include <iostream>

#include "helper.hpp"

const int FOV = 90;
const int EYES = 64;

Bug::Bug(int x, int y, float rotation) {
	color = sf::Color::Cyan;
	initX = x;
	initY = y;
	initRotation = rotation;
	bug.setRadius(20.f);
	bug.setFillColor(sf::Color::Cyan);
	eye.setRadius(5.f);
	eye.setFillColor(sf::Color::Blue);
}

void Bug::initPhysics(b2World* world) {
	b2BodyDef bodyDef;
	bodyDef.position.Set(initX / 100.f, initY / 100.f);
	bodyDef.angle = initRotation / 360.f * (2 * M_PI);
	body = world->CreateBody(&bodyDef);
	b2CircleShape shape_body;
	shape_body.m_radius = 20.f / 100.f;
	body->CreateFixture(&shape_body, 0.0);
	b2CircleShape shape_eye;
	shape_eye.m_radius = 6.f / 100.f;
	shape_eye.m_p = b2Vec2(0.f, -20.f);
	body->CreateFixture(&shape_eye, 0.0);
}

void Bug::update(World* world) {
	sf::Vector2f posBug = vecB2SFML(body->GetPosition());
	posEye = vecB2SFML(body->GetWorldPoint(b2Vec2(0.f / 100.f, -20.f / 100.f)));
	bug.setPosition(posBug.x - 20.f, posBug.y - 20.f);
	eye.setPosition(posEye.x - 6.f, posEye.y - 6.f);

	sf::Color image[EYES];
	std::cout << "START" << std::endl;
	for (int i = 0; i < EYES; i++) {
		float angle = FOV / -2.f + FOV / (float)(EYES - 1) * (float)i;
		sf::Transform rot;
		rot.rotate(angle);
		sf::Vector2f posLocal = rot * sf::Vector2f(0.f, -200.f);
		RayCastFirstCallback* cast = new RayCastFirstCallback();
		world->world.RayCast(cast, vecSFMLB2(posEye), body->GetWorldPoint(vecSFMLB2(posLocal)));
		if (cast->result != nullptr) {
			image[i] = reinterpret_cast<Entity*>(cast->result->GetBody()->GetUserData())->color;
		}
		std::cout << (int)image[i].r << "#" << (int)image[i].g << "#" << (int)image[i].b << std::endl;
	}
	std::cout << "END" << std::endl;

	/* DEEP REINFORCEMENT MAGIC */

	bool actionLeft = false;
	bool actionRight = true;
	bool actionForward = true;

	if (actionForward) {
		sf::Transform rot;
		rot.rotate(angleB2SFML(body->GetAngle()));
		sf::Vector2f posLocal = rot * sf::Vector2f(0.f, -1.f);
		body->SetTransform(body->GetPosition() + vecSFMLB2(posLocal), body->GetAngle());
	}
	if (actionLeft) {
		body->SetTransform(body->GetPosition(), body->GetAngle() - angleSFMLB2(1.f));
	}
	if (actionRight) {
		body->SetTransform(body->GetPosition(), body->GetAngle() + angleSFMLB2(1.f));
	}
}

void Bug::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(bug);
	target.draw(eye);

	for (int i = 0; i < EYES; i++) {
		float angle = FOV / -2.f + FOV / (float)(EYES - 1) * (float)i;
		sf::Transform rot;
		rot.rotate(angle);
		sf::Vector2f posLocal = rot * sf::Vector2f(0.f, -200.f);
		sf::Vertex line[] =
		{
			sf::Vertex(posEye),
			sf::Vertex(vecB2SFML(body->GetWorldPoint(vecSFMLB2(posLocal))))
		};
		target.draw(line, 2, sf::Lines);
	}
}
