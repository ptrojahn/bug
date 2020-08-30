#ifndef STATE_H
#define STATE_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <torch/torch.h>

#include "geometry.hpp"

#define BUG_FOV 90
#define BUG_RESOLUTION 64

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
	torch::Tensor getFeatures();
	void visual(std::function<Action(State)> policy);
	int evaluate(Action action);

private:
	std::optional<Blob> raycast(Ray ray);
	std::vector<Blob> blobs;
	sf::Vector2i agentPos;
	float agentRotation;
};

#endif
