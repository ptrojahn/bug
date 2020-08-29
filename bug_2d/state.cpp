#include "state.hpp"

#include <iostream>
#include <torch/torch.h>

#include "state.hpp"

#define DEBUG_DRAW
#define BLOB_SIZE 10

State::State() {
	agentPos = sf::Vector2i(200, 200);
	agentRotation = 0.;
	blobs.push_back(Blob{.pos = sf::Vector2i(200, 100), .color = sf::Color::Green});
	blobs.push_back(Blob{.pos = sf::Vector2i(100, 200), .color = sf::Color::Green});
	blobs.push_back(Blob{.pos = sf::Vector2i(300, 200), .color = sf::Color::Green});
	blobs.push_back(Blob{.pos = sf::Vector2i(200, 300), .color = sf::Color::Green});
}

Features State::getFeatures() {
	Features features;
	sf::Vector2i eyePos = agentPos - (rotate(sf::Vector2i(0, 20), agentRotation));
	for (int i = 0; i < BUG_RESOLUTION; i++) {
		float angle = -(BUG_FOV / 2.f) + BUG_FOV / (float)(BUG_RESOLUTION - 1) * (float)i + agentRotation;
		Ray ray(eyePos, rotate(sf::Vector2i(0, -256), angle));
		std::optional<Blob> blob = raycast(ray);
		if (blob.has_value()) {
			features[i] = blob.value().color;
		}
	}
	return features;
}

std::optional<Blob> State::raycast(Ray ray) {
	std::optional<std::tuple<Blob, float>> result = std::nullopt;
	for (Blob& blob : blobs) {
		Circle blobCircle(blob.pos, BLOB_SIZE);
		if (std::optional<float> i = ray.intersection(&blobCircle)) {
			if (result.has_value() && std::get<1>(result.value()) > i.value()) {
			} else {
				result = std::make_tuple(blob, i.value());
			}
		}
	}
	return result.has_value() ? std::optional<Blob>(std::get<0>(result.value())) : std::nullopt;
}

void sfmlDrawCircle(sf::RenderWindow& win, sf::Vector2i pos, int radius, sf::Color color) {
	sf::CircleShape circle;
	circle.setRadius(radius);
	circle.setPosition(sf::Vector2f(pos - sf::Vector2i(radius / 2, radius / 2)));
	circle.setFillColor(color);
	win.draw(circle);
}

void State::visual(std::function<Action(Features)> policy) {
	sf::RenderWindow window(sf::VideoMode(800, 800), "Bug");
	window.setFramerateLimit(4);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		Action action = policy(getFeatures());
		evaluate(action);

		for(Blob& b : blobs) {
			sfmlDrawCircle(window, b.pos, BLOB_SIZE, b.color);
		}
		sf::Vector2i eyePos = agentPos - (rotate(sf::Vector2i(0, 20), agentRotation));
		sfmlDrawCircle(window, agentPos, 20, sf::Color::Blue);
		sfmlDrawCircle(window, eyePos, 6, sf::Color::Cyan);

#ifdef DEBUG_DRAW
		for (int i = 0; i < BUG_RESOLUTION; i++) {
			float angle = -(BUG_FOV / 2.f) + BUG_FOV / (float)(BUG_RESOLUTION - 1) * (float)i + agentRotation;
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(eyePos) + sf::Vector2f(6.f, 6.f)),
				sf::Vertex(sf::Vector2f(sf::Vector2i(eyePos) + sf::Vector2i(6, 6) + rotate(sf::Vector2i(0, -256), angle)))
			};
			window.draw(line, 2, sf::Lines);
		}
#endif

		window.display();
	}
}

int State::evaluate(Action action) {
	if (action == Forward) {
		agentPos += rotate(sf::Vector2i(0, -10), agentRotation);
	} else if (action == Left) {
		agentRotation -= 4;
	} else if (action == Right) {
		agentRotation += 4;
	}

	int reward = -1;
	for (Blob& blob : blobs) {
		Circle blobCircle(blob.pos, BLOB_SIZE);
		if (Circle(agentPos, 20).intersection(&blobCircle)) {
			if (blob.color == sf::Color::Green) {
				reward += 60;
				torch::Tensor pos = torch::rand(2) * 700 + 50;
				blob.pos = sf::Vector2i(pos[0].item<int>(), pos[1].item<int>());
			}
		}
	}
}