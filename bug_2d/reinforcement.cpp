#include "reinforcement.hpp"


struct BugNet : torch::nn::Module {
	torch::nn::Linear lin0 = nullptr; // TODO what does nullptr do?
	torch::nn::Linear lin1 = nullptr;
	torch::nn::Linear lin2 = nullptr;

	BugNet() : lin0(BUG_RESOLUTION * 3, BUG_RESOLUTION), lin1(BUG_RESOLUTION, BUG_RESOLUTION / 2), lin2(BUG_RESOLUTION / 2, 3) {
		register_module("lin0", lin0);
		register_module("lin1", lin1);
		register_module("lin2", lin2);
		lin0 = init_linear(lin0);
		lin1 = init_linear(lin1);
		lin2 = init_linear(lin2);
	}
	std::shared_ptr<BugNet> copy_fuzzy() {
		std::shared_ptr<BugNet> bugnet = std::make_shared<BugNet>();
		bugnet->lin0 = mutate_linear(lin0, MUTATION);
		bugnet->lin1 = mutate_linear(lin1, MUTATION);
		bugnet->lin2 = mutate_linear(lin2, MUTATION);
		return bugnet;
	}

	torch::Tensor forward(torch::Tensor x) {
		x = torch::sigmoid(lin0->forward(x) / BUG_RESOLUTION);
		x = torch::sigmoid(lin1->forward(x) / (BUG_RESOLUTION / 2));
		x = torch::sigmoid(lin2->forward(x) / 3);
		std::cout << x << std::endl;
		exit(42);
		return x;
	}
};

void simulateVisual(BugNet &net, World world) {
	Bug *bug = world.getAll<Bug>()[0];
	std::vector<Food*> foods = world.getAll<Food>();

	bug->actionFunction = std::function([&](std::vector<sf::Color> vision) {
		std::vector<float> inputVec;
		inputVec.reserve(3 * vision.size());
		for (sf::Color color : vision) {
			inputVec.push_back(color.r / 255.f);
			inputVec.push_back(color.g / 255.f);
			inputVec.push_back(color.b / 255.f);
		}
		torch::Tensor input = torch::from_blob(inputVec.data(), (long)inputVec.size());
		torch::Tensor output = net.forward(input);
		//std::cout << output[0].item<float>() << " " << output[1].item<float>() << std::endl;
		return Direction(output[0].item<float>() > 0.5, output[1].item<float>() > 0.5, output[2].item<float>() > 0.5);
	});
	int health = HEALTHINIT;

	auto update = [&](World &world) {
		for (Food* food : foods) {
			if (food->shape->intersection(bug->shape)) {
				health += REWARD;
				torch::Tensor pos = torch::rand(2) * 700 + 50;
				food->setPosition(sf::Vector2i(pos[0].item<int>(), pos[1].item<int>()));
			}
		}
		health -= 1;
		return health > 0;
	};
	world.visual(std::function(update));
}

void reinforcement_train(World &world) {

}

void reinforcement_run(World &world) {

}
