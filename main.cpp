#include <SFML/Graphics.hpp>

#include "entity.hpp"
#include "world.hpp"
#include "food.hpp"
#include "bug.hpp"

int main()
{
	World world;
	world.addEntity(new Food(250, 200));
	world.addEntity(new Bug(300, 300, 30.f));
	world.simVisual();
	return 0;
}
