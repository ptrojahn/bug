#ifndef BUG_H
#define BUG_H

#include "entity.hpp"

class Bug : public Entity {
public:
	Bug(sf::Vector2i pos, float rotation);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(World* world);

	sf::Vector2i position;
	float rotation;

private:
	sf::CircleShape bug;
	sf::CircleShape eye;
};

#endif
