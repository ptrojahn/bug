#include "reinforcement.hpp"

#include <torch/torch.h>
#include <functional>

#include "state.hpp"

struct Replay {
	Features pre;
	Action action;
	int reward;
	Features post;
};

struct BugNet : torch::nn::Module {
	torch::nn::Linear lin0 = nullptr; // TODO what does nullptr do?
	torch::nn::Linear lin1 = nullptr;
	torch::nn::Linear lin2 = nullptr;

	BugNet() : lin0(BUG_RESOLUTION * 3 + 3, 64), lin1(64, 32), lin2(32, 3) {
		register_module("lin0", lin0);
		register_module("lin1", lin1);
		register_module("lin2", lin2);
	}

	torch::Tensor forward(torch::Tensor x) {
		x = torch::relu(lin0->forward(x));
		x = torch::relu(lin1->forward(x));
		x = torch::relu(lin2->forward(x));
		return x;
	}
};

Action egreedy(BugNet& net, State& state, double epsilon) {
	double random = torch::rand(1).item<float>();
	if (epsilon < random) // Explore
		return ceil((3 * torch::rand(1)).item<float()>);
	else { // Exploit
		/*std::vector<float> inputVec;
		inputVec.reserve(3 * features.size());
		for (sf::Color color : features) {
			inputVec.push_back(color.r / 255.f);
			inputVec.push_back(color.g / 255.f);
			inputVec.push_back(color.b / 255.f);
		}
		torch::Tensor input = torch::from_blob(inputVec.data(), (long)inputVec.size());
		torch::Tensor output = net.forward(input);
		return Action(output[0].item<float>() > 0.5, output[1].item<float>() > 0.5, output[2].item<float>() > 0.5);*/
	}
}

void simulateVisual(BugNet &net, State world) {
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
		return Action(output[0].item<float>() > 0.5, output[1].item<float>() > 0.5, output[2].item<float>() > 0.5);
	});
	int health = HEALTHINIT;

	auto update = [&](State &world) {
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

State stateInitial() {
	State world;
	world.addEntity(new Food(sf::Vector2i(200, 200)));
	world.addEntity(new Food(sf::Vector2i(500, 300)));
	world.addEntity(new Food(sf::Vector2i(300, 200)));
	world.addEntity(new Bug(sf::Vector2i(300, 300), -10.f));
	return world;
}

void reinforcement_train() {
	BugNet policy;
	const int episodes = 1000;
	const double epsilonDecay = 0.99;

	State state = stateInitial();
	double epsilon = 1.;
	std::vector<Replay> replays;

	for (int episode = 1; episode <= episodes; episode++) {
		int health = 100;
		int timeAlive = 0;
		epsilon = epsilon * epsilonDecay;

		while (health > 0) {
			timeAlive++;
			Features pre = state.getFeatures();
			Action action = egreedy(policy, state, epsilon);
			int reward = state.evaluate(action);
			health += reward;
			Features post = state.getFeatures();
			replays.push_back(Replay{.pre = pre, .post = post, .action = action, .reward = reward});
		}
	}
}

void reinforcement_run() {
	std::shared_ptr<BugNet> net = std::make_shared<BugNet>();
	torch::load(net, "reinforcement.pt");
	auto policyFun = [&](State state) {
		//egreedy(net, state, 0.);
		return Action(1, 0, 0);
	};
	State initial = worldInitial();
	initial.visual(policyFun);
	simulateVisual(*net, world);
}
