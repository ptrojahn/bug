#include <SFML/Graphics.hpp>
#include <torch/torch.h>

#include <string.h>

#include "state.hpp"
#include "geometry.hpp"
#include "evolutionary.hpp"
#include "reinforcement.hpp"

int main(int argc, char* argv[]) {
	/*torch::manual_seed(42);
	torch::Tensor z = torch::tensor(1);
	std::cout << z.sizes() << std::endl;
	std::cout << torch::stack({z, z}) << std::endl;
	return 0;*/

	if (argc == 1) {
		reinforcement_run();
	} else if (argc == 2 && strcmp(argv[1], "-t") == 0) {
		reinforcement_train();
	}
	return 0;
}
