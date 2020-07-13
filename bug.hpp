#ifndef BUG_H
#define BUG_H

#include "entity.hpp"

class Bug : public Entity {
public:
	Bug(int x, int y, float rotation);
	void initPhysics(b2World* world);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(World* world);

private:
	int initX, initY;
	float initRotation;
	sf::Vector2f posEye;
	sf::CircleShape bug;
	sf::CircleShape eye;
};

#endif
