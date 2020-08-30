#ifndef STATE_H
#define STATE_H

#include <vector>
#include <SFML/Graphics.hpp>

#include "geometry.hpp"

#define BUG_FOV 90
#define BUG_RESOLUTION 64

typedef std::array<sf::Color, BUG_RESOLUTION> Features;

class Entity;

enum Action {
	Forward,
	Left,
	Right
};

struct Blob {
	sf::Vector2i pos;
	sf::Color color;
};

class State {
public:
	State();
	Features getFeatures();
	void visual(std::function<Action(State)> policy);
	int evaluate(Action action);

private:
	std::optional<Blob> raycast(Ray ray);
	std::vector<Blob> blobs;
	sf::Vector2i agentPos;
	float agentRotation;
};

#endif
