#include <SFML/Graphics.hpp>

#include <string.h>

#include "state.hpp"
#include "geometry.hpp"
#include "evolutionary.hpp"
#include "reinforcement.hpp"

int main(int argc, char* argv[]) {
	if (argc == 1) {
		reinforcement_run();
	} else if (argc == 2 && strcmp(argv[1], "-t") == 0) {
		reinforcement_train();
	}
	return 0;
}
