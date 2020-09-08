#include <SFML/Graphics.hpp>
#include <torch/torch.h>

#include <string.h>

#include "state.hpp"
#include "geometry.hpp"
#include "evolutionary.hpp"
#include "reinforcement.hpp"

#include "eligibilityopt.h"

int main(int argc, char* argv[]) {
	torch::manual_seed(42);

	/*torch::Tensor linear = torch::ones(2, torch::TensorOptions().requires_grad(true));
	torch::Tensor loss = linear[0] + linear[1];
	std::vector<torch::Tensor> params;
	params.push_back(linear);
	torch::optim::SGD optim(params, 0.1);
	loss.backward();
	optim.step();
	std::cout << linear.grad() << std::endl;
	std::cout << linear << std::endl;*/
	/*torch::Tensor linear = torch::ones(2, torch::TensorOptions().requires_grad(true));
	torch::Tensor loss = linear[0] + linear[1];
	std::vector<torch::Tensor> params;
	params.push_back(linear);
	EligibilityOpt optim(params, 0.1, 0);
	loss.backward();
	optim.step(torch::tensor(1));
	std::cout << linear.grad() << std::endl;
	std::cout << linear << std::endl;
	return 0;*/

	if (argc == 1) {
		reinforcement_run();
	} else if (argc == 2 && strcmp(argv[1], "-t") == 0) {
		reinforcement_train();
	}
	return 0;
}
