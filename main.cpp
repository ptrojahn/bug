#include <SFML/Graphics.hpp>

#include "entity.hpp"
#include "world.hpp"
#include "food.hpp"

int main()
{
	World world;
	world.addEntity(new Food(100, 100));
	world.simVisual();
	return 0;
}
