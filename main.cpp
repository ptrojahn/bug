#include <SFML/Graphics.hpp>

#include <string.h>

#include "entity.hpp"
#include "world.hpp"
#include "food.hpp"
#include "bug.hpp"
#include "geometry.hpp"
#include "evolutionary.hpp"

int main(int argc, char* argv[]) {
	World world;
	world.addEntity(new Food(sf::Vector2i(200, 200)));
	world.addEntity(new Bug(sf::Vector2i(300, 300), -10.f));

	if (argc == 1) {
		evolutionary_run(world);
	} else if (argc == 2) {
		if (strcmp(argv[1], "--tevo") == 0){
			evolutionary_train(world);
		}
	}
	return 0;
}
