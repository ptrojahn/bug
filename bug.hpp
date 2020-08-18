#ifndef BUG_H
#define BUG_H

#include <functional>

#include "entity.hpp"

struct Direction {
	bool forward;
	bool left;
	bool right;

	Direction(bool forward, bool left, bool right) {
		this->forward = forward;
		this->left = left;
		this->right = right;
	}
};

class Bug : public Entity {
public:
	Bug(sf::Vector2i pos, float rotation);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(World* world);
	Entity* clone();

	sf::Vector2i position;
	float rotation;
	std::function<Direction(std::vector<sf::Color>)> actionFunction;

private:
	sf::CircleShape bug;
	sf::CircleShape eye;
};

#endif
