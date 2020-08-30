#include "reinforcement.hpp"

#include <torch/torch.h>
#include <functional>
#include <cmath>

#include "state.hpp"

struct Replay {
	torch::Tensor pre;
	Action action;
	int reward;
	torch::Tensor post;
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
	return (Action)net.forward(state.getFeatures()).argmax().item<int>();
}

Action egreedy(Action action, double epsilon) {
	double random = torch::rand(1).item<float>();
	if (epsilon < random) // Explore
		return (Action)torch::randint(0, 4, 1).item<int>();
	else { // Exploit
		return action;
	}
}

void reinforcement_train() {
	QNet qNet;
	const int episodes = 1000;
	const double epsilonDecay = 0.99;
	const int replaysMax = 100000;
	const int replayBatch = 64;

	State state;
	double epsilon = 1.;
	int replayIndex = 0;
	std::vector<Replay> replays;

	for (int episode = 1; episode <= episodes; episode++) {
		int timeAlive = 0;
		epsilon = epsilon * epsilonDecay;

		while (timeAlive < 1000) {
			// Make step
			timeAlive++;
			torch::Tensor pre = state.getFeatures();
			Action action = egreedy(qargmax(qNet, state), epsilon);
			int reward = state.evaluate(action);
			torch::Tensor post = state.getFeatures();
			Replay replay = {.pre = pre, .action = action, .reward = reward, .post = post};
			if (replays.size() < replaysMax)
				replays.push_back(replay);
			else {
				replays[replayIndex] = replay;
				replayIndex %= replaysMax;
			}

			//Train from replay
			float errorSum = 0;
			if (replays.size() >= replayBatch) { // TODO: How is a pointer a iterator?
				torch::Tensor batchIndices(torch::randint(0, replays.size(), replayBatch));
				for (int i = 0; i < replayBatch; i++) {
					int batchIndex = torch::randint(0, replays.size(), 1).item<int>();

				}
			}
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
