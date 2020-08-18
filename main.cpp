#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include <chrono>

#include <torch/torch.h>

#include "entity.hpp"
#include "world.hpp"
#include "food.hpp"
#include "bug.hpp"

#include "geometry.hpp"

int main() {
	World world;
	world.addEntity(new Food(sf::Vector2i(200, 200)));
	world.addEntity(new Bug(sf::Vector2i(300, 300), -10.f));
	world.worldVisual();
	return 0;
}

int evolutionary() {

}
