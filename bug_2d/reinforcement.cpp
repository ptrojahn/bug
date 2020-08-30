#include "reinforcement.hpp"

#include <torch/torch.h>
#include <functional>
#include <cmath>

#include "state.hpp"

struct Replay {
	Features pre;
	Action action;
	int reward;
	Features post;
};

struct QNet : torch::nn::Module {
	torch::nn::Linear lin0 = nullptr; // TODO what does nullptr do?
	torch::nn::Linear lin1 = nullptr;
	torch::nn::Linear lin2 = nullptr;

	QNet() : lin0(BUG_RESOLUTION * 3, 64), lin1(64, 32), lin2(32, 3) {
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

Action qargmax(QNet& net, State& state) {
	std::vector<float> inputVec;
	Features features = state.getFeatures();
	inputVec.reserve(3 * features.size() + 3);
	for (sf::Color color : features) {
		inputVec.push_back(color.r / 255.f);
		inputVec.push_back(color.g / 255.f);
		inputVec.push_back(color.b / 255.f);
	}
	torch::Tensor input = torch::from_blob(inputVec.data(), (long)inputVec.size());
	return (Action)net.forward(input).argmax().item<int>();
}

Action egreedy(Action action, double epsilon) {
	double random = torch::rand(1).item<float>();
	if (epsilon < random) // Explore
		return (Action)std::ceil((3 * torch::rand(1)).item<float>());
	else { // Exploit
		return action;
	}
}

void reinforcement_train() {
	QNet qNet;
	const int episodes = 1000;
	const double epsilonDecay = 0.99;

	State state;
	double epsilon = 1.;
	int replayIndex = 0;
	std::vector<Replay> replays;

	for (int episode = 1; episode <= episodes; episode++) {
		int timeAlive = 0;
		epsilon = epsilon * epsilonDecay;

		while (timeAlive < 1000) {
			timeAlive++;
			Features pre = state.getFeatures();
			Action action = egreedy(qargmax(qNet, state), epsilon);
			int reward = state.evaluate(action);
			Features post = state.getFeatures();
			replays.push_back(Replay{.pre = pre, .action = action, .reward = reward, .post = post});
		}
	}
}

void reinforcement_run() {
	std::shared_ptr<QNet> net = std::make_shared<QNet>();
	//torch::load(net, "reinforcement.pt");
	State initial;
	auto policyFun = std::function([&](State state) {
		//egreedy(net, state, 0.);
		return Action::Forward;
	});
	initial.visual(policyFun);
}
