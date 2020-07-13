#ifndef HELPER_H
#define HELPER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <cmath>

sf::Vector2f vecB2SFML(b2Vec2 vec) {
	return sf::Vector2f(vec.x * 100.f, vec.y * 100.f);
}

b2Vec2 vecSFMLB2(sf::Vector2f vec) {
	return b2Vec2(vec.x / 100.f, vec.y / 100.f);
}

float angleSFMLB2(float angle) {
	return angle / 360.f * (2*M_PI);
}

float angleB2SFML(float angle) {
	return angle / (2*M_PI) * 360.f;
}

class RayCastFirstCallback : public b2RayCastCallback {
public:
	float ReportFixture (b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float fraction) {
		result = fixture;
		this->point = point;
		return 0;
	}
	b2Fixture* result;
	b2Vec2 point;
};

#endif
