#include "state.hpp"

#include <iostream>
#include <torch/torch.h>

#include "state.hpp"

//#define DEBUG_DRAW
#define BLOB_SIZE 20
#define REWARD 200

State::State() {
	agentPos = sf::Vector2i(400, 400);
	agentRotation = 0.;
	blobs.push_back(Blob{.pos = sf::Vector2i(350, 300), .color = sf::Color::Green});
	blobs.push_back(Blob{.pos = sf::Vector2i(450, 300), .color = sf::Color::Green});
	blobs.push_back(Blob{.pos = sf::Vector2i(450, 500), .color = sf::Color::Green});
	blobs.push_back(Blob{.pos = sf::Vector2i(350, 500), .color = sf::Color::Green});
}

torch::Tensor State::getFeatures() {
	torch::Tensor features = torch::zeros(BUG_RESOLUTION * 3);
	sf::Vector2i eyePos = agentPos + (rotate(sf::Vector2i(0, -10), agentRotation));
	for (int i = 0; i < BUG_RESOLUTION; i++) {
		float angle = -(BUG_FOV / 2.f) + BUG_FOV / (float)(BUG_RESOLUTION - 1) * (float)i + agentRotation;
		Ray ray(eyePos, rotate(sf::Vector2i(0, -256), angle));
		std::optional<Blob> blob = raycast(ray);
		if (blob.has_value()) {
			sf::Color color = blob.value().color;
			features[i*3] = color.r / 255.f;
			features[i*3 + 1] = color.g / 255.f;
			features[i*3 + 2] = color.b / 255.f;
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
	circle.setPosition(sf::Vector2f(pos - sf::Vector2i(radius, radius)));
	circle.setFillColor(color);
	win.draw(circle);
}

void sfmlDrawRect(sf::RenderWindow& win, sf::Vector2i pos, sf::Vector2i size, sf::Color color) {
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(size));
	rect.setPosition(sf::Vector2f(pos));
	rect.setFillColor(color);
	win.draw(rect);
}

sf::RenderWindow* window = nullptr;

void State::updateWindow() {
	if (window == nullptr)
		window = new sf::RenderWindow(sf::VideoMode(800, 800), "Bug");

	window->clear();
	for(Blob& b : blobs) {
		sfmlDrawCircle(*window, b.pos, BLOB_SIZE, b.color);
	}
	sf::Vector2i eyePos = agentPos + (rotate(sf::Vector2i(0, -10), agentRotation));
	sfmlDrawCircle(*window, agentPos, 10, sf::Color::Blue);
	sfmlDrawCircle(*window, eyePos, 6, sf::Color::Cyan);
#ifdef DEBUG_DRAW
	for (int i = 0; i < BUG_RESOLUTION; i++) {
		float angle = -(BUG_FOV / 2.f) + BUG_FOV / (float)(BUG_RESOLUTION - 1) * (float)i + agentRotation;
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(eyePos)),
			sf::Vertex(sf::Vector2f(sf::Vector2i(eyePos) + rotate(sf::Vector2i(0, -256), angle)))
		};
		window->draw(line, 2, sf::Lines);
	}

	sfmlDrawRect(*window, sf::Vector2i(0, 0), sf::Vector2i(800, 20), sf::Color::White);
	torch::Tensor features = getFeatures();
	for (int i = 0; i < BUG_RESOLUTION; i++) {
		sf::Color color(features[i*3].item<float>()*255, features[i*3 + 1].item<float>()*255, features[i*3 + 2].item<float>()*255);
		sf::Vector2i pos(i*8+4, 10);
		sfmlDrawCircle(*window, pos, 4, color);
	}
#endif
	window->display();
}

void State::visual(std::function<Action(State)> policy) {
	updateWindow();
	window->setFramerateLimit(12);

	while (window->isOpen())
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
		}
		Action action = policy(*this);
		evaluate(action);

		updateWindow();
	}
}

int State::evaluate(Action action) {
	//if (action == Forward) {
	agentPos += rotate(sf::Vector2i(0, -10), agentRotation);
	if (action == Left) {
		agentRotation -= 18;
	} else if (action == Right) {
		agentRotation += 18;
	}

	int reward = -1;
	for (Blob& blob : blobs) {
		Circle blobCircle(blob.pos, BLOB_SIZE);
		if (Circle(agentPos, 20).intersection(&blobCircle)) {
			if (blob.color == sf::Color::Green) {
				reward += REWARD;
				torch::Tensor pos = torch::rand(2) * 700 + 50;
				blob.pos = sf::Vector2i(pos[0].item<int>(), pos[1].item<int>());
			}
		}
	}
	return reward;
}
